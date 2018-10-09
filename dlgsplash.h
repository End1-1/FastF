#ifndef DLGSPLASH_H
#define DLGSPLASH_H

#include <QDialog>

namespace Ui {
class DlgSplash;
}

class DlgSplash : public QDialog
{
    Q_OBJECT

public:
    explicit DlgSplash(QWidget *parent = 0);
    ~DlgSplash();
    void setText(const QString &text);

private:
    Ui::DlgSplash *ui;
};

#endif // DLGSPLASH_H
