#ifndef MPDRAFTSOCKET_H
#define MPDRAFTSOCKET_H

#include <QThread>

class MPDraftSocket : public QThread
{
    Q_OBJECT
public:
    MPDraftSocket(QObject *parent = 0);

public slots:

};

#endif // MPDRAFTSOCKET_H
