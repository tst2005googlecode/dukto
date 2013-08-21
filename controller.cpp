#include "controller.h"
#include <QDeclarativeContext>
#include <QDir>
#include <QFileDialog>
#include <QApplication>
#include <QRegExp>
#ifdef Q_WS_S60
#include <mgfetch.h>
#endif

#if defined(Q_WS_S60)
#define SYMBIAN
#endif

#if defined(Q_WS_SIMULATOR)
#define SYMBIAN
#endif

#ifdef SYMBIAN
#include <QNetworkConfigurationManager>
#include <QNetworkConfiguration>
#include <QMessageBox>
#endif

#define NETWORK_PORT 4644 // 6742

Controller::Controller(QmlApplicationViewer *view) :
    QObject(view), mView(view), mPeriodicHelloTimer(NULL), mShowBackTimer(NULL)
{
    // Status variables
    setCurrentTransferProgress(0);

    // Add "Me" entry
    mBuddiesList.addMeElement();

    // Add "Ip" entry
    mBuddiesList.addIpElement();

    // Settings
    mSettings = new Settings(this);

    // Destination buddy
    mDestBuddy = new DestinationBuddy(this);

    // Change current folder
    QDir::setCurrent(mSettings->currentPath());

    // Init buddy list
    view->rootContext()->setContextProperty("buddiesListData", &mBuddiesList);
    view->rootContext()->setContextProperty("ipAddressesData", &mIpAddresses);
    view->rootContext()->setContextProperty("controller", this);
    view->rootContext()->setContextProperty("destinationBuddy", mDestBuddy);

    // Register protocol signals
    connect(&mDuktoProtocol, SIGNAL(peerListAdded(Peer)), this, SLOT(peerListAdded(Peer)));
    connect(&mDuktoProtocol, SIGNAL(peerListRemoved(Peer)), this, SLOT(peerListRemoved(Peer)));
    connect(&mDuktoProtocol, SIGNAL(receiveFileStart(QString)), this, SLOT(receiveFileStart(QString)));
    connect(&mDuktoProtocol, SIGNAL(transferStatusUpdate(qint64,qint64)), this, SLOT(transferStatusUpdate(qint64,qint64)));
    connect(&mDuktoProtocol, SIGNAL(receiveFileComplete(QStringList*,qint64)), this, SLOT(receiveFileComplete(QStringList*,qint64)));
    connect(&mDuktoProtocol, SIGNAL(receiveTextComplete(QString*,qint64)), this, SLOT(receiveTextComplete(QString*,qint64)));
    connect(&mDuktoProtocol, SIGNAL(sendFileComplete(QStringList*)), this, SLOT(sendFileComplete(QStringList*)));
    connect(&mDuktoProtocol, SIGNAL(sendFileError(int)), this, SLOT(sendFileError(int)));
    connect(&mDuktoProtocol, SIGNAL(receiveFileCancelled()), this, SLOT(receiveFileCancelled()));
    connect(&mDuktoProtocol, SIGNAL(sendFileAborted()), this, SLOT(sendFileAborted()));

    // Register other signals
    connect(this, SIGNAL(remoteDestinationAddressChanged()), this, SLOT(remoteDestinationAddressHandler()));

    // Say "hello"
    mDuktoProtocol.setPorts(NETWORK_PORT, NETWORK_PORT);
    mDuktoProtocol.initialize();
    mDuktoProtocol.sayHello(QHostAddress::Broadcast);

    // Periodic "hello" timer
    mPeriodicHelloTimer = new QTimer(this);
    connect(mPeriodicHelloTimer, SIGNAL(timeout()), this, SLOT(periodicHello()));
    mPeriodicHelloTimer->start(60000);

    // Start random rotate
    mShowBackTimer = new QTimer(this);
    connect(mShowBackTimer, SIGNAL(timeout()), this, SLOT(showRandomBack()));
    qsrand(QDateTime::currentDateTime().toTime_t());;
    mShowBackTimer->start(10000);
}

Controller::~Controller()
{
    mDuktoProtocol.sayGoodbye();

    if (mShowBackTimer) mShowBackTimer->deleteLater();
    if (mPeriodicHelloTimer) mPeriodicHelloTimer->deleteLater();
    if (mView) mView->deleteLater();
    if (mSettings) mSettings->deleteLater();
    if (mDestBuddy) mDestBuddy->deleteLater();
}

// Add the new buddy to the buddy list
void Controller::peerListAdded(Peer peer)
{
    mBuddiesList.addBuddy(peer);
}

