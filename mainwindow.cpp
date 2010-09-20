#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include <QHash>
#include <QNetworkInterface>
#include <QAbstractSocket>
#include <QNetworkConfigurationManager>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "listwidgetpeeritem.h"
#include "peer.h"
#include "buddywidget.h"
#include "dialogsendip.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Creazione finestra
    ui->setupUi(this);
    mDialogSendIp = NULL;
    mNetworkSession = NULL;

    // Creazione menu'
    mCurrentIPAction = new QAction("Current IP address", this);
    menuBar()->addAction(mCurrentIPAction);
    connect(mCurrentIPAction, SIGNAL(triggered()), this, SLOT(showCurrentIP()));
    mChangeFolderAction = new QAction("Change folder", this);
    menuBar()->addAction(mChangeFolderAction);
    connect(mChangeFolderAction, SIGNAL(triggered()), this, SLOT(changeFolder()));
    mSendToIPAction = new QAction("Send by IP", this);
    menuBar()->addAction(mSendToIPAction);
    connect(mSendToIPAction, SIGNAL(triggered()), this, SLOT(sendToIp()));

    // Progress dialog per operazioni
    mProgressDialog = new QProgressDialog("", "", 0, 100);
    mProgressDialog->setWindowModality(Qt::WindowModal);

    // Connecting dialog
    mConnectingDialog = new QProgressDialog("\nConnecting...", "", 0, 0);
    mConnectingDialog->setCancelButton(NULL);
    mConnectingDialog->setAutoClose(false);
    mConnectingDialog->setMinimumDuration(0);
    mConnectingDialog->setWindowModality(Qt::WindowModal);

    // Percorso di default
    QDir d;
    d.mkpath("E:\\Dukto\\");
    QDir::setCurrent("E:\\Dukto\\");
    ui->labelDest->setText("Folder: E:\\Dukto");

}

MainWindow::~MainWindow()
{
    if (mNetworkSession) {
        mNetworkSession->close();
        delete mNetworkSession;
    }
    if (mProgressDialog) delete mProgressDialog;
    if (mConnectingDialog) delete mConnectingDialog;
    if (ui) delete ui;
}

