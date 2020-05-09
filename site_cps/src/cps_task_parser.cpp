#include "cps_task_parser.h"
#include <QMessageBox>

CPSTaskParser::CPSTaskParser(QObject *parent) : QObject(parent)
{


}

bool CPSTaskParser::parserJsonTask(QString strResponse, CPS_TASK &cpsTask)
{
    //strResponse.replace("\\", "");

    //clear
    cpsTask.ppList.clear();

    //parser
    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(strResponse.toUtf8(), &ok).toMap();
    if (!ok) {
        return false;
    }

    cpsTask.strTaskID = result["taskId"].toString();
    cpsTask.strTaskName = result["taskName"].toString();
    cpsTask.strTaskCode = result["taskCode"].toString();
    cpsTask.strTaskDescribe = result["taskDesc"].toString();
    cpsTask.strAddress = result["address"].toString();
    cpsTask.iTaskStatus = result["taskState"].toInt();//INIT_STATE
    if (result["startTime"].toString().length() > 0) {
        cpsTask.taskStartDate = result["startTime"].toString();
    }
    if (result["endTime"].toString().length() > 0) {
        cpsTask.taskPlanFinishDate = result["endTime"].toString();//QDateTime::fromString(result["endtime"].toString(), "yyyy-MM-dd hh:mm:ss");
    }

    QVariantList list = result["production"].toList();
    for (int i = 0; i < list.size(); i++)
    {
        QVariantMap job = list[i].toMap();
        CPS_PRODUCTION pp;
        pp.strTaskId = job["taskId"].toString();
        pp.strProductionId = job["prodId"].toString();
        pp.strProductionName = job["prodName"].toString();
        pp.iProductionState = job["prodState"].toInt();
        QVariantList listStart = job["position"].toList();
        pp.strStartVector = QString("%1 %2 %3").arg(listStart[0].toDouble()).arg(listStart[1].toDouble()).arg(listStart[2].toDouble());
        QVariantList listFinish = job["position"].toList();
        pp.strFinishVector = QString("%1 %2 %3").arg(listFinish[0].toDouble()).arg(listFinish[1].toDouble()).arg(listFinish[2].toDouble());

        cpsTask.ppList.append(pp);
    }
    cpsTask.iTaskDetail = list.size();

    return true;
}

bool CPSTaskParser::parserJsonXmpp(QString strResponse, CPS_XMPP_INFO &cpsXmpp)
{
    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(strResponse.toUtf8(), &ok).toMap();
    if (!ok)
    {
        return false;
    }
    cpsXmpp.strFlag = result["flag"].toString();
    cpsXmpp.strTaskID = result["id"].toString();

    return true;
}


QByteArray CPSTaskParser::jsonSerializer(const QString &strTaskID)
{
    QVariantMap taskAll;
    taskAll.insert("code", 9999),
    taskAll.insert("message", "");

    QVariantMap task;
    task.insert("taskId", strTaskID);
    taskAll.insert("data", task);

    QJson::Serializer serializer;
    bool ok;
    QByteArray json = serializer.serialize(taskAll, &ok);

    return json;
}

QByteArray CPSTaskParser::jsonSerializerTask(const QMap<QString, CPS_TASK> &listRecord)
{
    QVariantMap taskAll;
    taskAll.insert("code", 10000),
    taskAll.insert("message", "");

    QVariantList taskAllList;
    QMap<QString, CPS_TASK>::const_iterator it = listRecord.constBegin();
    for (;it!=listRecord.constEnd();++it)
    {
        CPS_TASK cpstask = it.value();

        QVariantMap task;
        task.insert("taskId", cpstask.strTaskID);
        task.insert("taskName", cpstask.strTaskName);
        task.insert("taskCode", cpstask.strTaskCode);
        task.insert("taskDesc", cpstask.strTaskDescribe);
        task.insert("taskState", cpstask.iTaskStatus);
        task.insert("address", cpstask.strAddress);
        task.insert("startTime", cpstask.taskStartDate);
        task.insert("endTime", cpstask.taskPlanFinishDate);

        QVariantList taskDetailList;
        for (int i=0; i<cpstask.ppList.size(); ++i)
        {
            QVariantMap taskDetail;
            taskDetail.insert("taskId", cpstask.strTaskID);
            taskDetail.insert("prodId", cpstask.ppList[i].strProductionId);
            taskDetail.insert("prodName", cpstask.ppList[i].strProductionName);
            taskDetail.insert("prodState", cpstask.ppList[i].iProductionState);
            QVariantList startVec = StringToList(cpstask.ppList[i].strStartVector);
            taskDetail.insert("position", startVec);
            QVariantList finishVec = StringToList(cpstask.ppList[i].strFinishVector);
            taskDetail.insert("rotation", finishVec);

            taskDetailList.append(taskDetail);
        }

        task.insert("production", taskDetailList);
        taskAllList.append(task);
    }
    taskAll.insert("data", taskAllList);

    QJson::Serializer serializer;
    bool ok;
    QByteArray json = serializer.serialize(taskAll, &ok);

    return json;
}

QVariantList CPSTaskParser::StringToList(const QString &strInfo) const
{
    QVariantList vec;
    QStringList list = strInfo.split(" ");
    if (3 == list.size())
    {
        vec.append(QVariant::fromValue(list[0]));
        vec.append(QVariant::fromValue(list[1]));
        vec.append(QVariant::fromValue(list[2]));
    }
    return vec;
}

CPS_Vector3 CPSTaskParser::StringToVector(const QString &strInfo) const
{
    CPS_Vector3 vec;
    QStringList list = strInfo.split(" ");
    if (3 == list.size())
    {
        QString str1 = list[0];
        QString str2 = list[1];
        QString str3 = list[2];
        vec.x = str1.toDouble();
        vec.y = str2.toDouble();
        vec.z = str3.toDouble();
    }
    return vec;
}