// Remove the buddy from the buddy list
void Controller::peerListRemoved(Peer peer) {

    // Check if currently is shown the "send" page for that buddy
    if ((uiState() == "send")
            && (mDestBuddy->ip() == peer.address.toString()))
        emit hideAllOverlays();

    // Check if currently is shown the "transfer complete" message box
    // for the removed user as destination
    if ((uiState() == "message") && (messagePageBackState() == "send")
            && (mDestBuddy->ip() == peer.address.toString()))
        setMessagePageBackState("");

    // Remove from the list
    mBuddiesList.removeBuddy(peer.address.toString());
}

// Periodic hello sending
void Controller::periodicHello()
{
    mDuktoProtocol.sayHello(QHostAddress::Broadcast);
}

void Controller::showSendPage(QString ip)
{
    // Check for a buddy with the provided IP address
    QStandardItem *buddy = mBuddiesList.buddyByIp(ip);
    if (buddy == NULL) return;

    // Update exposed data for the selected user
    mDestBuddy->fillFromItem(buddy);

    // Preventive update of destination buddy
    if (mDestBuddy->ip() == "IP")
        setCurrentTransferBuddy(remoteDestinationAddress());
    else
        setCurrentTransferBuddy(mDestBuddy->username());

    // Show send UI
    emit gotoSendPage();
}

void Controller::sendSomeFiles(QString fileType)
{

    // Show file selection dialog
    //    QStringList files = QFileDialog::getOpenFileNames(mView, tr("Send some files"));
    //    if (files.count() == 0) return;

    QString selected;
    // Show image selection dialog
    selected = mediaSelectionDialog(fileType);
    if(selected == "") return;
    // Send files
    QStringList toSend = selected.split("\n", QString::SkipEmptyParts);
    startTransfer(toSend);
}

// Abort current transfer while sending data
void Controller::abortTransfer()
{
    mDuktoProtocol.abortCurrentTransfer();
}

void Controller::startTransfer(QStringList files)
{
    // Prepare file transfer
    QString ip;
    qint16 port;
    if (!prepareStartTransfer(&ip, &port)) return;

    // Start files transfer
    mDuktoProtocol.sendFile(ip, port, files);
}

bool Controller::prepareStartTransfer(QString *ip, qint16 *port)
{
    // Check if it's a remote file transfer
    if (mDestBuddy->ip() == "IP") {

        // Remote transfer
        QString dest = remoteDestinationAddress();

        // Check if port is specified
        if (dest.contains(":")) {

            // Port is specified or destination is malformed...
            QRegExp rx("^(.*):([0-9]+)$");
            if (rx.indexIn(dest) == -1) {

                // Malformed destination
                setMessagePageTitle(tr("Send"));
                setMessagePageText(tr("Hey, take a look at your destination, it appears to be malformed!"));
                setMessagePageBackState("send");
                emit gotoMessagePage();
                return false;
            }

            // Get IP (or hostname) and port
            QStringList capt = rx.capturedTexts();
            *ip = capt[1];
            *port = capt[2].toInt();
        }
        else {

            // Port not specified, using default
            *ip = dest;
            *port = 0;
        }
        setCurrentTransferBuddy(*ip);
    }
    else {

        // Local transfer
        *ip = mDestBuddy->ip();
        *port = mDestBuddy->port();
        setCurrentTransferBuddy(mDestBuddy->username());
    }

    // Update GUI for file transfer
    setCurrentTransferSending(true);
    setCurrentTransferStats(tr("Connecting..."));
    setCurrentTransferProgress(0);

    emit transferStart();
    return true;
}

QString Controller::remoteDestinationAddress()
{
    return mRemoteDestinationAddress;
}

void Controller::setRemoteDestinationAddress(QString address)
{
    if (address == mRemoteDestinationAddress) return;
    mRemoteDestinationAddress = address;
    emit remoteDestinationAddressChanged();
}

QString Controller::messagePageTitle()
{
    return mMessagePageTitle;
}

void Controller::setMessagePageTitle(QString title)
{
    if (title == mMessagePageTitle) return;
    mMessagePageTitle = title;
    emit messagePageTitleChanged();
}

QString Controller::messagePageText()
{
    return mMessagePageText;
}

void Controller::setMessagePageText(QString message)
{
    if (message == mMessagePageText) return;
    mMessagePageText = message;
    emit messagePageTextChanged();
}

QString Controller::messagePageBackState()
{
    return mMessagePageBackState;
}

