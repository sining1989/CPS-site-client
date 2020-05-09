/*!
 * \file    settingpanel.h
 * \date    2018.10.24
 * \author  zhusizhi
 * \version 1.0.0
 *
 * \brief   setting panel
 *
 */
#ifndef SITE_CPS_SETTING_PANEL_H
#define SITE_CPS_SETTING_PANEL_H

#include "panel.h"
#include "ui_setting.h"
#include "configinfo.h"

/*! \brief setting panel class declaration */
namespace site_cps
{

class SettingPanel : public Panel
{
    Q_OBJECT

public:
    /*! \brief std constructor */
    SettingPanel(QWidget *parent = 0);
    /*! \brief std destructor */
    virtual ~SettingPanel();

    void initUI();
public Q_SLOTS:
    /*! \brief resets the dialogs counter control values */
    void resetCountersSlot();
    void saveCurSettingSlot();

private:
    /*! \brief dialogs GUI member class, build by Qt */
    Ui::settingPanelClass ui;
    CConfigInfo* m_pConfigInfo;
};
}
#endif // SITE_CPS_SETTING_PANEL_H
