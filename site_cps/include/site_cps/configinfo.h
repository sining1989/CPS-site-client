#ifndef CONFIGINFO_H
#define CONFIGINFO_H

#include <QString>
#include <QSettings>

class CConfigInfo
{
private:
    CConfigInfo();
    static CConfigInfo* instance;
public:
    static CConfigInfo* getInstace();
    static void init(QSettings *pCfg);
public:
    QString m_strXmppUrl;
    QString m_strXmppUserName;
    QString m_strXmppPassword;
    QString m_strTaskUrl;
    QString m_strTaskPort;
    QString m_strCreateTaskUrl;

public:
    CConfigInfo& operator =(CConfigInfo&);
};

#endif // CONFIGINFO_H