void Controller::setMessagePageBackState(QString state)
{
    if (state == mMessagePageBackState) return;
    mMessagePageBackState = state;
    emit messagePageBackStateChanged();
}

QString Controller::currentTransferBuddy()
{
    return mCurrentTransferBuddy;
}

void Controller::setCurrentTransferBuddy(QString buddy)
{
    if (buddy == mCurrentTransferBuddy) return;
    mCurrentTransferBuddy = buddy;
    emit currentTransferBuddyChanged();
}

bool Controller::currentTransferSending()
{
    return mCurrentTransferSending;
}

void Controller::setCurrentTransferSending(bool sending)
{
    if (sending == mCurrentTransferSending) return;
    mCurrentTransferSending = sending;
    emit currentTransferSendingChanged();
}

QString Controller::currentTransferStats()
{
    return mCurrentTransferStats;
}

void Controller::setCurrentTransferStats(QString stats)
{
    if (stats == mCurrentTransferStats) return;
    mCurrentTransferStats = stats;
    emit currentTransferStatsChanged();
}

int Controller::currentTransferProgress()
{
    return mCurrentTransferProgress;
}

void Controller::setCurrentTransferProgress(int value)
{
    if (value == mCurrentTransferProgress) return;
    mCurrentTransferProgress = value;
    emit currentTransferProgressChanged();
}

void Controller::transferStatusUpdate(qint64 total, qint64 partial)
{
    // Stats formatting
    if (total < 1024)
        setCurrentTransferStats(QString::number(partial) + tr(" B of ") + QString::number(total) + " B");
    else if (total < 1048576)
        setCurrentTransferStats(QString::number(partial * 1.0 / 1024, 'f', 1) + tr(" KB of ") + QString::number(total * 1.0 / 1024, 'f', 1) + " KB");
    else
        setCurrentTransferStats(QString::number(partial * 1.0 / 1048576, 'f', 1) + tr(" MB of ") + QString::number(total * 1.0 / 1048576, 'f', 1) + " MB");

    double percent = partial * 1.0 / total * 100;
    setCurrentTransferProgress(percent);

}

void Controller::sendFolder()
{
    // Show folder selection dialog
    QString dirname = QFileDialog::getExistingDirectory(mView, tr("Change folder"), ".",
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dirname == "") return;

    // Send files
    QStringList toSend;
    toSend.append(dirname);
    startTransfer(toSend);
}

void Controller::sendFileComplete(QStringList *files)
{
    // To remove warning
    files = files;

    // Show completed message
    setMessagePageTitle(tr("Send"));
    setMessagePageText(tr("Your data has been sent to your buddy!"));
    setMessagePageBackState("send");

    emit gotoMessagePage();
}

// Handles send error
void Controller::sendFileError(int code)
{
    setMessagePageTitle(tr("Error"));
    setMessagePageText(tr("Sorry, an error has occurred while sending your data...\n\nError code: ") + QString::number(code));
    setMessagePageBackState("send");

    emit gotoMessagePage();
}

// Protocol confirms that abort has been done
void Controller::sendFileAborted()
{
    emit gotoSendPage();
}

void Controller::receiveFileStart(QString senderIp)
{
    // Look for the sender in the buddy list
    QString sender = mBuddiesList.buddyNameByIp(senderIp);
    if (sender == "")
        setCurrentTransferBuddy("remote sender");
    else
        setCurrentTransferBuddy(sender);

    // Update user interface
    setCurrentTransferSending(false);

    emit transferStart();
}

void Controller::receiveFileComplete(QStringList *files, qint64 totalSize) {
    // Update GUI
    QApplication::alert(mView, 5000);
    emit receiveCompleted();
}

void Controller::receiveTextComplete(QString *text, qint64 totalSize)
{
    setMessagePageTitle(tr("Error"));
    setMessagePageText(tr("Unsupported feature."));
    setMessagePageBackState("");
    emit gotoMessagePage();
}

// Handles receive error
void Controller::receiveFileCancelled()
{
    setMessagePageTitle(tr("Error"));
    setMessagePageText(tr("Sender has cancelled sending... The data you received could be incomplete or broken."));
    setMessagePageBackState("");
    emit gotoMessagePage();
}

void Controller::showRandomBack()
{
    // Look for a random element
    int i = (qrand() * 1.0 / RAND_MAX) * (mBuddiesList.rowCount() + 1);

    // Show back
    if (i < mBuddiesList.rowCount()) mBuddiesList.showSingleBack(i);
}

