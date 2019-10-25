#ifndef FF_CORRECTTIME_H
#define FF_CORRECTTIME_H

#include <QThread>

class FF_CorrectTime : public QThread
{
    Q_OBJECT

private:
    int m_enabled;
    QStringList m_server;

public:
    FF_CorrectTime(int enabled, const QStringList &server);

protected:
    virtual void run() override;
};

#endif // FF_CORRECTTIME_H
