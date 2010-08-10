#include <QtGui/QApplication>
#include <QNetworkConfigurationManager>
#include <QNetworkSession>
#include <QMessageBox>
#include "mainwindow.h"
#include "duktoprotocol.h"

QTM_USE_NAMESPACE

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    DuktoProtocol p;
    w.setProtocolReference(&p);

    // Apertura sessione di rete
    QNetworkConfigurationManager cm;
    QNetworkSession s(cm.defaultConfiguration());
    s.open();
    if (!s.waitForOpened()) {
        QMessageBox::critical(0, "Dukto", "Can't open network connection.");
        return -1;
    }


    p.sayHello(QHostAddress::Broadcast);
#if defined(Q_WS_S60)
    w.showMaximized();
#else
    w.show();
#endif
    int ret = a.exec();
    p.sayGoodbye();
    return ret;
}
