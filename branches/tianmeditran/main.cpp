#include <QtGui/QApplication>
#include "qmlapplicationviewer.h"
#include "controller.h"
#include <QLocale>
#include <QTranslator>

#if defined(Q_WS_S60)
#define SYMBIAN
#endif

#if defined(Q_WS_SIMULATOR)
#define SYMBIAN
#endif

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));
    app->setOrganizationName("Vietsoft");
    app->setOrganizationDomain("tianpham.blogspot.com");
    app->setApplicationName("Tian Media Transfer");    // add app ver to app
#if defined(Q_OS_SYMBIAN)
    app->setApplicationVersion(APP_VERSION);
#else
    app->setApplicationVersion("1");
#endif

    // Get locale of the system
    QString location = QLocale::system().name();
    // Load the correct translation file (if available)
    QTranslator translator;
    if(location.contains("vi")) {
        translator.load("lang_vi", ":/languages");
        // Adds the loaded file to list of active translation files
        app->installTranslator(&translator);
    }

    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationLockPortrait);
    viewer.setMainQmlFile(QLatin1String("qml/TianMediTrans/main.qml"));
    viewer.showExpanded();

    Controller controller(&viewer);
#ifdef Q_WS_S60
    controller.initConnection();
#endif

    return app->exec();
}
