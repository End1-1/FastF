#ifndef KITREMINDERTHREAD_H
#define KITREMINDERTHREAD_H

#include <QThread>

class KitReminderThread : public QThread
{
public:
    KitReminderThread(const QStringList &list, QObject *parent = nullptr);

protected:
    virtual void run() override;

private:
    QStringList fData;
};

#endif // KITREMINDERTHREAD_H
