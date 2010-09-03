#include "initthread.h"

#include <QNetworkConfigurationManager>
#include <QNetworkSession>

QTM_USE_NAMESPACE

InitThread::InitThread() :
    QThread(NULL)
{
    connected = false;
}

void InitThread::setDuktoProtocol(DuktoProtocol *p)
{
    mProtocol = p;
}

bool InitThread::isConnected()
{
    return connected;
}

void InitThread::run()
{
    // Network connection request
    QNetworkConfigurationManager cm;
    QNetworkSession s(cm.defaultConfiguration());
    s.open();
    if (!s.waitForOpened()) return;
    connected = true;

    // Sending broadcast hello
    mProtocol->sayHello(QHostAddress::Broadcast);
}
