#ifndef SITE_CPS_TASKRECORD_MANAGER_H
#define SITE_CPS_TASKRECORD_MANAGER_H

#include <QSqlDatabase>
#include "cps.h"

namespace site_cps
{
class TaskRecordManager
{
public:
    TaskRecordManager();
    ~TaskRecordManager();

    bool createTaskTable();
    bool createTaskDetailTable();
    bool deleteSelectRow(const QString &strTaskId);
    bool addTaskRecord(CPS_TASK record);
    bool addTaskDetailRecord(QList<CPS_PRODUCTION> list);
    QMap<QString, CPS_TASK> taskRecordList();
    QList<CPS_PRODUCTION> taskDetailRecordList(const QString &strTaskId);
    QList<CPS_TASK> searchTaskRecordDetail(const QString &keyword);
    bool updateTaskStatus(QString strTaskID, int status );
    QList<CPS_TASK> searchTaskRecord(const QString & keyword);

private:
    QSqlDatabase m_database;

};
}
#endif // SITE_CPS_TASKRECORD_MANAGER_H
