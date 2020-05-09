/*!
 * \file    tabimage.cpp
 * \author  zhusizhi
 *
 * \brief   properties tab widget for camera image operation
 *
 */

#include "properties.h"

void properties::initTabImage()
{
	INT nBlacklevelCaps;
	INT nRet = 0;

	nRet = is_Blacklevel(m_hCamera, IS_BLACKLEVEL_CMD_GET_CAPS, &nBlacklevelCaps, sizeof(nBlacklevelCaps));
	if(nRet == IS_SUCCESS)
	{
		m_bHasAutoBlackLevel = (nBlacklevelCaps & IS_BLACKLEVEL_CAP_SET_AUTO_BLACKLEVEL) != 0;
		m_bHasManualBlackLevel = (nBlacklevelCaps & IS_BLACKLEVEL_CAP_SET_OFFSET) != 0;
	}

    m_bHasHardwareGamma = (is_SetHardwareGamma (m_hCamera, IS_GET_HW_SUPPORTED_GAMMA)
            & IS_SET_HW_GAMMA_ON);
    INT nGamma;
    if (m_SensorInfo.SensorID != IS_SENSOR_UI1013XC)
    {
        m_bHasSoftwareGamma = (is_Gamma (m_hCamera, IS_GAMMA_CMD_GET, &nGamma, sizeof(nGamma)) != IS_NO_SUCCESS);
    }
    else
    {
        // no gamma support fox UI-1013XC
        m_bHasSoftwareGamma = false;
    }

    m_bHasGainBoost = (is_SetGainBoost (m_hCamera, IS_GET_SUPPORTED_GAINBOOST)
            & IS_SET_GAINBOOST_ON);

    // Timer for automatic updates
    connect(m_pTimerUpdateImageCtrl, SIGNAL(timeout()), this, SLOT(OnAutoUpdateImageCtrls()));
    m_pTimerUpdateImageCtrl->start(750);

    // Get Range for EdgeEnhancement
    UINT nRange[3];
    ZeroMemory(nRange, sizeof(nRange));

    nRet = is_EdgeEnhancement(m_hCamera, IS_EDGE_ENHANCEMENT_CMD_GET_RANGE, (void*)nRange, sizeof(nRange));
    if(nRet == IS_SUCCESS)
    {
    	m_weakEdgeEnhancement = nRange[1] / 2;
    	m_strongEdgeEnhancement = nRange[1];
    }
}

// -------------------  UpdateMasterGainCtrls  -----------------------
//
void properties::UpdateMasterGainCtrls ()
{
    if (m_bInit || !m_hCamera)
        return;

    QString strTemp;
    double dblEnable = 0.0;

    // get values
    m_maxMasterGain = IS_MAX_GAIN;
    m_minMasterGain = IS_MIN_GAIN;
    m_incMasterGain = 1;
    m_nMasterGain = is_SetHardwareGain (m_hCamera, IS_GET_MASTER_GAIN, IS_IGNORE_PARAMETER,
                                        IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER);

    is_SetAutoParameter (m_hCamera, IS_GET_ENABLE_AUTO_GAIN, &dblEnable, NULL);
    m_bAutoGain = (dblEnable != 0.0);

    // set text
    strTemp.setNum ((short int)m_minMasterGain, 10);
    textMinMasterGain->setText (strTemp);
    strTemp.setNum ((short int)m_maxMasterGain, 10);
    textMaxMasterGain->setText (strTemp);

    m_bInit = TRUE;
    // update slider control
    sliderMaster->setMinimum(m_minMasterGain);
    sliderMaster->setMaximum(m_maxMasterGain);
    sliderMaster->setSingleStep(m_incMasterGain);
    sliderMaster->setPageStep(4*m_incMasterGain);
    sliderMaster->setValue (m_nMasterGain);
    sliderMaster->setEnabled ((m_SensorInfo.bMasterGain == TRUE) && !m_bAutoGain);
    // update spin control
    spinBoxMaster->setMinimum (m_minMasterGain);
    spinBoxMaster->setMaximum (m_maxMasterGain);
    spinBoxMaster->setSingleStep (m_incMasterGain);
    spinBoxMaster->setValue (m_nMasterGain);
    spinBoxMaster->setEnabled ((m_SensorInfo.bMasterGain == TRUE) && !m_bAutoGain);
    // auto
    //checkAutoGain->setChecked (m_bAutoGain);
    checkAutoGain->setEnabled (m_bHasAutoGain && !m_bAutoFramerate);
    m_bInit = FALSE;
}

