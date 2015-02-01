#include <QDebug>
#include <QSettings>

#include "p2pclient.h"


P2PClient::P2PClient(QHostAddress address, quint16 port, QObject *parent) :
    QObject(parent), address(address), port(port), globalGroup(0)
{
    createIdentity();
    socket = new QUdpSocket(this);

    connect(socket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));

}

P2PClient::~P2PClient()
{

}

void P2PClient::createIdentity()
{
    QSettings settings;
    QUuid uuid;
    QString name = "";
    if (settings.contains("identity/uuid")) {
        uuid = QUuid(settings.value("identity/uuid").toString());
    }
    if (settings.contains("identity/name")) {
        name = settings.value("identity/name").toString();
    }

    if (uuid.isNull()) {
        uuid = QUuid::createUuid();
        settings.setValue("identity/uuid", uuid.toString());
    }
    if (name == "") {
        name = uuid.toString();
        settings.setValue("identity/name", name);
    }

    me = new P2PNode(uuid, name);

}

void P2PClient::sendBroadcastMessage(const QString &keyword, const QString &payload)
{
    if (socket->state() != QUdpSocket::BoundState) return;
    QByteArray message = buildMessage(keyword, payload);
    qDebug() << "> BROADCAST:" << QString(message);
    socket->writeDatagram(message, QHostAddress::Broadcast, port);
}

void P2PClient::sendMessage(const P2PNode *node, const QString &keyword, const QString &payload)
{
    if (socket->state() != QUdpSocket::BoundState) return;
    QByteArray message = buildMessage(keyword, payload);
    qDebug() << ">" << node->address().toString() << ":" << QString(message);
    socket->writeDatagram(message, node->address(), node->port());
}

QByteArray P2PClient::buildMessage(const QString &keyword, const QString &payload)
{
    return QString("%1::%2::%3").arg(keyword).arg(me->uuid().toString()).arg(payload).toUtf8();
}

void P2PClient::onHello(QHostAddress &host, quint16 port, QUuid nodeUuid, const QString &payload)
{
    // a node joins the network
    QStringList message = payload.split(":");
    //qDebug() << message;
    P2PNode *node = new P2PNode(nodeUuid, message.at(0), this);
    //qDebug() << node->uuid() << me->uuid();
    if (node->uuid() != me->uuid()) {
        // only if it is someone else
        node->setAddress(host);
        node->setPort(port);
        nodes.insert(node->uuid(), node);
        sendMessage(node, "WELCOME", me->name());

        globalGroup->addMember(node);
        emit nodeArrives(node);
        emit groupJoined(node, globalGroup);
    }
    else {
        delete node;
    }
}

void P2PClient::onWelcome(QHostAddress &host, quint16 port, QUuid nodeUuid, const QString &payload)
{
    // a reply to HELLO
    QStringList message = payload.split(':');
    P2PNode *node = new P2PNode(nodeUuid, message.at(0), this);
    node->setAddress(host);
    node->setPort(port);
    nodes.insert(node->uuid(), node);

    globalGroup->addMember(node);

    emit nodeArrives(node);
    emit groupJoined(node, globalGroup);
}

void P2PClient::onBye(QHostAddress &host, quint16 port, QUuid nodeUuid, const QString &payload)
{
    // a node leaves
    Q_UNUSED(host);
    Q_UNUSED(port);
    Q_UNUSED(payload);

    nodes.remove(nodeUuid);

    emit nodeLeaves(nodeUuid);
}

void P2PClient::onName(QHostAddress &host, quint16 port, QUuid nodeUuid, const QString &payload)
{
    Q_UNUSED(host);
    Q_UNUSED(port);
    if (nodes.contains(nodeUuid)) {
        P2PNode *node = nodes[nodeUuid];
        node->setName(payload);
        emit nameChanged(node);
    }
}

void P2PClient::onJoin(QHostAddress &host, quint16 port, QUuid nodeUuid, const QString &payload)
{
    Q_UNUSED(host);
    Q_UNUSED(port);
    if (nodeUuid == me->uuid()) return;
    QStringList message = payload.split(':');

    QUuid groupUuid(message.at(0));
    P2PGroup *group = 0;
    if (!groups.contains(groupUuid)) {
        group = new P2PGroup(groupUuid, message.at(1), this);
        groups.insert(group->uuid(), group);
        emit groupCreated(group);
    }
    else {
        group = groups[groupUuid];
    }

    P2PNode *node = 0;
    if (!nodes.contains(nodeUuid)) {
        node = new P2PNode(nodeUuid, nodeUuid.toString(), this);
        nodes.insert(node->uuid(), node);
        emit nodeArrives(node);
    }
    else {
        node = nodes[nodeUuid];
    }

    emit groupJoined(node, group);
}

void P2PClient::onLeave(QHostAddress &host, quint16 port, QUuid nodeUuid, const QString &payload)
{
    Q_UNUSED(host);
    Q_UNUSED(port);

    if (nodeUuid == me->uuid()) return;
    QStringList message = payload.split(':');

    QUuid groupUuid(message.at(0));

    // can not leave Global group
    if (groupUuid.isNull()) return;

    if (!groups.contains(groupUuid)) return;
    P2PGroup *group = groups[groupUuid];

    if (!nodes.contains(nodeUuid)) return;
    P2PNode *node = nodes[nodeUuid];

    group->removeMember(node);

    emit groupLeft(node, group);
}

