#ifndef QLOGWINDOW_H
#define QLOGWINDOW_H

#include <QMainWindow>

namespace Ui {
class QLogWindow;
}

class QLogWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit QLogWindow(QWidget *parent = 0);
    ~QLogWindow();
    void log(const QString &log);
    static void writeLog(const QString &log);

private:
    Ui::QLogWindow *ui;
};

#endif // QLOGWINDOW_H
