#include <stdio.h>

#include <QHBoxLayout>
#include <QTextBrowser>

#include "failed_panel.h"

namespace site_cps
{

FailedPanel::FailedPanel( const QString& desired_class_id, const QString& error_message )
  : error_message_( error_message )
{  
  setClassId( desired_class_id );

  QTextBrowser* error_display = new QTextBrowser;
  error_display->setHtml( "The class required for this panel, '" + getClassId() + "', could not be loaded.<br><b>Error:</b><br>" + error_message_ );

  QHBoxLayout* layout = new QHBoxLayout;
  layout->addWidget( error_display );
  setLayout( layout );
}

void FailedPanel::load( const Config& config )
{
  saved_config_ = config;
  Panel::load( config );
}

void FailedPanel::save( Config config ) const
{
  if( saved_config_.isValid() )
  {
    config.copy( saved_config_ );
  }
  else
  {
    Panel::save( config );
  }
}

} // end namespace site_cps
