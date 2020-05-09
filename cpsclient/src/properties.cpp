/*!
 * \file    properties.cpp
 * \author  zhusizhi
 *
 * \brief   properties dialog class implementation for the Qt cps client
 *
 */

#include "properties.h"

properties::properties (HIDS hCam,CAMINFO camInfo,SENSORINFO sensorInfo,bool bLive,QWidget* parent) :
            QDialog (parent),
            m_hCamera (hCam),
            m_bInit (true),
            m_bLive (bLive),
            // framerate
            m_maxFramerate (0.0),
            m_minFramerate (0.0),
            m_incFramerate (0.0),
            m_dblFramerate (0.0),
            m_bAutoFramerate (FALSE),
            m_bHasAutoFps (FALSE),
            // exposure time
            m_maxExposure (0.0),
            m_minExposure (0.0),
            m_incExposure (0.0),
            m_dblExposure (0.0),
            m_bAutoExposure (FALSE),
            m_bMaxExposure (FALSE),
            m_bHasAutoExposure (FALSE),
            // masterGain
            m_maxMasterGain (0),
            m_minMasterGain (0),
            m_incMasterGain (0),
            m_nMasterGain (0),
            m_bAutoGain (FALSE),
            m_bHasAutoGain (FALSE),
            // blacklevel
            m_maxBlackLevel (0), m_minBlackLevel (0), m_incBlackLevel (0),
            m_nBlackLevel (0),
            m_bAutoBlackLevel (FALSE),
            m_bHasAutoBlackLevel (FALSE),
            m_bHasManualBlackLevel (FALSE),
            // rgbGains
            m_maxRGBGain (0), m_minRGBGain (0), m_incRGBGain (0),
            m_nRedGain (0),
            m_nGreenGain (0),
            m_nBlueGain (0),
            // features
            m_bSoftwareGammaSet (FALSE), m_bHardwareGammaSet (FALSE),
            m_bHasHardwareGamma (FALSE), m_bGainBoostSet (FALSE),
            m_bHasGainBoost (FALSE),
            // hotpixel
            m_bHotPixelCorrSet(FALSE)
{
    setupUi (this);

    QString strTemp, strTemp1;

    m_pDlgMainView = (Mainview*)parent;

    memcpy (&m_CamInfo, &camInfo, sizeof(camInfo));
    memcpy (&m_SensorInfo, &sensorInfo, sizeof(sensorInfo));

    UEYE_AUTO_INFO autoInfo;
    if (is_GetAutoInfo (m_hCamera, &autoInfo) == IS_SUCCESS)
    {
        m_bHasAutoExposure = autoInfo.AutoAbility & AC_SHUTTER;
        m_bHasAutoFps = m_bHasAutoExposure;
        m_bHasAutoGain = autoInfo.AutoAbility & AC_GAIN;
    }
    else
    {
        m_bHasAutoFps = FALSE;
        m_bHasAutoExposure = FALSE;
        m_bHasAutoGain = FALSE;
    }

    m_pTimerUpdateCameraCtrl = new QTimer(this);
    m_pTimerUpdateImageCtrl = new QTimer(this);

    /*! \note if camera uses IMAGE FORMATS instead of continuous AOI size  we have to disable the
     * normal size controls */
    INT nAOISupported = 0;
    INT ret = 0;
    if ((ret=is_ImageFormat(m_hCamera, IMGFRMT_CMD_GET_ARBITRARY_AOI_SUPPORTED, (void*)&nAOISupported,
                       sizeof(nAOISupported))) != IS_SUCCESS)
    {
        qDebug("ERROR: Properties -> initTabSize: is_ImageFormat returns %d", ret);
    }

    // init the image tab page
    initTabImage();

    // init the camera tab page
    initTabCamera();


    m_bInit = FALSE;
}

properties::~properties ()
{

}


// -------------------  showEvent  -----------------------
//
void properties::RefreshPage ()
{
    int curindex = tabWidget->currentIndex ();

    if (curindex == tabWidget->indexOf(Timing))
    {
        // CAMERA
        UpdateExposureCtrls ();
        /* UpdateFramerateCtrls and UpdatePixelclockCtrls depends on the results of UpdateExposureCtrls. */
        UpdatePixelclockCtrls ();
        UpdateFramerateCtrls ();
    }
    else if (curindex == tabWidget->indexOf(Image))
    {
        // IMAGE
        UpdateMasterGainCtrls ();
        UpdateBlackLevelCtrls ();
        UpdateRGBGainCtrls ();
        UpdateFeaturesCtrls ();
        UpdateEdgeEnhCtrls ();
    }
}

// -------------------  showEvent  -----------------------
//
void properties::showEvent (QShowEvent *)
{
    RefreshPage ();
}

// -------------------  tabWidget_currentChanged  -----------------------
//
void properties::tabWidget_currentChanged (int)
{
    RefreshPage ();
}

// -------------------  OnButtonDefault  -----------------------
//
void properties::OnButtonDefault ()
{
    if (m_bInit || !m_hCamera)
        return;

    INT colorMode;
    bool wasLive;
    // keep color format
    colorMode = is_SetColorMode (m_hCamera, IS_GET_COLOR_MODE);
    wasLive = (is_CaptureVideo (m_hCamera, IS_GET_LIVE) != 0);
    is_StopLiveVideo (m_hCamera, IS_WAIT);

    // restore all defaults
    is_ResetToDefault (m_hCamera);
    is_SetColorMode (m_hCamera, colorMode);

    // reset client flags
    m_bMaxExposure = false;

    RefreshPage ();

    m_pDlgMainView->SetupCapture();
    m_pDlgMainView->SetupTitles();

    if (wasLive)
        is_CaptureVideo (m_hCamera, IS_DONT_WAIT);
}

void properties::eventreceived (int event)
{
    if(isVisible())
    {
        switch (event)
        {
        case IS_SET_EVENT_WB_FINISHED:

            break;

        default:
            break;
        }
    }
}

void properties::OnUpdateLive(bool bLive)
{
    m_bLive = bLive;
}


