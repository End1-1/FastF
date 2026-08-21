#include "c5printing.h"
#include <QPainter>
#include <QGraphicsItem>
#include <QDebug>
#include <QTextDocument>
#include <QTextBlock>
#include <QPrinterInfo>
#include <QPrinter>
#include <QJsonObject>
#include <QJsonDocument>
#include <QAbstractTextDocumentLayout>
#include <QTcpSocket>

#define INCH_PER_MM 0.0393

C5Printing::C5Printing() :
    QObject()
{
    fCanvas = new QGraphicsScene();
    fCanvasList.append(fCanvas);
    fLinePen.setWidth(1);
    fLineHeight = 4;
    fTop = 0;
    fTempTop = 0;
    fCurrentPageIndex = 0;
    fNoNewPage = false;
}

C5Printing::~C5Printing()
{
    foreach (QGraphicsScene *gs, fCanvasList) {
        delete gs;
    }
}

void C5Printing::newPage()
{
    startNewPage();
    QJsonObject ob;
    ob["cmd"] = "newpage";
    fJsonData.append(ob);
}

void C5Printing::startNewPage()
{
    fTop = 0;
    fTempTop = 0;
    QPageLayout::Orientation o = fCanvasOrientation[fCanvas];
    fCanvas = new QGraphicsScene();
    fCanvasList.append(fCanvas);
    setSceneParams(fNormalWidth, fNormalHeight, o);
    fCurrentPageIndex++;
}

void C5Printing::ensureSpace(qreal neededHeight)
{
    if(fNoNewPage)
        return;

    if(neededHeight <= 0)
        neededHeight = fLineHeight;

    // Start a new page before drawing if the next block would be clipped
    // at the bottom of the current scene (those lines were previously lost).
    if(fTop > 0 && fTop + neededHeight > fNormalHeight) {
        QJsonObject ob;
        ob["cmd"] = "newpage";
        fJsonData.append(ob);
        startNewPage();
    }
}

void C5Printing::setSceneParams(qreal width, qreal height, QPageLayout::Orientation orientation)
{
    fNormalHeight = height;
    fNormalWidth = width;
    fCanvas->setSceneRect(0, 0, width, height);
    fCanvasOrientation[fCanvas] = orientation;
    QJsonObject o;
    o["cmd"] = "scene";
    o["width"] = width;
    o["height"] = height;
    o["orientation"] = (int) orientation;
    fJsonData.append(o);
}

void C5Printing::setFont(const QFont &font)
{
    fFont = font;
    setLineHeight();
    QJsonObject o;
    o["cmd"] = "font";
    o["family"] = font.family();
    o["size"] = font.pointSize();
    o["bold"] = font.bold();
    fJsonData.append(o);
}

void C5Printing::setFontBold(bool bold)
{
    fFont.setBold(bold);
    setLineHeight();
    QJsonObject o;
    o["cmd"] = "fontbold";
    o["bold"] = bold;
    fJsonData.append(o);
}

void C5Printing::setFontItalic(bool italic)
{
    fFont.setItalic(italic);
    setLineHeight();
    QJsonObject o;
    o["cmd"] = "fontitalic";
    o["italic"] = italic;
    fJsonData.append(o);
}

void C5Printing::setFontStrike(bool strike)
{
    fFont.setStrikeOut(strike);
    setLineHeight();
    QJsonObject o;
    o["cmd"] = "fontstrikeout";
    o["strike"] = strike;
    fJsonData.append(o);
}

void C5Printing::setFontSize(int size)
{
    fFont.setPointSize(size);
    setLineHeight();
    QJsonObject o;
    o["cmd"] = "fontsize";
    o["size"] = size;
    fJsonData.append(o);
}

void C5Printing::setPen(const QPen &p)
{
    fLinePen = p;
}

void C5Printing::line(qreal x1, qreal y1, qreal x2, qreal y2, int lineWidth)
{
    if (lineWidth > 0) {
        fLinePen.setWidth(lineWidth);
    }
    fCanvas->addLine(x1, y1, x2, y2, fLinePen);
    QJsonObject o;
    o["cmd"] = "line1";
    o["x1"] = x1;
    o["y1"] = y1;
    o["x2"] = x2;
    o["y2"] = y2;
    o["width"] = lineWidth;
    fJsonData.append(o);
}

