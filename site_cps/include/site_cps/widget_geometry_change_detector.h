#ifndef SITE_CPS_WIDGET_GEOMETRY_CHANGE_DETECTOR_H
#define SITE_CPS_WIDGET_GEOMETRY_CHANGE_DETECTOR_H

#include <QObject>

namespace site_cps
{

/** @brief Utility class for watching for events which indicate that widget geometry has changed. */
class WidgetGeometryChangeDetector: public QObject
{
Q_OBJECT
public:
  WidgetGeometryChangeDetector( QObject* parent = NULL );

  virtual bool eventFilter( QObject* watched, QEvent* event );

Q_SIGNALS:
  /** @brief This signal is emitted whenever any filtered events are detected. */
  void changed();
};

} // end namespace site_cps

#endif // SITE_CPS_WIDGET_GEOMETRY_CHANGE_DETECTOR_H