// -------------------  UpdateBlackLevelCtrls  -----------------------
//
void properties::UpdateBlackLevelCtrls ()
{
    if (m_bInit || !m_hCamera)
        return;

    QString strTemp;
    INT nMode;
    IS_RANGE_S32 nRange;

    // get values

    INT nRet = is_Blacklevel(m_hCamera, IS_BLACKLEVEL_CMD_GET_OFFSET_RANGE, (void*)&nRange, sizeof(nRange));

    if (nRet == IS_SUCCESS)
    {
    	m_maxBlackLevel = nRange.s32Max;
        m_minBlackLevel = nRange.s32Min;
        m_incBlackLevel = nRange.s32Inc;
    }

    is_Blacklevel(m_hCamera, IS_BLACKLEVEL_CMD_GET_OFFSET, &m_nBlackLevel, sizeof(m_nBlackLevel));

    nRet = is_Blacklevel(m_hCamera, IS_BLACKLEVEL_CMD_GET_MODE, (void*)&nMode, sizeof(nMode));
    if(nRet == IS_SUCCESS)
    {
    	m_bAutoBlackLevel = (nMode & IS_AUTO_BLACKLEVEL_ON) != 0;
    }

    // set text
    strTemp.setNum ((short int)m_minBlackLevel, 10);
    textMinBlackLevel->setText (strTemp);
    strTemp.setNum ((short int)m_maxBlackLevel, 10);
    textMaxBlackLevel->setText (strTemp);

    m_bInit = TRUE;
    // update slider control
    sliderBlackLevel->setMinimum (m_minBlackLevel);
    sliderBlackLevel->setMaximum (m_maxBlackLevel);
    sliderBlackLevel->setSingleStep(m_incBlackLevel);
    sliderBlackLevel->setPageStep(4*m_incBlackLevel);
    sliderBlackLevel->setValue (m_nBlackLevel);
    sliderBlackLevel->setEnabled (m_bHasManualBlackLevel);
    // update spin control
    spinBoxBlackLevel->setMinimum (m_minBlackLevel);
    spinBoxBlackLevel->setMaximum (m_maxBlackLevel);
    spinBoxBlackLevel->setSingleStep (m_incBlackLevel);
    spinBoxBlackLevel->setValue (m_nBlackLevel);
    spinBoxBlackLevel->setEnabled (m_bHasManualBlackLevel);
    // auto
    checkAutoBlacklevel->setChecked (m_bAutoBlackLevel);
    checkAutoBlacklevel->setEnabled (m_bHasAutoBlackLevel);
    m_bInit = FALSE;
}

// -------------------  UpdateRGBGainCtrls  -----------------------
//
void properties::UpdateRGBGainCtrls ()
{
    if (m_bInit || !m_hCamera)
        return;

    QString strTemp;
    double dblEnable = 0.0;

    // get values
    m_maxRGBGain = IS_MAX_GAIN;
    m_minRGBGain = IS_MIN_GAIN;
    m_incRGBGain = 1;
    m_nRedGain = is_SetHardwareGain (m_hCamera, IS_GET_RED_GAIN, IS_IGNORE_PARAMETER,
                                     IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER);
    m_nGreenGain = is_SetHardwareGain (m_hCamera, IS_GET_GREEN_GAIN, IS_IGNORE_PARAMETER,
                                       IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER);
    m_nBlueGain = is_SetHardwareGain (m_hCamera, IS_GET_BLUE_GAIN, IS_IGNORE_PARAMETER,
                                      IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER);
    is_SetAutoParameter (m_hCamera, IS_GET_ENABLE_AUTO_WHITEBALANCE, &dblEnable, NULL);

    // set text
    strTemp.setNum ((short int)m_minRGBGain, 10);
    textMinRGBGain->setText (strTemp);
    strTemp.setNum ((short int)m_maxRGBGain, 10);
    textMaxRGBGain->setText (strTemp);

    m_bInit = TRUE;
    // update slider controls
    // red
    sliderRed->setMinimum (m_minRGBGain);
    sliderRed->setMaximum (m_maxRGBGain);
    sliderRed->setSingleStep(m_incRGBGain);
    sliderRed->setPageStep(4*m_incRGBGain);
    sliderRed->setValue (m_nRedGain);
    sliderRed->setEnabled ((m_SensorInfo.bRGain == TRUE) && (dblEnable == 0.0));
    // green
    sliderGreen->setMinimum (m_minRGBGain);
    sliderGreen->setMaximum (m_maxRGBGain);
    sliderGreen->setSingleStep(m_incRGBGain);
    sliderGreen->setPageStep(4*m_incRGBGain);
    sliderGreen->setValue (m_nGreenGain);
    sliderGreen->setEnabled ((m_SensorInfo.bGGain == TRUE) && (dblEnable == 0.0));
    // blue
    sliderBlue->setMinimum (m_minRGBGain);
    sliderBlue->setMaximum (m_maxRGBGain);
    sliderBlue->setSingleStep(m_incRGBGain);
    sliderBlue->setPageStep(4*m_incRGBGain);
    sliderBlue->setValue (m_nBlueGain);
    sliderBlue->setEnabled ((m_SensorInfo.bBGain == TRUE) && (dblEnable == 0.0));
    // update spin controls
    // red
    spinBoxRed->setMinimum (m_minRGBGain);
    spinBoxRed->setMaximum (m_maxRGBGain);
    spinBoxRed->setSingleStep (m_incRGBGain);
    spinBoxRed->setValue (m_nRedGain);
    spinBoxRed->setEnabled ((m_SensorInfo.bRGain == TRUE) && (dblEnable == 0.0));
    // green
    spinBoxGreen->setMinimum (m_minRGBGain);
    spinBoxGreen->setMaximum (m_maxRGBGain);
    spinBoxGreen->setSingleStep (m_incRGBGain);
    spinBoxGreen->setValue (m_nGreenGain);
    spinBoxGreen->setEnabled ((m_SensorInfo.bGGain == TRUE) && (dblEnable == 0.0));
    // blue
    spinBoxBlue->setMinimum (m_minRGBGain);
    spinBoxBlue->setMaximum (m_maxRGBGain);
    spinBoxBlue->setSingleStep (m_incRGBGain);
    spinBoxBlue->setValue (m_nBlueGain);
    spinBoxBlue->setEnabled ((m_SensorInfo.bBGain == TRUE) && (dblEnable == 0.0));
    m_bInit = FALSE;
}

