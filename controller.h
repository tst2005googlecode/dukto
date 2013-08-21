#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#if defined(Q_WS_S60)
#include <QNetworkSession>
#endif

#include <QTimer>

#include "buddylistitemmodel.h"
#include "qmlapplicationviewer.h"
#include "peer.h"
#include "duktoprotocol.h"
#include "destinationbuddy.h"
#include "ipaddressitemmodel.h"
#include "settings.h"

class Controller : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentTransferBuddy READ currentTransferBuddy NOTIFY currentTransferBuddyChanged)
    Q_PROPERTY(int currentTransferProgress READ currentTransferProgress NOTIFY currentTransferProgressChanged)
    Q_PROPERTY(QString currentTransferStats READ currentTransferStats NOTIFY currentTransferStatsChanged)
    Q_PROPERTY(bool currentTransferSending READ currentTransferSending NOTIFY currentTransferSendingChanged)
    Q_PROPERTY(QString currentPath READ currentPath NOTIFY currentPathChanged)
    Q_PROPERTY(QString buddyName READ buddyName WRITE setBuddyName NOTIFY buddyNameChanged)
    Q_PROPERTY(QString remoteDestinationAddress READ remoteDestinationAddress WRITE setRemoteDestinationAddress NOTIFY remoteDestinationAddressChanged)
    Q_PROPERTY(QString messagePageText READ messagePageText WRITE setMessagePageText NOTIFY messagePageTextChanged)
    Q_PROPERTY(QString messagePageTitle READ messagePageTitle WRITE setMessagePageTitle NOTIFY messagePageTitleChanged)
    Q_PROPERTY(QString messagePageBackState READ messagePageBackState WRITE setMessagePageBackState NOTIFY messagePageBackStateChanged)
    Q_PROPERTY(QString uiState READ uiState WRITE setUIState NOTIFY uiStateChanged)
    Q_PROPERTY(bool isNokiaE6 READ isNokiaE6)

private:
    QmlApplicationViewer* mView;
    Settings *mSettings;
    DestinationBuddy *mDestBuddy;
    QTimer *mPeriodicHelloTimer;
    QTimer *mShowBackTimer;
    BuddyListItemModel mBuddiesList;
    IpAddressItemModel mIpAddresses;
    DuktoProtocol mDuktoProtocol;

    QString mRemoteDestinationAddress;
    QString mCurrentTransferBuddy;
    QString mMessagePageText;
    QString mMessagePageTitle;
    QString mMessagePageBackState;
    bool mCurrentTransferSending;
    QString mCurrentTransferStats;
    int mCurrentTransferProgress;
    QString mUIState;
#if defined(Q_WS_S60)
    QNetworkSession *mNetworkSession;
#endif

    void startTransfer(QStringList files);
    bool prepareStartTransfer(QString *ip, qint16 *port);

public:
    explicit Controller(QmlApplicationViewer *mView= 0);
    virtual ~Controller();

    QString remoteDestinationAddress();
    void setRemoteDestinationAddress(QString address);
    QString messagePageTitle();
    void setMessagePageTitle(QString title);
    QString messagePageText();
    void setMessagePageText(QString message);
    QString messagePageBackState();
    void setMessagePageBackState(QString state);
    QString currentTransferBuddy();
    void setCurrentTransferBuddy(QString buddy);
    bool currentTransferSending();
    void setCurrentTransferSending(bool sending);
    int currentTransferProgress();
    void setCurrentTransferProgress(int value);
    QString currentTransferStats();
    void setCurrentTransferStats(QString stats);
    QString currentPath();
    void setCurrentPath(QString path);
    QString buddyName();
    void setBuddyName(QString name);
    QString uiState();
    void setUIState(QString state);
    bool isNokiaE6();

#if defined(Q_WS_S60)
    void initConnection();
#endif
    
signals:
    void currentTransferBuddyChanged();
    void currentTransferProgressChanged();
    void currentTransferStatsChanged();
    void currentTransferSendingChanged();
    void messagePageTextChanged();
    void messagePageTitleChanged();
    void messagePageBackStateChanged();
    void currentPathChanged();
    void buddyNameChanged();
    void remoteDestinationAddressChanged();
    void uiStateChanged();

    // Received by QML
    void transferStart();
    void receiveCompleted();
    void gotoSendPage();
    void gotoMessagePage();
    void hideAllOverlays();

#if defined(Q_WS_S60)
    void connectOpened();
    void connectError(QNetworkSession::SessionError error);
#endif
    
public slots:
    void showRandomBack();
    void periodicHello();
    void remoteDestinationAddressHandler();
    QString mediaSelectionDialog(QString mediaType);

    // Called by Dukto protocol
    void peerListAdded(Peer peer);
    void peerListRemoved(Peer peer);
    void receiveFileStart(QString senderIp);
    void transferStatusUpdate(qint64 total, qint64 partial);
    void receiveFileComplete(QStringList *files, qint64 totalSize);
    void receiveTextComplete(QString *text, qint64 totalSize);
    void sendFileComplete(QStringList *files);
    void sendFileError(int code);
    void receiveFileCancelled();
    void sendFileAborted();

    // Called by QML
    void sendSomeFiles(QString fileType);
    void showSendPage(QString ip);
    void sendFolder();
    void changeDestinationFolder();
    void abortTransfer();
    void refreshIpList();
};

#endif // CONTROLLER_H
