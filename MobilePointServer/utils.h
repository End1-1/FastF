#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QMap>
#include <QDebug>

#define s__server_address "s__server_address"
#define s__server_port "s__server_port"
#define s__udp_server_port "s__udp_server_port"

class Utils
{
public:
    Utils();
    static void init();
    static QMap<QString, QString> fValues;
    static QString getString(const QString &name);
    static int getInt(const QString &name);
    static QString getVersionString(const QString name);
};

#endif // UTILS_H
