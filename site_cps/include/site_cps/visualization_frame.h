/**
 * @file /include/site_cps/visualization_frame.h
 *
 * @brief Qt based gui for site cps.
 *
 * @date November 2010
 **/
#ifndef SITE_CPS_VISUALOZATION_FRAME_H
#define SITE_CPS_VISUALIZATION_FRAME_H

/*****************************************************************************
** Includes
*****************************************************************************/

#include <QMainWindow>
#include <QProgressBar>
#include <QTimer>
#include <QTime>
#include <QScrollArea>
#include <QScrollBar>
#include <QVector>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTcpSocket>
#include <QHostAddress>
#include <QToolButton>
#include <deque>

#ifndef Q_MOC_RUN
#include <boost/shared_ptr.hpp>
#endif
#include "cps.h"
#include "common_config.h"
#include "config.h"
#include "paintlabel.h"
#include "urlutil.h"
//#include "setting_panel.h"
#include "task_configuration_panel.h"
#include "panel.h"
#include "window_manager_interface.h"
#include "widget_geometry_change_detector.h"
#include "panel_factory.h"
#include "screenshot_dialog.h"

//#include "yaml_config_writer.h"
//#include "yaml_config_reader.h"

/*****************************************************************************
** Namespace
*****************************************************************************/

namespace site_cps {

/*****************************************************************************
** Interface [MainWindow]
*****************************************************************************/
/**
 * @brief Qt central, all operations relating to the view part here.
 */
class VisualizationFrame : public QMainWindow
{
Q_OBJECT

public:
    VisualizationFrame(QWidget *parent = 0);
    ~VisualizationFrame();

    void setApp( QApplication * app );

    /** @brief Load the properties of all subsystems from the given Config.
     *
     * This is called by loadDisplayConfig().
     *
     * @param config Must have type Config::Map.
     * @sa save() */
    virtual void load( const Config& config );

    /** @brief Save the properties of each subsystem and most editable cps
     *         data.
     *
     * This is called by saveDisplayConfig().
     *
     * @param config The Config node to write into.
     * @sa load() */
    virtual void save( Config config );

    void initialize();
    void setHideButtonVisibility( bool visible );
    void loadDisplayConfig( const QString& qpath );
    bool saveDisplayConfig( const QString& path );
    QString getErrorMessage() const { return error_message_; }

protected:
    void initMenus();
    void initToolbars();

    virtual void closeEvent(QCloseEvent *event); // Overloaded function

    void updateRecentConfigMenu();

    QDockWidget* addPanelByName( const QString& name,
                                   const QString& class_lookup_name,
                                   Qt::DockWidgetArea area = Qt::LeftDockWidgetArea,
                                   bool floating = true);

    virtual PanelDockWidget* addPane( const QString& name,
                                  QWidget* pane,
                                  Qt::DockWidgetArea area = Qt::LeftDockWidgetArea,
                                  bool floating = true);

    /** @brief Loads custom panels from the given config node. */
    void loadPanels( const Config& config );

    /** @brief Saves custom panels to the given config node. */
    void savePanels( Config config );

    void loadWindowGeometry( const Config& config );
    void saveWindowGeometry( Config config );
    void hideDockImpl( Qt::DockWidgetArea area, bool hide );
    void setDisplayConfigFile( const std::string& path );

private:
    void createStatusBar();
    /*!
     * \brief Saves the position and window states
     */
    void writeSettings();
    /*!
     * \brief Reeads the position and window states
     */
    void readSettings();
    void sendGetTaskRequest(const QString &strTaskID);
   // void loadXmppServer();
    void UpdateControls();

Q_SIGNALS:
  /** @brief Emitted when the interface enters or leaves full screen mode. */
  void fullScreenChange( bool hidden );

public Q_SLOTS:
  virtual void onDockPanelVisibilityChange( bool visible );  
  void updateStateTextSlot(const QString &strText);

protected Q_SLOTS:
  void onSaveImage();
  void onShowMonitoring();
  void onHelpWiki();
  void onHelpAbout();
  void hideLeftDock( bool hide );
  void hideRightDock( bool hide );

  void onDockPanelChange();

  void changeMaster();
  void setImageSaveDirectory( const QString& directory);
  void setFullScreen(bool full_screen);
  void exitFullScreen();

private Q_SLOTS:
    void getTaskResposeSlot(QNetworkReply*reply);
    void getTaskErrorSlot(QNetworkReply::NetworkError errorCode);
    void displayError(QAbstractSocket::SocketError error);
    void dataReceivedSlot();
    void visualizationSlot();
    void noDisplaySlot();
    void showTaskListSlot();
    void topicWindowSlot();
    void settingSlot();
    void openCameraSlot();
    void closeCameraSlot();
    void setDisplayConfigModified();

protected:
    QApplication* app_;
    WidgetGeometryChangeDetector* geom_change_detector_;
    PanelFactory* panel_factory_;

    QMenu* file_menu_;
    QMenu* recent_configs_menu_;
    QMenu* view_menu_;

    QToolBar* toolbar_;
    QAction *m_action_setting;
    QAction *m_action_mastermsg;
    QAction *m_action_tasklist;
    QAction *m_action_opencamera;
    QAction *m_action_closecamera;

    struct PanelRecord
    {
      Panel* panel;
      PanelDockWidget* dock;
      QString name;
      QString class_id;
    };
    QList<PanelRecord> custom_panels_;

    std::string last_config_dir_;
    std::string last_image_dir_;
    std::string home_dir_;
    bool initialized_;
    typedef std::deque<std::string> D_string;
    D_string recent_configs_;
    bool show_choose_new_master_option_;
    std::string package_path_;

private:
    QLabel *m_textStatus;
    QLabel *m_strTextStatus;
    QStatusBar* m_statusbar;

    //QTcpSocket *m_tcpSocket;
    QByteArray  m_imgData;
    int         m_imgSize;

    //PaintLabel *m_imageLabel;
    //QScrollArea *m_scrollArea;
    QNetworkAccessManager *m_pGetTaskManager;

    bool m_bLive;

    QToolButton* hide_left_dock_button_;
    QToolButton* hide_right_dock_button_;
    TaskConfigurationPanel* m_taskPanel;
    QString error_message_; ///< Error message (if any) from most recent saveDisplayConfig() call.

    bool toolbar_visible_;
};

}  // namespace site_cps

#endif // SITE_CPS_VISUALIZATION_FRAME_H
