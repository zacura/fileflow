#ifndef P2PGROUP_H
#define P2PGROUP_H

#include <QObject>
#include <QUuid>

#include "p2pnode.h"

class P2PGroup : public QObject
{
    Q_OBJECT

    QUuid _uuid;
    QString _name;

    QList<P2PNode *> _members;

public:
    P2PGroup(QUuid uuid, QString name, QObject *parent = 0);
    ~P2PGroup();

    const QUuid uuid() const;
    const QString name() const;

    const QList<P2PNode *> members() const;
    void addMember(P2PNode *member);
    void removeMember(P2PNode *member);

signals:

public slots:
};

Q_DECLARE_METATYPE(P2PGroup*)

#endif // P2PGROUP_H
