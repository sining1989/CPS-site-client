#ifndef SITE_CPS_CURLUTIL_H
#define SITE_CPS_CURLUTIL_H

#include "configinfo.h"

class CUrlUtil
{ 
private:
    CUrlUtil();

    static QString getRoot();
    static CConfigInfo* pConfigInfo;
public:
    static QString getXmppUserName();
    static QString getXmppPassword();
    static QString getTaskDataUrl();
    static QString getCreateTaskUrl();

};

#endif // SITE_CPS_CURLUTIL_H
