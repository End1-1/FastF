#ifndef FF_SETTINGSDRV_H
#define FF_SETTINGSDRV_H

#include "qsqldrv.h"
#include <QDate>

#define SD_EMPTY QObject::tr("EMPTY")
#define SD_CASH_TIME QObject::tr("CASH_TIME")
#define SD_SERVICE_VALUE QObject::tr("SERVICE_VALUE")
#define SD_CHECK_PRINTER_NAME QObject::tr("CHECK_PRINTER_NAME")
#define SD_OBJECT_NAME QObject::tr("OBJECT_NAME")
#define SD_LOGO_FILENAME QObject::tr("LOGO_FILENAME")
#define SD_SERIVCE_CHECK_PAY QObject::tr("SERVICE_CHECK_PAY")
#define SD_DEFAULT_HALL_ID QObject::tr("DEFAULT_HALL_ID")
#define SD_DEFAULT_MENU_ID QObject::tr("DEFAULT_MENU_ID")
#define SD_REMINDER QObject::tr("REMINDER")
#define SD_AVAILABLE_HALL QObject::tr("Available halls")
#define SD_AVAILABLE_MENU QObject::tr("Available menu")
#define SD_UPDATE_TIME_FROM_SERVER QObject::tr("Update time from server")
#define SD_UPDATE_TIME_SERVER QObject::tr("Time server")
#define SD_PRINT_QRCODE QObject::tr("Print QR Code")
#define SD_ORDER_ID_PREFIX QObject::tr("Order id prefix")
#define SD_BTN_LOCKSTATIONS QObject::tr("Lock stations button")
#define SD_AVAILABLE_PRESENT QObject::tr("Available present")
#define SD_AUTODISCOUNT_V1 QObject::tr("Auto discount v1")
#define SD_IDRAM_SESSION_ID QObject::tr("IDram session id")
#define SD_IDRAM_ID QObject::tr("IDram id")
#define SD_IDRAM_PHONE QObject::tr("IDram phone")
#define SD_DISCOUNT_APP_QUERY QObject::tr("Discount app query")
#define SD_DISH_CORRECTION_RED_TIMEOUT QObject::tr("Dish correction red timeout")

#define SD_TAX_PRINT_SERVER QObject::tr("PRINT_TAX_SERVER")
#define SD_TAX_PRINT QObject::tr("PRINT_TAX")
#define SD_TAX_PRINT_FREQ QObject::tr("PRINT_TAX_FREQ")
#define SD_TAX_PRINT_IP QObject::tr("PRINT_TAX_IP")
#define SD_TAX_PRINT_PORT QObject::tr("PRINT_TAX_PORT")
#define SD_TAX_PRINT_PASS QObject::tr("PRINT_TAX_PASS")
#define SD_TAX_PRINT_TERMINAL_DEP QObject::tr("PRINT_TAX_TERMINAL_DEP")
#define SD_TAX_HALL_ID QObject::tr("PRINT_TAX_HALL_ID")

class FF_SettingsDrv : public QSqlDrv
{
    Q_OBJECT

private:
    static int m_version;
    static QMap<int, QString> m_table_options1; // overwrite table default service value

public:
    static int m_id;
    static QMap<int, QStringList> m_printSchema;
    static QMap<QString, QVariant> m_settings;
    explicit FF_SettingsDrv();
    static QVariant &value(const QString &name);
    static QDate cashDate();
    static void init();
    static int id();
    static void save(const QString &valueName, const QVariant &value, QSqlDrv *d);

    static bool tableOption1(int id, double &value);

signals:

public slots:

};

#endif // FF_SETTINGSDRV_H
