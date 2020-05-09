/*!
 * \file    mainview.cpp
 * \date    10.09.2018
 * \author  zhusizhi
 * \version 1.0.0
 *
 * \brief   MainWindow class implementation of the Qt cps client
 *
 */

/*****************************************************************************
** Includes
*****************************************************************************/

#include <QtGui>
#include <iostream>
#include <QtDebug>
#include <QSettings>
#include <wchar.h>
#include <QCoreApplication>
#include <QAction>
#include <QShortcut>
#include <QApplication>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QDockWidget>
#include <QDir>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QUrl>
#include <QStatusBar>
#include <QLabel>
#include <QToolButton>
#include <QHBoxLayout>
#include <QTabBar>

#include "visualization_frame.h"

#include "failed_panel.h"
#include "panel_dock_widget.h"

/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace site_cps {

using namespace Qt;

/*****************************************************************************
** Implementation [MainWindow]
*****************************************************************************/

VisualizationFrame::VisualizationFrame(QWidget *parent)
  : QMainWindow(parent)
  , app_(NULL)
  , file_menu_(NULL)
  , recent_configs_menu_(NULL)
  , view_menu_(NULL)
  , toolbar_(NULL)
  , m_statusbar(NULL)
  , m_pGetTaskManager(NULL)
  , m_bLive(true)
  , geom_change_detector_(new WidgetGeometryChangeDetector(this))
  , initialized_(false)
  , show_choose_new_master_option_(false)
  , m_action_setting(NULL)
  , m_action_mastermsg(NULL)
  , m_action_tasklist(NULL)
  , m_action_opencamera(NULL)
  , m_action_closecamera(NULL)
{
  panel_factory_ = new PanelFactory();
  installEventFilter(geom_change_detector_);
  connect( geom_change_detector_, SIGNAL( changed() ), this, SLOT( setDisplayConfigModified() ));

  setWindowTitle (APP_NAME);
  setWindowIcon(QIcon(":/images/icon.png"));

  createStatusBar();
  initialize();
  UpdateControls();

  /*********************
  ** video visualization
  **********************/
//  m_tcpSocket = new QTcpSocket(this);
//  QObject::connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));


  readSettings();
}

VisualizationFrame::~VisualizationFrame()
{
//  for( int i = 0; i < custom_panels_.size(); i++ ) zhusizhi
//  {
//    delete custom_panels_[ i ].dock;
//  }
  SAFE_DELETE(m_taskPanel);


  delete panel_factory_;

  writeSettings();
}

void VisualizationFrame::setApp( QApplication * app )
{
  app_ = app;
}

void VisualizationFrame::createStatusBar()
{
  m_textStatus = new QLabel();
  m_strTextStatus = new QLabel();

  m_textStatus->setText(STATUS_NAME);
  statusBar()->addPermanentWidget(m_textStatus, 0);
  statusBar()->addPermanentWidget(m_strTextStatus, 1);
  m_statusbar = statusBar();

}

void VisualizationFrame::initialize()
{
  m_taskPanel = new TaskConfigurationPanel();
  connect(m_taskPanel, SIGNAL(updateStateText(const QString &)), this, SLOT(updateStateTextSlot(const QString &)));

  if (app_) app_->processEvents();

  QWidget* central_widget = new QWidget(this);
  QHBoxLayout* central_layout = new QHBoxLayout;
  central_layout->setSpacing(0);
  central_layout->setMargin(0);

  hide_left_dock_button_ = new QToolButton();
  hide_left_dock_button_->setContentsMargins(0,0,0,0);
  hide_left_dock_button_->setArrowType( Qt::LeftArrow );
  hide_left_dock_button_->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding ) );
  hide_left_dock_button_->setFixedWidth(16);
  hide_left_dock_button_->setAutoRaise(true);
  hide_left_dock_button_->setCheckable(true);

  connect(hide_left_dock_button_, SIGNAL(toggled(bool)), this, SLOT(hideLeftDock(bool)));

  hide_right_dock_button_ = new QToolButton();
  hide_right_dock_button_->setContentsMargins(0,0,0,0);
  hide_right_dock_button_->setArrowType( Qt::RightArrow );
  hide_right_dock_button_->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding ) );
  hide_right_dock_button_->setFixedWidth(16);
  hide_right_dock_button_->setAutoRaise(true);
  hide_right_dock_button_->setCheckable(true);

  connect(hide_right_dock_button_, SIGNAL(toggled(bool)), this, SLOT(hideRightDock(bool)));

  central_layout->addWidget( hide_left_dock_button_, 0 );
  central_layout->addWidget( m_taskPanel, 1 );
  central_layout->addWidget( hide_right_dock_button_, 0 );

  central_widget->setLayout( central_layout );

  if (app_) app_->processEvents();
  initMenus();

  if (app_) app_->processEvents();
  initToolbars();

  if (app_) app_->processEvents();
  setCentralWidget( central_widget );

  if (app_) app_->processEvents();


  initialized_ = true;

}