void P2PClient::saveGroup(P2PGroup *group)
{
    QSettings settings;
    settings.setValue(QStringLiteral("groups/%1").arg(group->name()), group->uuid().toString());
}

void P2PClient::unsaveGroup(P2PGroup *group)
{
    QSettings settings;
    settings.remove(QStringLiteral("groups/%1").arg(group->name()));
}

void P2PClient::loadGroups()
{
    QSettings settings;
    settings.beginGroup("groups");
    QStringList groupNames = settings.childKeys();
    for (QString groupName: groupNames) {
        QUuid groupUuid = QUuid(settings.value(groupName).toString());
        if (!groupUuid.isNull()) {
            joinGroup(groupUuid, groupName);
        }
    }
}

void P2PClient::processPendingDatagrams()
{
    while (socket->hasPendingDatagrams()) {
        QByteArray datagram;
        QHostAddress host;
        quint16 port;
        datagram.resize(socket->pendingDatagramSize());
        socket->readDatagram(datagram.data(), datagram.size(), &host, &port);
        qDebug() << "<" << host.toString() << ":" << datagram;

        QString data = QString::fromUtf8(datagram);
        QStringList message = data.split("::");
        if (message.count() != 3) return;

        //qDebug() << message;
        //qDebug() << payload;
        if (message.at(0) == "HELLO") onHello(host, port, QUuid(message.at(1)), message.at(2));
        else if (message.at(0) == "WELCOME") onWelcome(host, port, QUuid(message.at(1)), message.at(2));
        else if (message.at(0) == "BYE") onBye(host, port, QUuid(message.at(1)), message.at(2));
        else if (message.at(0) == "NAME") onName(host, port, QUuid(message.at(1)), message.at(2));
        else if (message.at(0) == "JOIN") onJoin(host, port, QUuid(message.at(1)), message.at(2));
        else if (message.at(0) == "LEAVE") onLeave(host, port, QUuid(message.at(1)), message.at(2));
    }
}

void P2PClient::start()
{
    socket->bind(address, port);
    sendBroadcastMessage("HELLO", me->name());

    emit connected(me);
    emit nodeArrives(me);

    globalGroup = new P2PGroup(QUuid(), "Global", this);
    groups.insert(globalGroup->uuid(), globalGroup);
    globalGroup->addMember(me);

    emit groupCreated(globalGroup);
    emit groupJoined(me, globalGroup);

    loadGroups();
}

void P2PClient::stop()
{
    sendBroadcastMessage("BYE", "");
    socket->close();
}

void P2PClient::setName(QString name)
{
    QSettings settings;
    me->setName(name);
    sendBroadcastMessage("NAME", name);

    settings.setValue("identity/name", name);
    emit nameChanged(me);
}

void P2PClient::joinGroup(QString name)
{
    P2PGroup *group = new P2PGroup(QUuid::createUuid(), name, this);
    groups.insert(group->uuid(), group);

    QString payload = QStringLiteral("%1:%2").arg(group->uuid().toString()).arg(group->name());
    sendBroadcastMessage("JOIN", payload);

    group->addMember(me);
    saveGroup(group);
    emit groupCreated(group);
}

void P2PClient::joinGroup(P2PGroup *group)
{
    if (group->members().contains(me)) return;

    QString payload = QStringLiteral("%1:%2").arg(group->uuid().toString()).arg(group->name());
    sendBroadcastMessage("JOIN", payload);

    group->addMember(me);
    saveGroup(group);
    emit groupJoined(me, group);
}

void P2PClient::joinGroup(QUuid groupUuid, QString name)
{
    P2PGroup *group = 0;
    if (!groups.contains(groupUuid)) {
        group = new P2PGroup(groupUuid, name, 0);
        groups.insert(group->uuid(), group);
        emit groupCreated(group);
    }
    else {
        group = groups[groupUuid];
    }

    QString payload = QStringLiteral("%1:%2").arg(group->uuid().toString()).arg(group->name());
    sendBroadcastMessage("JOIN", payload);
    group->addMember(me);
    saveGroup(group);
    emit groupJoined(me, group);
}

void P2PClient::leaveGroup(P2PGroup *group)
{
    // can not leave Global group
    if (group->uuid().isNull()) return;
    QString payload = QStringLiteral("%1").arg(group->uuid().toString());
    sendBroadcastMessage("LEAVE", payload);
    group->removeMember(me);
    unsaveGroup(group);
    emit groupLeft(me, group);

    if (group->members().count() == 0) {
        groups.remove(group->uuid());
        emit groupRemoved(group->uuid());
        group->deleteLater();
    }
}

void P2PClient::shareFile(P2PGroup *group, QString fileName)
{
    QFile file(fileName);
    P2PFile *p2pFile = new P2PFile(QUuid::createUuid(), file.fileName(), file.size(), this);

    files.insert(p2pFile->uuid(), p2pFile);
    groupFiles[group].append(p2pFile);

    QString payload = QStringLiteral("%1:%2:%3:%4")
            .arg(group->uuid().toString())
            .arg(p2pFile->uuid().toString())
            .arg(p2pFile->name())
            .arg(p2pFile->size());

    sendBroadcastMessage("FILE", payload);
}

void P2PClient::shareFile(QUuid groupUuid, QString fileName)
{
    if (!groups.contains(groupUuid)) return;
    P2PGroup *group = groups[groupUuid];

    shareFile(group, fileName);
}
