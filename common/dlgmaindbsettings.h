#ifndef DLGMAINDBSETTINGS_H
#define DLGMAINDBSETTINGS_H

#include "core.h"

namespace Ui {
class dlgMainDBSettings;
}

class dlgMainDBSettings : public QDialog
{
    Q_OBJECT
    
public:
    explicit dlgMainDBSettings(QWidget *parent = 0);
    ~dlgMainDBSettings();
    
private slots:
    void takeConfiguration(const QString &qName, const QXmlAttributes &atts);
    void endOfTakeConfiguration(int code, const QString &message);
    void on_btnCancel_clicked();

    void on_btnSave_clicked();

    void on_btnGetConfigurationList_clicked();

private:
    Ui::dlgMainDBSettings *ui;
};

#endif // DLGMAINDBSETTINGS_H
