#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>
#include <QMap>
#include <QFile>

#include "p2pnode.h"
#include "p2pgroup.h"
#include "groupmodel.h"
#include "nodemodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    NodeModel *nodeModel;
    GroupModel *groupModel;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    void writeSettings();
    void readSettings();

    P2PGroup *getSelectedGroup();

signals:
    void closed();

    void nameChanged(QString name);
    void createGroup(QString name);
    void joinGroup(P2PGroup *group);
    void leaveGroup(P2PGroup *group);
    void shareFile(QUuid groupUuid, QString fileName);

public slots:
    void updateNode(P2PNode *node);

    void addNode(P2PNode *node);
    void removeNode(QUuid nodeUuid);

    void addGroup(P2PGroup *group);
    void removeGroup(QUuid groupUuid);

    void addNodeToGroup(P2PNode *node, P2PGroup *group);
    void removeNodeFromGroup(P2PNode *node, P2PGroup *group);

    void setConnection(P2PNode *me);

    //void addFile(P2PNode *node, file);

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
private slots:

    // QWidget interface
    void on_nameLineEdit_editingFinished();

    void on_createGroupPushButton_clicked();

    void on_groupsListView_clicked(const QModelIndex &index);

    void on_joinGroupPushButton_clicked();

    void on_leaveGroupPushButton_clicked();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
};

#endif // MAINWINDOW_H
