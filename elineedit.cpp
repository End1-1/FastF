#include "elineedit.h"
#include <QString>
#include <QRegularExpression>

#define float_str(value, f) QString::number(value, 'f', f).remove(QRegularExpression("\\.0+$")).remove(QRegularExpression("\\.$"))

ELineEdit::ELineEdit(QWidget *parent) :
    QLineEdit(parent)
{
}

void ELineEdit::setText(const QString &text)
{
    QLineEdit::setText(text);
}

void ELineEdit::setText(double text)
{
    QLineEdit::setText(float_str(text, 2));
}
