#include "nodemodel.h"

NodeModel::NodeModel(QObject *parent) :
    QAbstractListModel(parent)
{

}

NodeModel::~NodeModel()
{

}

void NodeModel::addNode(P2PNode *node)
{
    int i = 0;
    for (P2PNode *n: nodes) {
        if (n->name() >= node->name()) break;
        ++i;
    }

    beginInsertRows(QModelIndex(), i, i);
    nodes.insert(i, node);
    endInsertRows();
}

void NodeModel::removeNode(QUuid nodeUuid)
{
    int i;
    for (P2PNode *n: nodes) {
        if (n->uuid() == nodeUuid) break;
        ++i;
    }
    if (i == nodes.count()) return;

    beginRemoveRows(QModelIndex(), i, i);
    nodes.removeAt(i);
    endRemoveRows();
}

void NodeModel::setNodes(const QList<P2PNode *> &nodes)
{
    beginRemoveRows(QModelIndex(), 0, this->nodes.count());
    this->nodes.clear();
    endRemoveRows();

    beginInsertRows(QModelIndex(), 0, nodes.count());
    this->nodes = nodes;
    std::sort(this->nodes.begin(), this->nodes.end(), [](P2PNode *a, P2PNode *b) {
        return a->name() < b->name();
    });
    endInsertRows();
}

void NodeModel::updateNode(P2PNode *node)
{
    int i;
    for (P2PNode *n: nodes) {
        if (n->uuid() == node->uuid()) break;
        ++i;
    }
    if (i == nodes.count()) return;

    emit dataChanged(index(i), index(i));
}

int NodeModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return nodes.count();
}

QVariant NodeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    if (index.row() >= nodes.count()) return QVariant();

    if (role == Qt::DisplayRole) return nodes.at(index.row())->name();
    else if (role == DataItemRole) return QVariant::fromValue(nodes.at(index.row()));

    return QVariant();
}

Qt::ItemFlags NodeModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

