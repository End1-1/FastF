#ifndef LOGTHREAD_H
#define LOGTHREAD_H

#include <QThread>

class LogThread : public QThread
{
    Q_OBJECT
public:
    LogThread(QObject *parent = 0);
    ~LogThread();
    static void logOrderThread(const QString &user, const QString &order, const QString &body, const QString &action, const QString &data);
    static void logDiscountFailureThread(int user, const QString &code);
protected:
    virtual void run();
private:
    enum LogMode {logOrder, logDiscountFailure};
    LogMode fLogMode;
    QString fUser;
    QString fOrder;
    QString fBody;
    QString fAction;
    QString fData;
};

extern QString LogDb;
extern QString LogUser;
extern QString LogPass;

#endif // LOGTHREAD_H
