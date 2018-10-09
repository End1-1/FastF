#include "ff_user.h"
#include "qsystem.h"
#include <QCryptographicHash>

FF_User::FF_User(const QString &dbName) :
    QSqlDrv("FASTFF", dbName)
{

}

void FF_User::setCredentails(const QString &username, const QString &password)
{
    m_username = username;
    m_password = QString(QCryptographicHash::hash(password.toLatin1(), QCryptographicHash::Md5).toHex());
}

bool FF_User::auth1()
{
    m_sql = "select id, group_id, fname || ' ' || lname as fullname, group_id from employes where password2=:password2 and state_id=1";
    prepare();
    m_query->bindValue(":password2", m_password);
    if (!execSQL())
        return false;

    if (! m_query->next()) {
        return false;
    }

    id = m_query->value("ID").toInt();
    m_groupId = valStr("GROUP_ID");
    fullName = m_query->value("FULLNAME").toString();
    QSystem::setUserData(fullName, QString("%1").arg(id));
    close();

    getRoles();

    return true;
}

bool FF_User::auth2()
{
    m_sql = "select id, group_id, fname || ' ' || lname as fullname, group_id from employes where lower(login)=lower(:login) and password=:password and state_id=1";
    prepare();
    m_query->bindValue(":login", m_username);
    m_query->bindValue(":password", m_password);
    if (!execSQL())
        return false;

    if (! m_query->next()) {
        return false;
    }

    id = m_query->value("ID").toInt();
    m_groupId = valStr("GROUP_ID");
    fullName = m_query->value("FULLNAME").toString();
    QSystem::setUserData(fullName, QString("%1").arg(id));
    close();
    getRoles();
    return true;
}

void FF_User::getRoles()
{
    prepare("select key_name, key_value from employes_access where group_id=:group_id");
    bind(":group_id", m_groupId);
    execSQL();
    while (m_query->next()) {
        Role r;
        QString pr = valStr("KEY_VALUE");
        if (pr.length() == 2)
            pr += "0";
        r.read = pr.at(0) == '1';
        r.write = pr.at(1) == '1';
        r.edit = pr.at(2) == '1';
        m_roleSet[valStr("KEY_NAME")] = r;
    }
    close();
}

bool FF_User::roleRead(const QString &role)
{
    if (!m_roleSet.contains(role))
        return false;
    return m_roleSet[role].read;
}

bool FF_User::roleWrite(const QString &role)
{
    if (!m_roleSet.contains(role))
        return false;
    return m_roleSet[role].write;
}

bool FF_User::roleEdit(const QString &role)
{
    if (!m_roleSet.contains(role))
        return false;
    return m_roleSet[role].edit;
}

FF_User &FF_User::operator=(const FF_User &user)
{
    id = user.id;
    fullName = user.fullName;
    m_username = user.m_username;
    m_password = user.m_password;
    m_roleSet = user.m_roleSet;
    return *this;
}

