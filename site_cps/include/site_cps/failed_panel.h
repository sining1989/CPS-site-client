#ifndef SITE_CPS_FAILED_PANEL_H
#define SITE_CPS_FAILED_PANEL_H

#include <memory>
#include "config.h"
#include "panel.h"

namespace site_cps
{

class FailedPanel: public Panel
{
Q_OBJECT
public:
  FailedPanel( const QString& desired_class_id, const QString& error_message );

  /** @brief Store the given Config data for later, so we can return it
   * with save() when someone writes this back to a file. */
  virtual void load( const Config& config );

  /** @brief Copy Config data into config equivalent to the last which was sent to load(). */
  virtual void save( Config config ) const;

private:
  Config saved_config_;
  QString error_message_;
};

} // end namespace site_cps

#endif // SITE_CPS_FAILED_PANEL_H
