#ifndef QDLGQUERY_H
#define QDLGQUERY_H

#include "qdlgbase.h"

class QDlgQuery : public QDlgBase
{
    Q_OBJECT
public:
    explicit QDlgQuery(QMap<QString, QList<QVariant> > &values, QWidget *parent = 0);
    void setTable(const QString &table, const QList<SqlField *> fields);
    void mapValuesToWidget();
    void mapWidgetToValues();
    QString mCurrentCode;
    QString mCurrentCode1;
    QString mCurrentCode2;
    bool mNext;
    virtual void setNext(bool next, const QString &code);
signals:

public slots:

private:
    QMap<QString, QList<QVariant> > &m_values;

protected:
    QString m_tableName;
    virtual bool checkOk();
    virtual void btnOK();

};

#endif // QDLGQUERY_H
