/*!
 * \file    taskdetailpanel.cpp
 * \date    2018.10.24
 * \author  zhusizhi
 * \version 1.0.0
 *
 * \brief   task detail panel
 *
 */
#include "task_detail_panel.h"


namespace site_cps
{
TaskDetailPanel::TaskDetailPanel(QWidget *parent) :
    QWidget(parent)
    , m_tableView(NULL)
    , m_textOutput(NULL)
    , m_robotPanel(NULL)
{
    this->setMinimumSize(500, 700);

    m_tableView = new QTableWidget();
    m_textOutput = new QTextBrowser();
    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addWidget(m_tableView);
    vLayout->addWidget(m_textOutput);
    setLayout(vLayout);

    connect(m_tableView->horizontalHeader(), SIGNAL(sectionClicked(int)),
            this, SLOT(onHeaderClicked(int)));

    m_robotPanel = new RobotPanel();
    m_robotPanel->setVisible(false);
}

TaskDetailPanel::~TaskDetailPanel()
{
    if (NULL != m_tableView)
    {
        delete m_tableView;
        m_tableView = NULL;
    }

    if (NULL != m_textOutput)
    {
        delete m_textOutput;
        m_textOutput = NULL;
    }

}

void TaskDetailPanel::initTableView()
{
    clearTableViewItem();

    QFont font;
    font.setBold(true);
    m_tableView->horizontalHeader()->setFont(font);
    //表头设置最后一列数据填充整个控件
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    //隐藏列表头
    m_tableView->verticalHeader()->hide();
    //表头qss设置
    QString strHeaderQss;
    strHeaderQss = "QHeaderView::section { background:rgb(230, 230, 230); color:black;min-height:2em;}";
    m_tableView->setStyleSheet(strHeaderQss);

    m_tableView->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //表头内容均分填充整个控件
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void TaskDetailPanel::setHearder(QStringList list, QString strTitle)
{
    m_strHeader=list;
    strTitle = "任务ID:" + strTitle;
    this->setWindowTitle(strTitle);
}

void TaskDetailPanel::appendTaskDetailRecord(QMap<QString,QString> list)
{
    QMap<QString,QString>::const_iterator it = list.constBegin();
    for (;it!=list.constEnd();++it)
    {
        QString strRobotState = it.value();
        m_tableView->insertRow(0);

        QTableWidgetItem *item = new QTableWidgetItem(it.key());
        item->setTextAlignment(Qt::AlignCenter);
        m_tableView->setItem(0, 0, item);

        //loading robot
        QTableWidgetItem *item1 = new QTableWidgetItem(ROBOT_INIT);
        item1->setTextAlignment(Qt::AlignCenter);
        m_tableView->setItem(0, 1, item1);

        //handling robot
        QTableWidgetItem *item2 = new QTableWidgetItem(ROBOT_INIT);
        item2->setTextAlignment(Qt::AlignCenter);
        m_tableView->setItem(0, 2, item2);

        //installation robot
        QTableWidgetItem *item3 = new QTableWidgetItem(ROBOT_INIT);
        item3->setTextAlignment(Qt::AlignCenter);
        m_tableView->setItem(0, 3, item3);

    }
}

void TaskDetailPanel::clearTableViewItem()
{
    m_tableView->setRowCount(0);
    m_tableView->clear();

    //添加表头数据
    m_tableView->setHorizontalHeaderLabels(m_strHeader);
    //设置列数
    m_tableView->setColumnCount(m_strHeader.size());
}

void TaskDetailPanel::onHeaderClicked(int column)
{
    QString strTitle = "";
    if (0 != column)
    {
        strTitle = m_strHeader[column];
        m_robotPanel->setTitle(strTitle);
        m_robotPanel->setVisible(true);
        m_robotPanel->raise();
    }    

}


}

