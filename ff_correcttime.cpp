#include "ff_correcttime.h"
#include "dbdriver.h"
#include <windows.h>

FF_CorrectTime::FF_CorrectTime(int enabled, const QStringList &server) :
    QThread(0)
{
    m_enabled = enabled;
    m_server = server;
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void FF_CorrectTime::run()
{
    if(!m_enabled) {
        terminate();
        return;
    }

    QString dbHost, dbPath, dbUser, dbPass;

    if(m_server.count() == 4) {
        dbHost = m_server.at(0);
        dbPath = m_server.at(1);
        dbUser = m_server.at(2);
        dbPass = m_server.at(3);
    } else {
        terminate();
        return;
    }

    QDateTime d;
    DbDriver db;
    db.configureDb(dbHost, dbPath, dbUser, dbPass);

    if(db.openDB()) {
        db.execSQL("select cast(current_timestamp as timestamp) from rdb$database");

        if(db.next())
            d = db.v_dateTime(0);

        db.closeDB();
    }

    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
    LookupPrivilegeValue(NULL, SE_SYSTEMTIME_NAME, &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
    SYSTEMTIME st;
    GetSystemTime(&st);
    st.wDay = d.date().day();
    st.wMonth = d.date().month();
    st.wYear = d.date().year();
    st.wHour = d.time().hour();
    st.wMinute = d.time().minute();
    st.wSecond = d.time().second();
    st.wMilliseconds = 0;

    if(!SetLocalTime(&st)) {
        DWORD   dwLastError = GetLastError();
        TCHAR   lpBuffer[256];

        if(dwLastError != 0)    // Don't want to see a "operation done successfully" error ;-)
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,                 // It´s a system error
                          NULL,                                      // No string to be formatted needed
                          dwLastError,                               // Hey Windows: Please explain this error!
                          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Do it in the standard language
                          lpBuffer,              // Put the message here
                          255,                     // Number of bytes to store the message
                          NULL);
    }
}
