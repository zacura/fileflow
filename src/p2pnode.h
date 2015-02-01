#ifndef P2PNODE_H
#define P2PNODE_H

#include <QObject>
#include <QUuid>
#include <QHostAddress>

#include "p2pfile.h"

class P2PNode : public QObject
{
    Q_OBJECT

    QUuid _uuid;
    QString _name;
    QHostAddress _address;
    quint16 _port;

    QList<P2PFile*> files;

public:
    P2PNode(QUuid uuid, QString name, QObject *parent = 0);
    ~P2PNode();

    const QUuid &uuid() const;
    const QString &name() const;
    const QHostAddress &address() const;
    quint16 port() const;

    void setAddress(const QHostAddress &address);
    void setPort(const quint16 port);
    void setName(const QString &name);

    void addFile(P2PFile *file);
    void removeFile(QUuid fileUuid);

signals:

public slots:
};

#endif // P2PNODE_H