void C5Printing::line(int lineWidth)
{
    ensureSpace(fLineHeight);
    line(0, fTop, fNormalWidth, fTop, lineWidth);
    QJsonObject o;
    o["cmd"] = "line2";
    o["width"] = lineWidth;
    fJsonData.append(o);
}

void C5Printing::tableText(const QList<qreal> &points, const QStringList &vals, int rowHeight)
{
    if (points.count() < 2) {
        return;
    }
    ensureSpace(rowHeight > 0 ? rowHeight : fLineHeight);
    qreal totalWidth = 0.0;
    foreach (qreal w, points) {
        totalWidth += w;
    }
    line(points.at(0), fTop, totalWidth, fTop);
    totalWidth = 0.0;
    for (int i = 0; i < points.count(); i++) {
        totalWidth += points.at(i);
        if (i < points.count() - 1) {
            qreal textwidth = points.at(i + 1);
            ltext(vals.at(i), totalWidth + 1, textwidth);
        }
    }
    totalWidth = 0.0;
    for (int i = 0; i < points.count(); i++) {
        totalWidth += points.at(i);
        line(totalWidth, fTop, totalWidth, fTop + (fTempTop > 0 ? fTempTop - fLineHeight : 0) + rowHeight);
    }
    line(points.at(0), fTop + (fTempTop > 0 ? fTempTop - fLineHeight : 0) + rowHeight, totalWidth,
         fTop + (fTempTop > 0 ? fTempTop - fLineHeight : 0) + rowHeight);
}

void C5Printing::ltext(const QString &text, qreal x, qreal textWidth)
{
    ensureSpace(fLineHeight);
    QGraphicsTextItem *item = fCanvas->addText(text, fFont);
    item->moveBy(x, fTop);
    setTemptop(item, textWidth);
    QJsonObject o;
    o["cmd"] = "ltext";
    o["text"] = text;
    o["textwidth"] = textWidth;
    o["x"] = x;
    fJsonData.append(o);
}

void C5Printing::lrtext(const QString &leftText, const QString &rightText, qreal textWidth)
{
    ltext(leftText, 0,  textWidth);
    rtext(rightText);
}

void C5Printing::ltext90(const QString &text, qreal x)
{
    ensureSpace(fLineHeight);
    QGraphicsTextItem *item = fCanvas->addText(text, fFont);
    item->moveBy(x, fTop);
    item->setRotation(90);
    setTemptop(item,  -1);
    QJsonObject o;
    o["cmd"] = "ltext90";
    o["text"] = text;
    o["x"] = x;
    fJsonData.append(o);
}

void C5Printing::ctext(const QString &text)
{
    ensureSpace(fLineHeight);
    QGraphicsTextItem *item = fCanvas->addText(text, fFont);
    QTextOption op;
    op.setAlignment(Qt::AlignHCenter);
    item->document()->setDefaultTextOption(op);
    item->moveBy(0, fTop);
    setTemptop(item, -1);
    QJsonObject o;
    o["cmd"] = "ctext";
    o["text"] = text;
    fJsonData.append(o);
}

void C5Printing::ctextof(const QString &text, qreal x)
{
    ensureSpace(fLineHeight);
    QGraphicsTextItem *item = fCanvas->addText(text, fFont);
    int textwidth = item->boundingRect().width() / 2;
    item->moveBy(x - textwidth, fTop);
    setTemptop(item, -1);
    QJsonObject o;
    o["cmd"] = "ctextof";
    o["text"] = text;
    o["x"] = x;
    fJsonData.append(o);
}

void C5Printing::rtext(const QString text)
{
    ensureSpace(fLineHeight);
    QGraphicsTextItem *item = fCanvas->addText(text, fFont);
    int textwidth = item->boundingRect().width() + 25;
    item->moveBy(fNormalWidth - textwidth, fTop);
    setTemptop(item, -1);
    QJsonObject o;
    o["cmd"] = "rtext";
    o["text"] = text;
    fJsonData.append(o);
}

