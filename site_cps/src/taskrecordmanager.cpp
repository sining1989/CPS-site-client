#include "taskrecordmanager.h"
#include "common.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QApplication>
#include <QDebug>

namespace site_cps
{
TaskRecordManager::TaskRecordManager()
{
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    bool bDir = isDirExist(QApplication::applicationDirPath() + "/db/");

    QString dbpath = QApplication::applicationDirPath() + DB_PATH;
    m_database.setDatabaseName(dbpath);
    bool flag = m_database.open();
    //判断表是否存在，不存在则创建
    QSqlQuery query(m_database);
    QString strQuery = QString("select count(*) from sqlite_master where type='table' and name='%1'").arg("task_record");    //关键的判断
    query.prepare(strQuery);
    int iCount = -1;
    if (query.exec())
    {
        while (query.next())
        {
            iCount = query.value(0).toInt();
        }
    }
    if (0 == iCount)
    {
        qDebug() << "table does not exist";
        createTaskTable();
        createTaskDetailTable();
    }

}

TaskRecordManager::~TaskRecordManager()
{
    if(m_database.isOpen())
    {
        m_database.close();
    }
}

bool TaskRecordManager::createTaskTable()
{
	QString strQuery;
    strQuery = "CREATE TABLE [task_record] ("
        "[taskID] VARCHAR(32) NOT NULL ON CONFLICT REPLACE,"
        "[taskName] NVARCHAR2(64),"
        "[taskCode] NVARCHAR2(32),"
        "[taskDescribe] NVARCHAR2(64),"
        "[taskStatus] INT DEFAULT 0,"
		"[startTime] DATETIME,"
        "[planFinishTime] DATETIME,"
        "[actualFinishTime] DATETIME,"
        "[taskDetail] NVARCHAR2(10240))";
	QSqlQuery query;
	if (query.exec(strQuery))
	{
        qDebug() << "Create Task Table Successfully.";
	}
	else
	{
        qDebug() << "Create Task Table Failed";
        qDebug() << "The Task error is: " << query.lastError().databaseText();
		return false;
	}
	return true;
}

bool TaskRecordManager::createTaskDetailTable()
{
    QString strQuery;
    strQuery = "CREATE TABLE [task_detail_record] ("
        "[taskID] VARCHAR(32),"
        "[productionId] NVARCHAR2(64),"
        "[productionName] NVARCHAR2(32),"
        "[createTime] DATETIME,"
        "[finishTime] DATETIME,"
        "[startVector] NVARCHAR2(32),"
        "[finishVector] NVARCHAR2(32))";
    QSqlQuery query;
    if (query.exec(strQuery))
    {
        qDebug() << "Create Task Detail Table Successfully.";
    }
    else
    {
        qDebug() << "Create Task Detail Table Failed";
        qDebug() << "The Task Detail error is: " << query.lastError().databaseText();
        return false;
    }
    return true;
}

bool TaskRecordManager::addTaskRecord(CPS_TASK record)
{
    if (m_database.isOpen()) {
        QString sql = QString("INSERT INTO task_record "
                              "(taskID,taskName,taskCode,taskDescribe,taskStatus,startTime,planFinishTime,actualFinishTime,taskDetail)"
                              " VALUES ('%1','%2','%3','%4','%5','%6','%7','%8','%9')")
                            .arg(record.strTaskID)
                            .arg(record.strTaskName)
                            .arg(record.strTaskCode)
                            .arg(record.strTaskDescribe)
                            .arg(record.iTaskStatus)
                            .arg(record.taskStartDate)
                            .arg(record.taskPlanFinishDate)
                            .arg(record.taskActualFinishDate)
                            .arg(record.iTaskDetail);
		QSqlQuery qry;
        qry.prepare( sql);
        if( !qry.exec() ) {
            qDebug() << qry.lastError();
            return false;
        }
        else {
            qDebug() << "task数据插入成功";
            return true;
        }
    }
    else {
        qDebug() << m_database.lastError();
        m_database.close();
        return false;
    }
}

bool TaskRecordManager::addTaskDetailRecord(QList<CPS_PRODUCTION> list)
{
    if (m_database.isOpen())
    {
        for (int i=0; i<list.size(); ++i)
        {
           CPS_PRODUCTION record = list[i];
           QString sql = QString("INSERT INTO task_detail_record"
                                 "(taskID,productionId,productionName,productionstate,createTime,finishTime,startVector,finishVector)"
                                 " VALUES ('%1','%2','%3','%4','%5','%6','%7','%8')")
                               .arg(record.strTaskId)
                               .arg(record.strProductionId)
                               .arg(record.strProductionName)
                               .arg(record.iProductionState)
                               .arg(record.strCreateTime)
                               .arg(record.strFinishTime)
                               .arg(record.strStartVector)
                               .arg(record.strFinishVector);
           qDebug() << sql;

           QSqlQuery qry;
           qry.prepare( sql);
           if( !qry.exec() ) {
               qDebug() << qry.lastError();
               return false;
           }
           else {
               qDebug() << "detail数据插入成功";
               return true;
           }
        }

    }
    else
    {
        qDebug() << m_database.lastError();
        m_database.close();
        return false;
    }
}

bool TaskRecordManager::deleteSelectRow(const QString &strTaskId)
{
    if (m_database.isOpen()) {
        QString sql = QString("DELETE FROM task_record where taskID == '%1'")
                            .arg(strTaskId);
        QSqlQuery qry;
        qry.prepare( sql);
        if( !qry.exec() ) {
            qDebug() << qry.lastError();
            return false;
        }
        else {
            qDebug() << "task数据删除成功";
            return true;
        }
    }
    else {
        qDebug() << m_database.lastError();
        m_database.close();
        return false;
    }

    if (m_database.isOpen()) {
        QString sql = QString("DELETE FROM task_detail_record where taskID == '%1'")
                            .arg(strTaskId);
        QSqlQuery qry;
        qry.prepare( sql);
        if( !qry.exec() ) {
            qDebug() << qry.lastError();
            return false;
        }
        else {
            qDebug() << "taskdetail数据删除成功";
            return true;
        }
    }
    else {
        qDebug() << m_database.lastError();
        m_database.close();
        return false;
    }
}

QList<CPS_TASK> TaskRecordManager::searchTaskRecord(const QString &key)
{
    QString keyword = "";
    if (!key.isEmpty())
        keyword = QString("'%%1%'").arg(key);

    QList<CPS_TASK> list;
    list.clear();
    if (m_database.isOpen()) {
        QString sql = QString("select * from task_record where taskName like ") + keyword + QString("or taskCode like ") + keyword +
                QString("or taskDescribe like ") + keyword;

        QSqlQuery qry;
        qry.prepare(sql);
        if( qry.exec() ) {
            while(qry.next()) {
                CPS_TASK record;
                record.strTaskID = qry.value(0).toString();
                record.strTaskName = qry.value(1).toString();
                record.strTaskCode = qry.value(2).toString();
                record.strTaskDescribe = qry.value(3).toString();
                record.iTaskStatus = qry.value(4).toInt();
                record.taskStartDate = qry.value(5).toString();
                record.taskPlanFinishDate = qry.value(6).toString();
                record.taskActualFinishDate = qry.value(7).toString();
                record.iTaskDetail = qry.value(8).toInt();
                list.append(record);
            }
        }
    }
    return list;
}

QMap<QString, CPS_TASK> TaskRecordManager::taskRecordList()
{
    QMap<QString, CPS_TASK> list;
	list.clear();
    if (m_database.isOpen())
    {
        QSqlQuery qry;
        qry.prepare("select * from task_record");
        if( qry.exec() )
        {
            while(qry.next())
            {
                CPS_TASK record;
                record.strTaskID = qry.value(0).toString();
                record.strTaskName = qry.value(1).toString();
                record.strTaskCode = qry.value(2).toString();
                record.strTaskDescribe = qry.value(3).toString();
                record.iTaskStatus = qry.value(4).toInt();
                record.taskStartDate = qry.value(5).toString();
                record.taskPlanFinishDate = qry.value(6).toString();
                record.taskActualFinishDate = qry.value(7).toString();
                record.iTaskDetail = qry.value(8).toInt();
                record.ppList = taskDetailRecordList(record.strTaskID);
                list.insert(record.strTaskID, record);
            }
        }
    }

    return list;
}

QList<CPS_PRODUCTION> TaskRecordManager::taskDetailRecordList(const QString &strTaskId)
{
    QList<CPS_PRODUCTION> list;
    list.clear();
    if (m_database.isOpen())
    {
        QString sql = QString("select * from task_detail_record where taskID == '%1'").arg(strTaskId);
        QSqlQuery qry;
        qry.prepare( sql);
        if( qry.exec() )
        {
            while(qry.next())
            {
                CPS_PRODUCTION record;
                record.strTaskId = qry.value(0).toString();
                record.strProductionId = qry.value(1).toString();
                record.strProductionName = qry.value(2).toString();
                record.iProductionState = qry.value(3).toInt();
                record.strCreateTime = qry.value(4).toString();
                record.strFinishTime = qry.value(5).toString();
                record.strStartVector = qry.value(6).toString();
                record.strFinishVector = qry.value(7).toString();

                list.append(record);
            }
        }
    }

    return list;
}

QList<CPS_TASK> TaskRecordManager::searchTaskRecordDetail(const QString &keyword)
{
    QList<CPS_TASK> list;
	list.clear();
    if (m_database.isOpen()) {
        QString sql = QString("select * from task_record where taskID like '%%1%' ").arg(keyword);
        QSqlQuery qry;
        qry.prepare(sql);
        if( qry.exec() ) {
            while(qry.next()) {
                CPS_TASK record;
                record.strTaskID = qry.value(0).toString();
                record.strTaskName = qry.value(1).toString();
                record.strTaskCode = qry.value(2).toString();
                record.strTaskDescribe = qry.value(3).toString();
                record.iTaskStatus = qry.value(4).toInt();
                record.taskStartDate = qry.value(5).toString();
                record.taskPlanFinishDate = qry.value(6).toString();
                record.taskActualFinishDate = qry.value(7).toString();
                record.iTaskDetail = qry.value(8).toInt();

                list.append(record);
            }
        }
    }
    return list;
}

bool TaskRecordManager::updateTaskStatus(QString strTaskID, int status)
{
    if (m_database.isOpen())
    {
        QString sql = QString("update task_record set taskStatus = %2 where taskID == '%1'")
                .arg(strTaskID).arg(status);
        qDebug("sql: %s" ,qPrintable(sql));
        QSqlQuery query;
        query.prepare(sql);
        if(query.exec())
        {
            return true;
        }
    }
    return false;
}
}
