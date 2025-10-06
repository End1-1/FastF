#include "qplineedit.h"

QPLineEdit::QPLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    m_multiEdit = false;
}

QString &QPLineEdit::getField()
{
    return m_field;
}

bool &QPLineEdit::getMultiEdit()
{
    return m_multiEdit;
}

void QPLineEdit::setField(QString &f)
{
    m_field = f;
}

void QPLineEdit::setMultiEdit(bool &m)
{
    m_multiEdit = m;
}

void QPLineEdit::focusInEvent(QFocusEvent *e)
{
    emit focusEvent(true);
    QLineEdit::focusInEvent(e);
}

void QPLineEdit::focusOutEvent(QFocusEvent *e)
{
    emit focusEvent(false);
    QLineEdit::focusOutEvent(e);
}



QPColorEdit::QPColorEdit(QWidget *parent) :
    QPLineEdit(parent)
{
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(textChange(QString)));
    setReadOnly(true);
}

void QPColorEdit::textChange(const QString &text)
{
    QPalette p(palette());
    p.setColor(backgroundRole(), QColor::fromRgb(text.toInt()));
    setPalette(p);
}
