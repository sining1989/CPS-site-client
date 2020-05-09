#ifndef SITE_CPS_SCALED_IMAGE_WIDGET_H
#define SITE_CPS_SCALED_IMAGE_WIDGET_H

#include <QWidget>

namespace site_cps
{

/**
 * \brief A widget for showing a scaled version of an image (QPixmap).
 *
 * The scale is just a suggestion, given to Qt by calls to sizeHint(),
 * which returns the image size multiplied by the scale.  The actual
 * rendered size is the largest that fits the image into the current
 * widget size without changing the aspect ratio.  It is always
 * rendered in the center.
 */
class ScaledImageWidget: public QWidget
{
Q_OBJECT
public:
  ScaledImageWidget( float scale, QWidget* parent = 0 );
  virtual ~ScaledImageWidget() {}

  void setImage( QPixmap image );

  virtual QSize sizeHint() const;

protected:
  virtual void paintEvent( QPaintEvent* event );

private:
  QPixmap image_;
  float scale_;
};

} // namespace site_cps

#endif // SITE_CPS_SCALED_IMAGE_WIDGET_H
