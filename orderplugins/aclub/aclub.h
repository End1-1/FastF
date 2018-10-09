#ifndef ACLUB_H
#define ACLUB_H

#include <QString>
#include <QMap>
#include "dbdriver.h"
#include "od_drv.h"

extern "C" Q_DECL_EXPORT QString caption();
extern "C" Q_DECL_EXPORT bool filterDate();
extern "C" Q_DECL_EXPORT bool exec(const QMap<QString, QString> &filter, OD_Drv *o, QString &msg);


#endif // ACLUB_H