/*****************************************************************************
** Implementation [Menu]
*****************************************************************************/
void VisualizationFrame::initMenus()
{
  file_menu_ = menuBar()->addMenu( FILE_MENU );

  QAction * save_image_action= file_menu_->addAction( FILE_MENU_SAVEIMAGE, this, SLOT( onSaveImage() ));
  save_image_action->setIcon(QIcon(":/icon/icons/Image.png"));
  if( show_choose_new_master_option_ )//zhusizhi
  {
    file_menu_->addSeparator();
    file_menu_->addAction( FILE_MENU_CHANGEMASTER, this, SLOT( changeMaster() ));
  }
  file_menu_->addSeparator();

  QAction * file_menu_quit_action = file_menu_->addAction(FILE_MENU_QUIT, this, SLOT( close() ), QKeySequence( "Ctrl+Q" ));
  file_menu_quit_action->setIcon(QIcon(":/images/exit.png"));
  this->addAction(file_menu_quit_action);

  view_menu_ = menuBar()->addMenu( PANELS_MENU );
  QAction * fullscreen_action = view_menu_->addAction(PANELS_MENU_FULLSCREEN, this, SLOT( setFullScreen(bool) ), Qt::Key_F11);
  fullscreen_action->setIcon(QIcon(":/icon/icons/FullScreen.png"));
  fullscreen_action->setCheckable(true);
  this->addAction(fullscreen_action); // Also add to window, or the shortcut doest work when the menu is hidden.
  connect(this, SIGNAL( fullScreenChange( bool ) ), fullscreen_action, SLOT( setChecked( bool ) ) );
    new QShortcut(Qt::Key_Escape, this, SLOT( exitFullScreen() ));
  QAction * monitoring_action = view_menu_->addAction(PANELS_MENU_ROBOT_CONFIGUATION, this, SLOT( onShowMonitoring() ));
  monitoring_action->setIcon(QIcon(":/icon/icons/Monitoring.png"));
  this->addAction(monitoring_action);
  view_menu_->addSeparator();

  QMenu* help_menu = menuBar()->addMenu( HELP_MENU );
  help_menu->addAction( HELP_OPENWIKI, this, SLOT( onHelpWiki() ));
  help_menu->addSeparator();
  help_menu->addAction( HELP_MENU_ABOUTUS, this, SLOT( onHelpAbout() ));
}
/*********************
** toolbar
**********************/
void VisualizationFrame::initToolbars()
{
  QFont font;
  font.setPointSize( font.pointSizeF()*0.9 );

  // make toolbar with plugin tools
  toolbar_ = addToolBar( "Tools" );
  toolbar_->setFont( font );
  toolbar_->setContentsMargins(0,0,0,0);
  toolbar_->setObjectName( "Tools" );
  toolbar_->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
  //toolbar_actions_ = new QActionGroup( this );
  //connect( toolbar_actions_, SIGNAL( triggered( QAction* )), this, SLOT( onToolbarActionTriggered( QAction* )));
  //view_menu_->addAction( toolbar_->toggleViewAction() );

  QAction *action_visualization = new QAction( "", toolbar_ );
  action_visualization->setToolTip(TOOL_OPENVISUALIZATION );
  action_visualization->setIcon( QIcon( ":/images/media_play.png" ) );
  toolbar_->addAction( action_visualization );
  QObject::connect(action_visualization, SIGNAL(triggered()), this, SLOT(visualizationSlot()));

  QAction *action_noDisplay = new QAction( "", toolbar_ );
  action_noDisplay->setToolTip( TOOL_CLOSEVISUALIZATION );
  action_noDisplay->setIcon( QIcon( ":/images/display_no.png" ) );
  toolbar_->addAction( action_noDisplay );
  QObject::connect(action_noDisplay, SIGNAL(triggered()), this, SLOT(noDisplaySlot()));

  toolbar_->addSeparator();

  m_action_setting = new QAction( "", toolbar_ );
  m_action_setting->setToolTip( TOOL_SETTING );
  m_action_setting->setIcon( QIcon( ":/images/setting.png" ) );
  toolbar_->addAction( m_action_setting );
  QObject::connect(m_action_setting, SIGNAL(triggered()), this, SLOT(settingSlot()));

  m_action_mastermsg = new QAction( "", toolbar_ );
  m_action_mastermsg->setToolTip( TOOL_MASTERMSG );
  m_action_mastermsg->setIcon( QIcon( ":/images/master_info.png" ) );
  toolbar_->addAction( m_action_mastermsg );
  QObject::connect(m_action_mastermsg, SIGNAL(triggered()), this, SLOT(topicWindowSlot()));

  m_action_tasklist = new QAction( "", toolbar_ );
  m_action_tasklist->setToolTip( TOOL_TASKCONFIGURATION );
  m_action_tasklist->setIcon( QIcon( ":/images/load_task.png" ) );
  toolbar_->addAction( m_action_tasklist );
  QObject::connect(m_action_tasklist, SIGNAL(triggered()), this, SLOT(showTaskListSlot()));

  toolbar_->addSeparator();

  m_action_opencamera = new QAction( "", toolbar_ );
  m_action_opencamera->setToolTip( TOOL_OPENVIDEO );
  m_action_opencamera->setIcon( QIcon( ":/images/openplay.png" ) );
  toolbar_->addAction( m_action_opencamera );
  QObject::connect(m_action_opencamera, SIGNAL(triggered()), this, SLOT(openCameraSlot()));

  m_action_closecamera = new QAction( "", toolbar_ );
  m_action_closecamera->setToolTip( TOOL_CLOSEVIDEO );
  m_action_closecamera->setIcon( QIcon( ":/images/openstop.png" ) );
  toolbar_->addAction( m_action_closecamera );
  QObject::connect(m_action_closecamera, SIGNAL(triggered()), this, SLOT(closeCameraSlot()));
}

