#include "properties.h"
#include <iostream>

void properties::initTabHotpixel()
{
    RestoreHotpixelCtrls();

    connect(checkBoxHotpixelCorrection, SIGNAL(toggled(bool)), this, SLOT(OnCheckboxHotpixelCorrection(bool)));
    connect(checkBoxCameraHotpixelCorrection, SIGNAL(toggled(bool)), this, SLOT(OnCheckboxCameraHotpixelCorrection(bool)));
    connect(checkBoxSoftwareUserHotpixelCorrection, SIGNAL(toggled(bool)), this, SLOT(OnCheckboxSoftwareUserHotpixelCorrection(bool)));
    connect(checkBoxSensorHotpixelCorrection, SIGNAL(toggled(bool)), this, SLOT(OnCheckboxSensorHotpixelCorrection(bool)));
    connect(checkBoxAdaptiveHotpixelCorrection, SIGNAL(toggled(bool)), this, SLOT(OnCheckboxAdaptiveHotpixelCorrection(bool)));
    connect(checkBoxMarkHotpixel, SIGNAL(toggled(bool)), m_pDlgMainView, SLOT(setMarkHotpixel(bool)));

    connect(sliderAdaptiveHotpixelCorrectionSensitivity, SIGNAL(valueChanged(int)), this, SLOT(OnAdaptiveHotpixelCorrectionSensitivity(int)));
    connect(spinBoxAdaptiveHotpixelCorrectionSensitivity, SIGNAL(valueChanged(int)), this, SLOT(OnAdaptiveHotpixelCorrectionSensitivity(int)));
    connect(comboBoxAdaptiveHotpixelCorrectionMode, SIGNAL(currentIndexChanged(int)), this, SLOT(OnAdaptiveHotpixelCorrectionMode(int)));
    connect(pushButtonAdaptiveHotpixelCorrectionDetectAgain, SIGNAL(clicked(bool)), this, SLOT(OnAdaptiveHotpixelCorrectionDetectAgain(bool)));

    //connect(m_pDlgMainView, SIGNAL(adaptiveHotpixelNumberDetectedChanged(int)), this, SLOT(UpdateAdaptiveHotpixelCorrectionNumberDetected(int)));
    connect(m_pDlgMainView, SIGNAL(adaptiveHotpixelPercentDetectedChanged(double)), this, SLOT(UpdateAdaptiveHotpixelCorrectionPercentDetected(double)));
}

void properties::RestoreHotpixelCtrls()
{
    INT nModeHotpixel = 0;
    INT nSupportedModes = 0;

    checkBoxMarkHotpixel->setChecked(false);
    m_pDlgMainView->setMarkHotpixel(false);

    checkBoxHotpixelCorrection->setEnabled(false);
    checkBoxCameraHotpixelCorrection->setEnabled(false);
    checkBoxSoftwareUserHotpixelCorrection->setEnabled(false);
    checkBoxSensorHotpixelCorrection->setEnabled(false);
    if (IS_SUCCESS == is_HotPixel(m_hCamera, IS_HOTPIXEL_GET_SUPPORTED_CORRECTION_MODES, (void*)&nSupportedModes, sizeof(nSupportedModes)))
    {
        if (nSupportedModes & IS_HOTPIXEL_ENABLE_CAMERA_CORRECTION)
        {
            checkBoxHotpixelCorrection->setEnabled(true);
            checkBoxCameraHotpixelCorrection->setEnabled(true);
        }
        if (nSupportedModes & IS_HOTPIXEL_ENABLE_SOFTWARE_USER_CORRECTION)
        {
            checkBoxHotpixelCorrection->setEnabled(true);
            checkBoxSoftwareUserHotpixelCorrection->setEnabled(true);
        }
        if (nSupportedModes & IS_HOTPIXEL_ENABLE_SENSOR_CORRECTION)
        {
            checkBoxHotpixelCorrection->setEnabled(true);
            checkBoxSensorHotpixelCorrection->setEnabled(true);
        }
    }

    if (IS_SUCCESS == is_HotPixel(m_hCamera, IS_HOTPIXEL_GET_CORRECTION_MODE, (void*)&nModeHotpixel, sizeof(nModeHotpixel)))
    {
        switch (nModeHotpixel)
        {
        case 0:
            break;
        case 1:
            checkBoxHotpixelCorrection->setChecked(true);
            checkBoxSensorHotpixelCorrection->setChecked(true);
            break;
        case 2:
            checkBoxHotpixelCorrection->setChecked(true);
            checkBoxCameraHotpixelCorrection->setChecked(true);
            break;
        case 3:
            checkBoxHotpixelCorrection->setChecked(true);
            checkBoxSensorHotpixelCorrection->setChecked(true);
            checkBoxCameraHotpixelCorrection->setChecked(true);
            break;
        case 4:
            checkBoxHotpixelCorrection->setChecked(true);
            checkBoxSoftwareUserHotpixelCorrection->setChecked(true);
            break;
        case 5:
            checkBoxHotpixelCorrection->setChecked(true);
            checkBoxSensorHotpixelCorrection->setChecked(true);
            checkBoxSoftwareUserHotpixelCorrection->setChecked(true);
            break;
        }
    }

    sliderAdaptiveHotpixelCorrectionSensitivity->setTickInterval(1);
    sliderAdaptiveHotpixelCorrectionSensitivity->setSingleStep(1);
    sliderAdaptiveHotpixelCorrectionSensitivity->setPageStep(1);

    UpdateHotpixelCtrls();
    UpdateAdaptiveHotpixelCtrls();
    UpdateAdaptiveHotpixelCorrectionNumberDetected(0);
}

