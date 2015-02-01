#ifndef NODEMODEL_H
#define NODEMODEL_H

#include <QAbstractListModel>

#include "p2pnode.h"

class NodeModel : public QAbstractListModel
{
    Q_OBJECT

    QList<P2PNode*> nodes;

public:
    enum NodeRoles {
        DataItemRole = Qt::UserRole + 1
    };

    NodeModel(QObject *parent = 0);
    ~NodeModel();

    void addNode(P2PNode *node);
    void removeNode(QUuid nodeUuid);
    void setNodes(const QList<P2PNode*> &nodes);
    void updateNode(P2PNode *node);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
};

#endif // NODEMODEL_H
