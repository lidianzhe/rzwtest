#include "dialog.h"
#include "ui_dialog.h"
#include "eventthread.h"

#include <QDebug>


Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    connect(ui->pushButton_Open, SIGNAL(clicked()), this, SLOT(open()));
    connect(ui->pushButton_Close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->pushButton_WriteWiegand, SIGNAL(clicked()), this, SLOT(writeWiegand()));
    connect(ui->pushButton_WiegandSetConfig, SIGNAL(clicked()), this, SLOT(wiegandSetConfig()));
    connect(ui->pushButton_WiegandGetConfig, SIGNAL(clicked()), this, SLOT(wiegandGetConfig()));
    connect(ui->pushButton_WiegandAutoDetect, SIGNAL(clicked()), this, SLOT(wiegandAutoDetect()));

    connect(ui->pushButton_WriteGPO1, SIGNAL(clicked()), this, SLOT(writeGPO1()));
    connect(ui->pushButton_WriteGPO2, SIGNAL(clicked()), this, SLOT(writeGPO2()));
    connect(ui->pushButton_WriteGPO12, SIGNAL(clicked()), this, SLOT(writeGPO12()));
    connect(ui->pushButton_WriteRelay, SIGNAL(clicked()), this, SLOT(writeRelay()));

    connect(ui->pushButton_USBPowerOff, SIGNAL(clicked()), this, SLOT(usbPowerOff()));
    connect(ui->pushButton_USBPowerOn, SIGNAL(clicked()), this, SLOT(usbPowerOn()));

    m_eventThread = NULL;
    m_isOpen = false;

    if (ema_createEMA(&m_emaHandle) == EMA_SUCCESS) {
        m_eventThread = new EventThread(NULL, m_emaHandle);
        if (m_eventThread) {
            connect(m_eventThread, SIGNAL(dataReceived(EMA_EVENT *)),
                    this, SLOT(showReceivedData(EMA_EVENT *)));
        }
        m_eventThread->start();
    }

    m_curWiegandOutConfig.cbSize = sizeof(EMA_EVENT);
    m_curWiegandOutConfig.eventType = -1;
    m_curWiegandOutConfig.numOfBits = 0;
    m_curWiegandOutConfig.pulseWidth = 0;
    m_curWiegandOutConfig.pulseInterval = 0;
}

Dialog::~Dialog()
{
    m_eventThread->abort();
    m_eventThread->wait();

    ema_destroyEMA(m_emaHandle);
    delete ui;
}

void Dialog::open() {

    int ret = ema_open(m_emaHandle);
    if (ret == EMA_SUCCESS) {
        ui->plainTextEdit_Log->appendPlainText("Open SUCCESS!");
        m_isOpen = true;
    }
    else if (ret == EMA_ERROR_ALREADY_OPENED) {
        ui->plainTextEdit_Log->appendPlainText("Already opened!");
    }
    else if (ret == EMA_ERROR_OPEN_FAILED) {
        ui->plainTextEdit_Log->appendPlainText("Open Failed!");
    }
    else if (ret == EMA_ERROR_CANNOT_ALLOC_MEMORY) {
        ui->plainTextEdit_Log->appendPlainText("Out of Memory in Open()!");
    }

    return;
}

void Dialog::close() {

    int ret = ema_close(m_emaHandle);
    qDebug() << "Close()" << ret;
    if (ret == EMA_SUCCESS) {
        ui->plainTextEdit_Log->appendPlainText("Close SUCCESS!");
        m_isOpen = false;
    }
    else if (ret == EMA_ERROR_CANNOT_ALLOC_MEMORY) {
        ui->plainTextEdit_Log->appendPlainText("Out of Memory in Close()!");
    }
    else if (ret == EMA_ERROR_ALREADY_CLOSED) {
        ui->plainTextEdit_Log->appendPlainText("Already closed!");
    }

    return;
}

