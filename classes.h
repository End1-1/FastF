#ifndef CLASSES_H
#define CLASSES_H

#include <QObject>
#include <QSettings>
#include <QJsonObject>
#include <QJsonDocument>

#define HOSTNAME QHostInfo().hostName()
#define SETTINGS(x, y) QSettings("MOBILEPOINT", "MOBILEPOINT").setValue(x, y);
#define SETVALUE(x) QSettings("MOBILEPOINT", "MOBILEPOINT").value(x)
#define float_str(value, f) QString::number(value, 'f', f).remove(QRegExp("\\.0+$")).remove(QRegExp("\\.$"))

#define DISH_STATE_NORMAL 1
#define DISH_STATE_REMOVED_NORMAL 2
#define DISH_STATE_REMOVED_PRINTED 3
#define DISH_STATE_MOVED 4
#define DISH_STATE_MOVE_PARTIALY 5
#define DISH_STATE_REMOVE_AFTER_CHECKOUT 6

typedef struct {
    int fId;
    int fHall;
    int fStaff;
    QString fName;
    QString fOrder;
    QString fAmount;
    QString fPrinted;
} Table;
Q_DECLARE_METATYPE(Table*)

typedef struct {
    int fId;
    int fPart;
    QString fName;
} DishType;
Q_DECLARE_METATYPE(DishType*)

typedef struct {
    QString fShowName;
    QString fPrinterName;
} ReceiptPrinter;
Q_DECLARE_METATYPE(ReceiptPrinter*)

typedef struct {
    int fRec;
    int fId;
    int fType;
    QString fName;
    int fRemind;
    QString fPrint1;
    QString fPrint2;
    QString fComments;
    double fPrice;
    double fQty;
    double fQtyPrint;
    int fStore;
} Dish;
Q_DECLARE_METATYPE(Dish*)

class Classes
{
public:
    Classes();
};

extern QList<ReceiptPrinter*> __RECEPT_PRINTERS;

#endif // CLASSES_H
