#ifndef GATWRITER_H
#define GATWRITER_H

#include <QTimer>


class GATWriter : public QObject
{
    Q_OBJECT
private:
    QTimer m_timer;
    int m_seconds;

public:
    GATWriter();

private slots:
    void timeout();

signals:
    void readCode(const QString &code);
    void longTimeout();
};

#endif // GATWRITER_H