void Dialog::wiegandSetConfig() {

    if (!m_isOpen) {
        ui->plainTextEdit_Log->appendPlainText("Please open device first!");
        return;
    }

    EMA_EVENT emaEvent;
    emaEvent.cbSize = sizeof(EMA_EVENT);

    bool ok;
    emaEvent.wiegandOutChannel = ui->spinBox_ChannelWrite->value();
    emaEvent.numOfBits = ui->lineEdit_NumOfBitsWrite->text().toUShort(&ok);
    if (!ok) return;
    emaEvent.pulseWidth = ui->lineEdit_PulseWidthWrite->text().toUShort(&ok);
    if (!ok) return;
    emaEvent.pulseInterval = ui->lineEdit_PulseIntervalWrite->text().toUShort(&ok);
    if (!ok) return;


    emaEvent.eventType = EMA_EVENT_TYPE_WIEGAND_SET_CONFIG;

    int ret = ema_writeEvent(m_emaHandle, &emaEvent);

    if (ret == 0) {
        ui->plainTextEdit_Log->appendPlainText("Write success");
        m_curWiegandOutConfig.numOfBits = emaEvent.numOfBits;
        m_curWiegandOutConfig.pulseWidth = emaEvent.pulseWidth;
        m_curWiegandOutConfig.pulseInterval = emaEvent.pulseInterval;
    }
    else {
        ui->plainTextEdit_Log->appendPlainText(QString("Write error return= %1").arg(ret));
    }

    return;
}

void Dialog::wiegandGetConfig() {

    if (!m_isOpen) {
        ui->plainTextEdit_Log->appendPlainText("Please open device first!");
        return;
    }

    EMA_EVENT emaEvent;
    emaEvent.cbSize = sizeof(EMA_EVENT);

    emaEvent.eventType = EMA_EVENT_TYPE_WIEGAND_GET_CONFIG;
    emaEvent.wiegandOutChannel = ui->spinBox_ChannelWrite->value();

    int ret = ema_writeEvent(m_emaHandle, &emaEvent);

    if (ret == 0) {
        ui->plainTextEdit_Log->appendPlainText("Write success");
    }
    else {
        ui->plainTextEdit_Log->appendPlainText(QString("Write error return= %1").arg(ret));
    }

    return;
}

void Dialog::wiegandAutoDetect() {

    if (!m_isOpen) {
        ui->plainTextEdit_Log->appendPlainText("Please open device first!");
        return;
    }

    EMA_EVENT emaEvent;
    emaEvent.cbSize = sizeof(EMA_EVENT);

    emaEvent.eventType = EMA_EVENT_TYPE_WIEGAND_AUTO_DETECT;

    int ret = ema_writeEvent(m_emaHandle, &emaEvent);

    if (ret == 0) {
        ui->plainTextEdit_Log->appendPlainText("Write success");
    }
    else {
        ui->plainTextEdit_Log->appendPlainText(QString("Write error return= %1").arg(ret));
    }

    return;
}

void Dialog::writeWiegand() {

    if (!m_isOpen) {
        ui->plainTextEdit_Log->appendPlainText("Please open device first!");
        return;
    }

    EMA_EVENT emaEvent;
    emaEvent.cbSize = sizeof(EMA_EVENT);

    emaEvent.eventType = EMA_EVENT_TYPE_WIEGAND_WRITE_DATA;

    emaEvent.wiegandOutChannel = ui->spinBox_ChannelWrite->value();
    qDebug() << "Channel" << emaEvent.wiegandOutChannel;
    emaEvent.numOfBits = m_curWiegandOutConfig.numOfBits;
    emaEvent.pulseWidth = m_curWiegandOutConfig.pulseWidth;
    emaEvent.pulseInterval = m_curWiegandOutConfig.pulseInterval;

    QString str = ui->lineEdit_WiegandDataWrite->text();
    quint32 v = str.toInt();

    //QByteArray ba = QByteArray::fromHex(str.toAscii());
    QByteArray ba=sendWiegand(v,m_curWiegandOutConfig.numOfBits);

    int i;
    for (i = 0; i < ba.size(); i++) {
        emaEvent.wiegandData[i] = ba[i];
    }
    int ret = ema_writeEvent(m_emaHandle, &emaEvent);

    if (ret == 0) {
        ui->plainTextEdit_Log->appendPlainText("Write success");
    }
    else {
        ui->plainTextEdit_Log->appendPlainText(QString("Write error return= %1").arg(ret));
    }
    return;
}

