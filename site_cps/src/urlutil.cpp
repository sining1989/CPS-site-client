#include "urlutil.h"
#include <QSettings>
#include <QCoreApplication>
#include <QMessageBox>

CConfigInfo* CUrlUtil::pConfigInfo = NULL;

CUrlUtil::CUrlUtil()
{

}

QString CUrlUtil::getRoot()
{
    if (NULL == pConfigInfo->getInstace())
    {
        pConfigInfo = CConfigInfo::getInstace();
    }
    return "http://" + pConfigInfo->getInstace()->m_strTaskUrl + ":" + pConfigInfo->getInstace()->m_strTaskPort;
}

QString CUrlUtil::getXmppUserName()
{
    if (NULL == pConfigInfo->getInstace())
    {
        pConfigInfo = CConfigInfo::getInstace();
    }

    QString strUserName = QString("%1@%2").arg(pConfigInfo->getInstace()->m_strXmppUserName).arg(pConfigInfo->getInstace()->m_strXmppUrl);

    return strUserName;
}

QString CUrlUtil::getXmppPassword()
{
    if (NULL == pConfigInfo->getInstace())
    {
        pConfigInfo = CConfigInfo::getInstace();
    }
    return pConfigInfo->getInstace()->m_strXmppPassword;
}

QString CUrlUtil::getTaskDataUrl()
{
    QString url = getRoot() + "/Robot/mes/findProductionPlanningDetail";
    return url;
}

QString CUrlUtil::getCreateTaskUrl()
{
    if (NULL == pConfigInfo->getInstace())
    {
        pConfigInfo = CConfigInfo::getInstace();
    }
    QString strCreateTaskUrl = QString("http://%1:8080/cps/cps.html").arg(pConfigInfo->getInstace()->m_strCreateTaskUrl);

    return strCreateTaskUrl;
}


