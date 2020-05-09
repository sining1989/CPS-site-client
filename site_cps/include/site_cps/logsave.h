#ifndef CPS_CLIENT_LOGSAVE_H
#define CPS_CLIENT_LOGSAVE_H

#include <QApplication>
#include <QString>
#include <QDate>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QDir>


#define FILE_MAX_COUNT 9999
class LogSave
{
private:
    LogSave();


public:
    static LogSave* instance()
    {
        if (logSave == NULL)
        {
            logSave = new LogSave();
        }
        return logSave;
    }

    void outputMessage(QtMsgType type, const QString &msg)
    {
        static QMutex mutex;
        mutex.lock();
        QString text;
        switch(type)
        {
        case QtDebugMsg:
            text = QString("Debug:");
            break;
        case QtWarningMsg:
            text = QString("Warning:");
            break;
        case QtCriticalMsg:
            text = QString("Critical:");
            break;
        case QtFatalMsg:
            text = QString("Fatal:");
            break;
        }

        QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
        QString current_date = QString("(%1)").arg(current_date_time);
        QString message = QString("%1 %2 %3").arg(current_date).arg(text).arg(msg);
        QString rootPath = QCoreApplication::applicationDirPath();

        if (strFileName.isEmpty())
          strFileName = "/log/" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") +".txt";
        if (iSubCount > FILE_MAX_COUNT)
        {
          iSubCount = 0;
          strFileName = "/log/" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") +".txt";
        }
        ++iSubCount;

        QFile file(rootPath + strFileName);
        if(!file.exists())
        {
            QDir path(rootPath + "/log");
            if(!path.exists())
            {
                path.mkdir(rootPath + "/log");
            }
        }
        file.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream text_stream(&file);
        text_stream << message << "\r\n";
        file.flush();
        file.close();
        mutex.unlock();
    }

private:
    static LogSave *logSave;
    static int iSubCount;
    static QString strFileName;
};

#endif // CPS_CLIENT_LOGSAVE_H
