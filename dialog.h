#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "emaglobal.h"
#include "emalib.h"

class EventThread;

namespace Ui {
    class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void open();
    void close();
    void wiegandSetConfig();
    void wiegandGetConfig();
    void wiegandAutoDetect();
    void writeWiegand();

    void writeGPO1();
    void writeGPO2();
    void writeGPO12();
    void writeRelay();

    void usbPowerOn();
    void usbPowerOff();

    void showReceivedData(EMA_EVENT *event);

    QByteArray sendWiegand(int id,int numOfBits);
private:
    Ui::Dialog *ui;

    EventThread *m_eventThread;
    EMA_HANDLE m_emaHandle;
    bool m_isOpen;
    EMA_EVENT m_curWiegandOutConfig;
};

#endif // DIALOG_H
