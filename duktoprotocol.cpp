/* DUKTO - A simple, fast and multi-platform file transfer tool for LAN users
 * Copyright (C) 2011 Emanuele Colombo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "duktoprotocol.h"

#if defined(Q_WS_WIN)
    #include <windows.h>
#endif

#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QNetworkInterface>
#include <QTimer>

#include "platform.h"

#define DEFAULT_UDP_PORT 4644
#define DEFAULT_TCP_PORT 4644

DuktoProtocol::DuktoProtocol()
    : mSocket(NULL), mTcpServer(NULL), mCurrentSocket(NULL),
        mCurrentFile(NULL), mFilesToSend(NULL)
{
    mLocalUdpPort = DEFAULT_UDP_PORT;
    mLocalTcpPort = DEFAULT_TCP_PORT;

    mIsSending = false;
    mIsReceiving = false;
    mSendingScreen = false;
}

DuktoProtocol::~DuktoProtocol()
{
    if (mCurrentSocket) delete mCurrentSocket;
    if (mSocket) delete mSocket;
    if (mTcpServer) delete mTcpServer;
    if (mCurrentFile) delete mCurrentFile;
}

void DuktoProtocol::initialize()
{
    mSocket = new QUdpSocket(this);
    mSocket->bind(QHostAddress::Any, mLocalUdpPort);
    connect(mSocket, SIGNAL(readyRead()), this, SLOT(newUdpData()));

    mTcpServer = new QTcpServer(this);
    mTcpServer->listen(QHostAddress::Any, mLocalTcpPort);
    connect(mTcpServer, SIGNAL(newConnection()), this, SLOT(newIncomingConnection()));
}

void DuktoProtocol::setPorts(qint16 udp, qint16 tcp)
{
    mLocalUdpPort = udp;
    mLocalTcpPort = tcp;
}

QString DuktoProtocol::getSystemSignature()
{
    static QString signature = "";
    if (signature != "") return signature;

    signature = Platform::getSystemUsername()
              + " at " + Platform::getHostname()
              + " (" + Platform::getPlatformName() + ")";
    return signature;
}

void DuktoProtocol::sayHello(QHostAddress dest)
{
    sayHello(dest, mLocalUdpPort);
}

void DuktoProtocol::sayHello(QHostAddress dest, qint16 port)
{
    // preparation package
    QByteArray *packet = new QByteArray();
    if ((port == DEFAULT_UDP_PORT) && (mLocalUdpPort == DEFAULT_UDP_PORT))
    {
        if (dest == QHostAddress::Broadcast)
            packet->append(0x01);           // 0x01 -> HELLO MESSAGE (broadcast)
        else
            packet->append(0x02);           // 0x02 -> HELLO MESSAGE (unicast)
    }
    else
    {
        if (dest == QHostAddress::Broadcast)
            packet->append(0x04);           // 0x04 -> HELLO MESSAGE (broadcast) with PORT
        else
            packet->append(0x05);           // 0x05 -> HELLO MESSAGE (unicast) with PORT
        packet->append((char*)&mLocalUdpPort, sizeof(qint16));
    }
    packet->append(getSystemSignature());

    // Invio pacchetto
    if (dest == QHostAddress::Broadcast) {
        sendToAllBroadcast(packet, port);
        if (port != DEFAULT_UDP_PORT) sendToAllBroadcast(packet, DEFAULT_UDP_PORT);
    }
    else
        mSocket->writeDatagram(packet->data(), packet->length(), dest, port);

    delete packet;
}

void DuktoProtocol::sayGoodbye()
{
    // Create packet
    QByteArray *packet = new QByteArray();
    packet->append(0x03);               // 0x03 -> GOODBYE MESSAGE
    packet->append("Bye Bye");

    // Look for all the discovered ports
    QList<qint16> ports;
    ports.append(mLocalUdpPort);
    if (mLocalUdpPort != DEFAULT_UDP_PORT) ports.append(DEFAULT_UDP_PORT);
    foreach (const Peer &p, mPeers.values())
        if (!ports.contains(p.port))
            ports.append(p.port);

    // Send broadcast message to all discovered ports
    foreach (const qint16 &port, ports)
        sendToAllBroadcast(packet, port);

    // Free memory
    delete packet;
}

void DuktoProtocol::newUdpData()
{
    while (mSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        const int maxLength = 65536;  // Theoretical max length in IPv4
        datagram.resize(maxLength);
        // datagram.resize(mSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        int size = mSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        datagram.resize(size);
        handleMessage(datagram, sender);
     }
}

void DuktoProtocol::handleMessage(QByteArray &data, QHostAddress &sender)
{
    char msgtype = data.at(0);

    switch(msgtype)
    {
        case 0x01:  // HELLO (broadcast)
        case 0x02:  // HELLO (unicast)
            data.remove(0, 1);
            if (data != getSystemSignature()) {
                mPeers[sender.toString()] = Peer(sender, QString::fromUtf8(data), DEFAULT_UDP_PORT);
                if (msgtype == 0x01) sayHello(sender, DEFAULT_UDP_PORT);
                emit peerListAdded(mPeers[sender.toString()]);
            }
            break;

        case 0x03:  // GOODBYE
            emit peerListRemoved(mPeers[sender.toString()]);
            mPeers.remove(sender.toString());
            break;

        case 0x04:  // HELLO (broadcast) with PORT
        case 0x05:  // HELLO (unicast) with PORT
            data.remove(0, 1);
            qint16 port = *((qint16*) data.constData());
            data.remove(0, 2);
            if (data != getSystemSignature()) {
                mPeers[sender.toString()] = Peer(sender, QString::fromUtf8(data), port);
                if (msgtype == 0x04) sayHello(sender, port);
                emit peerListAdded(mPeers[sender.toString()]);
            }
            break;
    }

}

// Request incoming TCP connection
void DuktoProtocol::newIncomingConnection()
{

    // Check presence pendant free
    if (!mTcpServer->hasPendingConnections()) return;

    // recovery connection
    QTcpSocket *s = mTcpServer->nextPendingConnection();

    // If I am already receiving or sending, the connection refusal
    if (mIsReceiving || mIsSending)
    {
        s->close();
        return;
    }

    // Update graphical interface
    receiveFileStart(s->peerAddress().toString());

    // Setting TCP socket current
    mCurrentSocket = s;

    // Waiting header of the connection (timeout 10 sec)
    if (!s->waitForReadyRead(10000))
    {
        // I have not received the header of the connection, close
        mCurrentSocket->close();
        delete mCurrentSocket;
        mCurrentSocket = NULL;
        return;
    }

    // Registering Event Handlers socket
    connect(mCurrentSocket, SIGNAL(readyRead()), this, SLOT(readNewData()), Qt::DirectConnection);
    connect(mCurrentSocket, SIGNAL(disconnected()), this, SLOT(closedConnectionTmp()), Qt::QueuedConnection);

    // initialization of variables
    mIsReceiving = true;
    mTotalReceivedData = 0;
    mElementSize = -1;
    mReceivedFiles = new QStringList();
    mRootFolderName = "";
    mRootFolderRenamed = "";
    mReceivingText = false;
    mRecvStatus = FILENAME;

    // - Reading header general -
    // Number entities to receive
    mCurrentSocket->read((char*) &mElementsToReceiveCount, sizeof(qint64));
    // total Size
    mCurrentSocket->read((char*) &mTotalSize, sizeof(qint64));

    // Start reading data file
    readNewData();

}

// Process main reading
void DuktoProtocol::readNewData()
{

    // As long as there is data to read
    while (mCurrentSocket->bytesAvailable() > 0)
    {

        // Depending on the state in which I find myself read what I expect
        switch (mRecvStatus)
        {

            case FILENAME:
        {
            char c;
            while (1) {
                        int ret = mCurrentSocket->read(&c, sizeof(c));
                        if (ret < 1) return;
                        if (c == '\0')
                        {
                            mRecvStatus = FILESIZE;
                            break;
                        }
                mPartialName.append(c);
            }
                }
                break;

            case FILESIZE:
                {
                    if (!(mCurrentSocket->bytesAvailable() >= sizeof(qint64))) return;
            mCurrentSocket->read((char*) &mElementSize, sizeof(qint64));
            mElementReceivedData = 0;
                    QString name = QString::fromUtf8(mPartialName);
                    mPartialName.clear();

            // If the current item is a folder, I create it and step to the next
            if (mElementSize == -1)
            {
                // I check the folder name "root"
                QString rootName = name.section("/", 0, 0);

                // If I have not dealt with this root, do it now
                if (mRootFolderName != rootName) {

                    // I check if I already have a folder with this name
                    // if I find an alternative name
                    int i = 2;
                    QString originalName = name;
                    while (QFile::exists(name))
                        name = originalName + " (" + QString::number(i++) + ")";
                    mRootFolderName = originalName;
                    mRootFolderRenamed = name;
                    mReceivedFiles->append(name);

                }

                // If you've already been treated, then rename this path
                else if (mRootFolderName != mRootFolderRenamed)
                    name = name.replace(0, name.indexOf('/'), mRootFolderRenamed);

                // I create the directory
                QDir dir(".");
                        bool ret = dir.mkpath(name);
                        if (!ret)
                        {
                            emit receiveFileCancelled();
                            // closing socket
                            if (mCurrentSocket)
                            {
                                mCurrentSocket->disconnect();
                                mCurrentSocket->disconnectFromHost();
                                mCurrentSocket->close();
                                mCurrentSocket->deleteLater();
                                mCurrentSocket = NULL;
                            }

                            // Rilascio memory
                            delete mReceivedFiles;
                            mReceivedFiles = NULL;

                            // Set Status
                            mIsReceiving = false;
                            return;
                        }
                        mRecvStatus = FILENAME;
                        break;
            }

            // It could be a sending text
            else if (name == "___DUKTO___TEXT___")
            {
                mReceivedFiles->append(name);
                mReceivingText = true;
                mTextToReceive.clear();
                mCurrentFile = NULL;
            }

            // Otherwise create the new file
            else
            {
                // If the file is a renamed folder, I have to act accordingly
                if ((name.indexOf('/') != -1) && (name.section("/", 0, 0) == mRootFolderName))
                    name = name.replace(0, name.indexOf('/'), mRootFolderRenamed);

                // If the file already exists, change the name of the new
                int i = 2;
                QString originalName = name;
                while (QFile::exists(name)) {
                    QFileInfo fi(originalName);
                    name = fi.baseName() + " (" + QString::number(i) + ")." + fi.completeSuffix();
                    i++;
                }
                mReceivedFiles->append(name);
                mCurrentFile = new QFile(name);
                        bool ret = mCurrentFile->open(QIODevice::WriteOnly);
                        if (!ret)
                        {
                            emit receiveFileCancelled();
                            // closing socket
                            if (mCurrentSocket)
                            {
                                mCurrentSocket->disconnect();
                                mCurrentSocket->disconnectFromHost();
                                mCurrentSocket->close();
                                mCurrentSocket->deleteLater();
                                mCurrentSocket = NULL;
            }

                            // Rilascio memory
                            delete mReceivedFiles;
                            mReceivedFiles = NULL;

                            // Set Status
                            mIsReceiving = false;
                            return;
        }
                        mReceivingText = false;
                    }
                    mRecvStatus = DATA;
                }
                break;


            case DATA:
                {
        // I try to read what I need to finish the current file
        // (or to empty the buffer of data received)
        qint64 s = (mCurrentSocket->bytesAvailable() > (mElementSize - mElementReceivedData))
                    ? (mElementSize - mElementReceivedData)
                    : mCurrentSocket->bytesAvailable();
        QByteArray d = mCurrentSocket->read(s);
        mElementReceivedData += d.size();
        mTotalReceivedData += d.size();
        updateStatus();

        // Unless the data read
        if (!mReceivingText)
            mCurrentFile->write(d);
        else
            mTextToReceive.append(d);

        // I check if I have completed the current element
        if (mElementReceivedData == mElementSize)
        {
            // Completed, close the file and get ready for the next element
            mElementSize = -1;
            if (!mReceivingText)
            {
                mCurrentFile->deleteLater();
                mCurrentFile = NULL;
            }
                        mRecvStatus = FILENAME;
        }
    }
                break;

        }
    }
}

void DuktoProtocol::closedConnectionTmp()
{
    QTimer::singleShot(500, this, SLOT(closedConnection()));
}

// Closing the TCP connection in reception
void DuktoProtocol::closedConnection()
{
    // Flush the receive buffer
    readNewData();

    // Possible closing current file
    if (mCurrentFile)
    {
        QString name;
        name = mCurrentFile->fileName();
        mCurrentFile->close();
        delete mCurrentFile;
        mCurrentFile = NULL;
        QFile::remove(name);
        receiveFileCancelled();
    }

    // Receive File Closed
    else if (!mReceivingText)
        receiveFileComplete(mReceivedFiles, mTotalSize);

    // Receiving text ended
    else
    {
        QString rec = QString::fromUtf8(mTextToReceive);
        receiveTextComplete(&rec, mTotalSize);
    }

    // closing socket
    if (mCurrentSocket)
    {
        mCurrentSocket->disconnect();
        mCurrentSocket->disconnectFromHost();
        mCurrentSocket->close();
        mCurrentSocket->deleteLater();
        mCurrentSocket = NULL;
    }

    // Rilascio memory
    delete mReceivedFiles;
    mReceivedFiles = NULL;

    // Set status
    mIsReceiving = false;

}

void DuktoProtocol::sendFile(QString ipDest, qint16 port, QStringList files)
{
    // Check for default port
    if (port == 0) port = DEFAULT_TCP_PORT;

    // Check other ongoing activities
    if (mIsReceiving || mIsSending) return;
    mIsSending = true;

    // File to send
    mFilesToSend = expandTree(files);
    mFileCounter = 0;

    // Connecting to the recipient
    mCurrentSocket = new QTcpSocket(this);

    // Manages
    connect(mCurrentSocket, SIGNAL(connected()), this, SLOT(sendMetaData()), Qt::DirectConnection);
    connect(mCurrentSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(sendConnectError(QAbstractSocket::SocketError)), Qt::DirectConnection);
    connect(mCurrentSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(sendData(qint64)), Qt::DirectConnection);

    // connection
    mCurrentSocket->connectToHost(ipDest, port);
}

void DuktoProtocol::sendText(QString ipDest, qint16 port, QString text)
{
    // Check for default port
    if (port == 0) port = DEFAULT_TCP_PORT;

    // Check other ongoing activities
    if (mIsReceiving || mIsSending) return;
    mIsSending = true;

    // Text to send
    mFilesToSend = new QStringList();
    mFilesToSend->append("___DUKTO___TEXT___");
    mFileCounter = 0;
    mTextToSend = text;

    // Connecting to the recipient
    mCurrentSocket = new QTcpSocket(this);
    connect(mCurrentSocket, SIGNAL(connected()), this, SLOT(sendMetaData()), Qt::DirectConnection);
    connect(mCurrentSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(sendConnectError(QAbstractSocket::SocketError)), Qt::DirectConnection);
    connect(mCurrentSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(sendData(qint64)), Qt::DirectConnection);
    mCurrentSocket->connectToHost(ipDest, port);
}

void DuktoProtocol::sendScreen(QString ipDest, qint16 port, QString path)
{
    // Check for default port
    if (port == 0) port = DEFAULT_TCP_PORT;

    // Check other ongoing activities
    if (mIsReceiving || mIsSending) return;
    mIsSending = true;

    // File to send
    QStringList files;
    files.append(path);
    mFilesToSend = expandTree(files);
    mFileCounter = 0;
    mSendingScreen = true;

    // Connecting to the recipient
    mCurrentSocket = new QTcpSocket(this);

    // Manages
    connect(mCurrentSocket, SIGNAL(connected()), this, SLOT(sendMetaData()), Qt::DirectConnection);
    connect(mCurrentSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(sendConnectError(QAbstractSocket::SocketError)), Qt::DirectConnection);
    connect(mCurrentSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(sendData(qint64)), Qt::DirectConnection);

    // connection
    mCurrentSocket->connectToHost(ipDest, port);
}

void DuktoProtocol::sendMetaData()
{
    // Setting the send buffer
#if defined(Q_WS_WIN)
    int v = 49152;
    ::setsockopt(mCurrentSocket->socketDescriptor(), SOL_SOCKET, SO_SNDBUF, (char*)&v, sizeof(v));
#endif

    // Header
    // - N. entities (files, folders, etc ...)
    // - Total Size
    // - Name the first file
    // - Size first (and only) file (-1 for a folder)

    QByteArray header;
    qint64 tmp;

    // N. entity
    tmp = mFilesToSend->count();
    header.append((char*) &tmp, sizeof(tmp));
    // total Size
    mTotalSize = computeTotalSize(mFilesToSend);
    header.append((char*) &mTotalSize, sizeof(mTotalSize));

    // Primo element
    header.append(nextElementHeader());

    // Invio header
    mCurrentSocket->write(header);

    // initialization of variables
    mTotalSize += header.size();
    mSentData = 0;
    mSentBuffer = 0;

    // Updated user interface
    updateStatus();
}

void DuktoProtocol::sendData(qint64 b)
{
    QByteArray d;

    // update statistics
    mSentData += b;
    updateStatus();

    // Check if all the data put into the buffer
    // Have been sent
    mSentBuffer -= b;

    // If there is more data to send, wait
    // They are sent
    if (mSentBuffer > 0) return;

    // If it's sending a text, throw in
    // All text
    if ((!mTextToSend.isEmpty()) && (mFilesToSend->at(mFileCounter - 1) == "___DUKTO___TEXT___"))
    {
        d.append(mTextToSend.toUtf8().data());
        mCurrentSocket->write(d);
        mSentBuffer = d.size();
        mTextToSend.clear();
        return;
    }

    // If the current file is not yet finished
    // Send a new part of the file
    if (mCurrentFile)
        d = mCurrentFile->read(10000);
    if (d.size() > 0)
    {
        mCurrentSocket->write(d);
        mSentBuffer = d.size();
        return;
    }

    // Otherwise, close the file and the next step
    d.append(nextElementHeader());

    // There are no more files to send?
    if (d.size() == 0)
    {
        closeCurrentTransfer();
        return;
    }

    // Enter the header on the first chunk clear set of files
    mTotalSize += d.size();
    if (mCurrentFile)
        d.append(mCurrentFile->read(10000));
    mCurrentSocket->write(d);
    mSentBuffer += d.size();

    return;
}

// Closing data transfer
void DuktoProtocol::closeCurrentTransfer(bool aborted)
{
    mCurrentSocket->disconnect();
    mCurrentSocket->disconnectFromHost();
    if (mCurrentSocket->state() != QTcpSocket::UnconnectedState)
        mCurrentSocket->waitForDisconnected(1000);
    mCurrentSocket->close();
    mCurrentSocket->deleteLater();
    mCurrentSocket = NULL;
    if (mCurrentFile)
    {
        mCurrentFile->close();
        delete mCurrentFile;
        mCurrentFile = NULL;
    }
    mIsSending = false;
    if (!aborted)
        emit sendFileComplete(mFilesToSend);
    delete mFilesToSend;
    mFilesToSend = NULL;

    return;
}

// Updating statistics of sending
void DuktoProtocol::updateStatus()
{
    if (mIsSending)
        emit transferStatusUpdate(mTotalSize, mSentData);
    else if (mIsReceiving)
        emit transferStatusUpdate(mTotalSize, mTotalReceivedData);
}

// In case of connection failure
void DuktoProtocol::sendConnectError(QAbstractSocket::SocketError e)
{
    if (mCurrentSocket)
    {
        mCurrentSocket->close();
        mCurrentSocket->deleteLater();
        mCurrentSocket = NULL;
    }
    if (mCurrentFile)
    {
        mCurrentFile->close();
        delete mCurrentFile;
        mCurrentFile = NULL;
    }
    mIsSending = false;
    sendFileError(e);
}

// Given a list of files and folders, is expanded to
// Contain all the files and folders
QStringList* DuktoProtocol::expandTree(QStringList files)
{
    // base Path
    QString bp = files.at(0);
    if (bp.right(1) == "/") bp.chop(1);
    mBasePath = QFileInfo(bp).absolutePath();
    if (mBasePath.right(1) == "/") mBasePath.chop(1);

    // Iteration on the elements
    QStringList* expanded = new QStringList();
    for (int i = 0; i < files.count(); i++)
        addRecursive(expanded, files.at(i));

    return expanded;
}

// Add recursively all files and folders contained in a folder
void DuktoProtocol::addRecursive(QStringList *e, QString path)
{

    path.replace("//", "/");
    path.replace("\\", "/");
    if (path.right(1) == "/") path.chop(1);
    e->append(path);

    QFileInfo fi(path);
    if (fi.isDir())
    {
        QStringList entries = QDir(path).entryList(QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot);
        for (int i = 0; i < entries.count(); i++)
            addRecursive(e, path + "/" + entries.at(i));
    }
}

// Returns the header to be sent to the next item
// From tramsettere
QByteArray DuktoProtocol::nextElementHeader()
{
    QByteArray header;

    // Obtains the name of the file (if not the last)
    if (mFilesToSend->size() == mFileCounter) return header;
    QString fullname = mFilesToSend->at(mFileCounter++);

    // Closing the previous file, if it has not already been closed
    if (mCurrentFile) {
        mCurrentFile->close();
        delete mCurrentFile;
        mCurrentFile = NULL;
    }

    // I check if it is sending a text
    if (fullname == "___DUKTO___TEXT___") {
        header.append(fullname.toAscii() + '\0');
        qint64 size = mTextToSend.toUtf8().length();
        header.append((char*) &size, sizeof(size));
        return header;
    }

    // element Name
    QString name;

    // I check if it is sending a screen
    if (mSendingScreen) {

        name = "Screenshot.jpg";
        mSendingScreen = false;
    }
    else
        name = fullname;

    // Adding header file name
    name.replace(mBasePath + "/", "");
    header.append(name.toUtf8() + '\0');

    // Size element
    qint64 size = -1;
    QFileInfo fi2(fullname);
    if (fi2.isFile()) size = fi2.size();
    header.append((char*) &size, sizeof(size));

    // opening files
    if (size > -1) {
        mCurrentFile = new QFile(fullname);
        mCurrentFile->open(QIODevice::ReadOnly);
    }

    return header;
}

// Calculate the total employment of all files to be transferred
qint64 DuktoProtocol::computeTotalSize(QStringList *e)
{
    // If you send a text
    if ((e->length() == 1) && (e->at(0) == "___DUKTO___TEXT___"))
        return mTextToSend.toUtf8().length();

    // If it is a normal submission
    qint64 size = 0;
    for (int i = 0; i < e->count(); i++)
    {
        QFileInfo fi(e->at(i));
        if (!fi.isDir()) size += fi.size();
    }
    return size;
}

// Sends a packet to all the broadcast addresses of the PC
void DuktoProtocol::sendToAllBroadcast(QByteArray *packet, qint16 port)
{
    // List of available interfaces
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();

    // Iteration on the interfaces
    for (int i = 0; i < ifaces.size(); i++)
    {
        // Iteration for all interface IP
        QList<QNetworkAddressEntry> addrs = ifaces[i].addressEntries();

        // Enter each IP packet to the broadcast
        for (int j = 0; j < addrs.size(); j++)
            if ((addrs[j].ip().protocol() == QAbstractSocket::IPv4Protocol) && (addrs[j].broadcast().toString() != ""))
            {
                mSocket->writeDatagram(packet->data(), packet->length(), addrs[j].broadcast(), port);
                mSocket->flush();
            }
    }
}

// Stops a transfer in progress (can only send side)
void DuktoProtocol::abortCurrentTransfer()
{
    // Check if it's sending data
    if (!mIsSending) return;

    // Abort current connection
    closeCurrentTransfer(true);
    emit sendFileAborted();
}

// Refresh the buddy name of the local user
void DuktoProtocol::updateBuddyName()
{
    // Enter package disconnection
    sayGoodbye();

    // Enter ad package with the new name
    sayHello(QHostAddress::Broadcast, true);
}
