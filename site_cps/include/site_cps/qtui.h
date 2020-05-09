#ifndef SITE_CPS_QTUI_H
#define SITE_CPS_QTUI_H

#include <QtGlobal>

#if QT_VERSION > QT_VERSION_CHECK(5, 0, 0)
#    include <QDialog>
#    include <QMainWindow>
#    include <QDialog>
#else
#    include <QtGui/QDialog>
#    include <QtGui/QMainWindow>
#    include <QtGui/QDialog>
#endif

#endif // SITE_CPS_QTUI_H