void MainWindow::initConnection()
{
    // Show connecting dialog
    mConnectingDialog->show();

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

void MainWindow::connectOpened()
{
    // Sending broadcast hello
    mProtocol->initSockets();
    connect(mProtocol, SIGNAL(peerListChanged()), this, SLOT(refreshPeerList()));
    connect(mProtocol, SIGNAL(sendFileComplete(QStringList*)), this, SLOT(sendFileComplete(QStringList*)));
    connect(mProtocol, SIGNAL(sendFileError(int)), this, SLOT(sendFileError(int)));
    connect(mProtocol, SIGNAL(receiveFileStart()), this, SLOT(receiveFileStart()));
    connect(mProtocol, SIGNAL(receiveFileComplete(QStringList*)), this, SLOT(receiveFileComplete(QStringList*)));
    connect(mProtocol, SIGNAL(receiveFileCancelled()), this, SLOT(receiveFileCancelled()));
    connect(mProtocol, SIGNAL(transferStatusUpdate(int)), this, SLOT(transferStatusUpdate(int)), Qt::DirectConnection);
    mProtocol->sayHello(QHostAddress::Broadcast);

    // Hide connecting dialog
    mConnectingDialog->close();
    delete mConnectingDialog;
    mConnectingDialog = NULL;
}

void MainWindow::connectError(QNetworkSession::SessionError error)
{
    QMessageBox::critical(0, "Dukto", "Error: " + mNetworkSession->errorString());
    exit(-1);
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::setProtocolReference(DuktoProtocol *p)
{
    mProtocol = p;
}

// Aggiornamento lista dei client
void MainWindow::refreshPeerList()
{
    QHash<QString, Peer>& peers = mProtocol->getPeers();
    QHashIterator<QString, Peer> i(peers);

    // Salvataggio elemento corrente
    QString current;
    if (ui->listWidget->selectedItems().count() > 0)
    {
        ListWidgetPeerItem *i = static_cast<ListWidgetPeerItem*>(ui->listWidget->selectedItems().at(0));
        current = i->getPeerKey();
    }

    // Cancellazione lista corrente
    ui->listWidget->clear();

    // Riempimento lista
    while (i.hasNext())
    {
        i.next();
        BuddyWidget *bw = new BuddyWidget(ui->listWidget);
        bw->setFromIdentifier(i.value().name);
        ListWidgetPeerItem *item = new ListWidgetPeerItem(ui->listWidget, i.key(), 80);
        ui->listWidget->setItemWidget(item, bw);
        if (i.key() == current) ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);
    }

    // Selezione primo elemento
    if ((ui->listWidget->count() > 0) && (ui->listWidget->selectedItems().count() == 0))
        ui->listWidget->setCurrentRow(0);
}

void MainWindow::receiveFileStart()
{
    ui->listWidget->setEnabled(false);
    mProgressDialog->show();
}

void MainWindow::sendFileComplete(QStringList *files)
{
    ui->listWidget->setEnabled(true);
    mProgressDialog->close();

    if (files->count() == 1)
    {
        QFileInfo fi(files->at(0));
        QMessageBox::information(this, "Send file", "File '" + fi.fileName() + "' sent.");
    }
    else
        QMessageBox::information(this, "Send files", "Multiple files and folders sent.");
}

void MainWindow::receiveFileComplete(QStringList *files)
{
    ui->listWidget->setEnabled(true);
    mProgressDialog->close();

    if (files->count() == 1)
        QMessageBox::information(this, "Receive files", "File '" + files->at(0) +  "' received.");
    else
        QMessageBox::information(this, "Receive files", "Multiple files and folders received.");
}

void MainWindow::receiveFileCancelled()
{
    ui->listWidget->setEnabled(true);
    mProgressDialog->close();
    QMessageBox::critical(this, "Receive files", "Transfer cancelled.");
}

void MainWindow::sendFileError(int e)
{
    QMessageBox::critical(this, "Send file", "An error has occurred while sending the file (" + QString::number(e, 10) + ").");
    ui->listWidget->setEnabled(true);
    mProgressDialog->close();
}

void MainWindow::transferStatusUpdate(int p)
{
    mProgressDialog->setValue(p);
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem* item)
{
    QStringList files = QFileDialog::getOpenFileNames(this, "Select file to send", "", "Any file (*.*)");
    if (files.count() == 0) return;
    startFileTransfer(files);
}

void MainWindow::startFileTransfer(QStringList files, QString dest)
{
    ui->listWidget->setEnabled(false);
    mProgressDialog->show();
    mProtocol->sendFile(dest, files);
}

void MainWindow::startFileTransfer(QStringList files)
{
    QString dest;
    if (ui->listWidget->selectedItems().count() != 1) {
        QMessageBox::critical(this, "Error", "No peer selected as target.");
        return;
    }
    ListWidgetPeerItem *i = static_cast<ListWidgetPeerItem*>(ui->listWidget->selectedItems().at(0));
    dest = i->getPeerKey();
    startFileTransfer(files, dest);
}

void MainWindow::showCurrentIP()
{
    int count = 0;
    QString addrs = "";
    QList<QHostAddress> ips = QNetworkInterface::allAddresses();
    QListIterator<QHostAddress> i(ips);
    while (i.hasNext())
    {
        QHostAddress addr = i.next();
        if ((addr.protocol() == 0) && (addr.toString() != "127.0.0.1"))
        {
            count++;
            addrs += addr.toString() + ", ";
        }
    }
    if (count == 0)
        QMessageBox::critical(0, "", "Currently you don't have any IP address assigned");
    else {
        addrs = addrs.left(addrs.length() - 2);
        if (count == 1)
            QMessageBox::information(0, "", "Your current IP address is " + addrs);
        else
            QMessageBox::information(0, "", "Your current IP addresses are " + addrs);
    }
}

void MainWindow::changeFolder()
{
    QString dirname = QFileDialog::getExistingDirectory(this, "Select the folder where received files will be saved:", ".",
    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dirname == "") return;
    QDir::setCurrent(dirname);
    ui->labelDest->setText("Folder: " + QDir::currentPath().replace('/', "\\"));
}

void MainWindow::sendToIp()
{
    if (mDialogSendIp) delete mDialogSendIp;
    mDialogSendIp = new DialogSendIp(this);
    mDialogSendIp->showMaximized();
}
