/*!
 * \file    create_task_panel.h
 * \date    2018.10.24
 * \author  zhusizhi
 * \version 1.0.0
 *
 * \brief   create task panel
 *
 */
#ifndef SITE_CPS_CREATE_TASK_PANEL_H
#define SITE_CPS_CREATE_TASK_PANEL_H

#include <QSignalMapper>
#include <QWebEngineView>

#include <QUrl>
#include "panel.h"
#include "urlutil.h"

/*! \brief setting panel class declaration */
namespace site_cps
{

class SWebEngineView : public QWebEngineView
{
  Q_OBJECT
public:
  explicit SWebEngineView(QWidget *parent = 0);

  QList<QUrl> listUrl() const;
  void addItemUrl(QUrl url);
protected:
  /**
  * @brief createWindow
  * @param type
  * @return
  */
  QWebEngineView *createWindow(QWebEnginePage::WebWindowType type);
private:
  QList<QUrl> m_listUrl;
};

class CreateTaskPanel : public Panel
{
    Q_OBJECT

public:
    /*! \brief std constructor */
    CreateTaskPanel(QWidget *parent = 0);
    /*! \brief std destructor */
    virtual ~CreateTaskPanel();

protected:
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);

private:
    QWebEngineView* m_webView;

};
}
#endif // SITE_CPS_CREATE_TASK_PANEL_H