void VisualizationFrame::writeSettings()
{
    // saves window positions and states
    QSettings settings("site_cps", "visualizationFrame");

    settings.beginGroup("mainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState());
    settings.endGroup();
}

void VisualizationFrame::readSettings()
{
    // reads window positions and states
    QSettings settings("site_cps", "visualizationFrame");

    settings.beginGroup("mainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());
    settings.endGroup();
}

void VisualizationFrame::setFullScreen( bool full_screen )
{
  Q_EMIT( fullScreenChange( full_screen ) );

  if (full_screen)
    toolbar_visible_ = toolbar_->isVisible();
  menuBar()->setVisible(!full_screen);
  toolbar_->setVisible(!full_screen && toolbar_visible_);
  statusBar()->setVisible(!full_screen);
  setHideButtonVisibility(!full_screen);

  if (full_screen)
    setWindowState(windowState() | Qt::WindowFullScreen);
  else
    setWindowState(windowState() & ~Qt::WindowFullScreen);
  show();
}

void VisualizationFrame::setImageSaveDirectory( const QString& directory)
{
  last_image_dir_ = directory.toStdString();
}

void VisualizationFrame::exitFullScreen()
{
  setFullScreen( false );
}

/*****************************************************************************
** Implementation [Configuration]
*****************************************************************************/
bool VisualizationFrame::saveDisplayConfig( const QString& path )
{
  Config config;
  save( config );

//  YamlConfigWriter writer;
//  writer.writeFile( config, path );
//  if( writer.error() )
//  {
//    qDebug( "saveDisplayConfig error:%s", qPrintable( writer.errorMessage() ));
//    error_message_ = writer.errorMessage();
//    return false;
//  }
//  else
//  {
//    setWindowModified( false );
//    error_message_ = "";
//    return true;
//  }
}

void VisualizationFrame::save( Config config )
{
  savePanels( config.mapMakeChild( "Panels" ));
  saveWindowGeometry( config.mapMakeChild( "Window Geometry" ));

}

void VisualizationFrame::load( const Config& config )
{
  loadPanels( config.mapGetChild( "Panels" ));
  loadWindowGeometry( config.mapGetChild( "Window Geometry" ));

}

void VisualizationFrame::loadWindowGeometry( const Config& config )
{
  int x, y;
  if( config.mapGetInt( "X", &x ) &&
      config.mapGetInt( "Y", &y ))
  {
    move( x, y );
  }

  int width, height;
  if( config.mapGetInt( "Width", &width ) &&
      config.mapGetInt( "Height", &height ))
  {
    resize( width, height );
  }

  QString main_window_config;
  if( config.mapGetString( "QMainWindow State", &main_window_config ))
  {
    restoreState( QByteArray::fromHex( qPrintable( main_window_config )));
  }

  // load panel dock widget states (collapsed or not)
  QList<PanelDockWidget *> dock_widgets = findChildren<PanelDockWidget *>();

  for ( QList<PanelDockWidget *>::iterator it=dock_widgets.begin(); it!=dock_widgets.end(); it++ )
  {
    Config itConfig = config.mapGetChild((*it)->windowTitle());

    if (itConfig.isValid())
    {
      (*it)->load(itConfig);
    }
  }

  bool b;
  config.mapGetBool( "Hide Left Dock", &b );
  hide_left_dock_button_->setChecked( b );
  hideLeftDock(b);
  config.mapGetBool( "Hide Right Dock", &b );
  hideRightDock(b);
  hide_right_dock_button_->setChecked( b );
}

void VisualizationFrame::saveWindowGeometry( Config config )
{
  config.mapSetValue( "X", x() );
  config.mapSetValue( "Y", y() );
  config.mapSetValue( "Width", width() );
  config.mapSetValue( "Height", height() );

  QByteArray window_state = saveState().toHex();
  config.mapSetValue( "QMainWindow State", window_state.constData() );

  config.mapSetValue( "Hide Left Dock", hide_left_dock_button_->isChecked() );
  config.mapSetValue( "Hide Right Dock", hide_right_dock_button_->isChecked() );

  // save panel dock widget states (collapsed or not)
  QList<PanelDockWidget *> dock_widgets = findChildren<PanelDockWidget *>();

  for ( QList<PanelDockWidget *>::iterator it=dock_widgets.begin(); it!=dock_widgets.end(); it++ )
  {
    (*it)->save(config.mapMakeChild( (*it)->windowTitle() ));
  }
}

void VisualizationFrame::loadPanels( const Config& config )
{
  // First destroy any existing custom panels.
  for( int i = 0; i < custom_panels_.size(); i++ )
  {
    delete custom_panels_[ i ].dock;
  }
  custom_panels_.clear();

  // Then load the ones in the config.
  int num_custom_panels = config.listLength();
  for( int i = 0; i < num_custom_panels; i++ )
  {
    Config panel_config = config.listChildAt( i );

    QString class_id, name;
    if( panel_config.mapGetString( "Class", &class_id ) &&
        panel_config.mapGetString( "Name", &name ))
    {
      QDockWidget* dock = addPanelByName( name, class_id );
      // This is kind of ridiculous - should just be something like
      // createPanel() and addPanel() so I can do load() without this
      // qobject_cast.
      if( dock )
      {
        connect(dock, SIGNAL( dockLocationChanged( Qt::DockWidgetArea )), this, SLOT( onDockPanelChange() ) );
        Panel* panel = qobject_cast<Panel*>( dock->widget() );
        if( panel )
        {
          panel->load( panel_config );
        }
      }
    }
  }

  onDockPanelChange();
}

void VisualizationFrame::savePanels( Config config )
{
  config.setType( Config::List ); // Not really necessary, but gives an empty list if there are no entries, instead of an Empty config node.

  for( int i = 0; i < custom_panels_.size(); i++ )
  {
    custom_panels_[ i ].panel->save( config.listAppendNew() );
  }
}

void VisualizationFrame::closeEvent(QCloseEvent *event)
{
  //if( prepareToExit() )
  {
    event->accept();
  }
//  else
//  {
//    event->ignore();
//  }
}

void VisualizationFrame::changeMaster()
{
 // if( prepareToExit() )//zhusizhi
  {
    QApplication::exit( 255 );
  }
}

void VisualizationFrame::sendGetTaskRequest(const QString &strTaskID)
{
  CPS_TASK data;

  QByteArray rqByte;
  rqByte.clear();

  //序列化
  rqByte = "";//m_cpsTask->jsonSerializer(strTaskID);

  //编辑HTTP头部
  QString url = CUrlUtil::getTaskDataUrl();
  QNetworkRequest request = QNetworkRequest(QUrl(url));

  request.setRawHeader(QString("Content-Type").toLatin1(),QString("application/json; charset=utf-8").toLatin1());
  request.setRawHeader(QString("Content-Length").toLatin1(),QString::number(rqByte.length()).toLatin1());
  qDebug("get task url: %s", qPrintable(url));

  //执行post请求
  m_pGetTaskManager = new QNetworkAccessManager(this);
  if (m_pGetTaskManager)
  {
    QNetworkReply *reply = m_pGetTaskManager->post(request,rqByte);
    connect(m_pGetTaskManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(getTaskResposeSlot(QNetworkReply*)));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(getTaskErrorSlot(QNetworkReply::NetworkError)));
  }

}

void VisualizationFrame::loadDisplayConfig( const QString& qpath )
{
  std::string path = qpath.toStdString();
  std::string actual_load_path = path;

//  if( !fs::exists( path ) || fs::is_directory( path ) || fs::is_empty( path ))
//  {
//    actual_load_path = (fs::path(package_path_) / "default.cps").BOOST_FILE_STRING();
//    if( !fs::exists( actual_load_path ))
//    {
//      qDebug( "Default display config '%s' not found.  cps will be very empty at first.", actual_load_path.c_str() );
//      return;
//    }
//  }

  // Check if we have unsaved changes to the current config the same
  // as we do during exit, with the same option to cancel.


  setWindowModified( false );

//  YamlConfigReader reader;
//  Config config;
//  reader.readFile( config, QString::fromStdString( actual_load_path ));
//  if( !reader.error() )
//  {
//    load( config );
//  }

//  markRecentConfig( path );

//  setDisplayConfigFile( path );

  //last_config_dir_ = fs::path( path ).parent_path().BOOST_FILE_STRING();

}


/*****************************************************************************
** Implementation [Slots]
*****************************************************************************/

void VisualizationFrame::onSaveImage()
{
  ScreenshotDialog* dialog = new ScreenshotDialog( this, m_taskPanel, QString::fromStdString( last_image_dir_ ));
  connect( dialog, SIGNAL( savedInDirectory( const QString& )),
           this, SLOT( setImageSaveDirectory( const QString& )));
  dialog->show();
}



void VisualizationFrame::onShowMonitoring()
{
    addPanelByName("Setting", "site_cps/Setting");
}

void VisualizationFrame::onHelpWiki()
{
  QDesktopServices::openUrl( QUrl( "http://www.ros.org/wiki" ));
}

void VisualizationFrame::onHelpAbout()
{
    QMessageBox::information(this, ABOUTUS, ABOUTINFO),QMessageBox::Ok);
}