void properties::UpdateHotpixelCtrls()
{
    INT nModeHotpixel = 0;

    if (IS_SUCCESS == is_HotPixel(m_hCamera, IS_HOTPIXEL_GET_CORRECTION_MODE, (void*)&nModeHotpixel, sizeof(nModeHotpixel)))
    {
        switch (nModeHotpixel)
        {
        case 0:
            break;
        case 1:
            checkBoxSensorHotpixelCorrection->setChecked(true);
            break;
        case 2:
            checkBoxCameraHotpixelCorrection->setChecked(true);
            break;
        case 3:
            checkBoxSensorHotpixelCorrection->setChecked(true);
            checkBoxCameraHotpixelCorrection->setChecked(true);
            break;
        case 4:
            checkBoxSoftwareUserHotpixelCorrection->setChecked(true);
            break;
        case 5:
            checkBoxSensorHotpixelCorrection->setChecked(true);
            checkBoxSoftwareUserHotpixelCorrection->setChecked(true);
            break;
        }
    }

    buttonGroupAdaptiveHotpixelCorrection->setEnabled(false);
    UpdateAdaptiveHotpixelCtrls();
}

void properties::UpdateAdaptiveHotpixelCtrls()
{
    INT nSensitivityMin = 0;
    INT nSensitivityMax = 0;
    INT nSensitivityCurrent = 0;
    INT stateAdaptiveHotpixelCorrection = 0;
    INT nAdaptiveMode = 0;

    is_HotPixel(m_hCamera, IS_HOTPIXEL_ADAPTIVE_CORRECTION_GET_SENSITIVITY_MIN, (void*)&nSensitivityMin, sizeof(nSensitivityMin));
    is_HotPixel(m_hCamera, IS_HOTPIXEL_ADAPTIVE_CORRECTION_GET_SENSITIVITY_MAX, (void*)&nSensitivityMax, sizeof(nSensitivityMax));
    is_HotPixel(m_hCamera, IS_HOTPIXEL_ADAPTIVE_CORRECTION_GET_SENSITIVITY, (void*)&nSensitivityCurrent, sizeof(nSensitivityCurrent));
    is_HotPixel(m_hCamera, IS_HOTPIXEL_ADAPTIVE_CORRECTION_GET_ENABLE, (void*)&stateAdaptiveHotpixelCorrection, sizeof(stateAdaptiveHotpixelCorrection));
    is_HotPixel(m_hCamera, IS_HOTPIXEL_ADAPTIVE_CORRECTION_GET_MODE, (void*)&nAdaptiveMode, sizeof(nAdaptiveMode));

    switch (nAdaptiveMode)
    {
    case IS_HOTPIXEL_ADAPTIVE_CORRECTION_DETECT_ONCE:
        comboBoxAdaptiveHotpixelCorrectionMode->setCurrentIndex(0);
        break;
    case IS_HOTPIXEL_ADAPTIVE_CORRECTION_DETECT_DYNAMIC:
        comboBoxAdaptiveHotpixelCorrectionMode->setCurrentIndex(1);
        break;
    default:
        comboBoxAdaptiveHotpixelCorrectionMode->setEnabled(false);
    }

    sliderAdaptiveHotpixelCorrectionSensitivity->setMinimum(nSensitivityMin);
    sliderAdaptiveHotpixelCorrectionSensitivity->setMaximum(nSensitivityMax);
    sliderAdaptiveHotpixelCorrectionSensitivity->setValue(nSensitivityCurrent);
    spinBoxAdaptiveHotpixelCorrectionSensitivity->setValue(nSensitivityCurrent);
    checkBoxAdaptiveHotpixelCorrection->setChecked(static_cast<bool>(stateAdaptiveHotpixelCorrection));
    buttonGroupAdaptiveHotpixelCorrection->setEnabled(stateAdaptiveHotpixelCorrection);
}

