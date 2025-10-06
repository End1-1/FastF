#include "SqlField.h"
#include "qsystem.h"
#include <QLineEdit>
#include <QDateEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QTimeEdit>

QString Filter::whereClause() const
{
    if (listValues.length())
        return QString(" and %1 in (%2)").arg(fieldName).arg(listValues);
    else
        return "";
}

QWidget *Filter::widget(QWidget *parent)
{
    switch (m_filterType) {
    case ftNone:
        return 0;
    case ftEdit: {
        m_widget = new QLineEdit(parent);
        connect(qobject_cast<QLineEdit*>(m_widget), SIGNAL(textChanged(QString)), this, SLOT(slotUpdateValues(QString)));
        return m_widget;
    }
    case ftDate: {
        m_widget = new QDateEdit(QDate::fromString(listValues, DATE_FORMAT));
        qobject_cast<QDateEdit*>(m_widget)->setCalendarPopup(true);
        connect(qobject_cast<QDateEdit*>(m_widget), SIGNAL(dateChanged(QDate)), this, SLOT(slotDateChanged(QDate)));
        return m_widget;
    }
    case ftDateTime: {
        m_widget = new QDateTimeEdit(QDateTime::fromString(listValues, DATETIME_FORMAT));
        qobject_cast<QDateTimeEdit*>(m_widget)->setCalendarPopup(true);
        connect(qobject_cast<QDateTimeEdit*>(m_widget), SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(slotDateTimeChanged(QDateTime)));
        return m_widget;
    }
    case ftTime: {
        m_widget = new QTimeEdit(QTime::fromString(listValues, TIME_FORMAT));
        connect(qobject_cast<QTimeEdit*>(m_widget), SIGNAL(timeChanged(QTime)), this, SLOT(slotTimeChanged(QTime)));
        return m_widget;
    }
    default:
        return 0;
    }
}

QWidget *Filter::button(QWidget *parent)
{
    QPushButton *btn = new QPushButton("...", parent);
    btn->setMaximumSize(QSize(25, 25));
    btn->setVisible(m_showButton);
    connect(btn, SIGNAL(clicked()), SLOT(btnClick()));
    return btn;
}

QWidget *Filter::checkbox(QWidget *parent)
{
    QCheckBox *c = new QCheckBox("", parent);
    c->setChecked(isIncluded());
    c->setVisible(m_showCheckBox);
    connect(c, SIGNAL(clicked(bool)), this, SLOT(checkBoxClick(bool)));
    return c;
}

void Filter::updateWidget()
{
    switch (m_filterType) {
    case ftEdit: {
        qobject_cast<QLineEdit*>(m_widget)->setText(tempValues);
        qobject_cast<QLineEdit*>(m_widget)->setToolTip(tempNames);
        break;
    }
    case ftDate: {
        qobject_cast<QDateEdit*>(m_widget)->setDate(QDate::fromString(tempValues, DATE_FORMAT));
        break;
    }
    default:
        break;
    }
}

Filter::Filter(FilterType type, bool showButton)
{
    m_includeInGroup = true;
    m_includeInGroupRef = 0;
    m_filterType = type;
    m_showButton = showButton;
    m_showCheckBox = showButton;
    listNames = "";
    listValues = "";
    m_NamesEqValues = false;
}

Filter::~Filter()
{
}

Filter &Filter::operator=(const Filter &src)
{
    caption = src.caption;
    fieldName = src.fieldName;
    listValues = src.listValues;
    listNames = src.listNames;
    tempValues = src.tempValues;
    tempNames = src.tempNames;
    m_filterType = src.m_filterType;
    m_showButton = src.m_showButton;
    m_widget = src.m_widget;
    m_includeInGroup = src.m_includeInGroup;
    m_includeInGroupRef = src.m_includeInGroupRef;
    return *this;
}

bool Filter::isIncluded()
{
    if (m_includeInGroupRef)
        return m_includeInGroupRef->m_includeInGroup;
    return m_includeInGroup;
}

