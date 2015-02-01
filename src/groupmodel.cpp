#include "groupmodel.h"

GroupModel::GroupModel(QObject *parent) :
    QAbstractListModel(parent)
{

}

GroupModel::~GroupModel()
{

}

void GroupModel::addGroup(P2PGroup *group)
{
    int i = 0;
    for (P2PGroup *g: groups) {
        if (g->name() >= group->name()) break;
        ++i;
    }

    beginInsertRows(QModelIndex(), i, i);
    groups.insert(i, group);
    endInsertRows();
}

void GroupModel::removeGroup(QUuid groupUuid)
{
    int i;
    for (P2PGroup *g: groups) {
        if (g->uuid() == groupUuid) break;
        ++i;
    }
    if (i == groups.count()) return;

    beginRemoveRows(QModelIndex(), i, i);
    groups.removeAt(i);
    endRemoveRows();
}

QModelIndex GroupModel::getGroupIndex(P2PGroup *group)
{
    int i;
    for (P2PGroup *g: groups) {
        if (g->uuid() == group->uuid()) break;
        ++i;
    }
    if (i == groups.count()) QModelIndex();

    return index(i);
}

int GroupModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return groups.count();
}

QVariant GroupModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    if (index.row() >= groups.count()) return QVariant();

    if (role == Qt::DisplayRole) return groups.at(index.row())->name();
    else if (role == DataItemRole) return QVariant::fromValue(groups.at(index.row()));

    return QVariant();
}

Qt::ItemFlags GroupModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

