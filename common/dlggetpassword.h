#ifndef DLGGETPASSWORD_H
#define DLGGETPASSWORD_H

#include <QDialog>

namespace Ui {
class DlgGetPassword;
}

class DlgGetPassword : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgGetPassword(QWidget *parent = 0);
    ~DlgGetPassword();
    static bool password(const QString &caption, QString &out, bool echopass, QWidget *parent);
    void setWindowTitle(const QString &title);
    
private:
    Ui::DlgGetPassword *ui;

private slots:
    void keyEnter();
};

#endif // DLGGETPASSWORD_H