void C5Printing::image(const QString &fileName, Qt::Alignment align)
{
    QPixmap p(fileName);
    image(p, align);
}

void C5Printing::image(const QPixmap &image, Qt::Alignment align)
{
    QPixmap p(image);
    ensureSpace(p.height() > 0 ? p.height() : fLineHeight);
    //p = p.scaled(image.width(), p.height());
    fTempTop = fTempTop < (p.height()) ? (p.height()) : fTempTop;
    QGraphicsPixmapItem *pi = fCanvas->addPixmap(p);
    QPointF pos = pi->pos();
    switch (align) {
        case Qt::AlignLeft:
            break;
        case Qt::AlignRight:
            pos.setX(fNormalWidth - p.width() - 1);
            break;
        case Qt::AlignHCenter:
            pos.setX((fNormalWidth / 2) - (p.width() / 2));
            break;
        default:
            break;
    }
    pos.setY(fTop);
    pi->setPos(pos);
}

bool C5Printing::br(qreal height)
{
    QJsonObject o;
    o["cmd"] = "br";
    o["height"] = height;
    fJsonData.append(o);
    if (height == 0) {
        height = fLineHeight;
    }
    const qreal blockHeight = height + (fTempTop > 0 ? fTempTop - fLineHeight : 0);
    fTop += blockHeight;
    fTempTop = 0;
    if (fTop >= fNormalHeight) {
        if (fNoNewPage) {
            QRectF rf = fCanvas->sceneRect();
            rf.setHeight(fTop);
            fCanvas->setSceneRect(rf);
        } else {
            QJsonObject ob;
            ob["cmd"] = "newpage";
            fJsonData.append(ob);
            startNewPage();
            return true;
        }
    }
    return false;
}

bool C5Printing::checkBr(int height)
{
    return fTop + fTempTop + height >= fNormalHeight;
}

int C5Printing::currentPageIndex()
{
    return fCurrentPageIndex;
}

QGraphicsScene *C5Printing::page(int index)
{
    return fCanvasList.at(index);
}

int C5Printing::pageCount()
{
    return fCanvasList.count();
}

QPageLayout::Orientation C5Printing::orientation(int index)
{
    if(index < 0 || index >= fCanvasList.count()) {
        return QPageLayout::Portrait;
    }

    QGraphicsScene *scene = fCanvasList.at(index);
    if(!scene || !fCanvasOrientation.contains(scene)) {
        return QPageLayout::Portrait;
    }

    return fCanvasOrientation.value(scene);
}

