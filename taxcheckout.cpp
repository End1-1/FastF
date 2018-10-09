#include "taxcheckout.h"

TaxCheckout::TaxCheckout()
{

}

void TaxCheckout::setConnection(const QString &ip, const QString &port, const QString &pass)
{
    m_taxMachineIP = ip;
    m_taxMachinePort = port;
    m_taxMachinePass = pass;
}

void TaxCheckout::setJSONFileName(const QString &fileName)
{
    m_fileName = fileName;
}

void TaxCheckout::setHeader(float paidAmount, float paidAmountCard, float prePaymentAmount)
{
    m_headerData = (QString("{\"seq\":1, "
                            "\"paidAmount\":%1, \"paidAmountCard\":%2, \"partialAmount\":0, \"prePaymentAmount\":%3,"
                            "\"mode\":2, \"useExtPOS\":true, \"items\":[")
            .arg(paidAmount)
            .arg(paidAmountCard)
            .arg(prePaymentAmount));
}

void TaxCheckout::appendItem(const QString &dep, float qty, float price, const QString &productCode, const QString &adgCode, const QString &productName)
{
    m_bodyData.append(QString("{\"dep\":%1,\"qty\":%2,\"price\":%3,\"productCode\":\"%4\",\"adgCode\":\"%5\",\"productName\":\"%6\"}")
                  .arg(dep)
                  .arg(qty)
                  .arg(price)
                  .arg(productCode)
                  .arg(adgCode)
                      .arg(productName));
}

void TaxCheckout::finish()
{

}
