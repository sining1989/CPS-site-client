#include "load_resource.h"

#include <boost/filesystem.hpp>

#include <QPixmapCache>
#include <QPainter>

namespace site_cps
{

boost::filesystem::path getPath( QString url )
{
  boost::filesystem::path path;

  if ( url.indexOf(":/", 0, Qt::CaseInsensitive) == 0 )
  {
    path = url.toStdString();
  }
  else if ( url.indexOf("file://", 0, Qt::CaseInsensitive) == 0 )
  {
    path = url.section('/',2).toStdString();
  }
  else
  {
    qDebug("Invalid or unsupported URL: '%s'", url.toStdString().c_str() );
  }

  return path;
}


QPixmap loadPixmap( QString url, bool fill_cache )
{
  QPixmap pixmap;

  // if it's in the cache, no need to locate
  if ( QPixmapCache::find( url, &pixmap ) )
  {
    return pixmap;
  }

  boost::filesystem::path path = getPath( url );

  // If something goes wrong here, we go on and store the empty pixmap,
  // so the error won't appear again anytime soon.
  if ( boost::filesystem::exists( path ) )
  {
    qDebug( "load_resource", "Loading '%s'", path.string().c_str() );
    if ( !pixmap.load( QString::fromStdString( path.string() ) ) )
    {
      qDebug( "Could not load pixmap '%s'", path.string().c_str() );
    }
  }

  if ( fill_cache )
  {
    QPixmapCache::insert( url, pixmap );
  }

  return pixmap;
}

QCursor getDefaultCursor( bool fill_cache )
{
  return QCursor(Qt::ArrowCursor);
}

QCursor makeIconCursor( QString url, bool fill_cache )
{
  QPixmap icon = loadPixmap( url, fill_cache );
  if (icon.width() == 0 || icon.height() == 0)
  {
    qDebug( "Could not load pixmap '%s' -- using default cursor instead.", url.toStdString().c_str() );
    return getDefaultCursor();
  }
  QString cache_key = url + ".cursor";
  return makeIconCursor( icon, cache_key, fill_cache );
}

QCursor makeIconCursor( QPixmap icon, QString cache_key, bool fill_cache )
{
  // if it's in the cache, no need to locate
  QPixmap cursor_img;
  if ( QPixmapCache::find( cache_key, &cursor_img ) )
  {
    return QCursor( cursor_img, 0, 0 );
  }

  QPixmap base_cursor = loadPixmap( ":/icon/icons/cursor.svg", fill_cache );

  const int cursor_size = 32;

  cursor_img = QPixmap( cursor_size, cursor_size );
  cursor_img.fill( QColor(0,0,0,0) );

  // copy base cursor & icon into one image
  QPainter painter(&cursor_img);

  int draw_x = 12;
  int draw_y = 16;

  // if the icon is too large, move it to the left
  if( draw_x+icon.width() > cursor_size )
  {
    draw_x = cursor_size-icon.width();
  }
  if( draw_y+icon.height() > cursor_size )
  {
    draw_y = cursor_size-icon.height();
  }

  painter.drawPixmap( 0, 0, base_cursor );
  painter.drawPixmap( draw_x, draw_y, icon );

  if ( fill_cache )
  {
    QPixmapCache::insert( cache_key, cursor_img );
  }

  return QCursor( cursor_img, 1, 1 );
}

}
