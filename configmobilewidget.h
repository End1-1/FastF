#ifndef CONFIGMOBILEWIDGET_H
#define CONFIGMOBILEWIDGET_H

#include <QWidget>

namespace Ui {
class ConfigMobileWidget;
}

class ConfigMobileWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigMobileWidget(const QPixmap &pix, const QString &name, QWidget *parent = nullptr);
    ~ConfigMobileWidget();

private:
    Ui::ConfigMobileWidget *ui;
};

#endif // CONFIGMOBILEWIDGET_H
