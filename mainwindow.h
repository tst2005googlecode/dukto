#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QProgressDialog>
#include <QNetworkSession>

#include "duktoprotocol.h"
#include "dialogsendip.h"
#include "dialogtext.h"

QTM_USE_NAMESPACE

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setProtocolReference(DuktoProtocol *p);
    void startFileTransfer(QStringList files, QString dest);
    void startFileTransfer(QStringList files);
    void startTextTransfer(QString text, QString dest);
    void initConnection();

public slots:
    void refreshPeerList();
    void sendFileComplete(QStringList *files);
    void sendFileError(int e);
    void receiveFileStart();
    void receiveFileComplete(QStringList *files);
    void receiveFileCancelled();
    void transferStatusUpdate(int p);
    void showCurrentIP();
    void changeFolder();
    void sendToIp();
    void connectOpened();
    void connectError(QNetworkSession::SessionError error);
    void showSendTextDialog();
    void contextMenu_sendText(QString text);
    void receiveTextComplete(QString *text);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    DuktoProtocol *mProtocol;
    QProgressDialog *mProgressDialog;
    QProgressDialog *mConnectingDialog;
    DialogSendIp *mDialogSendIp;
    DialogText *mDialogText;
    QAction *mCurrentIPAction;
    QAction *mChangeFolderAction;
    QAction *mSendToIPAction;
    QAction *mSendTextAction;
    QNetworkSession *mNetworkSession;



private slots:
    void on_listWidget_itemDoubleClicked(QListWidgetItem* item);
};

#endif // MAINWINDOW_H
