#include <QDebug>
#include <QSettings>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QInputDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setAcceptDrops(true);

    nodeModel = new NodeModel(this);
    ui->nodesListView->setModel(nodeModel);

    groupModel = new GroupModel(this);
    ui->groupsListView->setModel(groupModel);

    readSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.setValue("mainWindow/geometry", saveGeometry());
    settings.setValue("mainWindow/mainSplitter", ui->mainSplitter->saveState());
    settings.setValue("mainWindow/contentSplitter", ui->contentSplitter->saveState());
    settings.setValue("mainWindow/fileSplitter", ui->fileSplitter->saveState());
}

void MainWindow::readSettings()
{
    QSettings settings;
    restoreGeometry(settings.value("mainWindow/geometry").toByteArray());
    ui->mainSplitter->restoreState(settings.value("mainWindow/mainSplitter").toByteArray());
    ui->contentSplitter->restoreState(settings.value("mainWindow/contentSplitter").toByteArray());
    ui->fileSplitter->restoreState(settings.value("mainWindow/fileSplitter").toByteArray());
}

P2PGroup *MainWindow::getSelectedGroup()
{
    QModelIndexList selIndexes = ui->groupsListView->selectionModel()->selectedIndexes();

    if (selIndexes.count() > 0) {
        QModelIndex selIndex = selIndexes.at(0);
        P2PGroup *selGroup = qvariant_cast<P2PGroup*>(groupModel->data(selIndex, GroupModel::DataItemRole));
        return selGroup;
    }
    return 0;
}

void MainWindow::updateNode(P2PNode *node)
{
    nodeModel->updateNode(node);
}

void MainWindow::addNode(P2PNode *node)
{
    //nodeModel->addNode(node);
    Q_UNUSED(node);
}

void MainWindow::removeNode(QUuid nodeUuid)
{
    nodeModel->removeNode(nodeUuid);
}

void MainWindow::addGroup(P2PGroup *group)
{
    groupModel->addGroup(group);
    P2PGroup *selGroup = getSelectedGroup();
    if (!selGroup) {
        QModelIndex index = groupModel->getGroupIndex(group);
        if (index.isValid()) {
            ui->groupsListView->selectionModel()->select(index, QItemSelectionModel::Select);
        }
    }
}

void MainWindow::removeGroup(QUuid groupUuid)
{
    groupModel->removeGroup(groupUuid);
    P2PGroup *selGroup = getSelectedGroup();
    if (!selGroup) {
        ui->groupsListView->selectionModel()->select(groupModel->index(0), QItemSelectionModel::Select);
    }
}

void MainWindow::addNodeToGroup(P2PNode *node, P2PGroup *group)
{
    QModelIndexList selIndexes = ui->groupsListView->selectionModel()->selectedIndexes();

    if (selIndexes.count() > 0) {
        QModelIndex selIndex = selIndexes.at(0);
        P2PGroup *selGroup = qvariant_cast<P2PGroup*>(groupModel->data(selIndex, GroupModel::DataItemRole));
        if (selGroup == group) {
            // only update model if the group is selected
            nodeModel->addNode(node);
        }
    }
}

void MainWindow::removeNodeFromGroup(P2PNode *node, P2PGroup *group)
{
    QModelIndexList selIndexes = ui->groupsListView->selectionModel()->selectedIndexes();

    if (selIndexes.count() > 0) {
        QModelIndex selIndex = selIndexes.at(0);
        P2PGroup *selGroup = qvariant_cast<P2PGroup*>(groupModel->data(selIndex, GroupModel::DataItemRole));
        if (selGroup == group) {
            // only update model if the group is selected
            nodeModel->removeNode(node->uuid());
        }
    }
}

void MainWindow::setConnection(P2PNode *me)
{
    ui->nameLineEdit->setText(me->name());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    QMainWindow::closeEvent(event);
    emit closed();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        QModelIndex selGroupIndex = ui->groupsListView->currentIndex();
        P2PGroup *group = qvariant_cast<P2PGroup*>(groupModel->data(selGroupIndex, GroupModel::DataItemRole));

        for (QUrl url: urlList) {
            emit shareFile(group->uuid(), url.toLocalFile());
        }

        event->acceptProposedAction();
    }
}

void MainWindow::on_nameLineEdit_editingFinished()
{
    emit nameChanged(ui->nameLineEdit->text());
}

void MainWindow::on_createGroupPushButton_clicked()
{
    bool ok;
    QString groupName = QInputDialog::getText(this, "Create Group", "Group Name", QLineEdit::Normal, "", &ok);

    if (ok) {
        emit createGroup(groupName);
    }
}

void MainWindow::on_groupsListView_clicked(const QModelIndex &index)
{
    Q_UNUSED(index);

    P2PGroup *selGroup = getSelectedGroup();
    if (selGroup) {
        nodeModel->setNodes(selGroup->members());
    }
}

void MainWindow::on_joinGroupPushButton_clicked()
{
    P2PGroup *selGroup = getSelectedGroup();
    if (selGroup) {
        emit joinGroup(selGroup);
    }
}

void MainWindow::on_leaveGroupPushButton_clicked()
{
    P2PGroup *selGroup = getSelectedGroup();
    if (selGroup) {
        emit leaveGroup(selGroup);
    }
}
