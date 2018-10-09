#ifndef LOGTHREAD_H
#define LOGTHREAD_H

#include <QThread>

class LogThread : public QThread
{
    Q_OBJECT
public:
    LogThread(QObject *parent = 0);
    ~LogThread();
    static void logOrderThread(int user, const QString &order, const QString &action, const QString &data);
    static void logDiscountFailureThread(int user, const QString &code);
protected:
    virtual void run();
private:
    enum LogMode {logOrder, logDiscountFailure};
    LogMode fLogMode;
    int fUser;
    QString fOrder;
    QString fAction;
    QString fData;
};

extern QString LogDb;
extern QString LogUser;
extern QString LogPass;

#endif // LOGTHREAD_H
