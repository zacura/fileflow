#include "p2pgroup.h"

P2PGroup::P2PGroup(QUuid uuid, QString name, QObject *parent) :
    QObject(parent), _uuid(uuid), _name(name)
{

}

P2PGroup::~P2PGroup()
{

}

const QUuid P2PGroup::uuid() const
{
    return _uuid;
}

const QString P2PGroup::name() const
{
    return _name;
}

const QList<P2PNode *> P2PGroup::members() const
{
    return _members;
}

void P2PGroup::addMember(P2PNode *member)
{
    _members.append(member);
}

void P2PGroup::removeMember(P2PNode *member)
{
    _members.removeAll(member);
}

