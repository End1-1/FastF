#include "elineedit.h"
#include <QString>

#define float_str(value, f) QString::number(value, 'f', f).remove(QRegExp("\\.0+$")).remove(QRegExp("\\.$"))

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
