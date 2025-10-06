#ifndef QETABLEWIDGET_H
#define QETABLEWIDGET_H

#include <QTableWidget>
#include <QPushButton>

class QETableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit QETableWidget(QWidget *parent = 0);
    void setButtons(QPushButton *btnUp, QPushButton *btnDown);

signals:

public slots:
    void pageUp();
    void pageDown();


private:
    void scrollPage(int direction);

};

#endif // QETABLEWIDGET_H
