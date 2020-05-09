#ifndef SITE_CPS_PANEL_DOCK_WIDGET_H
#define SITE_CPS_PANEL_DOCK_WIDGET_H

#include <QDockWidget>
#include <QLabel>

#include "config.h"

namespace site_cps
{

class PanelDockWidget: public QDockWidget
{
Q_OBJECT
public:
  PanelDockWidget( const QString& name );

  void setContentWidget( QWidget* child );

  void setCollapsed( bool collapsed );

  void setIcon( QIcon icon );

  virtual void save( Config config );
  virtual void load( Config config );

  /** @brief Override setVisible to respect the visibility override, */
  virtual void setVisible( bool visible );

protected:

  virtual void closeEvent ( QCloseEvent * event );

public Q_SLOTS:

  void setWindowTitle( QString title );

  /** @ Override the visibility of the widget. **/
  virtual void overrideVisibility( bool hide );

private Q_SLOTS:
  void onChildDestroyed( QObject* );

Q_SIGNALS:

  void closed();

private:
  // set to true if this panel was collapsed
  bool collapsed_;
  bool requested_visibility_;
  bool forced_hidden_;
  QLabel *icon_label_;
  QLabel *title_label_;
};


} // end namespace site_cps

#endif // SITE_CPS_PANEL_DOCK_WIDGET_H
