#ifndef MTPRINTKITCHEN_H
#define MTPRINTKITCHEN_H

#include <QObject>
#include <QMap>

class MTPrintKitchen : public QObject
{
    Q_OBJECT
public:
    MTPrintKitchen(const QMap<QString, QString>& data, const QList<QMap<QString, QString> >& dishes, bool kitchen, QObject *parent = nullptr);
    ~MTPrintKitchen();
    void run();

protected:

    void printKitchen();
    void printReceipt();

private:
    bool fKitchen;
    QStringList fLog;
    QMap<QString, QString> fData;
    QList<QMap<QString, QString> > fDishes;
};

#endif // MTPRINTKITCHEN_H
