#ifndef KARAOKE_SALE_H
#define KARAOKE_SALE_H

#include <QString>
#include <QMap>

class OD_Drv;

extern "C" Q_DECL_EXPORT QString caption();
extern "C" Q_DECL_EXPORT bool filterDate();
extern "C" Q_DECL_EXPORT bool exec(const QMap<QString, QString> &filter, OD_Drv *o, QString &msg);

#endif // KARAOKE_SALE_H
