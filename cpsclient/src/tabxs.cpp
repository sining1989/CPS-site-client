/*!
 * \file    tabxs.cpp
 * \author  zhusizhi
 *
 * \brief   properties tab widget for special cps xs camera features
 *
 */

#include "properties.h"

void properties::UpdateXs_JpegCompressionControls()
{
    INT compression = 0;
    if(is_DeviceFeature(m_hCamera, IS_DEVICE_FEATURE_CMD_GET_JPEG_COMPRESSION, (void*)&compression, sizeof(compression)) == IS_SUCCESS)
    {
        slider_xsJPEGCompression->setValue(compression);
        spinBox_xsJPEGCompression->setValue(compression);
    }
    cB_xsJPEGModeEnable->setChecked(m_bJPEG);

    if (! m_bJPEG)
    {
        /* Only activate JPEG CheckBox when a supported color mode is set */
        int nColorMode = is_SetColorMode (m_hCamera, IS_GET_COLOR_MODE);
        cB_xsJPEGModeEnable->setEnabled(CheckColorFormat(nColorMode, IS_CONV_MODE_JPEG));
    }

    // Switch off Manual Focus Setting if Auto-Focus is Active
    slider_xsJPEGCompression->setEnabled(m_bJPEG);
    spinBox_xsJPEGCompression->setEnabled(m_bJPEG);
    label_xsJPEGCompression->setEnabled(m_bJPEG);
    text_xsMinJPEGCompression->setEnabled(m_bJPEG);
    text_xsMaxJPEGCompression->setEnabled(m_bJPEG);

}

void properties::initXs_JpegCompressionControls()
{
    DWORD DeviceFeature;

    if (is_DeviceFeature(m_hCamera, IS_DEVICE_FEATURE_CMD_GET_SUPPORTED_FEATURES, (void*)&DeviceFeature, sizeof(DeviceFeature)) == IS_SUCCESS)
    {
        // Check JPEG-Compression Feature Supported
        if ((DeviceFeature & IS_DEVICE_FEATURE_CAP_JPEG_COMPRESSION) != 0)
        {
            // get JPEG compression Range
            IS_RANGE_S32 range;
            QString strTemp;

            if(is_DeviceFeature(m_hCamera, IS_DEVICE_FEATURE_CMD_GET_JPEG_COMPRESSION_RANGE, (void*)&range, sizeof(range)) == IS_SUCCESS)
            {
                m_xsMinJPEGCompression = range.s32Min;
                m_xsMaxJPEGCompression = range.s32Max;
                m_xsIncJPEGCompression = range.s32Inc;
            }

            // Set Min/Max to Slider and SpinBox
            slider_xsJPEGCompression->setRange(m_xsMinJPEGCompression, m_xsMaxJPEGCompression);
            spinBox_xsJPEGCompression->setRange(m_xsMinJPEGCompression, m_xsMaxJPEGCompression);

            // Set Min/Max to Labels
            strTemp.setNum (m_xsMinJPEGCompression);
            text_xsMinJPEGCompression->setText(strTemp);
            strTemp.setNum (m_xsMaxJPEGCompression);
            text_xsMaxJPEGCompression->setText(strTemp);

            // Connect Slots
            connect (slider_xsJPEGCompression, SIGNAL(valueChanged(int)), spinBox_xsJPEGCompression, SLOT(setValue(int)));
            connect (spinBox_xsJPEGCompression, SIGNAL(valueChanged(int)), this, SLOT(OnXsJPEGCompression(int)));
            connect (cB_xsJPEGModeEnable, SIGNAL(clicked(bool)), this, SLOT(OnXsJPEGMode(bool)));
        }
        else
        {
            // JPEG-Compression Feature not Supported
            cB_xsJPEGModeEnable->hide();
            label_xsJPEGCompression->hide();
            text_xsMinJPEGCompression->hide();
            text_xsMaxJPEGCompression->hide();
            slider_xsJPEGCompression->hide();
            spinBox_xsJPEGCompression->hide();

            group_xsJpegMode->hide();
        }
    }
}

void properties::UpdateXs_SharpnessControls()
{
    int nSharpness = 0;
    if (is_Sharpness(m_hCamera, SHARPNESS_CMD_GET_VALUE, (void*)&nSharpness, sizeof(nSharpness)) == IS_SUCCESS)
    {
        slider_xsSharpness->setValue(nSharpness);
        spinBox_xsSharpness->setValue(nSharpness);
    }
}

