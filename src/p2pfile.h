#ifndef P2PFILE_H
#define P2PFILE_H

#include <QObject>
#include <QUuid>

class P2PFile : public QObject
{
    Q_OBJECT

    QUuid _uuid;
    QString _name;
    qint64 _size;

public:
    P2PFile(QUuid uuid, QString name, qint64 size, QObject *parent = 0);
    ~P2PFile();

    const QUuid &uuid() const;
    const QString &name() const;
    qint64 size() const;

signals:

public slots:
};

#endif // P2PFILE_H