void properties::OnCheckboxHotpixelCorrection(bool bState)
{
    if (bState)
    {
        INT nSupportedModes = 0;

        checkBoxCameraHotpixelCorrection->setEnabled(false);
        checkBoxSoftwareUserHotpixelCorrection->setEnabled(false);
        checkBoxSensorHotpixelCorrection->setEnabled(false);
        if (IS_SUCCESS == is_HotPixel(m_hCamera, IS_HOTPIXEL_GET_SUPPORTED_CORRECTION_MODES, (void*)&nSupportedModes, sizeof(nSupportedModes)))
        {
            if (nSupportedModes & IS_HOTPIXEL_ENABLE_CAMERA_CORRECTION)
            {
                is_HotPixel(m_hCamera, IS_HOTPIXEL_ENABLE_CAMERA_CORRECTION, NULL, 0);
                checkBoxCameraHotpixelCorrection->setEnabled(true);
            }
            if (nSupportedModes & IS_HOTPIXEL_ENABLE_SOFTWARE_USER_CORRECTION)
            {
                is_HotPixel(m_hCamera, IS_HOTPIXEL_ENABLE_SOFTWARE_USER_CORRECTION, NULL, 0);
                checkBoxSoftwareUserHotpixelCorrection->setEnabled(true);
            }
            if (nSupportedModes & IS_HOTPIXEL_ENABLE_SENSOR_CORRECTION)
            {
                is_HotPixel(m_hCamera, IS_HOTPIXEL_ENABLE_SENSOR_CORRECTION, NULL, 0);
                checkBoxSensorHotpixelCorrection->setEnabled(true);
            }
        }
    }
    else
    {
        is_HotPixel(m_hCamera, IS_HOTPIXEL_DISABLE_CORRECTION, NULL, 0);
        checkBoxCameraHotpixelCorrection->setEnabled(false);
        checkBoxSensorHotpixelCorrection->setEnabled(false);
        checkBoxSoftwareUserHotpixelCorrection->setEnabled(false);
        checkBoxCameraHotpixelCorrection->setChecked(false);
        checkBoxSensorHotpixelCorrection->setChecked(false);
        checkBoxSoftwareUserHotpixelCorrection->setChecked(false);
    }
    UpdateHotpixelCtrls();
}


void properties::OnCheckboxCameraHotpixelCorrection(bool bState)
{
    if (bState)
    {
        is_HotPixel(m_hCamera, IS_HOTPIXEL_ENABLE_CAMERA_CORRECTION, NULL, 0);
    }
    else
    {
        bool enableSoftwareUser = false;
        bool enableSensor = false;
        INT nModeHotpixel = 0;
        if (IS_SUCCESS == is_HotPixel(m_hCamera, IS_HOTPIXEL_GET_CORRECTION_MODE, (void*)&nModeHotpixel, sizeof(nModeHotpixel)))
        {
            switch (nModeHotpixel)
            {
            case 3:
                enableSensor = true;
                break;
            case 4:
                enableSoftwareUser = true;
                break;
            case 5:
                enableSensor = true;
                enableSoftwareUser = true;
                break;
            }
        }
        is_HotPixel(m_hCamera, IS_HOTPIXEL_DISABLE_CORRECTION, NULL, 0);
        if (enableSoftwareUser)
        {
            is_HotPixel(m_hCamera, IS_HOTPIXEL_ENABLE_SOFTWARE_USER_CORRECTION, NULL, 0);
        }
        if (enableSensor)
        {
            is_HotPixel(m_hCamera, IS_HOTPIXEL_ENABLE_SENSOR_CORRECTION, NULL, 0);
        }
    }
    UpdateHotpixelCtrls();
}