void properties::initXs_SharpnessControls()
{
    INT nSharpnessCaps = 0;

    if (is_Sharpness(m_hCamera, SHARPNESS_CMD_GET_CAPABILITIES, (void*)&nSharpnessCaps, sizeof(nSharpnessCaps)) == IS_SUCCESS)
    {
        // Check Sharpness Feature Supported
        if ((nSharpnessCaps & SHARPNESS_CAP_SHARPNESS_SUPPORTED) != 0)
        {
            // get Sharpness Min
            if(is_Sharpness(m_hCamera, SHARPNESS_CMD_GET_MIN_VALUE, (void*)&m_xsMinSharpness, sizeof(m_xsMinSharpness)) == IS_SUCCESS)
            {
                // get Sharpness Max
                if(is_Sharpness(m_hCamera, SHARPNESS_CMD_GET_MAX_VALUE, (void*)&m_xsMaxSharpness, sizeof(m_xsMaxSharpness)) == IS_SUCCESS)
                {
                    // get Sharpness Increment
                    if(is_Sharpness(m_hCamera, SHARPNESS_CMD_GET_INCREMENT, (void*)&m_xsIncSharpness, sizeof(m_xsIncSharpness)) == IS_SUCCESS)
                    {
                        QString strTemp;

                        // Set Min/Max to Slider and SpinBox
                        slider_xsSharpness->setRange(m_xsMinSharpness, m_xsMaxSharpness);
                        spinBox_xsSharpness->setRange(m_xsMinSharpness, m_xsMaxSharpness);

                        // Set Min/Max to Labels
                        strTemp.setNum (m_xsMinSharpness);
                        text_xsMinSharpness->setText(strTemp);
                        strTemp.setNum (m_xsMaxSharpness);
                        text_xsMaxSharpness->setText(strTemp);

                        // Connect Slots
                        connect (slider_xsSharpness, SIGNAL(valueChanged(int)), spinBox_xsSharpness, SLOT(setValue(int)));
                        connect (spinBox_xsSharpness, SIGNAL(valueChanged(int)), this, SLOT(OnXsSharpness(int)));
                    }
                }
            }
        }
        else
        {
            label_xsSharpness->hide();
            text_xsMinSharpness->hide();
            text_xsMaxSharpness->hide();
            slider_xsSharpness->hide();
            spinBox_xsSharpness->hide();
        }
    }
}

void properties::UpdateXs_SaturationControls()
{
    INT nSaturation = 0;
    if (is_Saturation(m_hCamera, SATURATION_CMD_GET_VALUE, (void*)&nSaturation, sizeof(nSaturation)) == IS_SUCCESS)
    {
        slider_xsSaturation->setValue(nSaturation);
        spinBox_xsSaturation->setValue(nSaturation);
    }
}

void properties::initXs_SaturationControls()
{
    INT nSaturationCaps = 0;

    if(is_Saturation(m_hCamera, SATURATION_CMD_GET_CAPABILITIES, (void*)&nSaturationCaps, sizeof(nSaturationCaps)) == IS_SUCCESS)
    {
        // Check Saturation Feature Supported
        if ((nSaturationCaps & SATURATION_CAP_SATURATION_SUPPORTED) != 0)
        {
            // get Saturation Min
            if (is_Saturation(m_hCamera, SATURATION_CMD_GET_MIN_VALUE, (void*)&m_xsMinSaturation, sizeof(m_xsMinSaturation)) == IS_SUCCESS)
            {
                // get Saturation Max
                if (is_Saturation(m_hCamera, SATURATION_CMD_GET_MAX_VALUE, (void*)&m_xsMaxSaturation, sizeof(m_xsMaxSaturation)) == IS_SUCCESS)
                {
                    // get Saturation Increment
                    if (is_Saturation(m_hCamera, SATURATION_CMD_GET_INCREMENT, (void*)&m_xsIncSaturation, sizeof(m_xsIncSaturation)) == IS_SUCCESS)
                    {
                        QString strTemp;

                        // Set Min/Max to Slider and SpinBox
                        slider_xsSaturation->setRange(m_xsMinSaturation, m_xsMaxSaturation);
                        spinBox_xsSaturation->setRange(m_xsMinSaturation, m_xsMaxSaturation);

                        // Set Min/Max to Labels
                        strTemp.setNum (m_xsMinSaturation);
                        text_xsMinSaturation->setText(strTemp);
                        strTemp.setNum (m_xsMaxSaturation);
                        text_xsMaxSaturation->setText(strTemp);

                        // Connect Slots
                        connect (slider_xsSaturation, SIGNAL(valueChanged(int)), spinBox_xsSaturation, SLOT(setValue(int)));
                        connect (spinBox_xsSaturation, SIGNAL(valueChanged(int)), this, SLOT(OnXsSaturation(int)));
                    }
                }
            }
        }
        else
        {
            label_xsSaturation->hide();
            text_xsMinSaturation->hide();
            text_xsMaxSaturation->hide();
            slider_xsSaturation->hide();
            spinBox_xsSaturation->hide();
        }
    }
}

void properties::UpdateXs_FocusControls()
{
    INT nManFocus = 0, nStatFocus = 0;

    // Update Focus Controls
    INT  bEnable = 0;
    if (is_Focus(m_hCamera, FOC_CMD_GET_AUTOFOCUS_ENABLE, (void*)&bEnable, sizeof(bEnable)) == IS_SUCCESS)
    {
        cB_xsAutoFocusEnable->setChecked((bool) bEnable);
    }

    bool bAutoFocus = cB_xsAutoFocusEnable->isChecked();
    // Switch off Manual Focus Setting if Auto-Focus is Active
    slider_xsManualFocus->setEnabled(!bAutoFocus);
    spinBox_xsManualFocus->setEnabled(!bAutoFocus);
    label_xsManualFocus->setEnabled(!bAutoFocus);
    text_xsMinManualFocus->setEnabled(!bAutoFocus);
    text_xsMaxManualFocus->setEnabled(!bAutoFocus);
    button_xsReadDistanceAF->setEnabled(bAutoFocus);
    button_xsFocusOnce->setText(bAutoFocus ? QString("Trigger") : QString("Once"));

    if(m_xsFocusOnceActive)
    {
        if (is_Focus(m_hCamera, FOC_CMD_GET_AUTOFOCUS_STATUS, (void*)&nStatFocus, sizeof(nStatFocus)) == IS_SUCCESS)
        {
            if((nStatFocus & FOC_STATUS_FOCUSING) == 0)
            {
                m_xsFocusOnceActive = false;
            }
        }
    }

    // Deactivate Signals from spinBox ManualFocus if Auto-Focus is active
    spinBox_xsManualFocus->blockSignals(bAutoFocus || m_xsFocusOnceActive);

    if (is_Focus(m_hCamera, FOC_CMD_GET_MANUAL_FOCUS, (void*)&nManFocus, sizeof(nManFocus)) == IS_SUCCESS)
    {
        slider_xsManualFocus->setValue(nManFocus);
        spinBox_xsManualFocus->setValue(nManFocus);
    }

}

