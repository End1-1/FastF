#include "dlgconfigmobile.h"
#include "ui_dlgconfigmobile.h"
#include "cnfapp.h"
#include "configmobilewidget.h"
#include "QRCodeGenerator.h"

DlgConfigMobile::DlgConfigMobile(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgConfigMobile)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowFullScreen);
    QStringList qrList = __cnfapp.mobileConfigString().split("|", Qt::SkipEmptyParts);
    ui->hl->addStretch(1);

    for(const QString &configString : qrList) {
        int levelIndex = 1;
        int versionIndex = 0;
        bool bExtent = true;
        int maskIndex = -1;
        //Storage name, IP, port, login, password, store, only read
        QString storageName = configString.mid(0, configString.indexOf(";"));
        QString encodeString = QString(configString).remove(0, configString.indexOf(";") + 1);
        CQR_Encode qrEncode;
        bool successfulEncoding = qrEncode.EncodeData(levelIndex, versionIndex, bExtent, maskIndex, encodeString.toUtf8().data());

        if(!successfulEncoding) {
            //fLog.append("Cannot encode qr image");
        }

        int qrImageSize = qrEncode.m_nSymbleSize;
        int encodeImageSize = qrImageSize + (QR_MARGIN * 2);
        QImage encodeImage(encodeImageSize, encodeImageSize, QImage::Format_Mono);
        encodeImage.fill(1);

        for(int i = 0; i < qrImageSize; i++) {
            for(int j = 0; j < qrImageSize; j++) {
                if(qrEncode.m_byModuleData[i][j]) {
                    encodeImage.setPixel(i + QR_MARGIN, j + QR_MARGIN, 0);
                }
            }
        }

        QPixmap pix = QPixmap::fromImage(encodeImage).scaled(180, 180);
        auto *w = new ConfigMobileWidget(pix, storageName);
        ui->hl->addWidget(w);
    }

    ui->hl->addStretch(1);
}

DlgConfigMobile::~DlgConfigMobile()
{
    delete ui;
}

void DlgConfigMobile::on_btnClose_clicked()
{
    accept();
}
