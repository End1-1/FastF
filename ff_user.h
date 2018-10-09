#ifndef FF_USER_H
#define FF_USER_H

#include "qsqldrv.h"

#define ROLE_EDIT_ORDER QObject::tr("Edit order")
#define ROLE_ORDER_REMOVE_PRINTED_QTY QObject::tr("Remove printed quantity")
#define ROLE_ORDER_MULTIPLE_CHECKOUT QObject::tr("Multiple checkout")
#define ROLE_ORDER_DISCOUNT_AFTER_CHECKOUT QObject::tr("Discount after checkout")
#define ROLE_WAITER_REPORTS QObject::tr("Access to reports")
#define ROLE_ORDER_CHANGE_STAFF QObject::tr("Change staff of order")
#define ROLE_CHANGE_PASSWORD QObject::tr("Change users password")
#define ROLE_EDIT_AFTER_CHECKOUT QObject::tr("Edit after checkout")
#define ROLE_REMOVE_ORDER QObject::tr("Remove order")
#define ROLE_MOVE_DISH QObject::tr("Move dish")
#define ROLE_MOVE_ORDER QObject::tr("Move order")
#define ROLE_REMOVE_ORDER_FROM_CASH QObject::tr("Remove order from cash")
#define ROLE_REMOVE_ORDER_AFTER_CHECKOUT QObject::tr("Remove order after checkout")
#define ROLE_W_PAYMENT QObject::tr("Payment")
#define ROLE_W_VIEW_SALES_AMOUNT QObject::tr("View sales amount")
#define ROLE_W_PRESENT QObject::tr("Present")
#define ROLE_W_SPESIAL_ACTIONS QObject::tr("Spesial actions")

#define ROLE_M_SALARY QObject::tr("Salary")
#define ROLE_M_EMPLOYES QObject::tr("Employes")
#define ROLE_M_SYNC_WITH_AS QObject::tr("Synchronization with AS")
#define ROLE_M_CALC_OUTPUT QObject::tr("Calculate output")
#define ROLE_M_STORE_DOCUMENTS QObject::tr("Store documents")
#define ROLE_M_MENU QObject::tr("Menu and goods")
#define ROLE_M_COMMON_DIRECTORY QObject::tr("Common directory")
#define ROLE_M_REMOVE_ORDERS QObject::tr("Remove order in cash")
#define ROLE_M_VIEWER QObject::tr("Only viewer")
#define ROLE_M_SALARY_TO_AS QObject::tr("Salary to AS")
#define ROLE_M_SALES QObject::tr("View sales")
#define ROLE_M_DISCOUNT QObject::tr("Discount system")
#define ROLE_M_CANCELATION QObject::tr("Cancelations")
#define ROLE_M_GROUP_QUERIES QObject::tr("Group queries")

class FF_User : public QSqlDrv
{
    Q_OBJECT

public:
    struct Role {
        int role;
        bool read;
        bool write;
        bool edit;
        Role() {read = 0; write = 0; edit = 0;}
    };

private:
    QString m_groupId;
    QString m_username;
    QString m_password;
    QMap<QString, Role> m_roleSet;

public:
    int id;
    QString fullName;
    explicit FF_User(const QString &dbName);
    void setCredentails(const QString &username, const QString &password);
    bool auth1(); //only pass
    bool auth2(); //username and pass
    void getRoles();
    bool roleRead(const QString &role);
    bool roleWrite(const QString &role);
    bool roleEdit(const QString &role);
    FF_User &operator=(const FF_User &user);

signals:

public slots:

};

#endif // FF_USER_H
