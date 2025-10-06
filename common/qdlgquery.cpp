#include "qdlgquery.h"
#include "qplineedit.h"
#include "qpcombobox.h"
#include <QDate>

QDlgQuery::QDlgQuery(QMap<QString, QList<QVariant> >& values, QWidget *parent) :
    QDlgBase(parent),
    m_values(values)
{
    mNext = false;
    mCurrentCode = "";
}

void QDlgQuery::setTable(const QString &table, const QList<SqlField*> fields)
{
    m_tableName = table;

    for(QList<SqlField*>::const_iterator it = fields.begin(); it != fields.end(); it++)
        m_fields.append(new SqlField(**it));
}

void QDlgQuery::mapValuesToWidget()
{
    if(!m_values.count())
        return;

    QObjectList ol(children());

    for(QObjectList::iterator it = ol.begin(); it != ol.end(); it++) {
        QObject *o = *it;

        if(!strcmp(o->metaObject()->className(), "QPLineEdit")) {
            QString fieldName = o->property("field").toString();

            if(!fieldName.length()) {
                QMessageBox::critical(this, tr("Program error"), tr("Object have no field name") + ": " + o->objectName());
                continue;
            }

            qobject_cast<QPLineEdit*>(o)->setText(m_values[fieldName].at(0).toString());
        } else if(!strcmp(o->metaObject()->className(), "QPComboBox")) {
            QString fieldId = o->property("fieldId").toString();
            //QString fieldName = o->property("fieldName").toString();
            qobject_cast<QPComboBox*>(o)->setIndexOfData(m_values[fieldId].at(0).toInt());
        } else if(!strcmp(o->metaObject()->className(), "QPColorEdit")) {
            QString fieldName = o->property("field").toString();
            qobject_cast<QPColorEdit*>(o)->setText(m_values[fieldName].at(0).toString());
        }
    }
}

void QDlgQuery::mapWidgetToValues()
{
    QObjectList ol(children());

    for(QObjectList::iterator it = ol.begin(); it != ol.end(); it++) {
        QObject *o = *it;

        if(!strcmp(o->metaObject()->className(), "QPLineEdit") || !strcmp(o->metaObject()->className(), "QPColorEdit")) {
            QString fieldName = o->property("field").toString();
            QVariant value;
            int fIndex = fieldIndexByName(fieldName);

            if(fIndex < 0)
                continue;

            m_fields[fIndex]->widget = qobject_cast<QWidget*>(o);

            switch(m_fields[fIndex]->dataType) {
            case QVariant::String:
                value = qobject_cast<QPLineEdit*>(o)->text();
                break;

            case QVariant::Int:
                value = qobject_cast<QPLineEdit*>(o)->text().toInt();
                break;

            case QVariant::Double:
                value = qobject_cast<QPLineEdit*>(o)->text().toDouble();
                break;

            case QVariant::Date:
                value = QDate::fromString(qobject_cast<QPLineEdit*>(o)->text(), DATE_FORMAT);
                break;

            default:
                value = qobject_cast<QPLineEdit*>(o)->text();
                break;
            }

            if(!m_values[fieldName].count())
                m_values[fieldName].append(value);
            else
                for(int i = 0; i < m_values[fieldName].count(); i++)
                    m_values[fieldName][i] = value;
        } else if(!strcmp(o->metaObject()->className(), "QPComboBox")) {
            QString fieldId = o->property("fieldId").toString();
            QString fieldName = o->property("fieldName").toString();
            int fieldIndex = fieldIndexByName(fieldId);

            if(fieldIndex < 0)
                continue;

            m_fields[fieldIndex]->widget = qobject_cast<QWidget*>(o);
            fieldIndex = fieldIndexByName(fieldName);
            QPComboBox *c = qobject_cast<QPComboBox*>(o);
            QVariant value = c->currentText();

            if(!m_values[fieldName].count())
                m_values[fieldName].append(value);
            else
                for(int i = 0; i < m_values[fieldName].count(); i++)
                    m_values[fieldName][i] = value;

            value = c->itemData(c->currentIndex());

            if(!m_values[fieldId].count())
                m_values[fieldId].append(value);
            else
                for(int i = 0; i < m_values[fieldId].count(); i++)
                    m_values[fieldId][i] = value;
        }
    }
}

void QDlgQuery::setNext(bool next, const QString &code)
{
}

bool QDlgQuery::checkOk()
{
    return true;
}

void QDlgQuery::btnOK()
{
    if(!checkOk())
        return;

    mapWidgetToValues();
    int idIndex = fieldIndexByName("ID");

    if(idIndex < 0) {
        QMessageBox::critical(this, tr("Error"), tr("This values doesnt contain an id field"));
        return;
    }

    for(int i = 0, count = m_values["ID"].count(); i < count; i++) {
        QMap<QString, QVariant> values;

        for(int j = 0; j < m_fields.count(); j++) {
            QWidget *w = m_fields.at(j)->widget;

            if(w) {
                if(!strcmp(w->metaObject()->className(), "QPLineEdit") || !strcmp(w->metaObject()->className(), "QPColorEdit"))
                    values[m_fields.at(j)->name] = m_values[m_fields[j]->name].at(i);
                else if(!strcmp(w->metaObject()->className(), "QPComboBox")) {
                    if(m_fields.at(j)->dataType == QVariant::Int)
                        values[m_fields.at(j)->name] = m_values[m_fields[j]->name].at(i);
                }
            }
        }

        if(!m_values["ID"].at(0).toInt()) {
            m_values["ID"][0] = m_sqlDriver->genId("GEN_" + m_tableName + "_ID");
            values["ID"] = m_values["ID"][0];
            m_sqlDriver->insert(m_tableName, values);

            if(m_values["ID"][0].toInt() < 0) {
                QMessageBox::critical(this, tr("Error"), tr("SQL error"));
                return;
            }
        } else {
            m_sqlDriver->update(m_tableName, values, m_values["ID"][0].toInt());
        }
    }

    m_sqlDriver->close();
    accept();
}