void VisualizationFrame::getTaskResposeSlot(QNetworkReply* reply)
{
  if (reply->error() == QNetworkReply::NoError)
  {
      QByteArray bytes = reply->readAll();

  }

  reply->deleteLater();
}

void VisualizationFrame::getTaskErrorSlot(QNetworkReply::NetworkError errorCode)
{
    QString strError = QString("%1%2").arg(GET_SERVER_TASK).arg(QString::number(errorCode,10));
    m_strTextStatus->setText(strError);
}

void VisualizationFrame::displayError(QAbstractSocket::SocketError error)
{
//    QString strError = m_tcpSocket->errorString().toLatin1();
//    QString strErrorInfo = QString("%1%2").arg(GET_SERVER_VIDEO).arg(strError);
//    m_strTextStatus->setText(strErrorInfo);
}

void VisualizationFrame::dataReceivedSlot()
{
//    QByteArray data = m_tcpSocket->readAll();
//    QString str(data);
//    if (str.contains("IMG")) {

//    }

}

void VisualizationFrame::visualizationSlot()
{
//    m_tcpSocket->connectToHost(QHostAddress("192.168.43.10"), 33333);
//    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(dataReceivedSlot()));
//    m_bLive = false;
//    UpdateControls();
}

void VisualizationFrame::noDisplaySlot()
{
//    m_tcpSocket->disconnectFromHost();
//    m_bLive = true;
//    UpdateControls();
}

