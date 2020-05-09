/*!
 * \file    tabcamera.cpp
 * \author  zhusizhi
 *
 * \brief   properties tab widget for camera timing
 *
 */

#include <QDebug>
#include "properties.h"
#include <algorithm>


void properties::initTabCamera()
{
    connect(m_pTimerUpdateCameraCtrl, SIGNAL(timeout()), this, SLOT(OnAutoUpdateCameraCtrls()));

    m_pTimerUpdateCameraCtrl->stop();
    m_pTimerUpdateCameraCtrl->start(750);

    sliderPixelclock->setTracking(FALSE);
}
// -------------------  UpdatePixelclock  -----------------------
//
void properties::UpdatePixelclockCtrls ()
{
    if (m_bInit || !m_hCamera)
        return;

    QString strTemp;
    UINT Pixelclock;
    UINT nRange[3];
    INT nRet;

    ZeroMemory(nRange, sizeof(nRange));

    // get Number of Supported PixelClocks
    m_nNumberOfSupportedPixelClocks = 0;
    nRet = is_PixelClock(m_hCamera, IS_PIXELCLOCK_CMD_GET_NUMBER,
                            (void*)&m_nNumberOfSupportedPixelClocks,
                            sizeof(m_nNumberOfSupportedPixelClocks));
    if((nRet == IS_SUCCESS) && (m_nNumberOfSupportedPixelClocks > 0))
    {
        ZeroMemory(&m_nPixelclockList, sizeof(m_nPixelclockList));
        is_PixelClock(m_hCamera, IS_PIXELCLOCK_CMD_GET_LIST,
                        (void*)m_nPixelclockList,
                        m_nNumberOfSupportedPixelClocks * sizeof(int));
    }

    // get values
    nRet = is_PixelClock(m_hCamera, IS_PIXELCLOCK_CMD_GET_RANGE, (void*)nRange, sizeof(nRange));
    if (nRet == IS_SUCCESS)
    {
        m_minPixelclock = nRange[0];
        m_maxPixelclock = nRange[1];
        m_incPixelclock = std::max(nRange[2], static_cast<UINT>(1)); // pixelclock increment is sometimes zero --> spinbox will not work
    }

    is_PixelClock(m_hCamera, IS_PIXELCLOCK_CMD_GET, (void*)&Pixelclock, sizeof(Pixelclock));

    for (int i = 0; i < m_nNumberOfSupportedPixelClocks; i++)
    {
        if (Pixelclock == (unsigned int)m_nPixelclockList[i])
        {
            m_nPixelclockListIndex = i;
        }
    }
    qDebug("new pclk: %d", Pixelclock);

    sliderPixelclock->setEnabled (!m_bAutoExposure);
    spinPixelclock->setEnabled (!m_bAutoExposure);

    // set text
    strTemp.setNum (m_minPixelclock, 10);
    textMinPixelclock->setText (strTemp + " MHz");
    strTemp.setNum (m_maxPixelclock, 10);
    textMaxPixelclock->setText (strTemp + " MHz");

    m_bInit = TRUE;
    // update slider control
    sliderPixelclock->setMaximum (m_nNumberOfSupportedPixelClocks - 1);
    sliderPixelclock->setSingleStep (m_incPixelclock);
    sliderPixelclock->setPageStep (4* m_incPixelclock );
    sliderPixelclock->setValue (m_nPixelclockListIndex);
    // update spin control
    spinPixelclock->setRange (m_minPixelclock, m_maxPixelclock);
    spinPixelclock->setSingleStep (m_incPixelclock);
    spinPixelclock->setValue (Pixelclock);
    m_bInit = FALSE;
}

