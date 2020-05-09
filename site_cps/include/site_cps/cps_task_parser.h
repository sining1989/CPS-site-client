#ifndef SITE_CPS_TASK_PARSER_H
#define SITE_CPS_TASK_PARSER_H

#include "cps.h"
#ifdef __LINUX__
#include "parser.h"
#include "serializer.h"
#else
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#endif
#include <QObject>
#include <QList>
#include <QDateTime>
#include <QFile>
#include <QMap>
#include <QVariant>
#include <QTextStream>
#include <QStringList>
#include <QTextCodec>


#define SAFE_DELETE(p) {if (p != 0) {delete p; p = NULL;}}

class CPSTaskParser : public QObject
{
    Q_OBJECT
public:
    explicit CPSTaskParser(QObject *parent = 0);

    bool parserJsonTask(QString strResponse, CPS_TASK &cpsTask);
    bool parserJsonXmpp(QString strResponse, CPS_XMPP_INFO &cpsXmpp);
    QByteArray jsonSerializer(const QString &strTaskID);
    QByteArray jsonSerializerTask(const QMap<QString, CPS_TASK> &listRecord);
private:
    QVariantList StringToList(const QString &strInfo) const;
    CPS_Vector3 StringToVector(const QString &strInfo) const;

public:
    QString m_strTaskId;
    QString m_strTaskName;
    QDateTime m_taskStartTime;
    QDateTime m_taskFinishTime;

    QList<CPS_TASK*> m_cpsTaskList;

};

#endif // SITE_CPS_TASK_PARSER_H