void VisualizationFrame::updateStateTextSlot(const QString &strText)
{
    if (m_statusbar == statusBar())
        m_strTextStatus->setText(strText);

}

void VisualizationFrame::showTaskListSlot()
{
  for( int i = 0; i < custom_panels_.size(); i++ )
  {
    if ("TaskConfiguration" == custom_panels_[i].name)
    {
      if (!custom_panels_[i].dock->isVisible())
      {

          custom_panels_[i].dock->setVisible(true);
          custom_panels_[i].dock->raise();
          m_action_tasklist->setChecked(true);
      }
      else
      {
          custom_panels_[i].dock->setVisible(false);
          m_action_tasklist->setChecked(false);
      }
    }
  }
}

void VisualizationFrame::topicWindowSlot()
{
  for( int i = 0; i < custom_panels_.size(); i++ )
  {
    if ("MasterConfiguration" == custom_panels_[i].name)
    {
      if (!custom_panels_[i].dock->isVisible())
      {
          custom_panels_[i].dock->setVisible(true);
          custom_panels_[i].dock->raise();
          m_action_mastermsg->setChecked(true);
      }
      else
      {
          custom_panels_[i].dock->setVisible(false);
          m_action_mastermsg->setChecked(false);
      }
    }
  }

}

void VisualizationFrame::settingSlot()
{
  for( int i = 0; i < custom_panels_.size(); i++ )
  {
    if ("Setting" == custom_panels_[i].name)
    {
      if (!custom_panels_[i].dock->isVisible())
      {
          custom_panels_[i].dock->setVisible(true);
          custom_panels_[i].dock->raise();
          m_action_setting->setChecked(true);
      }
      else
      {
          custom_panels_[i].dock->setVisible(false);
          m_action_setting->setChecked(false);
      }
    }
  }
}