// -------------------  UpdateFramerate  -----------------------
//
void properties::UpdateFramerateCtrls ()
{
    if (m_bInit || !m_hCamera)
        return;

    QString strTemp;
    double dblEnable = 0.0;
    int stepsFps = 0;
    double rangeFPS = 0.0;
    int slidervalue = 0;

    // get values
    is_GetFrameTimeRange (m_hCamera, &m_maxFramerate, &m_minFramerate, &m_incFramerate);
    if (m_SensorInfo.SensorID == IS_SENSOR_UI1013XC )
    {
        m_incFramerate = 0.5; // increment should be 0.5;
    }
    else
    {
        m_incFramerate += 0.05; // increment should be min 0.05
    }
    is_SetFrameRate (m_hCamera, IS_GET_FRAMERATE, &m_dblFramerate);
    m_minFramerate = 1 / m_minFramerate;
    m_maxFramerate = 1 / m_maxFramerate;
    is_SetAutoParameter (m_hCamera, IS_GET_ENABLE_AUTO_FRAMERATE, &dblEnable, NULL);
    m_bAutoFramerate = (dblEnable != 0.0);

    // set text
    strTemp.setNum (m_minFramerate, 'f', 2);
    textMinFramerate->setText (strTemp + " fps");
    strTemp.setNum (m_maxFramerate, 'f', 2);
    textMaxFramerate->setText (strTemp + " fps");

    // update slider
    rangeFPS = m_maxFramerate - m_minFramerate;
    stepsFps = (int)(rangeFPS / m_incFramerate);
    slidervalue = (int)((double)((m_dblFramerate - m_minFramerate) / rangeFPS) * (double)stepsFps);
    qDebug("new slider framerate: %d", slidervalue);
    m_bInit = TRUE;
    // slider
    sliderFramerate->setMinimum(0);
    sliderFramerate->setMaximum (stepsFps);
    sliderFramerate->setSingleStep(2);
    sliderFramerate->setPageStep(20);
    sliderFramerate->setValue( slidervalue );
    sliderFramerate->setEnabled(!m_bAutoFramerate);
    // auto
    checkAutoFps->setChecked(m_bAutoFramerate);
    checkAutoFps->setEnabled(m_bHasAutoFps && m_bAutoExposure && !m_bAutoGain);
    // spin box
    dSpinFramerate->setEnabled (!m_bAutoFramerate);
    dSpinFramerate->setRange (m_minFramerate, m_maxFramerate);
    //dSpinFramerate->setSingleStep (m_incFramerate + 0.5);
    dSpinFramerate->setSingleStep (2*m_incFramerate);
    dSpinFramerate->setValue (m_dblFramerate);

    m_bInit=FALSE;
}

// -------------------  UpdateExposure  -----------------------
//
void properties::UpdateExposureCtrls ()
{
    if (m_bInit || !m_hCamera)
        return;

    QString strTemp;
    double dblEnable = 0.0;
    int stepsEXP = 0;
    int slidervalue = 0;
    double rangeEXP = 0.0;

    // apply max exposure
    if (m_bMaxExposure)
    {
        double dblValue = 0.0;
        is_Exposure(m_hCamera, IS_EXPOSURE_CMD_SET_EXPOSURE, (void*)&dblValue, sizeof(dblValue));
    }

    // get values
    double dblRange[3];
    is_Exposure(m_hCamera, IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE, (void*)&dblRange, sizeof(dblRange));
    m_minExposure = dblRange[0];
    m_maxExposure = dblRange[1];
    m_incExposure = dblRange[2];

    m_incExposure += 0.05;
    is_Exposure(m_hCamera, IS_EXPOSURE_CMD_GET_EXPOSURE, (void*)&m_dblExposure, sizeof(m_dblExposure));

    // set text
    strTemp.setNum (m_minExposure, 'f', 2);
    textMinExposure->setText (strTemp + " ms");
    strTemp.setNum (m_maxExposure, 'f', 2);
    textMaxExposure->setText (strTemp + " ms");

    // update slider
    rangeEXP = m_maxExposure - m_minExposure;
    stepsEXP = (int)(rangeEXP / m_incExposure);
    //curExp = (int)((double)((m_dblExposure - m_minExposure) / (m_maxExposure - m_minExposure))
    //        * (double)stepsExp + 0.05);
    slidervalue = (int)((double)((m_dblExposure - m_minExposure) / rangeEXP) * (double)stepsEXP);
    qDebug("new slider exposure: %d", slidervalue);

    m_bInit = TRUE;
    // slider
    sliderExposure->setMinimum(0);
    sliderExposure->setMaximum(stepsEXP);
    sliderExposure->setSingleStep(4);
    sliderExposure->setPageStep(40);
    sliderExposure->setValue(slidervalue);
    sliderExposure->setEnabled(!m_bAutoExposure && !m_bMaxExposure);
    // spin box
    dSpinExposure->setEnabled (!m_bAutoExposure);
    dSpinExposure->setRange (m_minExposure, m_maxExposure);
    dSpinExposure->setSingleStep (2*m_incExposure);
    dSpinExposure->setValue (m_dblExposure);
    // auto
    checkAutoExp->setChecked(m_bAutoExposure);
    checkAutoExp->setEnabled(m_bHasAutoExposure && !m_bMaxExposure);
    // max
    checkMaxExp->setChecked(m_bMaxExposure);
    checkMaxExp->setEnabled(!m_bAutoExposure);
    m_bInit = FALSE;
}

