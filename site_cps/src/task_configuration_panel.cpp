/*!
 * \file    taskconfigurationpanel.cpp
 * \date    2018.10.24
 * \author  zhusizhi
 * \version 1.0.0
 *
 * \brief   task configuration panel
 *
 */
#include "task_configuration_panel.h"
#include "push_button.h"

namespace site_cps
{

ShadowWidget::ShadowWidget(QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
}

ShadowWidget::~ShadowWidget()
{

}

void ShadowWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    //this->drawWindowShadow(painter);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawRect(QRect(0, 0, this->width(), this->height()));
}


TaskConfigurationPanel::TaskConfigurationPanel(QWidget *parent) :
    ShadowWidget(parent)
    , m_splitterH(NULL)
    , m_tableWidget(NULL)
    , m_pWebSocketServer(new QWebSocketServer("site_cps Server", QWebSocketServer::NonSecureMode, this))
    , m_clients()
    , m_cpsTask(new CPSTaskParser())
    , m_createTask(NULL)
    , m_showTaskModel(NULL)
    , m_taskDetailPanel(NULL)
    , m_current_clients(NULL)
    , m_tcpSocket(NULL)
{
    this->setMinimumSize(400, 300);
    setAttribute(Qt::WA_TranslucentBackground);

    m_splitterH = new QSplitter();
    m_splitterH->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_splitterH->setOrientation(Qt::Horizontal);
    m_splitterH->setHandleWidth(1);
    m_splitterH->setStyleSheet("QSplitter::handle { background-color: green }"); //设置分界线的样式

    m_createTask = new CreateTaskPanel();
    m_showTaskModel = new CreateTaskPanel(this);

    initLeftWidget();
    initRightWidget();

    m_splitterH->addWidget(m_leftWidget);
    m_splitterH->addWidget(m_rightWidget);

    QHBoxLayout *main_layout = new QHBoxLayout();
    main_layout->addWidget(m_splitterH);
    main_layout->setSpacing(0);
    main_layout->setContentsMargins(0, 0, 0, 0);

    this->setLayout(main_layout);

    m_listRecord.clear();

    initTableWidget();

    if (m_pWebSocketServer->listen(QHostAddress::Any, 9999))
    {
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
                    this, &TaskConfigurationPanel::onNewConnection);
        connect(m_pWebSocketServer, &QWebSocketServer::closed,
            this, &TaskConfigurationPanel::closed);

    }

    m_tcpSocket = new QTcpSocket;
    m_tcpSocket->connectToHost("127.0.0.1",8000,QTcpSocket::ReadWrite);
    connect(m_tcpSocket,SIGNAL(connected()),this,SLOT(onTcpConnected()));

}

TaskConfigurationPanel::~TaskConfigurationPanel()
{
    SAFE_DELETE(m_leftWidget);
    SAFE_DELETE(m_rightWidget);

    SAFE_DELETE(m_createTask);
    SAFE_DELETE(m_cpsTask);

    m_pWebSocketServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
}