bool C5Printing::print(const QString &printername, QPageSize pageSize, bool rotate90)
{
    if (printername.contains("print://", Qt::CaseInsensitive)) {
        //        QRegularExpression re("(print:\\/\\/)(.*):(\\d*)\\/(.*)", Qt::CaseInsensitive);
        //        re.indexIn(printername);
        //        if (re.captureCount() > 0) {
        //            QStringList l = re.capturedTexts();
        //            QString addr = l.at(2);
        //            QString prt = l.at(3);
        //            QString prn = l.at(4);
        //            QJsonObject jobj;
        //            jobj["data"] = fJsonData;
        //            jobj["cmd"] = 1;
        //            jobj["printer"] = prn;
        //            jobj["pagesize"] = pageSize;
        //            QJsonDocument jdoc(jobj);
        //            QTcpSocket ts;
        //            ts.connectToHost(addr, prt.toInt());
        //            if (ts.waitForConnected(5000)) {
        //                QByteArray out = jdoc.toJson();
        //                int datasize = out.length();
        //                ts.write(reinterpret_cast<const char *>(&datasize), sizeof(datasize));
        //                int cmd = 1;
        //                ts.write(reinterpret_cast<const char *>(&cmd), sizeof(cmd));
        //                ts.write(out, out.length());
        //                ts.waitForBytesWritten();
        //                ts.waitForReadyRead(60000);
        //                ts.disconnectFromHost();
        //            } else {
        //                NotificationWidget::showMessage(tr("Failed send order to remote printer"), 1);
        //            }
        //        }
        //        return true;
    }

    const QString requestedName = printername.trimmed();
    if(requestedName.isEmpty()) {
        fErrorString = QString("Printer name is empty");
        return false;
    }

    QString exactName;
    const QStringList printers = QPrinterInfo::availablePrinterNames();
    for(const QString &name : printers) {
        if(name.compare(requestedName, Qt::CaseInsensitive) == 0) {
            exactName = name;
            break;
        }
    }
    if(exactName.isEmpty()) {
        fErrorString = QString("%1 not exists in the system").arg(requestedName);
        return false;
    }

    const QPrinterInfo printerInfo = QPrinterInfo::printerInfo(exactName);
    if(printerInfo.isNull()) {
        fErrorString = QString("%1 is not available").arg(exactName);
        return false;
    }

    if (fCanvasList.isEmpty()) {
        fErrorString = QString("Nothing to print");
        return false;
    }

    QGraphicsScene *firstScene = fCanvasList.at(0);
    if(!firstScene) {
        fErrorString = QString("Invalid print page");
        return false;
    }

    QPrinter printer(printerInfo, QPrinter::PrinterResolution);
    if(!printer.isValid()) {
        fErrorString = QString("%1 is not valid").arg(exactName);
        return false;
    }

    QPageLayout::Orientation o = fCanvasOrientation.value(firstScene, QPageLayout::Portrait);
    printer.setPageOrientation(o);
    printer.setPageSize(pageSize);
    QPainter painter(&printer);
    if(!painter.isActive()) {
        fErrorString = QString("Not printed");
        return false;
    }
    if (rotate90) {
        painter.rotate(90);
        painter.translate(0, -painter.viewport().width());
    }
    for (int i = 0; i < fCanvasList.count(); i++) {
        QGraphicsScene *scene = fCanvasList.at(i);
        if(!scene) {
            continue;
        }
        if (i > 0) {
            o = fCanvasOrientation.value(scene, QPageLayout::Portrait);
            printer.setPageOrientation(o);
            printer.newPage();
        }
        scene->render(&painter);
    }
    if (printer.printerState() == QPrinter::Error) {
        fErrorString = QString("Not printed");
        return false;
    }
    return true;
}

void C5Printing::print(QPainter *p)
{
    if(!p || fCanvasList.isEmpty() || !fCanvasList.at(0)) {
        return;
    }
    fCanvasList.at(0)->render(p);
}

QJsonArray C5Printing::jsonData()
{
    return fJsonData;
}

void C5Printing::setLineHeight()
{
    QFontMetrics fm(fFont);
    fLineHeight = (fm.height());
}

void C5Printing::setTemptop(QGraphicsTextItem *item, qreal textwidth)
{
    item->setTextWidth(textwidth == -1 ? fNormalWidth : textwidth);
    const qreal h = item->document()->documentLayout()->documentSize().height();
    if (h > fLineHeight) {
        fTempTop = h < fTempTop ? fTempTop : h;
    }

    // Wrapped/tall text drawn near the bottom would be clipped by sceneRect.
    // Move the just-drawn item(s) at this fTop onto a new page.
    const qreal blockHeight = fTempTop > 0 ? fTempTop : fLineHeight;
    if(!fNoNewPage && fTop > 0 && fTop + blockHeight > fNormalHeight) {
        const qreal oldTop = fTop;
        QList<QGraphicsItem *> toMove;
        const QList<QGraphicsItem *> items = fCanvas->items();
        for(QGraphicsItem *gi : items) {
            if(qAbs(gi->y() - oldTop) < 0.5)
                toMove.append(gi);
        }

        QJsonObject ob;
        ob["cmd"] = "newpage";
        fJsonData.append(ob);
        startNewPage();

        for(QGraphicsItem *gi : toMove) {
            const QPointF pos = gi->pos();
            fCanvasList.at(fCanvasList.count() - 2)->removeItem(gi);
            fCanvas->addItem(gi);
            gi->setPos(pos.x(), pos.y() - oldTop);
        }
        fTop = 0;
        fTempTop = blockHeight > fLineHeight ? blockHeight : 0;
    }
}