void properties::initXs_FocusControls()
{
    INT m_nFocusCaps;
    UINT bEnable = 0;

    if (is_Focus(m_hCamera, FOC_CMD_GET_CAPABILITIES, (void*)&m_nFocusCaps, sizeof(m_nFocusCaps)) == IS_SUCCESS)
    {
        // Check Manual Focus Supported
        if ((m_nFocusCaps & FOC_CAP_MANUAL_SUPPORTED) != 0)
        {
            // get Saturation Min
            if (is_Focus(m_hCamera, FOC_CMD_GET_MANUAL_FOCUS_MIN, (void*)&m_xsMinManualFocus, sizeof(m_xsMinManualFocus)) == IS_SUCCESS)
            {
                // get Saturation Max
                if (is_Focus(m_hCamera, FOC_CMD_GET_MANUAL_FOCUS_MAX, (void*)&m_xsMaxManualFocus, sizeof(m_xsMaxManualFocus)) == IS_SUCCESS)
                {
                    // get Saturation Increment
                    if (is_Focus(m_hCamera, FOC_CMD_GET_MANUAL_FOCUS_INC, (void*)&m_xsIncManualFocus, sizeof(m_xsIncManualFocus)) == IS_SUCCESS)
                    {
                        QString strTemp;

                        // Set Min/Max to Slider and SpinBox
                        slider_xsManualFocus->setRange(m_xsMinManualFocus, m_xsMaxManualFocus);
                        spinBox_xsManualFocus->setRange(m_xsMinManualFocus, m_xsMaxManualFocus);

                        // Set Min/Max to Labels
                        strTemp.setNum (m_xsMinManualFocus);
                        text_xsMinManualFocus->setText(strTemp);
                        strTemp.setNum (m_xsMaxManualFocus);
                        text_xsMaxManualFocus->setText(strTemp);

                        // Connect Slots
                        connect (slider_xsManualFocus, SIGNAL(valueChanged(int)), spinBox_xsManualFocus, SLOT(setValue(int)));
                        connect (spinBox_xsManualFocus, SIGNAL(valueChanged(int)), this, SLOT(OnXsManualFocus(int)));
                    }
                }
            }
        }
        else
        {
            label_xsManualFocus->hide();
            text_xsMinManualFocus->hide();
            text_xsMaxManualFocus->hide();
            slider_xsManualFocus->hide();
            spinBox_xsManualFocus->hide();
        }

        // Check Auto-Focus Supported
        if ((m_nFocusCaps & FOC_CAP_AUTOFOCUS_SUPPORTED) != 0)
        {
            if (is_Focus(m_hCamera, FOC_CMD_GET_AUTOFOCUS_ENABLE, (void*)&bEnable, sizeof(bEnable)) == IS_SUCCESS)
            {
                cB_xsAutoFocusEnable->setChecked((bool) bEnable);
            }

            connect (cB_xsAutoFocusEnable, SIGNAL(clicked(bool)), this, SLOT(OnXsAutoFocus(bool)));
        }
        else
        {
            cB_xsAutoFocusEnable->hide();
        }

        // Check AutoFocus-Once Supported
        if (is_Focus(m_hCamera, FOC_CMD_SET_ENABLE_AUTOFOCUS_ONCE, NULL, 0) == IS_SUCCESS)
        {
            connect (button_xsFocusOnce, SIGNAL(clicked()), this, SLOT(OnXsFocusOnce()));
        }
        else
        {
            button_xsFocusOnce->hide();
        }

        // Check Get-Distance Supported
        if ((m_nFocusCaps & FOC_CAP_GET_DISTANCE) != 0)
        {
            // Connect Slot
            connect (button_xsReadDistanceAF, SIGNAL(clicked()), this, SLOT(OnXsFocusDistance()));
        }
        else
        {
            label_xsDistance->hide();
            text_xsDistance->hide();
            button_xsReadDistanceAF->hide();
        }

        m_xsFocusOnceActive = false;
    }
}

