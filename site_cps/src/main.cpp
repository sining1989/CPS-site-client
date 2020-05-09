/**
 * @file /src/main.cpp
 *
 * @brief Qt based gui.
 *
 * @date November 2010
 **/
/*****************************************************************************
** Includes
*****************************************************************************/

#include <QtGui>
#include <QApplication>
#include <QMutex>
#include <QTranslator>

#include "visualizer_app.h"

/*****************************************************************************
** Main
*****************************************************************************/

int main(int argc, char **argv)
{
    execl("/home/zsz/site_cps_topic/devel/lib/", "site_cps_topic");
    /*********************
    ** Qt
    **********************/
    QApplication app(argc, argv);

    QTextCodec* codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

    //加载QSS样式表
    QFile qss(":/qss/site_cps.qss");
    qss.open(QFile::ReadOnly);
    qApp->setStyleSheet(qss.readAll());
    qss.close();

    site_cps::VisualizerApp vapp;
    vapp.setApp(&app);
    if (vapp.init())
    {
      app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
      return app.exec();
    }
    else
    {
      return 1;
    }

}
