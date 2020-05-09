/*!
 * \file    properties.h
 * \date    22.09.2018
 * \author  zhusizhi
 * \version $Revision:$
 *
 */

#ifndef PROPERTIES_H
#define PROPERTIES_H

#include "mainview.h"
#include "eventthread.h"
#include "ui_properties.h"
#include "cps.h"

#include "qtui.h"
#include <QSlider>
#include <QLineEdit>
#include <QSpinBox>
#include <QString>
#include <QLabel>
#include <QTabWidget>
#include <QCheckBox>
#include <QButtonGroup>
#include <QMap>
#include "qsliderex.h"

class Mainview;

/*!
 * \defgroup UEYEPROPERTIES Property dialog
 * \{
 */
/*!
 * \brief Default minimum gain
 */
#define IS_MIN_GAIN 0
/*!
 * \brief Default maximum gain
 */
#define IS_MAX_GAIN 100

/*!
 * \brief Default minimum blacklevel offset
 */
#define IS_MIN_BL_OFFSET 0
/*!
 * \brief Default maximum blacklevel offset
 */
#define IS_MAX_BL_OFFSET 255
/*!
 * \brief default slider check period in msecs
 */
#define SLIDER_CHECK_PERIOD 500 //ms
/*!
 * \brief cps camera properties dialog
 */
class properties : public QDialog, public Ui::_DlgProperties
{
Q_OBJECT

public:
    /*!
     * \brief Constructor
     * \param hCam          Camera handle as returned from libcps_api
     * \param camInfo       Camera info structure
     * \param sensorInfo    Sensor info structure
     * \param bLive         Live mode indicator
     * \param parent        Qt parent object
     */
    properties (HIDS hCam, CAMINFO camInfo, SENSORINFO sensorInfo, bool bLive, QWidget* parent = 0);
    /*!
     * \brief Destructor
     */
    virtual ~properties ();


public slots:
    /*!
     * \brief Received event slot
     * \param event Event ID
     */
    void eventreceived (int event);

    virtual void OnRadioEdgeEnhancement ();
    virtual void OnSliderChangeFrameRate (int value);
    virtual void OnSliderChangeExposure (int value);
    virtual void OnSpinBoxChangePixelClock (int value);
    virtual void OnSliderChangePixelClock (int value);
    virtual void OnChangePixelClockSetting (int value);
    virtual void OnSliderChangeBlue (int value);
    virtual void OnSliderChangeGreen (int value);
    virtual void OnSliderChangeRed (int value);
    virtual void OnSliderChangeMaster (int value);
    virtual void OnButtonDefault ();

    virtual void checkAutoFps_clicked ();
    virtual void checkAutoExp_clicked ();
    virtual void checkMaxExp_clicked ();
    virtual void checkAutoGain_clicked ();

    virtual void sliderBlackLevel_valueChanged (int value);
    virtual void checkAutoBlacklevel_clicked ();
    virtual void checkBoxSoftwareGamma_clicked ();
    virtual void checkBoxHardwareGamma_clicked ();
    virtual void checkBoxGainBoost_clicked ();

    virtual void tabWidget_currentChanged (int);

    virtual void OnAutoUpdateCameraCtrls();
    virtual void OnAutoUpdateImageCtrls();

    virtual void OnUpdateLive(bool bLive);

    /*!
     * \brief Change frame rate
     */
    void ChangeFramerate (double value);
    /*!
     * \brief Update pixel clock controls
     */
    void ChangeExposure (double value);

    /*!
     * \brief Refresh page
     */
    void RefreshPage ();

protected:
    /*!
     * \brief Show event slot
     * \param \ Show event object
     */
    void showEvent (QShowEvent *);

private:
    /*!
     *  \brief Camera handle as returned from libcps_api
     */
    HIDS m_hCamera;
    /*!
     * \brief Camera info structure
     */
    CAMINFO m_CamInfo;
    /*!
     * \brief Sensor info structure
     */
    SENSORINFO m_SensorInfo;

    /*!
     * \brief Initialization indicator
     */
    bool m_bInit;
    /*!
     * \brief Live operation indicator
     */
    bool m_bLive;

    /*!
     * \brief Update pixel clock controls
     */
    void UpdatePixelclockCtrls ();
    /*!
     * \brief Update frame rate controls
     */
    void UpdateFramerateCtrls ();
    /*!
     * \brief Update exposure controls
     */
    void UpdateExposureCtrls ();
    /*!
     * \brief Update master gain controls
     */
    void UpdateMasterGainCtrls ();
    /*!
     * \brief Update black level controls
     */
    void UpdateBlackLevelCtrls ();
    /*!
     * \brief Update RGB gain controls
     */
    void UpdateRGBGainCtrls ();
    /*!
     * \brief Update feature controls
     */
    void UpdateFeaturesCtrls ();
    /*!
     * \brief Update edge enhancement controls
     */
    void UpdateEdgeEnhCtrls ();

    /*! \brief tab camera initialization */
    void initTabCamera();
    /*! \brief tab image initialization */
    void initTabImage();

    // pixelclock
    int m_maxPixelclock;
    int m_minPixelclock;
    int m_incPixelclock;
    int m_nPixelclockListIndex;
    int m_nPixelclockList[150];
    int m_nNumberOfSupportedPixelClocks;
    // framerate
    double m_maxFramerate;
    double m_minFramerate;
    double m_incFramerate;
    double m_dblFramerate;
    bool m_bAutoFramerate;
    bool m_bHasAutoFps;
    // exposure time
    double m_maxExposure;
    double m_minExposure;
    double m_incExposure;
    double m_dblExposure;
    bool m_bAutoExposure;
    bool m_bMaxExposure;
    bool m_bHasAutoExposure;

    // edgeenhancement
    int m_weakEdgeEnhancement;
    int m_strongEdgeEnhancement;

    // masterGain
    int m_maxMasterGain;
    int m_minMasterGain;
    int m_incMasterGain;
    int m_nMasterGain;
    bool m_bAutoGain;
    bool m_bHasAutoGain;
    // blacklevel
    int m_maxBlackLevel;
    int m_minBlackLevel;
    int m_incBlackLevel;
    int m_nBlackLevel;
    bool m_bAutoBlackLevel;
    bool m_bHasAutoBlackLevel;
    bool m_bHasManualBlackLevel;
    // rgbGains
    int m_maxRGBGain;
    int m_minRGBGain;
    int m_incRGBGain;
    int m_nRedGain;
    int m_nGreenGain;
    int m_nBlueGain;
    // features
    bool m_bSoftwareGammaSet;
    bool m_bHardwareGammaSet;
    bool m_bHasHardwareGamma;
    bool m_bHasSoftwareGamma;
    bool m_bHotPixelCorrSet;
    bool m_bGainBoostSet;
    bool m_bHasGainBoost;

    QTimer *m_pTimer;

    // Timer automatic update
    QTimer *m_pTimerUpdateCameraCtrl;
    QTimer *m_pTimerUpdateImageCtrl;

    Mainview* m_pDlgMainView;

};

#endif // PROPERTIES_H