void Dialog::writeGPO1() {

    if (!m_isOpen) {
        ui->plainTextEdit_Log->appendPlainText("Please open device first!");
        return;
    }

    EMA_EVENT emaEvent;
    emaEvent.cbSize = sizeof(EMA_EVENT);

    emaEvent.eventType = EMA_EVENT_TYPE_GPO1_WRITE;

    if (ui->lineEdit_GPO1->text().toLower() == QString("high")) {
        emaEvent.gpo1Value = EMA_EVENT_VALUE_GPIO_HIGH;
    }
    else if (ui->lineEdit_GPO1->text().toLower() == QString("low")) {
        emaEvent.gpo1Value = EMA_EVENT_VALUE_GPIO_LOW;
    }
    else {
        ui->plainTextEdit_Log->appendPlainText("Invalid GPO1 value!");
        return;
    }

    int ret = ema_writeEvent(m_emaHandle, &emaEvent);

    if (ret == 0) {
        ui->plainTextEdit_Log->appendPlainText("Write success");
    }
    else {
        ui->plainTextEdit_Log->appendPlainText(QString("Write error return= %1").arg(ret));
    }
    return;
}

void Dialog::writeGPO2() {

    if (!m_isOpen) {
        ui->plainTextEdit_Log->appendPlainText("Please open device first!");
        return;
    }

    EMA_EVENT emaEvent;
    emaEvent.cbSize = sizeof(EMA_EVENT);

    emaEvent.eventType = EMA_EVENT_TYPE_GPO2_WRITE;

    if (ui->lineEdit_GPO2->text().toLower() == QString("high")) {
        emaEvent.gpo2Value = EMA_EVENT_VALUE_GPIO_HIGH;
    }
    else if (ui->lineEdit_GPO2->text().toLower() == QString("low")) {
        emaEvent.gpo2Value = EMA_EVENT_VALUE_GPIO_LOW;
    }
    else {
        ui->plainTextEdit_Log->appendPlainText("Invalid GPO2 value!");
        return;
    }

    int ret = ema_writeEvent(m_emaHandle, &emaEvent);

    if (ret == 0) {
        ui->plainTextEdit_Log->appendPlainText("Write success");
    }
    else {
        ui->plainTextEdit_Log->appendPlainText(QString("Write error return= %1").arg(ret));
    }
    return;
}

void Dialog::writeGPO12() {

    if (!m_isOpen) {
        ui->plainTextEdit_Log->appendPlainText("Please open device first!");
        return;
    }

    EMA_EVENT emaEvent;
    emaEvent.cbSize = sizeof(EMA_EVENT);

    emaEvent.eventType = EMA_EVENT_TYPE_GPO12_WRITE;

    if (ui->lineEdit_GPO1->text().toLower() == QString("high")) {
        emaEvent.gpo1Value = EMA_EVENT_VALUE_GPIO_HIGH;
    }
    else if (ui->lineEdit_GPO1->text().toLower() == QString("low")) {
        emaEvent.gpo1Value = EMA_EVENT_VALUE_GPIO_LOW;
    }
    else {
        ui->plainTextEdit_Log->appendPlainText("Invalid GPO1 value!");
        return;
    }

    if (ui->lineEdit_GPO2->text().toLower() == QString("high")) {
        emaEvent.gpo2Value = EMA_EVENT_VALUE_GPIO_HIGH;
    }
    else if (ui->lineEdit_GPO2->text().toLower() == QString("low")) {
        emaEvent.gpo2Value = EMA_EVENT_VALUE_GPIO_LOW;
    }
    else {
        ui->plainTextEdit_Log->appendPlainText("Invalid GPO2 value!");
        return;
    }

    int ret = ema_writeEvent(m_emaHandle, &emaEvent);

    if (ret == 0) {
        ui->plainTextEdit_Log->appendPlainText("Write success");
    }
    else {
        ui->plainTextEdit_Log->appendPlainText(QString("Write error return= %1").arg(ret));
    }
    return;
}

void Dialog::writeRelay() {

    if (!m_isOpen) {
        ui->plainTextEdit_Log->appendPlainText("Please open device first!");
        return;
    }

    EMA_EVENT emaEvent;
    emaEvent.cbSize = sizeof(EMA_EVENT);

    emaEvent.eventType = EMA_EVENT_TYPE_RELAY_WRITE;

    emaEvent.relayInterval = ui->spinBox_RelayInterval->value();
    if (ui->radioButton_Abnormal->isChecked()) {
        emaEvent.relayValue = EMA_EVENT_VALUE_RELAY_ABNORMAL;
    }
    else if (ui->radioButton_Normal->isChecked()) {
        emaEvent.relayValue = EMA_EVENT_VALUE_RELAY_NORMAL;
    }
    else {
        ui->plainTextEdit_Log->appendPlainText("Invalid relay value!");
        return;
    }

    int ret = ema_writeEvent(m_emaHandle, &emaEvent);

    if (ret == 0) {
        ui->plainTextEdit_Log->appendPlainText("Write success");
    }
    else {
        ui->plainTextEdit_Log->appendPlainText(QString("Write error return= %1").arg(ret));
    }
    return;
}

