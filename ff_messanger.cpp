#include "ff_messanger.h"
#include "dbdriver.h"
#include "ff_settingsdrv.h"
#include "printing.h"
#include "cnfmaindb.h"

QMap<int, QString> FF_Messanger::msg;

FF_Messanger::FF_Messanger(QObject *parent) :
    QThread(parent)
{
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void FF_Messanger::updatePrintersList()
{
    DbDriver db;
    db.configureDb(__cnfmaindb.fHost, __cnfmaindb.fDatabase, __cnfmaindb.fUser, __cnfmaindb.fPassword);
    if (!db.openDB()) {
        return;
    }
    PrinterInfo prnInfo;
    db.prepare("delete from sys_workstation_printers where workstation=:workstation");
    db.bindValue(":workstation", getLockName());
    db.execSQL();
    db.prepare("insert into sys_workstation_printers values (null, :workstation, :printer)");
    db.bindValue(":workstation", getLockName());
    for (int i = 0; i < prnInfo.m_printers.count(); i++) {
        db.bindValue(":printer", prnInfo.m_printers.at(i).printerName());
        db.execSQL();
    }
    db.closeDB();
}

void FF_Messanger::run()
{
    DbDriver db;
    db.configureDb(__cnfmaindb.fHost, __cnfmaindb.fDatabase, __cnfmaindb.fUser, __cnfmaindb.fPassword);
    if (!db.openDB()) {
        return;
    }

    int id = FF_SettingsDrv::value(SD_MESSANGER_MAX_ID).toInt();
    db.prepare("select id, type_id, message from messanger where dst=-1 and id>:id order by 1");
    db.bindValue(":id", FF_SettingsDrv::value(SD_MESSANGER_MAX_ID));
    db.execSQL();
    while (db.next()) {
        id = db.v_int(0);
        msg[db.v_int(1)] = db.v_str(2);
    }

    FF_SettingsDrv::m_settings[SD_MESSANGER_MAX_ID] = id;
    db.prepare("update sys_settings_values set key_value=:key_value where settings_id=:settings_id and key_name=:key_name");
    db.bindValue(":key_value", id);
    db.bindValue(":settings_id", FF_SettingsDrv::id());
    db.bindValue(":key_name", SD_MESSANGER_MAX_ID);
    db.execSQL();

    for (QMap<int, QString>::const_iterator it = msg.begin(); it != msg.end(); it++) {
        switch (it.key()) {
        case MSG_UPDATE_PRINTERS_LIST: {
            PrinterInfo prnInfo;
            db.prepare("delete from sys_workstation_printers where workstation=:workstation or workstation not like ('" + getHostName() + "%')");
            db.bindValue(":workstation", getLockName());
            db.execSQL();
            db.prepare("insert into sys_workstation_printers values (null, :workstation, :printer)");
            db.bindValue(":workstation", getLockName());
            for (int i = 0; i < prnInfo.m_printers.count(); i++) {
                db.bindValue(":printer", prnInfo.m_printers.at(i).printerName());
                db.execSQL();
            }
            break;
        }
        default:
            break;
        }
    }
    db.closeDB();
    quit();
}

