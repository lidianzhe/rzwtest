#ifndef EVENTTHREAD_H
#define EVENTTHREAD_H

#include "emaglobal.h"
#include "emalib.h"

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

class EventThread : public QThread
{
	Q_OBJECT

public:
    explicit EventThread(QObject *parent, EMA_HANDLE cmiHandle);
	~EventThread();

	void abort();

signals:
    void dataReceived(EMA_EVENT *event);

protected:
    void run();

private:
    EMA_HANDLE m_emaHandle;
	bool m_abort;
    EMA_EVENT *m_event;
};

#endif // EVENTTHREAD_H
