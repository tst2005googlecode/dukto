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

#ifndef DUKTOPROTOCOL_H
#define DUKTOPROTOCOL_H

#include <QObject>

#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QHostInfo>
#include <QHash>
#include <QFile>

#include "peer.h"

class DuktoProtocol : public QObject
{
    Q_OBJECT

public:
    DuktoProtocol();
    virtual ~DuktoProtocol();
    void initialize();
    void setPorts(qint16 udp, qint16 tcp);
    void sayHello(QHostAddress dest);
    void sayHello(QHostAddress dest, qint16 port);
    void sayGoodbye();
    inline QHash<QString, Peer>& getPeers() { return mPeers; }
    void sendFile(QString ipDest, qint16 port, QStringList files);
    void sendText(QString ipDest, qint16 port, QString text);
    void sendScreen(QString ipDest, qint16 port, QString path);
    inline bool isBusy() { return mIsSending || mIsReceiving; }
    void abortCurrentTransfer();
    void updateBuddyName();
    
public slots:
    void newUdpData();
    void newIncomingConnection();
    void readNewData();
    void closedConnection();
    void closedConnectionTmp();
    void sendMetaData();
    void sendData(qint64 b);
    void sendConnectError(QAbstractSocket::SocketError);

signals:
     void peerListAdded(Peer peer);
     void peerListRemoved(Peer peer);
     void sendFileComplete(QStringList *files);
     void sendFileError(int code);
     void sendFileAborted();
     void receiveFileStart(QString senderIp);
     void receiveFileComplete(QStringList *files, qint64 totalSize);
     void receiveTextComplete(QString *text, qint64 totalSize);
     void receiveFileCancelled();
     void transferStatusUpdate(qint64 total, qint64 partial);

private:
    QString getSystemSignature();
    QStringList* expandTree(QStringList files);
    void addRecursive(QStringList *e, QString path);
    qint64 computeTotalSize(QStringList *e);
    QByteArray nextElementHeader();
    void sendToAllBroadcast(QByteArray *packet, qint16 port);
    void closeCurrentTransfer(bool aborted = false);

    void handleMessage(QByteArray &data, QHostAddress &sender);
    void updateStatus();

    QUdpSocket *mSocket;            // UDP Socket signaling
    QTcpServer *mTcpServer;         // TCP socket waiting for data
    QTcpSocket *mCurrentSocket;     // TCP socket of the file transfer

    QHash<QString, Peer> mPeers;    // List peer identified

    // Send and receive members
    qint16 mLocalUdpPort;
    qint16 mLocalTcpPort;
    bool mIsSending;
    bool mIsReceiving;
    QFile *mCurrentFile;            // Current open file pointer
    qint64 mTotalSize;              // Total amount of data to send or receive
    int mFileCounter;               // Pointer to currently be transmitted or received

    // Sending members
    QStringList *mFilesToSend;      // List of elements to be transmitted
    qint64 mSentData;               // Total amount of data transmitted
    qint64 mSentBuffer;             // Amount of data remaining in the transmission buffer
    QString mBasePath;              // Base path for sending files and folders
    QString mTextToSend;            // Text to be sent (in the case of sending text)
    bool mSendingScreen;            // Flag that indicates whether you are sending a screenshot

    // Receive members
    qint64 mElementsToReceiveCount;    // Number of elements to receive
    qint64 mTotalReceivedData;         // Total amount of data received
    qint64 mElementReceivedData;       // Amount of data received for the current element
    qint64 mElementSize;               // Size of the current
    QString mRootFolderName;           // Name of the parent folder received
    QString mRootFolderRenamed;        // The name of the root folder to use
    QStringList *mReceivedFiles;       // List of elements to be transmitted
    QByteArray mTextToReceive;         // Received text when sending text
    bool mReceivingText;               // Receiving text in progress
    QByteArray mPartialName;           // Name next file read only in part
    enum RecvStatus {
        FILENAME,
        FILESIZE,
        DATA
    } mRecvStatus;

};

#endif // DUKTOPROTOCOL_H