void Filter::btnClick()
{
    emit viewCache(cache);
}

void Filter::checkBoxClick(bool checked)
{
    m_includeInGroup = checked;
}

void Filter::slotDateChanged(QDate date)
{
    tempValues = date.toString(DATE_FORMAT);
}

void Filter::slotDateTimeChanged(QDateTime dateTime)
{
    tempValues = dateTime.toString(DATETIME_FORMAT);
}

void Filter::slotTimeChanged(QTime time)
{
    tempValues = time.toString(TIME_FORMAT);
}

void Filter::slotUpdateValues(const QString &str)
{
    tempValues = str;
}


SqlField::SqlField()
{
    name = "";
    title = "";
    width = 0;
    value = QVariant();
    dataType = QVariant::String;
    widget = 0;
    multiedit = false;
    m_filter = 0;
}

SqlField::SqlField(QString n, QString t, int w, QVariant v, int d, QWidget *wi, bool m)
{
    name = n;
    title = t;
    width = w;
    value = v;
    dataType = d;
    widget = wi;
    multiedit = m;
    m_filter = new Filter(Filter::ftNone, false);
}

SqlField::SqlField(const SqlField &src)
{
    m_filter = 0;
    *this = src;
}

SqlField::~SqlField()
{
    if (m_filter) {
        delete m_filter;
        m_filter = 0;
    }
}

void SqlField::addFilter(Filter::FilterType filterType, bool showButton, const QString &cacheName)
{
    if (!m_filter)
        m_filter = new Filter(filterType, showButton);
    m_filter->m_filterType = filterType;
    m_filter->m_showButton = showButton;
    m_filter->cache = cacheName;
    m_filter->caption = title;
    m_filter->fieldName = name;
}

SqlField &SqlField::operator=(const SqlField &src)
{
    name = src.name;
    title = src.title;
    width = src.width;
    value = src.value;
    dataType = src.dataType;
    widget = src.widget;
    multiedit = src.multiedit;
    if (m_filter && src.m_filter)
        *m_filter = *src.m_filter;
    else if (m_filter && !src.m_filter) {
            delete m_filter;
            m_filter = nullptr;
    } else if (!m_filter && src.m_filter) {
        m_filter = new Filter(Filter::ftNone, false);
        *m_filter = *src.m_filter;
    }
    return *this;
}

void SqlField::setIncludeRef(SqlField *field)
{
    m_filter->m_includeInGroup = false;
    m_filter->m_includeInGroupRef = field->m_filter;
}

void SqlField::setInclude(bool include)
{
    m_filter->m_includeInGroup = include;
}

bool SqlField::isIncluded()
{
    return m_filter->isIncluded();
}

bool SqlField::haveFilter(QString &where, const QString &fieldName)
{
    if (!m_filter)
        return false;
    if (m_filter->listValues.length())
        where += " " + fieldName + " in (" + m_filter->listValues + ") and";
    return m_filter->listValues.length();
}

void SqlField::sqlAddFrom(QString &srcTable, QString &srcWhere, const QString &table, const QString &where)
{
    if (!srcTable.contains(table))
        srcTable += table + ",";
    if (!srcWhere.contains(where))
        srcWhere += " " + where + " and";
}

bool SqlField::sqlAddCond(QString &src, const QString &field)
{
    if (m_filter->listValues.length()) {
        src += QString(" %1 in (%2) and").arg(field).arg(m_filter->listValues);
        return true;
    }
    return false;
}

void SqlField::sqlLeftJoin(QString &join, const QString &table, const QString &cond)
{
    if (join.contains(table))
        return;
    join += " left join " + table + " on " + cond + " ";
}

void SqlField::sqlRightJoin(QString &join, const QString &table, const QString &cond)
{
    if (join.contains(table))
        return;
    join += " right join " + table + " on " + cond + " ";
}