void properties::UpdateXs_AutoParamControls()
{
    double dEnable = 0;

    if (m_bXsHasBacklightComp && (is_SetAutoParameter(m_hCamera, IS_GET_SENS_AUTO_BACKLIGHT_COMP, &dEnable, NULL) == IS_SUCCESS))
    {
        cB_xsBacklightCompEnable->setChecked((bool)dEnable);
    }

    if (m_bXsHasAES && (is_SetAutoParameter(m_hCamera, IS_GET_ENABLE_AUTO_SENSOR_SHUTTER, &dEnable, NULL) == IS_SUCCESS))
    {
        cB_xsAES->setChecked((bool)dEnable);
        checkAutoExp->setChecked((bool)dEnable);
        sliderExposure->setEnabled(dEnable != 0);
    }

    if (m_bXsHasAGS && (is_SetAutoParameter(m_hCamera, IS_GET_ENABLE_AUTO_SENSOR_GAIN, &dEnable, NULL) == IS_SUCCESS))
    {
        cB_xsAGC->setChecked((bool)dEnable);
        checkAutoGain->setChecked((bool)dEnable);
    }

    if (m_bXsHasAGES && (is_SetAutoParameter(m_hCamera, IS_GET_ENABLE_AUTO_SENSOR_GAIN_SHUTTER, &dEnable, NULL) == IS_SUCCESS))
    {
        cB_xsAES->setChecked((bool)dEnable);
        checkAutoExp->setChecked((bool)dEnable);
        checkAutoGain->setChecked((bool)dEnable);
        sliderExposure->setEnabled(dEnable != 0);
    }

    if (cB_xsImgStab->isVisible())
    {
        /* image stabilization is only supported if auto gain and auto shutter is active */
        cB_xsImgStab->setEnabled(cB_xsAGC->isChecked() && cB_xsAES->isChecked());
    }

    cB_xsBacklightCompEnable->setEnabled(cB_xsAGC->isChecked() || cB_xsAES->isChecked());
}

void properties::initXs_AutoParamControls()
{
    UEYE_AUTO_INFO autoInfo;
    double dEnable = 0;

    if (is_GetAutoInfo(m_hCamera, &autoInfo) == IS_SUCCESS)
    {
        // Check Backlight Compensation Support
        if ((autoInfo.AutoAbility & AC_SENSOR_AUTO_BACKLIGHT_COMP) != 0)
        {
            if (is_SetAutoParameter(m_hCamera, IS_GET_SENS_AUTO_BACKLIGHT_COMP, &dEnable, NULL) == IS_SUCCESS)
            {
                cB_xsBacklightCompEnable->setChecked((bool)dEnable);
            }

            connect (cB_xsBacklightCompEnable, SIGNAL(clicked(bool)), this, SLOT(OnXsBacklightComp(bool)));
            m_bXsHasBacklightComp = true;
        }
        else
        {
            cB_xsBacklightCompEnable->hide();
        }

        // Check Auto-Shutter Support
        if ((autoInfo.AutoAbility & AC_SENSOR_SHUTTER) != 0)
        {
            if (is_SetAutoParameter(m_hCamera, IS_GET_ENABLE_AUTO_SENSOR_SHUTTER, &dEnable, NULL) == IS_SUCCESS)
            {
                cB_xsAES->setChecked((bool)dEnable);
            }

            connect (cB_xsAES, SIGNAL(clicked(bool)), this, SLOT(OnXsAES(bool)));
            m_bXsHasAES = true;
        }

        // Check Auto-Gain-Shutter Support
        else if ((autoInfo.AutoAbility & AC_SENSOR_GAIN_SHUTTER) != 0)
        {
            // Use the AES Checkbox for Auto Sensor Gain Shutter
            if (is_SetAutoParameter(m_hCamera, IS_GET_ENABLE_AUTO_SENSOR_GAIN_SHUTTER, &dEnable, NULL) == IS_SUCCESS)
            {
                cB_xsAES->setChecked((bool)dEnable);
            }
            cB_xsAES->setText(QString("Auto Gain-Shutter"));
            connect (cB_xsAES, SIGNAL(clicked(bool)), this, SLOT(OnXsAGEC(bool)));
            m_bXsHasAGES = true;
        }
        else
        {
            cB_xsAES->hide();
        }

        // Check Auto-Sensor-Gain Support
        if ((autoInfo.AutoAbility & AC_SENSOR_GAIN) != 0)
        {
            // Auto Gain Supported
            if (is_SetAutoParameter(m_hCamera, IS_GET_ENABLE_AUTO_SENSOR_GAIN, &dEnable, NULL) == IS_SUCCESS)
            {
                cB_xsAGC->setChecked((bool)dEnable);
            }
            connect (cB_xsAGC, SIGNAL(clicked(bool)), this, SLOT(OnXsAGC(bool)));
            m_bXsHasAGS = true;
        }
        else
        {
            cB_xsAGC->hide();
        }
    }
}

void properties::UpdateXs_LSCControls()
{

}

void properties::initXs_LSCControls()
{

    INT nSupported = 0;
    INT nDefault = 0;

    if(is_ColorTemperature(m_hCamera, COLOR_TEMPERATURE_CMD_GET_LENS_SHADING_MODEL_SUPPORTED, (void*)&nSupported, sizeof(nSupported)) == IS_SUCCESS)
    {
        for(int i = 1; i < nSupported; i<<=1)
        {
            if(i & LSC_MODEL_AGL)
            {
                combo_xsLSC->addItem(QString("AGL"), LSC_MODEL_AGL);
            }
            if(i & LSC_MODEL_TL84)
            {
                combo_xsLSC->addItem(QString("TL84"), LSC_MODEL_TL84);
            }
            if(i & LSC_MODEL_D50)
            {
                combo_xsLSC->addItem(QString("D50"), LSC_MODEL_D50);
            }
            if(i & LSC_MODEL_D65)
            {
                combo_xsLSC->addItem(QString("D65"), LSC_MODEL_D65);
            }
        }

        if(is_ColorTemperature(m_hCamera, COLOR_TEMPERATURE_CMD_GET_LENS_SHADING_MODEL_DEFAULT, (void*)&nDefault, sizeof(nDefault)) == IS_SUCCESS)
        {
            m_xsLSCDefaultValue = combo_xsLSC->findData(nDefault);
            combo_xsLSC->setCurrentIndex(m_xsLSCDefaultValue);
        }
        connect (combo_xsLSC, SIGNAL(currentIndexChanged(int)), this, SLOT(OnXsLSC(int)));
    }

    if (nSupported == 0)
    {
        combo_xsLSC->hide();
        label_xsLSC->hide();
    }
}