// -------------------  OnSliderChangeMaster  -----------------------
//
void properties::OnSliderChangeMaster (int value)
{
    if (m_bInit || !m_hCamera)
        return;

    value = (value < m_minMasterGain) ? m_minMasterGain : value;
    value = (value > m_maxMasterGain) ? m_maxMasterGain : value;

    is_SetHardwareGain (m_hCamera, value, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER,
                        IS_IGNORE_PARAMETER);

    UpdateMasterGainCtrls ();
}

// -------------------  sliderBlackLevel_valueChanged  -----------------------
//
void properties::sliderBlackLevel_valueChanged (int value)
{
    if (m_bInit || !m_hCamera)
        return;

    value = (value < m_minBlackLevel) ? m_minBlackLevel : value;
    value = (value > m_maxBlackLevel) ? m_maxBlackLevel : value;

    is_Blacklevel (m_hCamera, IS_BLACKLEVEL_CMD_SET_OFFSET, (void*)&value, sizeof(value));

    UpdateBlackLevelCtrls ();
}

// -------------------  OnSliderChangeBlue  -----------------------
//
void properties::OnSliderChangeBlue (int value)
{
    if (m_bInit || !m_hCamera)
        return;

    value = (value < m_minRGBGain) ? m_minRGBGain : value;
    value = (value > m_maxRGBGain) ? m_maxRGBGain : value;

    is_SetHardwareGain (m_hCamera, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER,
                        value);

    UpdateRGBGainCtrls ();
}

// -------------------  OnSliderChangeGreen  -----------------------
//
void properties::OnSliderChangeGreen (int value)
{
    if (m_bInit || !m_hCamera)
        return;

    value = (value < m_minRGBGain) ? m_minRGBGain : value;
    value = (value > m_maxRGBGain) ? m_maxRGBGain : value;

    is_SetHardwareGain (m_hCamera, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER, value,
                        IS_IGNORE_PARAMETER);

    UpdateRGBGainCtrls ();
}

// -------------------  OnSliderChangeRed  -----------------------
//
void properties::OnSliderChangeRed (int value)
{
    if (m_bInit || !m_hCamera)
        return;

    value = (value < m_minRGBGain) ? m_minRGBGain : value;
    value = (value > m_maxRGBGain) ? m_maxRGBGain : value;

    is_SetHardwareGain (m_hCamera, IS_IGNORE_PARAMETER, value, IS_IGNORE_PARAMETER,
                        IS_IGNORE_PARAMETER);

    UpdateRGBGainCtrls ();
}

// -------------------  checkAutoGain_clicked  -----------------------
//
void properties::checkAutoGain_clicked ()
{
    if (m_bInit || !m_hCamera)
        return;

    double dblEnable = (checkAutoGain->isChecked ()) ? 1.0 : 0.0;

    if (is_SetAutoParameter (m_hCamera, IS_SET_ENABLE_AUTO_GAIN, &dblEnable, NULL) != IS_SUCCESS)
        QMessageBox::information (0, "Error !!!", "SetAutoGain failed", 0);

    // we may want to start a timer that updates the current values?
    UpdateMasterGainCtrls ();
}

