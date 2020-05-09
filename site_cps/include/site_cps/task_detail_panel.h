/*!
 * \file    taskdetailpanel.h
 * \date    2018.10.24
 * \author  zhusizhi
 * \version 1.0.0
 *
 * \brief   taskdetailpanel panel
 *
 */
#ifndef SITE_CPS_TASK_DETAIL_PANEL_H
#define SITE_CPS_TASK_DETAIL_PANEL_H

#include <QWidget>
#include <QSplitter>
#include <QTableWidget>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextBrowser>

#include "cps.h"
#include "config.h"
#include "common_config.h"
#include "robot_panel.h"

namespace site_cps
{

/*! \brief setting dialog class declaration */
class TaskDetailPanel : public QWidget
{
    Q_OBJECT

public:
    /*! \brief std constructor */
    TaskDetailPanel(QWidget *parent = 0);
    /*! \brief std destructor */
    virtual ~TaskDetailPanel();

    void setHearder(QStringList list, QString strTitle="");
    void initTableView();
    void appendTaskDetailRecord(QMap<QString,QString> list);
    void clearTableViewItem();
Q_SIGNALS:
    void closed();
    void updateStateText(const QString &);

private Q_SLOTS:
    void onHeaderClicked(int column);

private:
    QTableWidget* m_tableView;
    QTextBrowser* m_textOutput;
    QStringList m_strHeader;
    RobotPanel* m_robotPanel;

};
}
#endif // SITE_CPS_TASK_DETAIL_PANEL_H