void properties::UpdateXs_Temperature()
{
    int nSupported = 0;
    if (IS_SUCCESS == is_DeviceFeature(m_hCamera, IS_DEVICE_FEATURE_CMD_GET_SUPPORTED_FEATURES, &nSupported, sizeof(nSupported)))
    {
        if (nSupported & IS_DEVICE_FEATURE_CAP_TEMPERATURE)
        {
            double fTemperature = 0;
            if (is_DeviceFeature(m_hCamera, IS_DEVICE_FEATURE_CMD_GET_TEMPERATURE, (void*)&fTemperature, sizeof(fTemperature)) == IS_SUCCESS)
            {
                text_xsTemp->setText(QString("%1 °C").arg(fTemperature,0, 'f', 0));
            }
        }
    }
}

void properties::initTabXs()
{
    int ret = 0;
    // get AutoFocus Enable
    INT nCaps = 0;
    DWORD DeviceFeature;
    UINT bEnable = 0;

    /* Patch xs name strings when using a xc camera */
    if (m_SensorInfo.SensorID == IS_SENSOR_UI1013XC)
    {
        tabWidget->setTabText(tabWidget->indexOf(XsFeatures), "XC");
        groupBoxXsSpecialFeatures->setTitle("Special uEye XC Sensor Features");
    }

    /**********************************/
    /*     Get Supported Features     */
    /**********************************/

    if (is_DeviceFeature(m_hCamera, IS_DEVICE_FEATURE_CMD_GET_SUPPORTED_FEATURES, (void*)&DeviceFeature, sizeof(DeviceFeature)) != IS_SUCCESS)
    {
        qDebug("error: is_DeviceFeature (IS_DEVICE_FEATURE_CMD_GET_SUPPORTED_FEATURES) returned %d", ret);
        return;
    }

    /***********************************/
    /*     Noise-Reduction Feature     */
    /***********************************/

    if ((DeviceFeature & IS_DEVICE_FEATURE_CAP_NOISE_REDUCTION) != 0)
    {
        // Noise Reduction Feature Supported
        ret = is_DeviceFeature(m_hCamera, IS_DEVICE_FEATURE_CMD_GET_NOISE_REDUCTION_MODE, (void*)&bEnable, sizeof(bEnable));
        if (ret != IS_SUCCESS)
        {
            qDebug("error: is_DeviceFeature (IS_DEVICE_FEATURE_CMD_GET_NOISE_REDUCTION_MODE) returned %d", ret);
        }
        else
        {
            cB_xsDenoise->setChecked((bool)bEnable);
        }

        connect (cB_xsDenoise, SIGNAL(clicked(bool)), this, SLOT(OnXsNoise(bool)));
    }
    else
    {
        // Noise Reduction Feature not Supported
        cB_xsDenoise->hide();
    }


    /******************************/
    /*     Temperatur Feature     */
    /******************************/

    if ((DeviceFeature & IS_DEVICE_FEATURE_CAP_TEMPERATURE) != 0)
    {
        // Temperature Feature Supported
        UpdateXs_Temperature();
    }
    else
    {
        label_xsTemp->hide();
        text_xsTemp->hide();
    }


    /************************************/
    /*     JPEG-Compression Feature     */
    /************************************/

    initXs_JpegCompressionControls();
    UpdateXs_JpegCompressionControls();


    /*****************************/
    /*     Sharpness Control     */
    /*****************************/

    initXs_SharpnessControls();
    UpdateXs_SharpnessControls();


    /******************************/
    /*     Saturation Control     */
    /******************************/

    initXs_SaturationControls();
    UpdateXs_SaturationControls();


    /******************************/
    /*    Lens-Shading Control    */
    /******************************/

    initXs_LSCControls();
    UpdateXs_LSCControls();


    if(label_xsSharpness->isHidden() && label_xsSaturation->isHidden() && label_xsLSC->isHidden())
    {
        group_xsImageParam->hide();
    }

    /*************************/
    /*     Focus Control     */
    /*************************/

    initXs_FocusControls();
    UpdateXs_FocusControls();

    /**********************************/
    /*     Face-Detection Feature     */
    /**********************************/

    if (is_FaceDetection(m_hCamera, FDT_CMD_GET_CAPABILITIES, (void*)&nCaps, sizeof(nCaps)) == IS_SUCCESS)
    {
        if ((nCaps & FDT_CAP_SUPPORTED) != 0)
        {
            // Face-Detection Feature Supported
            if (is_FaceDetection(m_hCamera, FDT_CMD_GET_ENABLE, &bEnable, sizeof(bEnable)) == IS_SUCCESS)
            {
                cB_xsFaceDetectEnable->setChecked((bool)bEnable);
                connect (cB_xsFaceDetectEnable, SIGNAL(clicked(bool)), this, SLOT(OnXsFaceDetect(bool)));
            }
        }
        else
        {
            cB_xsFaceDetectEnable->hide();
        }
    }


    /***************************************/
    /*     Image-Stabilization Feature     */
    /***************************************/

    if (is_ImageStabilization(m_hCamera, IMGSTAB_CMD_GET_CAPABILITIES, &nCaps, sizeof(nCaps)) == IS_SUCCESS)
    {
        if ((nCaps & IMGSTAB_CAP_IMAGE_STABILIZATION_SUPPORTED) != 0)
        {
            // Image-Stabilization Feature Supported
            if (is_ImageStabilization(m_hCamera, IMGSTAB_CMD_GET_ENABLE, &bEnable, sizeof(UINT)) == IS_SUCCESS)
            {
                cB_xsImgStab->setChecked((bool)bEnable);

                cB_xsAES->setEnabled(bEnable == 0);
                cB_xsAGC->setEnabled(bEnable == 0);
            }

            connect (cB_xsImgStab, SIGNAL(clicked(bool)), this, SLOT(OnXsImgStabilization(bool)));
        }
        else
        {
            cB_xsImgStab->hide();
        }
    }


    /********************************/
    /*     AutoAbility Features     */
    /********************************/

    initXs_AutoParamControls();
    UpdateXs_AutoParamControls();


    // Timer for automatic updates
    connect(m_pTimerUpdateXsCtrl, SIGNAL(timeout()), this, SLOT(OnAutoUpdateXsCtrls()));
    m_pTimerUpdateXsCtrl->stop();
    m_pTimerUpdateXsCtrl->start(750);

}

