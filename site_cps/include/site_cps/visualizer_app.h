#ifndef SITE_CPS_VISUALIZER_APP_H
#define SITE_CPS_VISUALIZER_APP_H

#include <QApplication>
#include <QObject>

#include "cps.h"

class QTimer;

namespace site_cps
{

class VisualizationFrame;

class VisualizerApp: public QObject
{
Q_OBJECT
public:
  VisualizerApp();
  virtual ~VisualizerApp();

  void setApp( QApplication * app );

  /** Start everything.  Pass in command line arguments.
   * @return false on failure, true on success. */
  bool init();


private:
  QApplication* app_;
  VisualizationFrame* frame_;

};

} // end namespace site_cps

#endif // SITE_CPS_VISUALIZER_APP_H
