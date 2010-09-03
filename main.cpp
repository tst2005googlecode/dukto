#include <QtGui/QApplication>
#include <QMessageBox>
#include "mainwindow.h"
#include "duktoprotocol.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    DuktoProtocol p;
    w.setProtocolReference(&p);

#if defined(Q_WS_S60)
    w.showMaximized();
#else
    w.show();
#endif
    int ret = a.exec();
    p.sayGoodbye();
    return ret;
}
