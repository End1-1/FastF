#include "ff_settingsdrv.h"
#include "cnfmaindb.h"
#include <QTime>
#include <QDir>

QMap<QString, QVariant> FF_SettingsDrv::m_settings;
QMap<int, QStringList> FF_SettingsDrv::m_printSchema;
QMap<int, QString> FF_SettingsDrv::m_table_options1;
int FF_SettingsDrv::m_id;
int FF_SettingsDrv::m_version;

FF_SettingsDrv::FF_SettingsDrv() :
    QSqlDrv("FASTFF", "main")
{

}

QVariant &FF_SettingsDrv::value(const QString &name)
{
    if (!m_settings.contains(name))
        return m_settings[SD_EMPTY];

    return m_settings[name];
}

QDate FF_SettingsDrv::cashDate()
{
    QDate dateNow = QDate::currentDate();
    QTime timeNow = QTime::currentTime();
    if (timeNow < value(SD_CASH_TIME).toTime())
        dateNow = dateNow.addDays(-1);
    return dateNow;
}

void FF_SettingsDrv::init()
{
    QDir dir;
    m_settings[SD_EMPTY] = QVariant();
    m_settings[SD_CASH_TIME] = QTime::fromString("08:00:00");
    m_settings[SD_SERVICE_VALUE] = 0;
    m_settings[SD_CHECK_PRINTER_NAME] = "";
    m_settings[SD_LOGO_FILENAME] = dir.absolutePath() + "/logo.png";
    m_settings[SD_SERIVCE_CHECK_PAY] = 0;
    if (!QFile::exists(m_settings[SD_LOGO_FILENAME].toString()))
        m_settings[SD_LOGO_FILENAME] = "";
    m_settings[SD_DEFAULT_HALL_ID] = 0;
    m_settings[SD_DEFAULT_MENU_ID] = 0;
    m_settings[SD_REMINDER] = 0;
    m_settings[SD_AVAILABLE_HALL] = "";
    m_settings[SD_AVAILABLE_MENU] = "";
    m_settings[SD_UPDATE_TIME_FROM_SERVER] = 0;
    m_settings[SD_UPDATE_TIME_SERVER] = "";
    m_settings[SD_PRINT_QRCODE] = 0;
    m_settings[SD_ORDER_ID_PREFIX] = "N";
    m_settings[SD_MESSANGER_MAX_ID] = 0;
    m_settings[SD_AVAILABLE_PRESENT] = "";
    m_settings[SD_AUTODISCOUNT_V1] = "";
    m_settings[SD_IDRAM_SESSION_ID] = "";
    m_settings[SD_IDRAM_ID] = "100000502";
    m_settings[SD_IDRAM_PHONE] = "";
    m_settings[SD_DISCOUNT_APP_QUERY] = "jazzve_";

    m_settings[SD_TAX_PRINT] = "0";
    m_settings[SD_TAX_PRINT_FREQ] = "1";
    m_settings[SD_TAX_PRINT_IP] = "0.0.0.0";
    m_settings[SD_TAX_PRINT_PORT] = "0";
    m_settings[SD_TAX_PRINT_PASS] = "";
    m_settings[SD_TAX_PRINT_SERVER] = "0";
    m_settings[SD_TAX_PRINT_TERMINAL_DEP] = "2";
    m_settings[SD_TAX_HALL_ID] = "0";

    FF_SettingsDrv s;

    if (!s.prepare("select id, version from sys_settings where name=:name"))
        return;
    s.bind(":name", "W:" + getLockName());
    if (!s.execSQL())
        return;

    if (s.m_query->next()) {
        m_id = s.m_query->value(0).toInt();
        m_version = s.m_query->value(1).toInt();
    } else {
        m_id = s.genId("GEN_SYS_SETTINGS_ID");
        m_version = 1;
        if (!s.prepare("insert into sys_settings values (:id, :module_id, :name, :version)"))
            return;
        s.bind(":id", m_id);
        s.bind(":module_id", 2);
        s.bind(":name", "W:" + getLockName());
        s.bind(":version", m_version);
        if (!s.execSQL())
            return;
        s.prepare("insert into sys_settings_values (settings_id, key_name, key_value) values (:settings_id, :key_name, :key_value)");
        for (QMap<QString, QVariant>::const_iterator it = m_settings.begin(); it != m_settings.end(); it++) {
            s.bind(":settings_id", m_id);
            s.bind(":key_name", it.key());
            s.bind(":key_value", it.value().toString());
            s.execSQL();
        }
    }

    QStringList keys;
    s.prepare("select key_name, key_value from sys_settings_values where settings_id=:settings_id");
    s.bind(":settings_id", m_id);
    s.execSQL();
    while (s.m_query->next()) {
        m_settings[s.valStr("KEY_NAME")] = s.m_query->value("KEY_VALUE");
        keys.append(s.valStr("KEY_NAME"));
    }

    /* Update settings keys, insert new and delete old */
    s.prepare("insert into sys_settings_values (settings_id, key_name, key_value) values (:settings_id, :key_name, :key_value)");
    QStringList list = m_settings.keys();
    for (QStringList::const_iterator i = list.begin(); i != list.end(); i++)
        if (!keys.contains(*i)) {
            s.bind(":settings_id", m_id);
            s.bind(":key_name", *i);
            s.bind(":key_value", m_settings[*i]);
            s.execSQL();
        }

    if (!s.prepare("select schema_id, name from me_printers"))
        return;
    if (!s.execSQL())
        return;
    m_printSchema.clear();
    while (s.m_query->next()) {
        int printSchemaId = s.m_query->value(0).toInt();
        if (!m_printSchema.contains(printSchemaId))
            m_printSchema.insert(printSchemaId, QStringList());
        m_printSchema[printSchemaId].append(s.m_query->value(1).toString());
    }

    s.m_sql = "select obj_name from sys_databases where upper(db_path)=upper(:db_path)";
    if (!s.prepare())
        return;
    s.bind(":db_path", __cnfmaindb.fHost + ":" + __cnfmaindb.fDatabase);
    if (!s.execSQL())
        return;
    if (s.m_query->next())
        m_settings[SD_OBJECT_NAME] = s.m_query->value(0).toString();
    else
        m_settings[SD_OBJECT_NAME] = "?";

    m_table_options1.clear();
    s.prepare("select table_id, options from h_table_option1");
    s.execSQL();
    while (s.next())
        m_table_options1[s.m_query->value(0).toInt()] = s.m_query->value(1).toString();
    s.close();

}

int FF_SettingsDrv::id()
{
    return m_id;
}

void FF_SettingsDrv::save(const QString &valueName, const QVariant &value, QSqlDrv *d)
{
    m_settings[valueName] = value;
    d->openDB();
    d->prepare("update sys_settings_values set key_value=:key_value where settings_id=:settings_id and key_name=:key_name");
    d->bind(":key_value", value);
    d->bind(":settings_id", m_id);
    d->bind(":key_name", valueName);
    d->execSQL();
    d->close();
}

bool FF_SettingsDrv::tableOption1(int id, double &value)
{
    if (!m_table_options1.contains(id))
        return false;
    value = m_table_options1[id].toFloat();
    return true;
}
