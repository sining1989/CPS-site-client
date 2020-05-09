#ifndef SITE_CPS_COMMON_CONFIG_H
#define SITE_CPS_COMMON_CONFIG_H

#define CPS_VERSION "V1.0.0"
#define APP_NAME tr(u8"CPS 客户端")
#define ABOUTUS tr("关于我们")
#define ABOUTINFO tr("<h2>CPS 客户端 V1.0.0</h2><p>Copyright Foxconn Robot</p><p>This client need an about description.</p>"
#define STATUS_NAME tr("状态：")
#define XMPP_STATUS_ONLINE tr("在线")
#define XMPP_STATUS_AWAY tr("暂时离开")
#define XMPP_STATUS_DND tr("忙碌")
#define XMPP_STATUS_INVISIBLE tr("不在线")
#define XMPP_LOGIN tr("登录")
#define GET_SERVER_TASK tr("从后台获取任务失败，错误码：")
#define PARSER_JSON_OK tr("新增创建解析成功")
#define PARSER_JSON_ERROR tr("新增任务解析失败")
#define GET_SERVER_VIDEO tr("与可视化连接出错：")
#define MSG_EMPTY tr("消息为空")
#define TASK_REQUEST tr("正在请求任务...")

#define FILE_MENU tr("&文件")
#define FILE_MENU_OPEN tr("&打开")
#define FILE_MENU_SAVE tr("&保存")
#define FILE_MENU_SAVEAS tr("&另存为")
#define FILE_MENU_RECENT tr("&最近使用")
#define FILE_MENU_SAVEIMAGE tr("&保存截图")
#define FILE_MENU_CHANGEMASTER tr("&更改Matser")
#define FILE_MENU_PREFERENCES tr("&首选项")
#define FILE_MENU_QUIT tr("&退出")

#define PANELS_MENU tr("&视图")
#define PANELS_MENU_FULLSCREEN tr("&全屏")
#define PANELS_MENU_ROBOT_CONFIGUATION tr("&机器人管理")
#define PANELS_MENU_VISUALIZATION tr("&可视化")
#define PANELS_MENU_SIMULATION tr("&仿真")
#define PANELS_MENU_SETTING tr("&设置")
#define PANELS_MENU_MASTERMSG tr("&Master消息")
#define PANELS_MENU_TASKCONFUGURATIN tr("&任务配置")

#define HELP_MENU tr("&帮助")
#define HELP_OPENWIKI tr("&打开wiki")
#define HELP_MENU_ABOUTUS tr("&关于我们")

#define PANEL_SETTING tr("设置")
#define PANEL_MASTERMSG tr("Master消息")
#define PANEL_TASKPRESENT tr("任务呈现")
#define PANEL_VIDEO tr("视频")

#define TOOL_OPENVISUALIZATION tr("打开可视化")
#define TOOL_CLOSEVISUALIZATION tr("关闭可视化")
#define TOOL_SETTING tr("设置")
#define TOOL_MASTERMSG tr("Master消息")
#define TOOL_TASKCONFIGURATION tr("任务")
#define TOOL_OPENVIDEO tr("打开视频")
#define TOOL_CLOSEVIDEO tr("关闭视频")

#define TASK_SEARCH tr("搜索")
#define TASK_ALLSHOW tr("显示全部")
#define TASK_ADD tr("添加")
#define TASK_DELETE tr("删除")

#define TASKLIST_GROUP tr("任务列表")
#define TASKSHOW_GROUP tr("任务展示")
#define TASKANALYSIS_GROUP tr("任务分析")
#define HEARDER_TASKNAME tr("任务名")
#define HEARDER_TASKCODE tr("任务代码")
#define HEARDER_TASKDESCRIBE tr("任务描述")
#define HEARDER_TASKSTATUS tr("任务状态")
#define HEARDER_TASKSTARTTIME tr("开始时间")
#define HEARDER_TASKENDTIME tr("计划结束时间")
#define HEARDER_TASKACTUALTIME tr("实际结束时间")
#define HEARDER_TASKDETAIL tr("任务明细")
#define HEARDER_TASKCONTROL tr("任务控制")

#define HEARDER_DETAIL_MATERIAL tr("物料")
#define HEARDER_DETAIL_LOADING_ROBOT tr("上下料机器人")
#define HEARDER_DETAIL_HANDLING_ROBOT tr("搬运机器人")
#define HEARDER_DETAIL_INSTALLATION_ROBOT tr("安装机器人")

#define SCREENSHOT_SAVEIMAGE tr("保存图片")
#define SCREENSHOT_ERROR tr("错误")
#define SCREENSHOT_ERROR tr("错误")
#define SCREENSHOT_SAVEERROR tr("将图像写入文件失败")
#define SCREENSHOT_SUPPORTEDFORMATS tr("支持的图像格式有：")
#define SCREENSHOT_ALLWINDOW tr("保存整个site_cps窗口")
#define SCREENSHOT_ORIGINAL_RESOLUTION tr("图像将以原始分辨率保存。")

#define TASKDETAILEMPTY tr("任务明细为空")
#define TASKINSTAERTFAILED tr("任务插入数据库失败")

#define ROBOT_INIT tr("未执行")
#define ROBOT_DOING tr("执行中")
#define ROBOT_DONE tr("执行完")

#define ROBOT_SENSOR_STATUS tr("传感器状态")
#define ROBOT_RUN_LOG tr("运行日志")

#define LinuxUDiskRootPath "/media/cps"


#define SAFE_DELETE(p) {if (p != 0) {delete p; p = NULL;}}

#endif // SITE_CPS_COMMON_CONFIG_H
