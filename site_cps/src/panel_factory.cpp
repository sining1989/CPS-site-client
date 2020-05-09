//#include "monitoring_panel.h"
#include "setting_panel.h"
//#include "task_configuration_panel.h"
#include "panel_factory.h"

namespace site_cps
{
//static Panel* newMonitoringPanel()            { return new MonitoringPanel(); }
static Panel* newSettingPanel()               { return new SettingPanel(); }
//static Panel* newTaskConfigurationPanel()     { return new TaskConfigurationPanel(); }

PanelFactory::PanelFactory()
  : PluginlibFactory<Panel>( "site_cps", "site_cps::Panel" )
{
 // addBuiltInClass( "site_cps", "Monitoring", "Monitoring show", &newMonitoringPanel );
  addBuiltInClass( "site_cps", "Setting", "Setting xmpp url etc", &newSettingPanel );
  //addBuiltInClass( "site_cps", "TaskConfiguration", "Task configuration and progress output", &newTaskConfigurationPanel );
}

} // end namespace site_cps
