#ifndef QSQLCACHE_H
#define QSQLCACHE_H

#include "qbasesqldriver.h"


class QSqlCache : public QBaseSqlDriver
{
public:
    QSqlCache();
    QMap<int, QString> &getCache(const QString &name);
    QStringList getCacheValues(const QString &name);
    static QString getValueByKey(const QString &cache, int key);
    static void removeChache(const QString &name);
    bool updateCacheItem(int key, const QString &value, const QString &cacheName, bool loadCache = false);
    static void clear();

private:
    static QMap<QString, QMap<int, QString> > m_cache;
};

#endif // QSQLCACHE_H
