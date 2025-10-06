#include "utils.h"
#include <QSettings>

#ifdef WIN32
#include <windows.h>
#endif

QMap<QString, QString> Utils::fValues;

Utils::Utils()
{

}

void Utils::init()
{
    QSettings s;
    s.setValue(s__server_address, "10.2.1.2");
    s.setValue(s__server_port, 888);
    fValues[s__server_address] = s.value(s__server_address).toString();
    fValues[s__server_port] = s.value(s__server_port).toString();
}

QString Utils::getString(const QString &name)
{
    return fValues[name];
}

int Utils::getInt(const QString &name)
{
    return fValues[name].toInt();
}

QString Utils::getVersionString(const QString name)
{
#ifdef WIN32
    // first of all, GetFileVersionInfoSize
    DWORD dwHandle;
    DWORD dwLen = GetFileVersionInfoSize(name.toStdWString().c_str(), &dwHandle);

    // GetFileVersionInfo
    BYTE *lpData = new BYTE[dwLen];
    if(!GetFileVersionInfo(name.toStdWString().c_str(), dwHandle, dwLen, lpData)) {
        delete[] lpData;
        return "";
    }

    // VerQueryValue
    VS_FIXEDFILEINFO *lpBuffer = NULL;
    UINT uLen;
    if(VerQueryValue(lpData, QString("\\").toStdWString().c_str(), (LPVOID*)&lpBuffer, &uLen)) {
        return
            QString::number((lpBuffer->dwFileVersionMS >> 16) & 0xffff) + "." +
            QString::number((lpBuffer->dwFileVersionMS) & 0xffff ) + "." +
            QString::number((lpBuffer->dwFileVersionLS >> 16 ) & 0xffff ) + "." +
            QString::number((lpBuffer->dwFileVersionLS) & 0xffff );
    }
#endif
    return name;
}

