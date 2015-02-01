#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include <QAbstractListModel>

#include "p2pgroup.h"


class GroupModel : public QAbstractListModel
{
    Q_OBJECT

    //QHash<QUuid, P2PGroup*> groups;
    QList<P2PGroup*> groups;

public:
    enum GroupRoles {
        DataItemRole = Qt::UserRole + 1
    };

    GroupModel(QObject *parent = 0);
    ~GroupModel();

    void addGroup(P2PGroup *group);
    void removeGroup(QUuid groupUuid);
    QModelIndex getGroupIndex(P2PGroup *group);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
};

#endif // GROUPMODEL_H
