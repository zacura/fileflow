#ifndef P2PCLIENT_H
#define P2PCLIENT_H

#include <QObject>
#include <QHash>
#include <QUdpSocket>
#include <QFile>

#include "p2pnode.h"
#include "p2pgroup.h"
#include "p2pfile.h"

class P2PClient : public QObject
{
    Q_OBJECT

    QUdpSocket *socket;

    P2PNode *me;
    QHash<QUuid, P2PNode*> nodes;
    QHash<QUuid, P2PGroup*> groups;
    QHash<QUuid, P2PFile*> files;
    QHash<P2PGroup*, QList<P2PFile*> > groupFiles;

    QHostAddress address;
    quint16 port;

public:
    P2PClient(QHostAddress address, quint16 port, QObject *parent = 0);
    virtual ~P2PClient();


private:
    void createIdentity();

    void sendBroadcastMessage(const QString &keyword, const QString &payload);
    void sendMessage(const P2PNode *node, const QString &keyword, const QString &payload);
    QByteArray buildMessage(const QString &keyword, const QString &payload);

    void onHello(QHostAddress &host, quint16 port, QUuid nodeUuid, const QString &payload);
    void onWelcome(QHostAddress &host, quint16 port, QUuid nodeUuid, const QString &payload);
    void onBye(QHostAddress &host, quint16 port, QUuid nodeUuid, const QString &payload);
    void onName(QHostAddress &host, quint16 port, QUuid nodeUuid, const QString &payload);
    void onJoin(QHostAddress &host, quint16 port, QUuid nodeUuid, const QString &payload);
    void onLeave(QHostAddress &host, quint16 port, QUuid nodeUuid, const QString &payload);

    void saveGroup(P2PGroup *group);
    void unsaveGroup(P2PGroup *group);
    void loadGroups();

signals:
    void nodeArrives(P2PNode*);
    void nodeLeaves(QUuid);
    void groupCreated(P2PGroup*);
    void groupRemoved(QUuid);
    void groupJoined(P2PNode*, P2PGroup*);
    void groupLeft(P2PNode*, P2PGroup*);
    void nameChanged(P2PNode*);

    void connected(P2PNode*);

public slots:
    void processPendingDatagrams();

    void start();
    void stop();

    void setName(QString name);
    void joinGroup(QString name);
    void joinGroup(P2PGroup *group);
    void joinGroup(QUuid groupUuid, QString name);
    void leaveGroup(P2PGroup *group);
    void shareFile(P2PGroup *group, QString fileName);
    void shareFile(QUuid groupUuid, QString fileName);

};

#endif // P2PCLIENT_H
