#ifndef QELINEEDIT_H
#define QELINEEDIT_H

#include <QLineEdit>

class QELineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit QELineEdit(QWidget *parent = 0);

signals:
    void focusEvent(bool in);

public slots:

private:
    virtual void focusInEvent();
    virtual void focusOutEvent();

};

#endif // QELINEEDIT_H