void TaskConfigurationPanel::initLeftWidget()
{
    m_leftWidget = new QWidget(this);
    m_taskListGroup = new QGroupBox();

    m_searchEdit = new QLineEdit();
    m_searchBtn = new QPushButton();
    m_showAllBtn = new QPushButton();
    QSpacerItem* searchSpacerEnd = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_tableWidget = new QTableWidget();
    m_addTaskBtn = new QPushButton();
    m_deleteTaskBtn = new QPushButton();


    m_taskListGroup->setTitle(TASKLIST_GROUP);
    m_searchBtn->setFixedSize(60, 25);
    m_showAllBtn->setFixedSize(80, 25);
    m_addTaskBtn->setFixedSize(60, 25);
    m_deleteTaskBtn->setFixedSize(60, 25);

    m_searchBtn->setText(TASK_SEARCH);
    m_showAllBtn->setText(TASK_ALLSHOW);
    m_addTaskBtn->setText(TASK_ADD);
    m_deleteTaskBtn->setText(TASK_DELETE);

    m_searchBtn->setObjectName("confirmGreenButton");
    m_showAllBtn->setObjectName("confirmGreenButton");
    m_addTaskBtn->setObjectName("greenButton");
    m_deleteTaskBtn->setObjectName("redButton");

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->addWidget(m_searchEdit, 0, Qt::AlignVCenter);
    hLayout->addWidget(m_searchBtn, 0, Qt::AlignVCenter);
    hLayout->addWidget(m_showAllBtn, 0, Qt::AlignVCenter);
    hLayout->addItem(searchSpacerEnd);
    hLayout->setSpacing(10);
    hLayout->setContentsMargins(0, 0, 0, 0);

    QSpacerItem* hSpacerBegin = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QSpacerItem* hSpacerEnd = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setSpacing(30);
    gridLayout->setContentsMargins(10, 10, 10, 10);
    gridLayout->addItem(hSpacerBegin, 0, 0, 1, 1);
    gridLayout->addWidget(m_addTaskBtn, 0, 1, 1, 1);
    gridLayout->addWidget(m_deleteTaskBtn, 0, 2, 1, 1);
    gridLayout->addItem(hSpacerEnd, 0, 3, 1, 1);

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addLayout(hLayout);
    vLayout->addWidget(m_tableWidget);
    vLayout->addLayout(gridLayout);
    m_taskListGroup->setLayout(vLayout);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addWidget(m_taskListGroup);
    m_leftWidget->setLayout(mainLayout);

    connect(m_searchBtn, SIGNAL(clicked()), this, SLOT(on_search_clicked()));
    connect(m_showAllBtn, SIGNAL(clicked()), this, SLOT(on_showAll_clicked()));
    connect(m_addTaskBtn, SIGNAL(clicked()), this, SLOT(on_addTask_clicked()));
    connect(m_deleteTaskBtn, SIGNAL(clicked()), this, SLOT(on_deleteTask_clicked()));

}

void TaskConfigurationPanel::initRightWidget()
{
    m_rightWidget = new QWidget(this);
    m_taskShowGroup = new QGroupBox();
    m_taskAnalysisGroup = new QGroupBox();

    m_rightWidget->setMinimumWidth(100);
    m_taskShowGroup->setTitle(TASKSHOW_GROUP);
    m_taskAnalysisGroup->setTitle(TASKANALYSIS_GROUP);

    m_showTaskModel->setVisible(true);
    m_showTaskModel->raise();

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addWidget(m_showTaskModel);
    m_taskShowGroup->setLayout(vLayout);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addWidget(m_taskShowGroup);
    mainLayout->addWidget(m_taskAnalysisGroup);
    m_rightWidget->setLayout(mainLayout);
}

void TaskConfigurationPanel::initTableWidget()
{
    QStringList strHeader;
    strHeader << HEARDER_TASKNAME << HEARDER_TASKCODE << HEARDER_TASKDESCRIBE << HEARDER_TASKSTATUS
              << HEARDER_TASKSTARTTIME << HEARDER_TASKENDTIME << HEARDER_TASKACTUALTIME << HEARDER_TASKDETAIL << HEARDER_TASKCONTROL;

    QFont font;
    font.setBold(true);
    m_tableWidget->horizontalHeader()->setFont(font);
    //设置列数
    m_tableWidget->setColumnCount(strHeader.size());
    //添加表头数据
    m_tableWidget->setHorizontalHeaderLabels(strHeader);
    m_tableWidget->horizontalHeader()->setStretchLastSection(true);
    //隐藏列表头
    m_tableWidget->verticalHeader()->hide();

    //表头qss设置
    QString strHeaderQss;
    strHeaderQss = "QHeaderView::section { background:rgb(230, 230, 230); color:black;min-height:2em;}";
    m_tableWidget->setStyleSheet(strHeaderQss);
    //表头设置最后一列数据填充整个控件
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(m_tableWidget,SIGNAL(cellPressed(int, int)),this,SLOT(slotOnTableItemChanged(int, int)));
    QMap<QString, CPS_TASK> list = m_recordManager.taskRecordList();
    m_listRecord = list;

    QMap<QString, CPS_TASK>::const_iterator it = list.constBegin();
    for (; it != list.constEnd(); ++it)
    {
        appendTaskRecord(it.value(), false);

    }
    list.clear();
}

