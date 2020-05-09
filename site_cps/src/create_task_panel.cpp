/*!
 * \file    create_task_panel.cpp
 * \date    2018.10.24
 * \author  zhusizhi
 * \version 1.0.0
 *
 * \brief   create task panel
 *
 */
#include "create_task_panel.h"

namespace site_cps
{
SWebEngineView::SWebEngineView(QWidget *parent) : QWebEngineView(parent)
{
}

QList<QUrl> SWebEngineView::listUrl() const
{
  return m_listUrl;
}

void SWebEngineView::addItemUrl(QUrl url)
{
  m_listUrl << url;
}

QWebEngineView *SWebEngineView::createWindow(QWebEnginePage::WebWindowType type)
{
  Q_UNUSED(type);
  m_listUrl << this->url();
  return this;
}


CreateTaskPanel::CreateTaskPanel(QWidget *parent) :
    Panel( parent )
{    
    this->resize(900, 700);

    m_webView = new QWebEngineView(this);
    m_webView->setStyleSheet("background-color:rgb(150,147,88); padding: 7px ; color:rgb(255,255,255)");
    QString url = CUrlUtil::getCreateTaskUrl();
    m_webView->load(url);
    m_webView->show();

}

CreateTaskPanel::~CreateTaskPanel()
{
    if (NULL != m_webView)
    {
        delete m_webView;
        m_webView = NULL;
    }
}

void CreateTaskPanel::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawRect(QRect(0,0,this->width(),this->height()));
}

void CreateTaskPanel::resizeEvent(QResizeEvent* event)
{
    m_webView->resize(this->size());
}



}


