#include "mainwindow.h"
#include <QApplication>
#include <QSettings>

#include "p2pclient.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("nfx.fi");
    QCoreApplication::setOrganizationDomain("nfx.fi");
    QCoreApplication::setApplicationName("fileflow");

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings settings;

    P2PClient client(QHostAddress::Any, settings.value("connection/port", 5678).value<quint16>());
    MainWindow w;

    QObject::connect(&client, SIGNAL(nodeArrives(P2PNode*)), &w, SLOT(addNode(P2PNode*)));
    QObject::connect(&client, SIGNAL(nodeLeaves(QUuid)), &w, SLOT(removeNode(QUuid)));
    QObject::connect(&client, SIGNAL(connected(P2PNode*)), &w, SLOT(setConnection(P2PNode*)));
    QObject::connect(&client, SIGNAL(groupCreated(P2PGroup*)), &w, SLOT(addGroup(P2PGroup*)));
    QObject::connect(&client, SIGNAL(groupRemoved(QUuid)), &w, SLOT(removeGroup(QUuid)));
    QObject::connect(&client, SIGNAL(groupJoined(P2PNode*,P2PGroup*)), &w, SLOT(addNodeToGroup(P2PNode*,P2PGroup*)));
    QObject::connect(&client, SIGNAL(groupLeft(P2PNode*,P2PGroup*)), &w, SLOT(removeNodeFromGroup(P2PNode*,P2PGroup*)));
    QObject::connect(&client, SIGNAL(nameChanged(P2PNode*)), &w, SLOT(updateNode(P2PNode*)));

    QObject::connect(&w, SIGNAL(closed()), &client, SLOT(stop()));
    QObject::connect(&w, SIGNAL(nameChanged(QString)), &client, SLOT(setName(QString)));
    QObject::connect(&w, SIGNAL(createGroup(QString)), &client, SLOT(joinGroup(QString)));
    QObject::connect(&w, SIGNAL(shareFile(QUuid,QString)), &client, SLOT(shareFile(QUuid,QString)));
    QObject::connect(&w, SIGNAL(joinGroup(P2PGroup*)), &client, SLOT(joinGroup(P2PGroup*)));
    QObject::connect(&w, SIGNAL(leaveGroup(P2PGroup*)), &client, SLOT(leaveGroup(P2PGroup*)));

    client.start();
    w.show();

    return a.exec();
}