void properties::UpdateXsFeatures()
{
    UpdateXs_FocusControls();
    UpdateXs_JpegCompressionControls();
    UpdateXs_SaturationControls();
    UpdateXs_SharpnessControls();
    UpdateXs_AutoParamControls();

    INT nSupported = 0;
    /* update nose reduction */
    if (is_DeviceFeature(m_hCamera, IS_DEVICE_FEATURE_CMD_GET_SUPPORTED_FEATURES, (void*)&nSupported, sizeof(nSupported)) == IS_SUCCESS)
    {
        if ((nSupported & IS_DEVICE_FEATURE_CAP_NOISE_REDUCTION) != 0)
        {
            // Noise Reduction Feature Supported
            INT bEnable = 0;
            if (is_DeviceFeature(m_hCamera, IS_DEVICE_FEATURE_CMD_GET_NOISE_REDUCTION_MODE, (void*)&bEnable, sizeof(bEnable)) == IS_SUCCESS)
            {
                cB_xsDenoise->setChecked((bool)bEnable);
            }
        }
    }

    /* update image stabilization */
    if (is_ImageStabilization(m_hCamera, IMGSTAB_CMD_GET_CAPABILITIES, &nSupported, sizeof(nSupported)) == IS_SUCCESS)
    {
        if ((nSupported & IMGSTAB_CAP_IMAGE_STABILIZATION_SUPPORTED) != 0)
        {
            // Image-Stabilization Feature Supported
            INT bEnable = 0;
            if (is_ImageStabilization(m_hCamera, IMGSTAB_CMD_GET_ENABLE, &bEnable, sizeof(UINT)) == IS_SUCCESS)
            {
                cB_xsImgStab->setChecked((bool)bEnable);

                cB_xsAES->setEnabled(bEnable == 0);
                cB_xsAGC->setEnabled(bEnable == 0);
            }
        }
    }

    /* update face detection */
    if (is_FaceDetection(m_hCamera, FDT_CMD_GET_CAPABILITIES, (void*)&nSupported, sizeof(nSupported)) == IS_SUCCESS)
    {
        if ((nSupported & FDT_CAP_SUPPORTED) != 0)
        {
            INT bEnable = 0;
            if ( is_FaceDetection(m_hCamera, FDT_CMD_GET_ENABLE, &bEnable, sizeof(bEnable)) == IS_SUCCESS)
            {
                cB_xsFaceDetectEnable->setChecked(bEnable);
            }
        }
    }
}

void properties::RestoreXsSettings()
{
    // Restore Focus Settings
    OnXsAutoFocus(true);
    OnXsFocusOnce();

    // Restore Auto-Features
    if(m_bXsHasAES)
    {
        OnXsAES(false);
    }
    if(m_bXsHasAGS)
    {
        OnXsAGC(false);
    }
    if(m_bXsHasAGES)
    {
        OnXsAGEC(true);
    }
    OnXsBacklightComp(false);
    OnXsFaceDetect(false);
    OnXsImgStabilization(false);
    OnXsNoise(true);

    // Restore Noise Setting
    OnXsSharpness(0);
    OnXsSaturation(0);
    OnXsLSC(m_xsLSCDefaultValue);
    OnXsJPEGMode(true);
    OnXsJPEGCompression(2);
}


/**************************************/
/* Functions called from GUI-Elements */
/**************************************/

// Auto-Focus Changed
void properties::OnXsAutoFocus(bool bEnable)
{
    if (bEnable)
    {
        if (is_Focus (m_hCamera, FOC_CMD_SET_ENABLE_AUTOFOCUS, NULL, 0) != IS_SUCCESS)
        {
            cB_xsAutoFocusEnable->setChecked(!bEnable);
        }
        else
        {
            cB_xsAutoFocusEnable->setChecked(bEnable);
        }
    }
    else
    {
        if (is_Focus (m_hCamera, FOC_CMD_SET_DISABLE_AUTOFOCUS, NULL, 0) != IS_SUCCESS)
        {
            cB_xsAutoFocusEnable->setChecked(!bEnable);
        }
        else
        {
            cB_xsAutoFocusEnable->setChecked(bEnable);
        }
    }
    UpdateXs_FocusControls();
}

