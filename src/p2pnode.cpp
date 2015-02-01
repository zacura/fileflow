#include "p2pnode.h"

P2PNode::P2PNode(QUuid uuid, QString name, QObject *parent) :
    QObject(parent), _uuid(uuid), _name(name)
{

}

P2PNode::~P2PNode()
{

}

const QUuid& P2PNode::uuid() const
{
    return _uuid;
}

const QString& P2PNode::name() const
{
    return _name;
}

const QHostAddress &P2PNode::address() const
{
    return _address;
}

quint16 P2PNode::port() const
{
    return _port;
}

void P2PNode::setAddress(const QHostAddress &address)
{
    _address = address;
}

void P2PNode::setPort(const quint16 port)
{
    _port = port;
}

void P2PNode::setName(const QString &name)
{
    _name = name;
}

void P2PNode::addFile(P2PFile *file)
{
    file->setParent(this);
    files.append(file);
}

void P2PNode::removeFile(QUuid fileUuid)
{

}