// -------------------  OnSpinBoxChangePixelClock  -----------------------
//
void properties::OnSpinBoxChangePixelClock(int value)
{
    for (int i = 0; i <= m_nNumberOfSupportedPixelClocks; i++)
    {
        if (value == m_nPixelclockList[i])
        {
            m_nPixelclockListIndex = i;
            break;
        }
        else if (i == m_nNumberOfSupportedPixelClocks)
        {
            if ((value - m_nPixelclockList[m_nPixelclockListIndex]) == -1 || (value - m_nPixelclockList[m_nPixelclockListIndex]) == 1)
            {
                m_nPixelclockListIndex += (value - m_nPixelclockList[m_nPixelclockListIndex]);
            }
            spinPixelclock->setValue(m_nPixelclockList[m_nPixelclockListIndex]);
            break;
        }
    }
    sliderPixelclock->setValue(m_nPixelclockListIndex);
}

// -------------------  OnSliderChangePixelClock  -----------------------
//
void properties::OnSliderChangePixelClock(int value)
{
    spinPixelclock->blockSignals(true);
    spinPixelclock->setValue(m_nPixelclockList[value]);
    spinPixelclock->blockSignals(false);
}

// -------------------  OnSliderChangePixelClock  -----------------------
//
void properties::OnChangePixelClockSetting(int value)
{
    spinPixelclock->blockSignals(false);

    if (m_bInit || !m_hCamera)
    {
        return;
    }

    qDebug("onSlider Pixelclock: %d", m_nPixelclockList[value]);
    value = (value < 0) ? 0 : value;
    value = (value > m_nNumberOfSupportedPixelClocks - 1) ? m_nNumberOfSupportedPixelClocks - 1 : value;
    qDebug(" -> set %d", m_nPixelclockList[value]);
    int ret = is_PixelClock(m_hCamera, IS_PIXELCLOCK_CMD_SET, (void*)&m_nPixelclockList[value], sizeof(m_nPixelclockList[value]));
    qDebug("SetPixelClock returned %d", ret);

    /* we have to update exposure controls before pixelclock controls because we need there a valid m_bAutoExposure value */
    UpdateExposureCtrls();
    UpdatePixelclockCtrls();

    /* for UpdateFrameCtrls we need to update Gain Controls (to get auto gain value) */
    UpdateMasterGainCtrls();
    UpdateFramerateCtrls();
    spinPixelclock->blockSignals(false);
}

void properties::ChangeFramerate (double value)
{
    if (m_bInit || !m_hCamera)
        return;

    value = (value < m_minFramerate) ? m_minFramerate : value;
    value = (value > m_maxFramerate) ? m_maxFramerate : value;

    is_SetFrameRate (m_hCamera, value, &m_dblFramerate);

    /* for UpdateFrameCtrls we need to update Gain Controls (to get auto gain value) */
    UpdateMasterGainCtrls();
    UpdateFramerateCtrls ();
    UpdateExposureCtrls ();
}

void properties::ChangeExposure (double value)
{
    if (m_bInit || !m_hCamera)
        return;

    if (value != 0)
    {
        value = (value < m_minExposure) ? m_minExposure : value;
        value = (value > m_maxExposure) ? m_maxExposure : value;
    }

    m_dblExposure = value;

    is_Exposure(m_hCamera, IS_EXPOSURE_CMD_SET_EXPOSURE, (void*)&m_dblExposure, sizeof(m_dblExposure));

    // We do not want to change the value of the exposure spinbox
    // block setting values
    sliderExposure->blockSignals(true);
    dSpinExposure->blockSignals(true);

    int stepsEXP = 0;
    int slidervalue = 0;
    double rangeEXP = 0.0;

    // update slider
    rangeEXP = m_maxExposure - m_minExposure;
    stepsEXP = (int)(rangeEXP / m_incExposure);
    slidervalue = (int)((double)((m_dblExposure - m_minExposure) / rangeEXP) * (double)stepsEXP);

    sliderExposure->setValue(slidervalue);
    dSpinExposure->setValue(value);

    // do not block anymore
    sliderExposure->blockSignals(false);
    dSpinExposure->blockSignals(false);
}