// AES (Auto-Exposure-Setting) Changed
void properties::OnXsAES(bool bEnable)
{
    double dEnable = (bEnable)? 1:0;
    if (is_SetAutoParameter(m_hCamera, IS_SET_ENABLE_AUTO_SENSOR_SHUTTER, &dEnable, NULL) != IS_SUCCESS)
    {
        cB_xsAES->setChecked (!bEnable);
    }
    UpdateXs_AutoParamControls();
}

// AES (Auto-Gain-Control) Changed
void properties::OnXsAGC(bool bEnable)
{
    double dEnable = (bEnable)? 1:0;
    if (is_SetAutoParameter(m_hCamera, IS_SET_ENABLE_AUTO_SENSOR_GAIN, &dEnable, NULL) != IS_SUCCESS)
    {
        cB_xsAGC->setChecked (!bEnable);
    }
    UpdateXs_AutoParamControls();
}

// AGES (Auto-Gain-Exposure-Control) Changed
void properties::OnXsAGEC(bool bEnable)
{
    double dEnable = (bEnable)? 1:0;
    if (is_SetAutoParameter(m_hCamera, IS_SET_ENABLE_AUTO_SENSOR_GAIN_SHUTTER, &dEnable, NULL) != IS_SUCCESS)
    {
        cB_xsAES->setChecked (!bEnable);
    }
    UpdateXs_AutoParamControls();
}

// Backlight Compensation Changed
void properties::OnXsBacklightComp (bool bEnable)
{
    double dEnable = (bEnable)? 1:0;
    if (is_SetAutoParameter(m_hCamera, IS_SET_SENS_AUTO_BACKLIGHT_COMP, &dEnable, NULL) != IS_SUCCESS)
    {
        cB_xsBacklightCompEnable->setChecked (!bEnable);
    }
    UpdateXs_AutoParamControls();
}

// Face-Detection Changed
void properties::OnXsFaceDetect (bool bEnable)
{
    UINT nMaxFaces = 3;
    UINT bIsEnabled = false;
    int ret = 0;

    if (bEnable)
    {
        ret = is_FaceDetection(m_hCamera, FDT_CMD_SET_INFO_MAX_NUM_OVL, &nMaxFaces, sizeof(UINT));
        if (ret != IS_SUCCESS)
        {
            qDebug("error: is_FaceDetection (FDT_CMD_SET_INFO_MAX_NUM_OVL) returned %d", ret);
        }
        ret = is_FaceDetection(m_hCamera, FDT_CMD_SET_ENABLE, NULL, 0);
        if (ret != IS_SUCCESS)
        {
            qDebug("error: is_FaceDetection (FDT_CMD_SET_ENABLE) returned %d", ret);
            if ((ret = is_FaceDetection(m_hCamera, FDT_CMD_GET_ENABLE, &bIsEnabled, sizeof(bIsEnabled)) != IS_SUCCESS))
            {
                qDebug("error: is_FaceDetection (FDT_CMD_GET_ENABLE) returned %d", ret);
            }

            if((bool)bIsEnabled != bEnable)
            {
                cB_xsFaceDetectEnable->setChecked (!bEnable);
            }
        }
        else
        {
            cB_xsFaceDetectEnable->setChecked (bEnable);
        }
    }
    else
    {
        ret = is_FaceDetection (m_hCamera, FDT_CMD_SET_DISABLE, NULL, 0);
        if (ret != IS_SUCCESS)
        {
            qDebug("error: is_FaceDetection (FDT_CMD_SET_DISABLE) returned %d", ret);
            if ((ret = is_FaceDetection(m_hCamera, FDT_CMD_GET_ENABLE, &bIsEnabled, sizeof(bIsEnabled)) != IS_SUCCESS))
            {
                qDebug("error: is_FaceDetection (FDT_CMD_GET_ENABLE) returned %d", ret);
            }

            if((bool)bIsEnabled != bEnable)
            {
                cB_xsFaceDetectEnable->setChecked (!bEnable);
            }
        }
        else
        {
            cB_xsFaceDetectEnable->setChecked (bEnable);
        }
    }
}

// Image-Stabilization Changed
void properties::OnXsImgStabilization(bool bEnable)
{
    if (bEnable)
    {
        if (is_ImageStabilization(m_hCamera, IMGSTAB_CMD_SET_ENABLE, NULL, 0) != IS_SUCCESS)
        {
            cB_xsImgStab->setChecked (!bEnable);
        }
        else
        {
            cB_xsImgStab->setChecked (bEnable);
        }
    }
    else
    {
        if (is_ImageStabilization (m_hCamera, IMGSTAB_CMD_SET_DISABLE, NULL, 0) != IS_SUCCESS)
        {
            cB_xsImgStab->setChecked (!bEnable);
        }
        else
        {
            cB_xsImgStab->setChecked (bEnable);
        }
    }

    cB_xsAES->setEnabled(!bEnable);
    cB_xsAGC->setEnabled(!bEnable);
}

// Noise Reduction Changed
void properties::OnXsNoise(bool bEnable)
{
    INT nValue = bEnable ? IS_NOISE_REDUCTION_ADAPTIVE : IS_NOISE_REDUCTION_OFF;
    if (is_DeviceFeature(m_hCamera, IS_DEVICE_FEATURE_CMD_SET_NOISE_REDUCTION_MODE, (void*)&nValue, sizeof(nValue)) != IS_SUCCESS)
    {
        cB_xsDenoise->setChecked(bEnable);
    }
    else
    {
        cB_xsDenoise->setChecked(bEnable);
    }
}

