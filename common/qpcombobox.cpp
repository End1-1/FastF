#include "qpcombobox.h"
#include "qsqlcache.h"

QPComboBox::QPComboBox(QWidget *parent) :
    QComboBox(parent)
{
    m_multiEdit = false;
}

void QPComboBox::setIndexOfData(int id)
{
    QString text = QSqlCache().getCache(m_table).value(id);
    setCurrentIndex(findText(text));
}

QVariant QPComboBox::itemData(int index, int role) const
{
    Q_UNUSED(role)
    QSqlCache sqlCache;
    return sqlCache.getCache(m_table).key(itemText(index));
}

int QPComboBox::currentItemData()
{
    if (currentIndex() < 0)
        return 0;
    else
        return itemData(currentIndex()).toInt();
}

void QPComboBox::setFieldId(const QString &fieldId)
{
    m_fieldId = fieldId;
}

void QPComboBox::setFieldName(const QString &fieldName)
{
    m_fieldName = fieldName;
}

void QPComboBox::setMultiEdit(bool multiEdit)
{
    m_multiEdit = multiEdit;
}

void QPComboBox::setTable(const QString &table)
{
    m_table = table;
    if (m_table.length()) {
        QSqlCache sqlCache;
        QStringList cache = sqlCache.getCache(m_table).values();
        cache.sort(Qt::CaseInsensitive);
        addItems(cache);
    }
}

QString &QPComboBox::getFieldId()
{
    return m_fieldId;
}

QString &QPComboBox::getFieldName()
{
    return m_fieldName;
}

bool &QPComboBox::getMultiEdit()
{
    return m_multiEdit;
}

QString &QPComboBox::getTable()
{
    return m_table;
}
