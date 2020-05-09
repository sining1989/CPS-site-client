/*!
 * \file    robotpanel.h
 * \date    2018.10.24
 * \author  zhusizhi
 * \version 1.0.0
 *
 * \brief   robot panel
 *
 */
#ifndef SITE_CPS_ROBOT_PANEL_H
#define SITE_CPS_ROBOT_PANEL_H

#include <QWidget>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPushButton>

#include "cps.h"
#include "config.h"
#include "common_config.h"

namespace site_cps
{

/*! \brief setting dialog class declaration */
class RobotPanel : public QWidget
{
    Q_OBJECT

public:
    /*! \brief std constructor */
    RobotPanel(QWidget *parent = 0);
    /*! \brief std destructor */
    virtual ~RobotPanel();

    void setTitle(const QString &strTitle);
Q_SIGNALS:


private Q_SLOTS:


private:
    QGroupBox* m_robotStatusGroup;
    QGroupBox* m_robotLogGroup;
};
}
#endif // SITE_CPS_ROBOT_PANEL_H
