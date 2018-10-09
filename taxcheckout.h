#ifndef TAXCHECKOUT_H
#define TAXCHECKOUT_H

#include <QObject>

class TaxCheckout : public QObject
{
private:
    QString m_taxMachineIP;
    QString m_taxMachinePort;
    QString m_taxMachinePass;
    QString m_headerData;
    QString m_bodyData;
    QString m_fileName;
    QString m_data;
public:
    TaxCheckout();
    void setConnection(const QString &ip, const QString &port, const QString &pass);
    void setJSONFileName(const QString &fileName);
    void setHeader(float paidAmount, float paidAmountCard, float prePaymentAmount);
    void appendItem(const QString &dep, float qty, float price, const QString &productCode, const QString &adgCode, const QString &productName);
    void finish();
};

#endif // TAXCHECKOUT_H
