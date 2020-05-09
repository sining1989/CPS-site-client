
#ifndef SITE_CPS_PANEL_FACTORY_H
#define SITE_CPS_PANEL_FACTORY_H

#include "panel.h"
#include "pluginlib_factory.h"

namespace site_cps
{

class PanelFactory: public PluginlibFactory<Panel>
{
public:
  PanelFactory();
};

} // end namespace site_cps

#endif // SITE_CPS_PANEL_FACTORY_H
