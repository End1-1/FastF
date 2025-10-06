#ifndef QPLINEEDIT_H
#define QPLINEEDIT_H

#include <QLineEdit>

class QPLineEdit : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY (QString field READ getField WRITE setField)
    Q_PROPERTY (bool multiEdit READ getMultiEdit WRITE setMultiEdit)

public:
    explicit QPLineEdit(QWidget *parent = 0);
    QList<QVariant> m_data;

signals:
    void focusEvent(bool focused);

private:
    QString m_field;
    bool m_multiEdit;
    QString &getField();
    bool &getMultiEdit();
    void setField(QString &f);
    void setMultiEdit(bool &m);

protected:
    virtual void focusInEvent(QFocusEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);

};

class QPColorEdit : public QPLineEdit {
    Q_OBJECT
public:
    explicit QPColorEdit(QWidget *parent = 0);

private slots:
    virtual void textChange(const QString &text);
};

#endif // QPLINEEDIT_H
