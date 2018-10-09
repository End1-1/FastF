#ifndef ELINEEDIT_H
#define ELINEEDIT_H

#include <QLineEdit>

class ELineEdit : public QLineEdit
{
    Q_OBJECT
public:
    ELineEdit(QWidget *parent = 0);
    void setText(const QString &text);
    void setText(double text);
};

#endif // ELINEEDIT_H
