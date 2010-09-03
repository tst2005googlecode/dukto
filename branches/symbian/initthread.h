#ifndef INITTHREAD_H
#define INITTHREAD_H

#include <QThread>
#include "duktoprotocol.h"

class InitThread : public QThread
{
    Q_OBJECT
public:
    explicit InitThread();
    void setDuktoProtocol(DuktoProtocol *p);
    bool isConnected();

protected:
    virtual void run();

signals:

public slots:

private:
    DuktoProtocol *mProtocol;
    bool connected;

};

#endif // INITTHREAD_H