void Dialog::usbPowerOff() {

    if (!m_isOpen) {
        ui->plainTextEdit_Log->appendPlainText("Please open device first!");
        return;
    }

    EMA_EVENT emaEvent;
    emaEvent.cbSize = sizeof(EMA_EVENT);

    emaEvent.eventType = EMA_EVENT_TYPE_USB_POWER_OFF;

    int ret = ema_writeEvent(m_emaHandle, &emaEvent);

    if (ret == 0) {
        ui->plainTextEdit_Log->appendPlainText("Write success");
    }
    else {
        ui->plainTextEdit_Log->appendPlainText(QString("Write error return= %1").arg(ret));
    }
    return;
}

void Dialog::usbPowerOn() {

    if (!m_isOpen) {
        ui->plainTextEdit_Log->appendPlainText("Please open device first!");
        return;
    }

    EMA_EVENT emaEvent;
    emaEvent.cbSize = sizeof(EMA_EVENT);

    emaEvent.eventType = EMA_EVENT_TYPE_USB_POWER_ON;

    int ret = ema_writeEvent(m_emaHandle, &emaEvent);

    if (ret == 0) {
        ui->plainTextEdit_Log->appendPlainText("Write success");
    }
    else {
        ui->plainTextEdit_Log->appendPlainText(QString("Write error return= %1").arg(ret));
    }

    return;
}