// JPEG-Compression Changed
void properties::OnXsJPEGCompression (int value)
{
    if (value != 0)
    {
        value = (value < m_xsMinJPEGCompression) ? m_xsMinJPEGCompression : value;
        value = (value > m_xsMaxJPEGCompression) ? m_xsMaxJPEGCompression : value;
    }

    slider_xsJPEGCompression->setValue(value);

    if(is_DeviceFeature(m_hCamera, IS_DEVICE_FEATURE_CMD_SET_JPEG_COMPRESSION, (void*)&value, sizeof(value)) != IS_SUCCESS)
    {
        qDebug("error: is_DeviceFeature (IS_DEVICE_FEATURE_CMD_SET_JPEG_COMPRESSION)");
    }

    UpdateXs_JpegCompressionControls();
}

// JPEG-Mode Changed
void properties::OnXsJPEGMode (bool bEnable)
{
    SetJPEGMode(bEnable);
    UpdateXs_JpegCompressionControls();

    /* supported color formats changes when switching jpeg mode => update formats page */
    DisableColorFormats();
    EnableSupportedColorFormats();
}

// Sharpness Changed
void properties::OnXsSharpness (int value)
{
    if (value != 0)
    {
        value = (value < m_xsMinSharpness) ? m_xsMinSharpness : value;
        value = (value > m_xsMaxSharpness) ? m_xsMaxSharpness : value;
    }

    slider_xsSharpness->setValue(value);

    if(is_Sharpness(m_hCamera, SHARPNESS_CMD_SET_VALUE, (void*)&value, sizeof(value)) != IS_SUCCESS)
    {
        qDebug("error: is_Sharpness (SHARPNESS_CMD_SET_VALUE)");
    }

    UpdateXs_SharpnessControls();
}

// Saturation Changed
void properties::OnXsSaturation (int value)
{
    if (value != 0)
    {
        value = (value < m_xsMinSaturation) ? m_xsMinSaturation : value;
        value = (value > m_xsMaxSaturation) ? m_xsMaxSaturation : value;
    }

    slider_xsSaturation->setValue(value);

    if(is_Saturation(m_hCamera, SATURATION_CMD_SET_VALUE, (void*)&value, sizeof(value)) != IS_SUCCESS)
    {
        qDebug("error: is_Saturation (SATURATION_CMD_SET_VALUE)");
    }

    UpdateXs_SaturationControls();
}

// Manual Focus Changed
void properties::OnXsManualFocus (int value)
{
    if (value != 0)
    {
        value = (value < m_xsMinManualFocus) ? m_xsMinManualFocus : value;
        value = (value > m_xsMaxManualFocus) ? m_xsMaxManualFocus : value;
    }

    slider_xsManualFocus->setValue(value);

    if (is_Focus(m_hCamera, FOC_CMD_SET_MANUAL_FOCUS, (void*)&value, sizeof(value)) != IS_SUCCESS)
    {
         qDebug("error: is_Focus (FOC_CMD_SET_MANUAL_FOCUS)");
    }
    UpdateXs_FocusControls();
}

// Get Focus Distance Clicked
void properties::OnXsFocusDistance ()
{
    UINT nDistance;
    if (is_Focus(m_hCamera, FOC_CMD_GET_DISTANCE, (void*)&nDistance, sizeof(nDistance)) == IS_SUCCESS)
    {
        QString str;

        // nDistance = 100 mm = 10 cm = "Macro"
        if (nDistance == 100)
        {
            str = "Macro";
        }
        // nDistance = 500 mm = 50 cm = "Near"
        else if (nDistance == 500)
        {
            str = "Near";
        }
        // nDistance = 20000 mm = 2000 cm = 20 m = "Far"
        else if (nDistance == 20000)
        {
            str = "Far";
        }

        text_xsDistance->setText(str);
    }
    else
    {
        text_xsDistance->setText(QString("-"));
    }
}

// Focus Once Clicked
void properties::OnXsFocusOnce()
{
    if (m_xsFocusOnceActive)
        return;

    if (is_Focus(m_hCamera, FOC_CMD_SET_ENABLE_AUTOFOCUS_ONCE, NULL, 0) != IS_SUCCESS)
    {
        qDebug("error: is_Focus (FOC_CMD_SET_ENABLE_AUTOFOCUS_ONCE)");
    }

    if(!cB_xsAutoFocusEnable->isChecked())
    {
        m_xsFocusOnceActive = TRUE;
    }
    UpdateXs_FocusControls();
}

// LSC Changed
void properties::OnXsLSC(int index)
{
    qDebug("LSC Preset %d choosen.", index);

    int nLSCModel = combo_xsLSC->itemData(index).toInt();
    if(is_ColorTemperature(m_hCamera, COLOR_TEMPERATURE_CMD_SET_LENS_SHADING_MODEL, (void*)&nLSCModel, sizeof(nLSCModel)) != IS_SUCCESS)
    {
            qDebug("error: is_ColorTemperature (COLOR_TEMPERATURE_CMD_SET_LENS_SHADING_MODEL)");
    }

    combo_xsLSC->setCurrentIndex(index);
    UpdateXs_LSCControls();
}

// Auto-Updater
void properties::OnAutoUpdateXsCtrls ()
{
    if (isVisible())
    {
        UpdateXs_Temperature();

        if (m_xsFocusOnceActive)
            UpdateXs_FocusControls();

        if (cB_xsAutoFocusEnable->isChecked ())
            UpdateXs_FocusControls();
    }
}
