#ifndef JSONUTILS_H
#define JSONUTILS_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

class JSONUtils
{
public:
    JSONUtils();
private:
    QJsonDocument fDoc;
    QString fResult;
};

#endif // JSONUTILS_H
