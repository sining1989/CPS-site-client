#ifndef SITE_CPS_WINDOW_MANAGER_INTERFACE_H
#define SITE_CPS_WINDOW_MANAGER_INTERFACE_H

class QWidget;
class QString;

namespace site_cps
{
class PanelDockWidget;

class WindowManagerInterface
{
public:
  virtual QWidget* getParentWindow() = 0;

  /** Add a pane to the visualizer.  To remove a pane, just delete it.
   * For example: "delete my_panel_dock_widget;".  Other operations
   * can also be done directly to the PanelDockWidget: show(), hide(),
   * close(), etc. */ 
  virtual PanelDockWidget* addPane( const QString& name,
                                QWidget* pane,
                                Qt::DockWidgetArea area = Qt::LeftDockWidgetArea,
                                bool floating = true ) = 0;

  /** Set the message displayed in the status bar */
  virtual void setStatus( const QString & message ) = 0;
};

} // end namespace site_cps

#endif
