#ifndef MENU_AM_H
#define MENU_AM_H

#include <QString>
#include "../../orderdrv/od_drv.h"

extern "C" Q_DECL_EXPORT QString caption();
extern "C" Q_DECL_EXPORT bool exec(const QMap<QString, QString> &filter, OD_Drv *o, QString &msg);
#endif // MENU_AM_H
