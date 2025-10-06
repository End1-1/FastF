#ifndef SQLFIELD_H
#define SQLFIELD_H

#include <QString>
#include <QVariant>
#include <QWidget>
#include <QDate>

class Filter : public QObject {
private:
    Q_OBJECT

public:
    enum FilterType {ftNone = 0, ftEdit, ftDateTime, ftDate, ftTime};
    QString caption;
    QString fieldName;
    QString listValues;
    QString listNames;
    QString tempValues;
    QString tempNames;
    QString cache;
    QString whereClause() const;
    FilterType m_filterType;
    bool m_includeInGroup;
    Filter *m_includeInGroupRef;
    bool m_showButton;
    bool m_showCheckBox;
    bool m_NamesEqValues;

    QWidget *widget(QWidget *parent);
    QWidget *button(QWidget *parent);
    QWidget *checkbox(QWidget *parent);
    void updateWidget();

    Filter(FilterType type, bool showButton);
    ~Filter();
    Filter &operator=(const Filter &src);
    bool isIncluded();

private:
    QWidget *m_widget;

private slots:
    void btnClick();
    void checkBoxClick(bool checked);
    void slotDateChanged(QDate date);
    void slotDateTimeChanged(QDateTime dateTime);
    void slotTimeChanged(QTime time);
    void slotUpdateValues(const QString &str);

signals:
    void viewCache(QString &cacheName);
};

class SqlField {
public:
    QString name;
    QString title;
    int width;
    QVariant value;
    int dataType;
    QWidget *widget;
    bool multiedit;
    Filter *m_filter;

    SqlField();
    SqlField(QString n, QString t, int w, QVariant v, int d, QWidget *wi, bool m);
    SqlField(const SqlField &src);
    ~SqlField();
    void addFilter(Filter::FilterType filterType, bool showButton, const QString &cacheName);
    SqlField &operator=(const SqlField &src);
    void setIncludeRef(SqlField *field);
    void setInclude(bool include);
    bool isIncluded();
    bool haveFilter(QString &where, const QString &fieldName);
    void sqlAddFrom(QString &srcTable, QString &srcWhere, const QString &table, const QString &where);
    bool sqlAddCond(QString &src, const QString &field);
    void sqlLeftJoin(QString &join, const QString &table, const QString &cond);
    void sqlRightJoin(QString &join, const QString &table, const QString &cond);
};

#endif // SQLFIELD_H
