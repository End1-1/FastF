#ifndef HTABLE_H
#define HTABLE_H

#include <QObject>

class HTable : public QObject
{
    Q_OBJECT
public:
    explicit HTable(QObject *parent = nullptr);

    bool tryLock(int id);

    void unlock(int id);

signals:
};

#endif // HTABLE_H