void VisualizationFrame::openCameraSlot()
{
  for( int i = 0; i < custom_panels_.size(); i++ )
  {
    if ("Camera" == custom_panels_[i].name)
    {
      if (!custom_panels_[i].dock->isVisible())
      {
          custom_panels_[i].dock->setVisible(true);
          custom_panels_[i].dock->raise();
          m_action_setting->setChecked(true);
      }
    }
  }
}

void VisualizationFrame::closeCameraSlot()
{
  for( int i = 0; i < custom_panels_.size(); i++ )
  {
    if ("Camera" == custom_panels_[i].name)
    {
      if (custom_panels_[i].dock->isVisible())
      {
          custom_panels_[i].dock->setVisible(false);
          m_action_setting->setChecked(false);
      }
    }
  }
}

void VisualizationFrame::UpdateControls ()
{
    //if (m_bLive)
//    {
//        m_imageLabel->setScaledContents(false);
//        m_imageLabel->setPixmap(QPixmap(":/images/cps_bg.png"));
//        m_imageLabel->setAlignment(Qt::AlignCenter);
//        m_scrollArea->setWidgetResizable(true);
//    }
//    else
//    {
//        m_imageLabel->setScaledContents(true);
//    }
}

void VisualizationFrame::setDisplayConfigModified()
{
  if(!isWindowModified())
  {
    setWindowModified(true);
  }
}

QDockWidget* VisualizationFrame::addPanelByName( const QString& name,
                                                 const QString& class_id,
                                                 Qt::DockWidgetArea area,
                                                 bool floating)
{

  QString error;
  Panel* panel = panel_factory_->make(class_id, &error);

  if( !panel )
  {
    panel = new FailedPanel( class_id, error );
  }

  panel->setName( name );
  connect( panel, SIGNAL( configChanged() ), this, SLOT( setDisplayConfigModified() ));

  PanelRecord record;
  record.dock = addPane( name, panel, area, floating);
  record.panel = panel;
  record.name = name;
  custom_panels_.append( record );

  record.dock->setIcon( panel_factory_->getIcon( name ) );

  return record.dock;
}

