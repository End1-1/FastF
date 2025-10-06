#ifndef QPCOMBOBOX_H
#define QPCOMBOBOX_H

#include <QComboBox>

class QPComboBox : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY (QString fieldId READ getFieldId WRITE setFieldId)
    Q_PROPERTY (QString fieldName READ getFieldName WRITE setFieldName)
    Q_PROPERTY (bool multiEdit READ getMultiEdit WRITE setMultiEdit)
    Q_PROPERTY (QString table READ getTable WRITE setTable)

public:
    explicit QPComboBox(QWidget *parent = 0);
    void setIndexOfData(int id);
    QVariant itemData(int index, int role = Qt::UserRole) const;
    int currentItemData();

signals:

public slots:

private:
    QString m_fieldId;
    QString m_fieldName;
    bool m_multiEdit;
    QString m_table;
    void setFieldId(const QString &fieldId);
    void setFieldName(const QString &fieldName);
    void setMultiEdit(bool multiEdit);
    void setTable(const QString &table);
    QString &getFieldId();
    QString &getFieldName();
    bool &getMultiEdit();
    QString &getTable();
};

#endif // QPCOMBOBOX_H
