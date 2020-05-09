/*!
 * \file    taskconfigurationpanel.h
 * \date    2018.10.24
 * \author  zhusizhi
 * \version 1.0.0
 *
 * \brief   taskconfiguration panel
 *
 */
#ifndef SITE_CPS_TASK_CONFIGURATION_PANEL_H
#define SITE_CPS_TASK_CONFIGURATION_PANEL_H

#include <QWidget>
#include <QSplitter>
#include <QLineEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QtWebEngineWidgets>
#include <QSpacerItem>
#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>
#include <QTcpSocket>

#include "config.h"
#include "common_config.h"
#include "create_task_panel.h"
#include "taskrecordmanager.h"
#include "cps_task_parser.h"
#include "task_detail_panel.h"

namespace site_cps
{

class ShadowWidget : public QDialog
{
    Q_OBJECT

public:

    explicit ShadowWidget(QWidget *parent = 0);
    ~ShadowWidget();

protected:

    virtual void paintEvent(QPaintEvent *event);

};

/*! \brief setting dialog class declaration */
class TaskConfigurationPanel : public ShadowWidget
{
    Q_OBJECT

public:
    /*! \brief std constructor */
    TaskConfigurationPanel(QWidget *parent = 0);
    /*! \brief std destructor */
    virtual ~TaskConfigurationPanel();

Q_SIGNALS:
    void closed();
    void updateStateText(const QString &);

public Q_SLOTS:
    void on_search_clicked();
    void on_showAll_clicked();
    void on_addTask_clicked();
    void on_deleteTask_clicked();
    void taskDetailBtnClicked();

private Q_SLOTS:
    void onNewConnection();
    void processTextMessage(QString message);
    void socketDisconnected();

    void starTaskClicked();
    void endTaskClicked();
    void visualizationBtnClicked();
    void slotOnTableItemChanged(int row, int column);

    void onTcpConnected();
    void onTextMessageReceived();
protected:
    void initLeftWidget();
    void initRightWidget();
    void initTableWidget();

private:
    void clearTableWidgetItem();
    bool appendTaskRecord(CPS_TASK record, bool inserDb);

private:
    QWidget* m_leftWidget;
    QWidget* m_rightWidget;

    QSplitter *m_splitterH;

    QGroupBox* m_taskListGroup;
    QLineEdit* m_searchEdit;
    QPushButton* m_searchBtn;
    QPushButton* m_showAllBtn;
    QTableWidget* m_tableWidget;
    QPushButton* m_addTaskBtn;
    QPushButton* m_deleteTaskBtn;

    QGroupBox* m_taskShowGroup;
    QGroupBox* m_taskAnalysisGroup;

    CreateTaskPanel* m_createTask;
    CreateTaskPanel* m_showTaskModel;

    TaskRecordManager m_recordManager;
    QMap<QString, CPS_TASK> m_listRecord;

    QWebSocketServer* m_pWebSocketServer;
    QList<QWebSocket *> m_clients;

    CPSTaskParser* m_cpsTask;
    TaskDetailPanel* m_taskDetailPanel;
    QWebSocket* m_current_clients;

    QTcpSocket* m_tcpSocket;
};
}
#endif // SITE_CPS_TASK_CONFIGURATION_PANEL_H
