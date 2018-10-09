#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QStringList>

extern "C" Q_DECL_EXPORT QString caption();
extern "C" Q_DECL_EXPORT QString sql();
extern "C" Q_DECL_EXPORT QStringList fields();
extern "C" Q_DECL_EXPORT QStringList totalFields();

#endif // MAINWINDOW_H
