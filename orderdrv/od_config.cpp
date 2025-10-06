#include "od_config.h"

#include <QLocale>
#include <QRegularExpression>

static const QLocale locale = QLocale();
static const QString DecimalPoint = QLocale().decimalPoint();
static const QRegularExpression reZero = QRegularExpression("(?!\\d[\\" + DecimalPoint + "][1-9]+)0+$");
static const QRegularExpression rDP("[\\" + DecimalPoint + "]$");

QString double_str(double value, int f)
{
    f = 2;
    return locale.toString(value, 'f', f).remove(reZero).remove(rDP);
}
