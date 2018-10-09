#ifndef FF_MESSANGER_H
#define FF_MESSANGER_H

#include <QThread>

#define MSG_UPDATE_PRINTERS_LIST 9

class FF_Messanger : public QThread
{
    Q_OBJECT

public:
    FF_Messanger(QObject *parent = 0);
    static void updatePrintersList();
    static QMap<int, QString> msg;

protected:
    void run();
};

#endif // FF_MESSANGER_H