void TaskConfigurationPanel::on_search_clicked()
{
    QString keyword = m_searchEdit->text();
    if (keyword.isEmpty())
    {
        emit updateStateText( tr("请输入搜索关键词"));
        return;
    }

    clearTableWidgetItem();

    QList<CPS_TASK> list = m_recordManager.searchTaskRecord(keyword);
    for (int i = 0; i < list.size(); i++)
    {
        appendTaskRecord(list[i], false);
    }
    if (0 == list.size())
    {
       emit updateStateText( tr("没有查找到相关记录，请重新输入"));
    }
    list.clear();
}

void TaskConfigurationPanel::on_showAll_clicked()
{
    clearTableWidgetItem();

    QMap<QString, CPS_TASK> list = m_recordManager.taskRecordList();
    m_listRecord = list;

    QMap<QString, CPS_TASK>::const_iterator it = list.constBegin();
    for (; it != list.constEnd(); ++it)
    {
        appendTaskRecord(it.value(), false);

    }
    list.clear();
}

void TaskConfigurationPanel::on_addTask_clicked()
{
    if (!m_createTask->isVisible())
    {
        m_createTask->setVisible(true);
        m_createTask->raise();
    }

}

void TaskConfigurationPanel::on_deleteTask_clicked()
{
    int iRow = m_tableWidget->currentRow();
    QString strTaskId = m_tableWidget->item(iRow, 0)->data(Qt::DisplayPropertyRole).toString();
    if (-1 != iRow)
    {
        m_tableWidget->removeRow(iRow);
    }

    //数据库中删除该行
    if (m_recordManager.deleteSelectRow(strTaskId))
    {
        m_listRecord.clear();
        m_listRecord = m_recordManager.taskRecordList();

        if (m_current_clients)
        {
            QByteArray taskByte = m_cpsTask->jsonSerializerTask(m_listRecord);
            m_current_clients->sendTextMessage(taskByte);
            m_current_clients->flush();
        }
//        QMap<QString, CPS_TASK>::iterator it = m_listRecord.begin();
//        for (;it!=m_listRecord.end();++it)
//        {
//            if (strTaskId == it.key())
//                m_listRecord.erase(*it);
//        }
    }

}

void TaskConfigurationPanel::onNewConnection()
{
    QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();
    m_current_clients = pSocket;
    connect(pSocket, &QWebSocket::textMessageReceived, this, &TaskConfigurationPanel::processTextMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &TaskConfigurationPanel::socketDisconnected);

    QByteArray taskByte = m_cpsTask->jsonSerializerTask(m_listRecord);
    m_current_clients->sendTextMessage(taskByte);
    m_current_clients->flush();
}

void TaskConfigurationPanel::onTcpConnected()
{
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(onTextMessageReceived()));
}

void TaskConfigurationPanel::onTextMessageReceived()
{
    while (m_tcpSocket->bytesAvailable())
    {
        QByteArray data;
        data = m_tcpSocket->readLine();
        std::string strData(data);
        //qDebug() << "Message received:" << strData;

        //ui->textBrowser->append(QString::fromStdString(strData));
    }

}

void TaskConfigurationPanel::processTextMessage(QString message)
{
    CPS_TASK taskInfo;
    //parser json
    if(m_cpsTask->parserJsonTask(message, taskInfo))
    {
        //ok
        if (taskInfo.iTaskDetail > 0)
        {
            m_createTask->setVisible(false);
            emit updateStateText(PARSER_JSON_OK);

            if (appendTaskRecord(taskInfo, true))
            {
                m_listRecord.insert(taskInfo.strTaskID, taskInfo);

                if (m_current_clients)
                {
                    QByteArray taskByte = m_cpsTask->jsonSerializerTask(m_listRecord);
                    m_current_clients->sendTextMessage(taskByte);
                    m_current_clients->flush();
                }
            }
        }
        else
        {
            emit updateStateText(TASKDETAILEMPTY);
        }

    }
    else
    {
        //error
        emit updateStateText(PARSER_JSON_ERROR);
    }

}

