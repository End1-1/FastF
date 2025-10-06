#include "fastfdialog.h"

FastfDialog::FastfDialog(QWidget *parent) :
#ifdef QT_DEBUG
    QDialog(parent)
#else
    QDialog(parent, Qt::FramelessWindowHint)
#endif
{
#ifdef QT_DEBUG
    setMaximumSize(QSize(1024, 768));
#endif
}
