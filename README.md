# CPS-site-client
一种基于ros_qtc_plugin开发的site、客户端，cps site是基于ros_qtc_plugin开发的一个单独的工作空间，对上与cps global（云端）通过qxmpp进行数据交互，对下与cps client通过ros topic进行
消息互通，基于Qt框架开发的客户端使用QWebEngine嵌套基于three.js开发的3D页面做可视化，同时与基于node.js服务端开发的vrep仿真平台串联；
cps client也是基于ros_qtc_plugin开发的一个单独的workspace，对上与cps site通过ros topic进行消息互通，对下串联手机客户端和机器人本体。

系统版本：Ubuntu 16.04
ROS版本：kinetic
Qt：Qt5.9.1
插件版本：Qt57creator-plugin-ros

sudo apt-get install build-essential qt5-default qtcreator -y

//更新ros plugin 的get 路径
sudo add-apt-repository ppa:levi-armstrong/qt-libraries-xenial
sudo add-apt-repository ppa:levi-armstrong/ppa

//更新qt到QT5.9
sudo apt update
sudo apt install qt59creator

//安装插件
sudo apt install qt57creator-plugin-ros