PanelDockWidget* VisualizationFrame::addPane( const QString& name, QWidget* panel, Qt::DockWidgetArea area, bool floating )
{
  PanelDockWidget *dock;
  dock = new PanelDockWidget( name );
  dock->setContentWidget( panel );
  dock->setFloating( floating );
  dock->setObjectName( name ); // QMainWindow::saveState() needs objectName to be set.
  addDockWidget( area, dock );

  // we want to know when that panel becomes visible
  connect( dock, SIGNAL( visibilityChanged( bool )), this, SLOT( onDockPanelVisibilityChange( bool ) ));
  connect( this, SIGNAL( fullScreenChange(bool) ), dock, SLOT( overrideVisibility(bool) ));

  QAction* toggle_action = dock->toggleViewAction();
  view_menu_->addAction( toggle_action );

  connect( toggle_action, SIGNAL( triggered( bool )), this, SLOT( setDisplayConfigModified() ));
  connect( dock, SIGNAL( closed()), this, SLOT( setDisplayConfigModified() ));

  dock->installEventFilter( geom_change_detector_ );

  // repair/update visibility status
  hideLeftDock( area == Qt::LeftDockWidgetArea ? false : hide_left_dock_button_->isChecked() );
  hideRightDock( area == Qt::RightDockWidgetArea ? false : hide_right_dock_button_->isChecked() );

  return dock;
}

void VisualizationFrame::onDockPanelVisibilityChange( bool visible )
{
  // if a dock widget becomes visible and is resting inside the
  // left or right dock area, we want to unhide the whole area
  if ( visible )
  {
    QDockWidget* dock_widget = dynamic_cast<QDockWidget*>( sender() );
    if ( dock_widget )
    {
      Qt::DockWidgetArea area = dockWidgetArea( dock_widget );
      if ( area == Qt::LeftDockWidgetArea )
      {
        hide_left_dock_button_->setChecked( false );
      }
      if ( area == Qt::RightDockWidgetArea )
      {
        hide_right_dock_button_->setChecked( false );
      }
    }
  }

}

void VisualizationFrame::hideDockImpl( Qt::DockWidgetArea area, bool hide )
{
  QList<PanelDockWidget *> dock_widgets = findChildren<PanelDockWidget *>();

  for ( QList<PanelDockWidget *>::iterator it=dock_widgets.begin(); it!=dock_widgets.end(); it++ )
  {
    Qt::DockWidgetArea curr_area = dockWidgetArea ( *it );
    if ( area == curr_area )
    {
      (*it)->setCollapsed(hide);
    }
    // allow/disallow docking to this area for all widgets
    if ( hide )
    {
      (*it)->setAllowedAreas( (*it)->allowedAreas() & ~area );
    }
    else
    {
      (*it)->setAllowedAreas( (*it)->allowedAreas() | area );
    }
  }
}

void VisualizationFrame::setHideButtonVisibility( bool visible )
{
  hide_left_dock_button_->setVisible( visible );
  hide_right_dock_button_->setVisible( visible );
}

void VisualizationFrame::hideLeftDock( bool hide )
{
  hideDockImpl( Qt::LeftDockWidgetArea, hide );
  hide_left_dock_button_->setArrowType( hide ? Qt::RightArrow : Qt::LeftArrow );
}

void VisualizationFrame::hideRightDock( bool hide )
{
  hideDockImpl( Qt::RightDockWidgetArea, hide );
  hide_right_dock_button_->setArrowType( hide ? Qt::LeftArrow : Qt::RightArrow );
}

void VisualizationFrame::onDockPanelChange()
{
  QList<QTabBar *> tab_bars = findChildren<QTabBar *>(QString());
  for ( QList<QTabBar *>::iterator it = tab_bars.begin(); it != tab_bars.end(); it++ )
  {
    (*it)->setElideMode( Qt::ElideNone );
  }
}

}  // namespace site_cps