void properties::checkAutoBlacklevel_clicked ()
{
    if (m_bInit || !m_hCamera)
        return;

    INT nEnable = (checkAutoBlacklevel->isChecked ()) ? IS_AUTO_BLACKLEVEL_ON
                                                      : IS_AUTO_BLACKLEVEL_OFF;

    if (is_Blacklevel(m_hCamera, IS_BLACKLEVEL_CMD_SET_MODE, (void*)&nEnable, sizeof(nEnable)) != IS_SUCCESS)
        QMessageBox::information (0, "Error !!!", "SetAutoBlackLevel failed", 0);

    UpdateBlackLevelCtrls ();
}

// -------------------  UpdateFeaturesCtrls  -----------------------
//
void properties::UpdateFeaturesCtrls ()
{
    if (m_bInit || !m_hCamera)
        return;

    // get values
    INT nGamma = 0;
    is_Gamma(m_hCamera, IS_GAMMA_CMD_GET, &nGamma, sizeof(nGamma));
    m_bSoftwareGammaSet = nGamma == 180;
    m_bHardwareGammaSet = (is_SetHardwareGamma (m_hCamera, IS_GET_HW_GAMMA) == IS_SET_HW_GAMMA_ON);

    m_bGainBoostSet = (is_SetGainBoost (m_hCamera, IS_GET_GAINBOOST) == IS_SET_GAINBOOST_ON);

    // set checked
    checkBoxSoftwareGamma->setChecked (m_bSoftwareGammaSet);
    checkBoxHardwareGamma->setChecked (m_bHardwareGammaSet);
    checkBoxGainBoost->setChecked (m_bGainBoostSet);
    // enable
    checkBoxSoftwareGamma->setEnabled (m_bHasSoftwareGamma);
    checkBoxHardwareGamma->setEnabled (m_bHasHardwareGamma);

    checkBoxGainBoost->setEnabled (m_bHasGainBoost);
}

// -------------------  checkBoxSoftwareGamma_clicked  -----------------------
//
void properties::checkBoxSoftwareGamma_clicked ()
{
    if (m_bInit || !m_hCamera)
        return;

    INT nGamma = checkBoxSoftwareGamma->isChecked () ? 180 : 100;
    is_Gamma(m_hCamera, IS_GAMMA_CMD_SET, &nGamma, sizeof(nGamma));
}

// -------------------  checkBoxHardwareGamma_clicked  -----------------------
//
void properties::checkBoxHardwareGamma_clicked ()
{
    if (m_bInit || !m_hCamera)
        return;

    is_SetHardwareGamma (m_hCamera, (checkBoxHardwareGamma->isChecked ()) ? IS_SET_HW_GAMMA_ON
                                                                          : IS_SET_HW_GAMMA_OFF);
}

// -------------------  checkBoxGainBoost_clicked  -----------------------
//
void properties::checkBoxGainBoost_clicked ()
{
    if (m_bInit || !m_hCamera)
        return;

    is_SetGainBoost (m_hCamera, checkBoxGainBoost->isChecked () ? IS_SET_GAINBOOST_ON
                                                                : IS_SET_GAINBOOST_OFF);
}

// -------------------  UpdateEdgeEnhCtrls  -----------------------
//  IS_EDGE_EN_DISABLE      0
//  IS_EDGE_EN_STRONG       1
//  IS_EDGE_EN_WEAK         2
void properties::UpdateEdgeEnhCtrls ()
{
    if (m_bInit || !m_hCamera)
        return;

    INT mode;

    is_EdgeEnhancement(m_hCamera, IS_EDGE_ENHANCEMENT_CMD_GET, (void*)&mode, sizeof(mode));

    if(mode == m_weakEdgeEnhancement)
    {
    	radioButtonEdgeStrong->setChecked(true);
    }
    else if(mode == m_strongEdgeEnhancement)
    {
    	radioButtonEdgeWeak->setChecked(true);
    }
    else
    {
    	radioButtonEdgeDisabled->setChecked(true);
    }
}

// -------------------  OnRadioEdgeEnhancement  -----------------------
//
void properties::OnRadioEdgeEnhancement ()
{
    if (m_bInit || !m_hCamera)
        return;

    INT mode = 0;

    if (radioButtonEdgeDisabled->isChecked())
    {
        mode = 0;
    }
    if (radioButtonEdgeWeak->isChecked())
    {
        mode = m_weakEdgeEnhancement;
    }
    if (radioButtonEdgeStrong->isChecked())
    {
        mode = m_strongEdgeEnhancement;
    }

    int nRet = is_EdgeEnhancement(m_hCamera, IS_EDGE_ENHANCEMENT_CMD_SET, (void*)&mode, sizeof(mode));
    qDebug("Edge Enhancement returns: %d", nRet);
}

void properties::OnAutoUpdateImageCtrls ()
{
    if (isVisible())
    {
        if (checkAutoBlacklevel->isChecked ())
            UpdateBlackLevelCtrls ();

        if (checkAutoGain->isChecked ())
            UpdateMasterGainCtrls();

        UpdateRGBGainCtrls ();
    }
}