// -------------------  OnSliderChangeFrameRate  -----------------------
//
void properties::OnSliderChangeFrameRate (int value)
{
    if (m_bInit || !m_hCamera)
        return;

    qDebug("onSlider Framerate: %d", value);
    double newFps = m_minFramerate + (double) (value * m_incFramerate);
    //double newFps = m_minFramerate + (double)(value * (m_incFramerate+0.5));
    //printf ("new fps %0.3f\n", newFps);

    qDebug(" -> set %.3f", newFps);
    ChangeFramerate (newFps);

}

// -------------------  OnSliderChangePixelClock  -----------------------
//
void properties::OnSliderChangeExposure (int value)
{
    if (m_bInit || !m_hCamera)
        return;

    qDebug("onSlider Exposure: %d", value);
    double newExpTime = m_minExposure + (double)(value * m_incExposure);
    qDebug(" -> set %.3f", newExpTime);
    ChangeExposure (newExpTime);
}

// -------------------  checkAutoFps_clicked  -----------------------
//
void properties::checkAutoFps_clicked ()
{
    if (m_bInit || !m_hCamera)
        return;

    double dblEnable = (checkAutoFps->isChecked ()) ? 1.0 : 0.0;

    if (is_SetAutoParameter (m_hCamera, IS_SET_ENABLE_AUTO_FRAMERATE, &dblEnable, NULL)
            != IS_SUCCESS)
    {
        QMessageBox::information (0, "Error !!!", "SetAutoFPS failed", 0);
    }
    /* for UpdateFrameCtrls we need to update Gain Controls (to get auto gain value) */
    UpdateMasterGainCtrls();
    // we may want to start a timer that updates the current values?
    UpdateFramerateCtrls ();
}

// -------------------  checkAutoExposure_clicked  -----------------------
//
void properties::checkAutoExp_clicked ()
{
    if (m_bInit || !m_hCamera)
    {
        return;
    }

    double dEnable = 0.0;

    if (checkAutoExp->isChecked())
    {
        dEnable = 1.0;
    }
    else
    {
        dEnable = 0.0;

        /* auto framerate is applicable only if auto exposure is active. so disable aut framerate if active. */
        if (m_bAutoFramerate)
        {
            if (is_SetAutoParameter(m_hCamera, IS_SET_ENABLE_AUTO_FRAMERATE, &dEnable, NULL) != IS_SUCCESS)
            {
                QMessageBox::information(0, "Error !!!", "SetAutoFramerate failed", 0);
            }
        }
    }

    // we may want to start a timer that updates the current values?
    // keep this order, because this time the exposure influences the framerate
    UpdateExposureCtrls();
    /* for UpdateFrameCtrls we need to update Gain Controls (to get auto gain value) */
    UpdateMasterGainCtrls();
    /* UpdateFramerateCtrls depends on the results of UpdateExposureCtrls. */
    UpdateFramerateCtrls();
    UpdatePixelclockCtrls();
}

// -------------------  checkMaxExposure_clicked  -----------------------
//
void properties::checkMaxExp_clicked ()
{
    if (m_bInit || !m_hCamera)
        return;

    m_bMaxExposure = checkMaxExp->isChecked ();
    if (m_bMaxExposure)
    {
        IS_RANGE_F64 f64Range = { 0, 0, 0 };
        if (is_Exposure(m_hCamera, IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE, &f64Range, sizeof(f64Range)) == IS_SUCCESS)
        {
            ChangeExposure (f64Range.f64Max);
        }
    }

    UpdateExposureCtrls ();
}

void properties::OnAutoUpdateCameraCtrls()
{
    if (checkAutoExp->isChecked() && isVisible())
    {
        // keep this order, because this time the exposure influences the framerate
        UpdateExposureCtrls();
        /* for UpdateFrameCtrls we need to update Gain Controls (to get auto gain value) */
        UpdateMasterGainCtrls();
        /* UpdateFramerateCtrls depends on the results of UpdateExposureCtrls. */
        UpdateFramerateCtrls();
        UpdatePixelclockCtrls();
    }
}

