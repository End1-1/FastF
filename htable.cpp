#include "htable.h"
#include "database.h"
#include "classes.h"

HTable::HTable(QObject *parent) : QObject{parent}
{
}

bool HTable::tryLock(int id)
{
    Database db;

    if(!db.startTransaction()) {
        return false;
    }

    db[":id"] = id;
    db.exec("select * from h_table where id=:id with lock ");

    if(!db.next()) {
        db.commit();
        return false;
    }

    if(!db.string("LOCK_HOST").isEmpty() && db.string("LOCK_HOST") != HOSTNAME) {
        db.commit();
        return false;
    }

    db[":id"] = id;
    db[":lock_host"] = HOSTNAME;

    if(!db.exec("update h_table set lock_host=:lock_host where id=:id")) {
        db.rollback();
        return false;
    }

    db.commit();
    return true;
}

void HTable::unlock(int id)
{
    Database db;
    db.open();
    db.startTransaction();
    db[":id"] = id;
    db[":lock_host"] = HOSTNAME;
    db.exec("update h_table set lock_host=null where lock_host=:lock_host and id=:id");
    db.commit();
}
