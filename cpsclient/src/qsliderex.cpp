/*!
 * \file    qsliderex.cpp
 * \date    02.09.2018
 * \author  zhusizhi
 *
 * \brief   extended QSlider class provides additional functionality
 *
 */
#include "qsliderex.h"

QSliderEx::QSliderEx(QWidget *parent, int slack)
:QSlider(parent)
{
    m_slack = slack;
    Init();
}

QSliderEx::QSliderEx(Qt::Orientation orientation, QWidget *parent, int slack)
:QSlider(orientation, parent)
{
    m_slack = slack;
    Init();
}

QSliderEx::~QSliderEx()
{
    // TODO Auto-generated destructor stub
}

void QSliderEx::Init()
{

    m_pTimer = new QTimer(this);
    m_pTimer->setSingleShot(true);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(slacktimeout()));

    // handle base class signal
    EnableSignaling(true);

}

void QSliderEx::EnableSignaling(bool bEnable)
{
    if (bEnable)
    {
        qDebug("QSliderEx - enable base class signaling");
        connect(this, SIGNAL(valueChanged(int)), this, SLOT(baseValueChanged(int)));
    }
    else
    {
        qDebug("QSliderEx - disable base class signaling");
        disconnect(this, SIGNAL(valueChanged(int)),0,0);
    }
}

void QSliderEx::setSlackness(int slack)
{
    m_slack = slack;
}

void QSliderEx::baseValueChanged(int value)
{
    // store new base value
    qDebug("QSliderEx - baseValueChanged: %d", value);
    m_value = value;
    if (!m_pTimer->isActive())
    {
        emit valueChangedEx(value);
        m_lastvalue = value;
        m_pTimer->start(m_slack);
    }
}

void QSliderEx::slacktimeout()
{
    if (m_value != m_lastvalue)
        baseValueChanged(m_value);
}