void properties::OnCheckboxSoftwareUserHotpixelCorrection(bool bState)
{
    if (bState)
    {
        bool enableCamera = false;
        bool enableSoftwareUser = false;
        bool enableSensor = false;
        INT nModeHotpixel = 0;
        if (IS_SUCCESS == is_HotPixel(m_hCamera, IS_HOTPIXEL_GET_CORRECTION_MODE, (void*)&nModeHotpixel, sizeof(nModeHotpixel)))
        {
            switch (nModeHotpixel)
            {
            case 2:
                enableCamera = true;
                break;
            case 3:
                enableSensor = true;
                enableCamera = true;
                break;
            case 4:
                enableSoftwareUser = true;
                break;
            case 5:
                enableSensor = true;
                enableSoftwareUser = true;
                break;
            }
        }
        is_HotPixel(m_hCamera, IS_HOTPIXEL_ENABLE_SOFTWARE_USER_CORRECTION, NULL, 0);
        if (enableCamera)
        {
            is_HotPixel(m_hCamera, IS_HOTPIXEL_ENABLE_CAMERA_CORRECTION, NULL, 0);
        }
        if (enableSoftwareUser)
        {
            is_HotPixel(m_hCamera, IS_HOTPIXEL_ENABLE_SOFTWARE_USER_CORRECTION, NULL, 0);
        }
        if (enableSensor)
        {
            is_HotPixel(m_hCamera, IS_HOTPIXEL_ENABLE_SENSOR_CORRECTION, NULL, 0);
        }
    }
    else
    {
        is_HotPixel(m_hCamera, IS_HOTPIXEL_DISABLE_CORRECTION, NULL, 0);
    }
    UpdateHotpixelCtrls();
}

void properties::OnCheckboxSensorHotpixelCorrection(bool bState)
{
    if (bState)
    {
        is_HotPixel(m_hCamera, IS_HOTPIXEL_ENABLE_SENSOR_CORRECTION, NULL, 0);
    }
    else
    {
        is_HotPixel(m_hCamera, IS_HOTPIXEL_DISABLE_SENSOR_CORRECTION, NULL, 0);
    }
    UpdateHotpixelCtrls();
}

void properties::OnCheckboxAdaptiveHotpixelCorrection(bool bState)
{
    INT nState = static_cast<INT>(bState);
    is_HotPixel(m_hCamera, IS_HOTPIXEL_ADAPTIVE_CORRECTION_SET_ENABLE, (void*)&nState, sizeof(nState));
    UpdateHotpixelCtrls();
}

void properties::OnAdaptiveHotpixelCorrectionSensitivity(int nValue)
{
    is_HotPixel(m_hCamera, IS_HOTPIXEL_ADAPTIVE_CORRECTION_SET_SENSITIVITY, (void*)&nValue, sizeof(nValue));
    is_HotPixel(m_hCamera, IS_HOTPIXEL_ADAPTIVE_CORRECTION_RESET_DETECTION, NULL, 0);
    UpdateAdaptiveHotpixelCtrls();
}

void properties::OnAdaptiveHotpixelCorrectionMode(int nIndex)
{
    INT nAdaptiveMode = 0;
    switch (nIndex)
    {
    case 0:
        nAdaptiveMode = IS_HOTPIXEL_ADAPTIVE_CORRECTION_DETECT_ONCE;
        break;
    case 1:
        nAdaptiveMode = IS_HOTPIXEL_ADAPTIVE_CORRECTION_DETECT_DYNAMIC;
        break;
    default:
        is_HotPixel(m_hCamera, IS_HOTPIXEL_ADAPTIVE_CORRECTION_GET_MODE, (void*)&nAdaptiveMode, sizeof(nAdaptiveMode));
    }
    is_HotPixel(m_hCamera, IS_HOTPIXEL_ADAPTIVE_CORRECTION_SET_MODE, (void*)&nAdaptiveMode, sizeof(nAdaptiveMode));
    OnAdaptiveHotpixelCorrectionDetectAgain(true);
    UpdateAdaptiveHotpixelCtrls();
}

void properties::OnAdaptiveHotpixelCorrectionDetectAgain(bool bState)
{
    is_HotPixel(m_hCamera, IS_HOTPIXEL_ADAPTIVE_CORRECTION_RESET_DETECTION, NULL, 0);
    UpdateAdaptiveHotpixelCtrls();
}

void properties::UpdateAdaptiveHotpixelCorrectionNumberDetected(int nNumber)
{
    lineEditAdaptiveHotpixelCorrectionNumberDetected->setText(QString::number(nNumber));
}

void properties::UpdateAdaptiveHotpixelCorrectionPercentDetected(double fPercent)
{
    lineEditAdaptiveHotpixelCorrectionNumberDetected->setText(QString::number(fPercent, 'f', 2).append(" %"));
}