void Dialog::showReceivedData(EMA_EVENT *event) {

    int numOfBytes;
    QByteArray ba;

    qDebug() << "==== EVENT TYPE" << event->eventType;

    switch (event->eventType) {

    case EMA_EVENT_TYPE_CLOSE:
        ui->plainTextEdit_Log->appendPlainText("Close returned. Config initialized");
        m_curWiegandOutConfig.eventType = -1;
        m_curWiegandOutConfig.numOfBits = 0;
        m_curWiegandOutConfig.pulseWidth = 0;
        m_curWiegandOutConfig.pulseInterval = 0;
        ui->lineEdit_NumOfBitsRead->setText("");
        ui->lineEdit_PulseWidthRead->setText("");
        ui->lineEdit_PulseIntervalRead->setText("");
        ui->lineEdit_WiegandDataRead->setText("");
        break;

    case EMA_EVENT_TYPE_OPEN:
        ui->plainTextEdit_Log->appendPlainText("Open returned. Config initialized");
        m_curWiegandOutConfig.eventType = -1;
        m_curWiegandOutConfig.numOfBits = 0;
        m_curWiegandOutConfig.pulseWidth = 0;
        m_curWiegandOutConfig.pulseInterval = 0;
        ui->lineEdit_NumOfBitsRead->setText("");
        ui->lineEdit_PulseWidthRead->setText("");
        ui->lineEdit_PulseIntervalRead->setText("");
        ui->lineEdit_WiegandDataRead->setText("");
        break;

    case EMA_EVENT_TYPE_WIEGAND_READ_DATA:
        ui->plainTextEdit_Log->appendPlainText("Wiegand Received");
        ui->lineEdit_ChannelRead->setText(QString("%1").arg(event->wiegandOutChannel));
        ui->lineEdit_NumOfBitsRead->setText(QString("%1").arg(event->numOfBits));
        ui->lineEdit_PulseWidthRead->setText(QString("%1").arg(event->pulseWidth));
        ui->lineEdit_PulseIntervalRead->setText(QString("%1").arg(event->pulseInterval));

        if (event->numOfBits%8 == 0) numOfBytes = event->numOfBits/8;
        else numOfBytes = event->numOfBits/8 + 1;
        ba = QByteArray((const char *)event->wiegandData, numOfBytes);
        ui->lineEdit_WiegandDataRead->setText(QString(ba.toHex()));
        break;

    case EMA_EVENT_TYPE_WIEGAND_WRITE_DATA:
        ui->plainTextEdit_Log->appendPlainText("Wiegand Write returned");
        break;

    case EMA_EVENT_TYPE_WIEGAND_SET_CONFIG:
        ui->plainTextEdit_Log->appendPlainText("Wiegand Set Config returned");
        ui->lineEdit_ChannelRead->setText("");
        ui->lineEdit_NumOfBitsRead->setText("");
        ui->lineEdit_PulseWidthRead->setText("");
        ui->lineEdit_PulseIntervalRead->setText("");
        ui->lineEdit_WiegandDataRead->setText("");

        m_curWiegandOutConfig.wiegandOutChannel = event->wiegandOutChannel;
        m_curWiegandOutConfig.numOfBits = event->numOfBits;
        m_curWiegandOutConfig.pulseWidth = event->pulseWidth;
        m_curWiegandOutConfig.pulseInterval = event->pulseInterval;
        break;

    case EMA_EVENT_TYPE_WIEGAND_GET_CONFIG:
        ui->plainTextEdit_Log->appendPlainText("Wiegand Get Config returned");
        ui->lineEdit_ChannelRead->setText(QString("%1").arg(event->wiegandOutChannel));
        ui->lineEdit_NumOfBitsRead->setText(QString("%1").arg(event->numOfBits));
        ui->lineEdit_PulseWidthRead->setText(QString("%1").arg(event->pulseWidth));
        ui->lineEdit_PulseIntervalRead->setText(QString("%1").arg(event->pulseInterval));
        ui->lineEdit_WiegandDataRead->setText("");
        break;

    case EMA_EVENT_TYPE_WIEGAND_AUTO_DETECT:
        ui->plainTextEdit_Log->appendPlainText("Wiegand Auto Detect Received");
        ui->lineEdit_NumOfBitsRead->setText(QString("%1").arg(event->numOfBits));
        ui->lineEdit_PulseWidthRead->setText(QString("%1").arg(event->pulseWidth));
        ui->lineEdit_PulseIntervalRead->setText(QString("%1").arg(event->pulseInterval));

        if (event->numOfBits%8 == 0) numOfBytes = event->numOfBits/8;
        else numOfBytes = event->numOfBits/8 + 1;
        ba = QByteArray((const char *)event->wiegandData, numOfBytes);
        ui->lineEdit_WiegandDataRead->setText(QString(ba.toHex()));

        ui->lineEdit_NumOfBitsWrite->setText(QString("%1").arg(event->numOfBits));
        ui->lineEdit_PulseWidthWrite->setText(QString("%1").arg(event->pulseWidth));
        ui->lineEdit_PulseIntervalWrite->setText(QString("%1").arg(event->pulseInterval));
        ui->lineEdit_NumOfBitsRead->setText("");
        ui->lineEdit_PulseWidthRead->setText("");
        ui->lineEdit_PulseIntervalRead->setText("");
        ui->lineEdit_WiegandDataRead->setText("");
        break;

    case EMA_EVENT_TYPE_GPI12_READ:
        ui->plainTextEdit_Log->appendPlainText("GPI12 Read");
        if (event->gpi1Value == EMA_EVENT_VALUE_GPIO_HIGH)
            ui->lineEdit_GPI1->setText("High");
        else if (event->gpi1Value == EMA_EVENT_VALUE_GPIO_LOW)
            ui->lineEdit_GPI1->setText("Low");
        if (event->gpi2Value == EMA_EVENT_VALUE_GPIO_HIGH)
            ui->lineEdit_GPI2->setText("High");
        else if (event->gpi2Value == EMA_EVENT_VALUE_GPIO_LOW)
            ui->lineEdit_GPI2->setText("Low");
        break;

    case EMA_EVENT_TYPE_GPO1_WRITE:
        ui->plainTextEdit_Log->appendPlainText("GPO1 Write Return");
        break;

    case EMA_EVENT_TYPE_GPO2_WRITE:
        ui->plainTextEdit_Log->appendPlainText("GPO2 Write Return");
        break;

    case EMA_EVENT_TYPE_GPO12_WRITE:
        ui->plainTextEdit_Log->appendPlainText("GPO12 Write Return");
        break;

    case EMA_EVENT_TYPE_RELAY_WRITE:
        ui->plainTextEdit_Log->appendPlainText("Relay Write Return");
        break;

    }

    return;
}

QByteArray Dialog::sendWiegand(int id,int numOfBits)
{
    QString hv=QString::number(id/2,16);

    QByteArray ba =QByteArray::fromHex(hv.toAscii());
    int n ;
    if(numOfBits==26)
        n=3-ba.size();
    if(numOfBits==34)
        n=4-ba.size();
    for(int i=0;i<n;i++){
        ba.insert(0,'\0');
    }
    if(id%2==1)
        ba.append(255);
    return ba;
}




















