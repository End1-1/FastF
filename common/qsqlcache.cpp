#include "qsqlcache.h"

QMap<QString, QMap<int, QString> > QSqlCache::m_cache;

QSqlCache::QSqlCache() :
    QBaseSqlDriver()
{
}

QMap<int, QString> &QSqlCache::getCache(const QString &name)
{
    if (m_cache.contains(name))
        return m_cache[name];

    m_cache[name] = QMap<int, QString>();

    if (!m_sqlDriver->prepare("select id, name from " + name + " order by name"))
        return m_cache[name];
    if (!m_sqlDriver->execSQL())
        return m_cache[name];
    while (m_sqlDriver->m_query->next())
        m_cache[name][m_sqlDriver->m_query->value(0).toInt()] = m_sqlDriver->m_query->value(1).toString();
    m_sqlDriver->close();
    return m_cache[name];
}

QStringList QSqlCache::getCacheValues(const QString &name)
{
    QMap<int, QString> &cache = getCache(name);
    QStringList values = cache.values();
    values.sort();
    return values;
}

QString QSqlCache::getValueByKey(const QString &cache, int key)
{
    QSqlCache sqlCache;
    QMap<int, QString> &c = sqlCache.getCache(cache);
    if (!c.contains(key))
        return "";
    return c.value(key);
}

void QSqlCache::removeChache(const QString &name)
{
    m_cache.remove(name);
}

bool QSqlCache::updateCacheItem(int key, const QString &value, const QString &cacheName, bool loadCache)
{
    if (!m_cache.contains(cacheName)) {
        if (!loadCache)
            return false;
    }
    QMap<int, QString> &cache = getCache(cacheName);
    QMap<int, QString>::iterator it = cache.find(key);
    it.value() = value;
    return true;
}

void QSqlCache::clear()
{
    m_cache.clear();
}
