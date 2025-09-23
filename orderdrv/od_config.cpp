#include "od_config.h"

#include <QLocale>

static const QLocale locale = QLocale();
static const QString DecimalPoint = QLocale().decimalPoint();
static const QRegExp reZero = QRegExp("(?!\\d[\\" + DecimalPoint +"][1-9]+)0+$");
static const QRegExp rDP("[\\" + DecimalPoint + "]$");

QString double_str(double value, int f)
{
    f = 2;
    return locale.toString(value, 'f', f).remove(reZero).remove(rDP);
}
