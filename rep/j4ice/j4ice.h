#ifndef J4ICE_H
#define J4ICE_H

#include <QObject>
#include <QStringList>

extern "C" Q_DECL_EXPORT QString caption();
extern "C" Q_DECL_EXPORT QString sql();
extern "C" Q_DECL_EXPORT QStringList fields();
extern "C" Q_DECL_EXPORT QStringList totalFields();

#endif // J4ICE_H