void Controller::changeDestinationFolder()
{
    // Show system dialog for folder selection
    QString dirname = QFileDialog::getExistingDirectory(mView, tr("Change folder"), ".",
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dirname == "") return;

#ifdef Q_OS_SYMBIAN//SYMBIAN
    // Disable saving on C:
    if (dirname.toUpper().startsWith("C:")) {

        setMessagePageTitle(tr("Destination"));
        setMessagePageText(tr("Receiving data on C: is disabled for security reasons. Please select another destination folder."));
        setMessagePageBackState("settings");
        emit gotoMessagePage();
        return;
    }
#endif

    // Set the new folder as current
    QDir::setCurrent(dirname);
    if (!QDir::current().exists("Tian Media Transfer")) {
        QDir::current().mkdir("Tian Media Transfer");
    }
    QDir::setCurrent("Tian Media Transfer");

    // Save the new setting
    setCurrentPath(QDir::current().absolutePath());
}

QString Controller::currentPath()
{
    return mSettings->currentPath();
}

void Controller::setCurrentPath(QString path)
{
    if (path == mSettings->currentPath()) return;
    mSettings->savePath(path);
    emit currentPathChanged();
}

void Controller::setBuddyName(QString name)
{
    mSettings->saveBuddyName(name/*.replace(' ', "")*/);
    mDuktoProtocol.updateBuddyName();
    mBuddiesList.updateMeElement();
    emit buddyNameChanged();
}

QString Controller::buddyName()
{
    return mSettings->buddyName();
}

QString Controller::uiState()
{
    return mUIState;
}

void Controller::setUIState(QString state)
{
    if (state == mUIState) return;
    mUIState = state;
    emit uiStateChanged();
}

bool Controller::isNokiaE6()
{
    bool value;
    QStringList tmp = buddyName().toLower().split(" ", QString::SkipEmptyParts);
    value = tmp.contains("e6");
    return value;
}

void Controller::refreshIpList()
{
    mIpAddresses.refreshIpList();
}

#if defined(Q_WS_S60)
void Controller::initConnection()
{
    // Connection
    QNetworkConfigurationManager manager;
    const bool canStartIAP = (manager.capabilities() & QNetworkConfigurationManager::CanStartAndStopInterfaces);
    QNetworkConfiguration cfg = manager.defaultConfiguration();
    if (!cfg.isValid() || (!canStartIAP && cfg.state() != QNetworkConfiguration::Active)) return;
    mNetworkSession = new QNetworkSession(cfg, this);
    connect(mNetworkSession, SIGNAL(opened()), this, SLOT(connectOpened()));
    connect(mNetworkSession, SIGNAL(error(QNetworkSession::SessionError)), this, SLOT(connectError(QNetworkSession::SessionError)));
    mNetworkSession->open();
}

void Controller::connectOpened()
{
    mDuktoProtocol.sayHello(QHostAddress::Broadcast);
}

void Controller::connectError(QNetworkSession::SessionError error)
{
    QString msg = tr("Unable to connect to the network (code ") + QString::number(error) + ").";
    QMessageBox::critical(NULL, "Tian Media Transfer", msg);
    exit(-1);
}

#endif

void Controller::remoteDestinationAddressHandler()
{
    // Update GUI status
    setCurrentTransferBuddy(remoteDestinationAddress());
}

QString Controller::mediaSelectionDialog(QString mediaType)
{
    QString selected;

#ifdef Q_WS_S60
    CDesCArrayFlat* fileArray = new CDesCArrayFlat( 3 );
    TBool selectionOk = EFalse;
    TMediaFileType type;
    if(mediaType == "image")
        type = EImageFile;
    else if(mediaType == "video")
        type = EVideoFile;
    else if(mediaType == "audio")
        type = EAudioFile;
    else if(mediaType == "music")
        type = EMusicFile;

    if( !fileArray )
        return selected;

    TRAP_IGNORE(
                selectionOk = MGFetch::RunL(
                *fileArray,
                type,
                ETrue,
                KNullDesC,
                KNullDesC
                );
            );

    if( selectionOk ) {
        for(int i = 0; i < fileArray->Count(); i++) {
            selected += QString::fromUtf16(fileArray->MdcaPoint(i).Ptr(),
                                           fileArray->MdcaPoint(i).Length()) + "\n";
        }
    } else {
        // selection dialog was cancelled
        selected = "";
    }

    delete fileArray;
#endif // Q_WS_S60
    return selected;
}
