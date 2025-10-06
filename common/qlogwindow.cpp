#include "qlogwindow.h"
#include "ui_qlogwindow.h"
#include "qsystem.h"
#include <QFile>
#include <QDateTime>

QLogWindow::QLogWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QLogWindow)
{
    ui->setupUi(this);
}

QLogWindow::~QLogWindow()
{
    delete ui;
}

void QLogWindow::log(const QString &log)
{
    QString logStr = QDateTime::currentDateTime().toString(DATETIME_FORMAT);
    logStr += "\t" + QString(log).replace("[\r\n]", " ");    
    ui->txtLogWindow->append(logStr);
    writeLog(log);
}

void QLogWindow::writeLog(const QString &log)
{
    QString logStr = QDateTime::currentDateTime().toString(DATETIME_FORMAT);
    logStr += "\t" + QString(log).replace("[\r\n]", " ");
    QFile f(QSystem::homePath() + "log.txt");
    if (f.open(QIODevice::Append)) {
        f.write(logStr.toUtf8());
        f.write("\r\n");
        f.close();
    }
}
