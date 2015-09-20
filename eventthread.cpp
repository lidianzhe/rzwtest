#include "eventthread.h"
#include <QDebug>

EventThread::EventThread(QObject *parent, EMA_HANDLE emaHandle) :
		QThread(parent)
{
     m_emaHandle = emaHandle;
	 m_abort = false;
     m_event = NULL;
}

EventThread::~EventThread()
{
	if (isRunning()) {
		abort();
		wait();
	}
}

void EventThread::abort()
{
	m_abort = true;
    ema_clearEventQueue(m_emaHandle);
    ema_cancelPendingReadEvent(m_emaHandle);
}

void EventThread::run() {
    if (!m_emaHandle) return;

	m_abort = false;
    int ret;
    if (m_event) delete m_event;
    m_event = NULL;

    m_event = new EMA_EVENT;
    m_event->cbSize = sizeof(EMA_EVENT);

	while (1) { 
		if (m_abort) {
			qDebug() << "EventThread run() aborted";
            if (m_event) delete m_event;
			return;
		}

        ret = ema_readEvent(m_emaHandle, m_event, 10);

        if (ret == EMA_ERROR_INVALID_HANDLE) {
            if (m_event) delete m_event;
            return;
        }
        else if (ret == EMA_ERROR_READ_EVENT_CANCELLED) continue;
        else if (ret == EMA_ERROR_WAIT_TIMEOUT) continue;
        else if (ret == EMA_SUCCESS) {
            emit dataReceived(m_event);
            m_event = new EMA_EVENT;
            m_event->cbSize = sizeof(EMA_EVENT);
		}
	}
}