void TaskConfigurationPanel::socketDisconnected()
{
    QWebSocket *pClient = qobject_cast<QWebSocket*>(sender());

    qDebug() << "socketDisconnected: " << pClient;

    if (pClient) {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}

void TaskConfigurationPanel::clearTableWidgetItem()
{
    m_tableWidget->clear();
    m_tableWidget->setRowCount(0);

    QStringList strHeader;
    strHeader << HEARDER_TASKNAME << HEARDER_TASKCODE << HEARDER_TASKDESCRIBE << HEARDER_TASKSTATUS
              << HEARDER_TASKSTARTTIME << HEARDER_TASKENDTIME << HEARDER_TASKACTUALTIME << HEARDER_TASKDETAIL << HEARDER_TASKCONTROL;

    QFont font;
    font.setBold(true);
    m_tableWidget->horizontalHeader()->setFont(font);
    //设置列数
    m_tableWidget->setColumnCount(strHeader.size());
    //添加表头数据
    m_tableWidget->setHorizontalHeaderLabels(strHeader);
    m_tableWidget->horizontalHeader()->setStretchLastSection(true);
    //隐藏列表头
    m_tableWidget->verticalHeader()->hide();
}

bool TaskConfigurationPanel::appendTaskRecord(CPS_TASK record, bool inserDb)
{
    m_tableWidget->insertRow(0);

    //task name
    QTableWidgetItem *item = new QTableWidgetItem(record.strTaskName);
    item->setData(Qt::DisplayPropertyRole, QVariant::fromValue(record.strTaskID));
    m_tableWidget->setItem(0,0,item);

    //task code
    m_tableWidget->setItem(0,1,new QTableWidgetItem(record.strTaskCode));

    //task describe
    m_tableWidget->setItem(0,2,new QTableWidgetItem(record.strTaskDescribe));

    //task status
    QString strStatus = "";
    if (0 == record.iTaskStatus)
    {
        strStatus = tr("未开始");
    }
    else if (1 == record.iTaskStatus)
    {
        strStatus = tr("进行中");
    }
    else if (2 == record.iTaskStatus)
    {
        strStatus = tr("完成");
    }
    else if (3 == record.iTaskStatus)
    {
        strStatus = tr("暂停");
    }
    QTableWidgetItem* item3 = new QTableWidgetItem(strStatus);
    //0--未开始 1--进行中 2--完成 3--暂停
    if (record.iTaskStatus == 1)
    {
        item3->setBackground(Qt::green);
    }
    else if (record.iTaskStatus == 2)
    {
        item3->setBackground(Qt::blue);
    }
    else if (record.iTaskStatus == 3)
    {
        item3->setBackground(Qt::yellow);
    }
    item3->setTextAlignment(Qt::AlignCenter);
    m_tableWidget->setItem(0,3,item3);

    //start time
    m_tableWidget->setItem(0,4,new QTableWidgetItem(record.taskStartDate));

    //plan finish time
    m_tableWidget->setItem(0,5,new QTableWidgetItem(record.taskPlanFinishDate));

    //actual finish time
    m_tableWidget->setItem(0,6,new QTableWidgetItem(record.taskActualFinishDate));

    //查看详情
    if (record.iTaskDetail > 0)
    {
        QPushButton *btn = new QPushButton(tr("查看详情"));
        btn->setObjectName("blueButton");//skinButton

        connect(btn,SIGNAL(clicked()),this,SLOT(taskDetailBtnClicked()));
        btn->setProperty("taskdetail", QVariant::fromValue(record.strTaskID));
        m_tableWidget->setCellWidget(0,7, btn);
    }

    //control btn
    QWidget* btn_group = new QWidget();
    QPushButton *startBtn = new QPushButton(tr("开始"));
    QPushButton *endBtn = new QPushButton(tr("结束"));
    QPushButton *visualizationBtn = new QPushButton(tr("可视化"));
    startBtn->setObjectName("greenButton");
    endBtn->setObjectName("redButton");
    visualizationBtn->setObjectName("blueButton");
    startBtn->setFixedSize(60, 24);
    endBtn->setFixedSize(60, 24);
    visualizationBtn->setFixedSize(60, 24);
    visualizationBtn->hide();
    if (1 == record.iTaskStatus) //1--进行中
        visualizationBtn->show();
    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->addWidget(startBtn, 0, Qt::AlignVCenter);
    hLayout->addWidget(endBtn, 0, Qt::AlignVCenter);
    hLayout->addWidget(visualizationBtn, 0, Qt::AlignVCenter);
    hLayout->setContentsMargins(0, 0, 0, 0);
    btn_group->setLayout(hLayout);

    connect(startBtn,SIGNAL(clicked()),this,SLOT(starTaskClicked()));
    connect(endBtn,SIGNAL(clicked()),this,SLOT(endTaskClicked()));
    connect(visualizationBtn,SIGNAL(clicked()),this,SLOT(visualizationBtnClicked()));
    startBtn->setProperty("taskId", QVariant::fromValue(record.strTaskID));
    m_tableWidget->setCellWidget(0,8, btn_group);

    //insert into db
    if (inserDb)
    {
        if (!m_recordManager.addTaskRecord(record) || !m_recordManager.addTaskDetailRecord(record.ppList))
        {
            emit updateStateText(TASKINSTAERTFAILED);
            return false;
        }
    }
    return true;
}

void TaskConfigurationPanel::taskDetailBtnClicked()
{
    QPushButton* btn = (QPushButton*)sender();
    QString strTaskDetail = btn->property("taskdetail").toString();
    CPS_TASK task = m_listRecord[strTaskDetail];
    QList<CPS_PRODUCTION> detail = task.ppList;
    //hearder name 可配
    QStringList strHeader;
    strHeader << HEARDER_DETAIL_MATERIAL << HEARDER_DETAIL_LOADING_ROBOT << HEARDER_DETAIL_HANDLING_ROBOT << HEARDER_DETAIL_INSTALLATION_ROBOT;
    if (NULL == m_taskDetailPanel)
    {
        m_taskDetailPanel = new TaskDetailPanel();
    }
    m_taskDetailPanel->setHearder(strHeader, task.strTaskID);
    m_taskDetailPanel->initTableView();

    QMap<QString,QString> robotStateMap;
    robotStateMap.clear();

    for (int i=0; i<detail.size(); ++i)
    {
        robotStateMap.insert(detail[i].strProductionId,strHeader[1]);//second为各机器人状态 zhusizhi
    }
    m_taskDetailPanel->appendTaskDetailRecord(robotStateMap);
    m_taskDetailPanel->setVisible(true);
}

void TaskConfigurationPanel::starTaskClicked()
{
    //to cps client
    static int i=0;
    QString msg = "hello"+QString::number(i);
    m_tcpSocket->write(msg.toUtf8(), msg.toUtf8().length());
}

void TaskConfigurationPanel::endTaskClicked()
{
//    QString msg = "hello"+QString::number(i);
//    m_tcpSocket->write(msg.toUtf8(), msg.toUtf8().length());
}

void TaskConfigurationPanel::visualizationBtnClicked()
{
//    QString msg = "hello"+QString::number(i);
//    m_tcpSocket->write(msg.toUtf8(), msg.toUtf8().length());
}

void TaskConfigurationPanel::slotOnTableItemChanged(int row, int column)
{
    QString strTaskId = m_tableWidget->item(row, 0)->data(Qt::DisplayPropertyRole).toString();

    QByteArray taskByte = m_cpsTask->jsonSerializer(strTaskId);
    if (m_current_clients)
    {
        qint64 iRet = m_current_clients->sendTextMessage(taskByte);
        m_current_clients->flush();
    }

}
}

