#include "qelineedit.h"

QELineEdit::QELineEdit(QWidget *parent) :
    QLineEdit(parent)
{
}

void QELineEdit::focusInEvent()
{
    emit focusEvent(true);
}

void QELineEdit::focusOutEvent()
{
    emit focusEvent(false);
}
