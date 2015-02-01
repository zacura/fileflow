#include "p2pfile.h"

P2PFile::P2PFile(QUuid uuid, QString name, qint64 size, QObject *parent) :
    QObject(parent), _uuid(uuid), _name(name), _size(size)
{

}

P2PFile::~P2PFile()
{

}

const QUuid &P2PFile::uuid() const
{
    return _uuid;
}

const QString &P2PFile::name() const
{
    return _name;
}

qint64 P2PFile::size() const
{
    return _size;
}

