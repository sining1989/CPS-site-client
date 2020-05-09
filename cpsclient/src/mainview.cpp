/*!
 * \file    mainview.cpp
 * \date    10.09.2018
 * \author  zhusizhi
 * \version 1.0.0
 *
 * \brief   MainWindow class implementation of the Qt cps client
 *
 */

#include "mainview.h"
#include "version.h"
#include <QtDebug>
#include <QSettings>
#include <wchar.h>

#define IMAGEINFO_TIMEOUT 500 //ms

Mainview::Mainview (QWidget *parent, QString strStartID) :
    QMainWindow (parent)
{
    setupUi (this);
    pDlgCamerList = 0;
    m_eDispMode = display_fit;

    m_bMarkHotpixel = false;
    m_nHotpixelListSize = 0;
    m_nHotpixelList = NULL;

    initPassiveMulticast();

    QPalette palette;
    palette.setColor( QPalette::Background, QColor(220,220,220));
    //palette.setBrush(QPalette::Background, QBrush(QPixmap(":/new/prefix1/images/exit.png")));

    m_imageLabel = new PaintLabel(this);
    m_imageLabel->setPalette( palette );
    m_imageLabel->setBackgroundRole( QPalette::Background );
    m_imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_imageLabel->setPixmap(QPixmap(":/new/prefix1/images/ueye_background_logo.png"));
    m_imageLabel->setScaledContents(true);
    m_imageLabel->setAlignment(Qt::AlignCenter);

    m_scrollArea = new QScrollArea;
    m_scrollArea->setPalette( palette );
    m_scrollArea->setBackgroundRole( QPalette::Background );
    m_scrollArea->setAlignment(Qt::AlignCenter);
    m_scrollArea->setWidget(m_imageLabel);
    setCentralWidget(m_scrollArea);

    QString strWindowTitle = QString ("CPS Client");
    setWindowTitle (strWindowTitle);

    //init progressbar
    m_pProgBar = NULL;
    m_bAutomaticSEStarterUpload = false;
    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(updateProgress()));

    // start time variable
    m_Time.start();

    // create Image Info Dlg parameters
    m_pImageInfoDlg = new imageinfodlg(this);
    connect (actionImageinfo, SIGNAL(triggered(bool)), this, SLOT(onImageInfo()));
    connect (m_pImageInfoDlg, SIGNAL(close()), this, SLOT(onImageInfo()));

    // create statusbar
    createStatusBar ();
    // init worker Thread
    m_workThread = NULL;
    prepareWorkThread();

    // menu action connections
    connect (actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect (actionOpenCamera, SIGNAL(triggered()), this, SLOT(onOpenCameraLive()));
    connect (actionOpenCameraStop, SIGNAL(triggered()), this, SLOT(onOpenCamera()));
    connect (actionCloseCamera, SIGNAL(triggered()), this, SLOT(onCloseCamera()));
    connect (actionLive, SIGNAL(triggered()), this, SLOT(onLive()));
    connect (actionSnapshot, SIGNAL(triggered()), this, SLOT(onSnapshot()));
    connect (actionResetCamera, SIGNAL(triggered()), this, SLOT(onResetCamera()));
    connect (menuLoad_parameter, SIGNAL(aboutToShow()), this, SLOT(onPrepareLoadParameter()));
    connect (menuLoad_parameter, SIGNAL(triggered(QAction*)), this, SLOT(onLoadParameter(QAction*)));
    connect (menuSave_parameter, SIGNAL(triggered(QAction*)), this, SLOT(onSaveParameter(QAction*)));
    connect (actionSave_image, SIGNAL(triggered()), this, SLOT(onSaveImage()));
    connect (actionLoad_image, SIGNAL(triggered()), this, SLOT(onLoadImage()));

    connect (actionProperties, SIGNAL(triggered()), this, SLOT(onProperties()));
    connect (actionTrigger, SIGNAL(triggered()), this, SLOT(onUseTrigger()));
    connect (actionDisplay, SIGNAL(triggered()), this, SLOT(onDisplay()));
    connect (actionView_Fit, SIGNAL(triggered()), this, SLOT(onDisplay()));
    connect (actionOriginal_size, SIGNAL(triggered()), this, SLOT(onDisplay()));
    connect (actionHalf_size, SIGNAL(triggered()), this, SLOT(onDisplay()));
    connect (actionQuarter_size, SIGNAL(triggered()), this, SLOT(onDisplay()));
    connect (actionDouble_size, SIGNAL(triggered()), this, SLOT(onDisplay()));
    connect (actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect (actionUseErrorReport, SIGNAL(triggered()), this, SLOT(onApiErrorReport()));
    connect (actionHistogram, SIGNAL(toggled(bool)), this, SLOT(onHistogram(bool)));

    // setup actiongroup display
    QActionGroup *pDisplayGroup = new QActionGroup (this);
    pDisplayGroup->addAction (actionDisplay);
    pDisplayGroup->addAction (actionView_Fit);
    pDisplayGroup->addAction (actionOriginal_size);
    pDisplayGroup->addAction (actionHalf_size);
    pDisplayGroup->addAction (actionQuarter_size);
    pDisplayGroup->addAction (actionDouble_size);
    actionView_Fit->setChecked (true);

    m_hCamera = (HIDS)0;
    ZeroMemory (m_Images, sizeof(m_Images));
    ZeroMemory (&m_SensorInfo, sizeof(SENSORINFO));
    ZeroMemory (&m_CamInfo, sizeof(CAMINFO));
    ZeroMemory (&m_CamListInfo, sizeof(m_CamListInfo));

    m_BufferProps.width = 0;
    m_BufferProps.height = 0;
    m_BufferProps.bitspp = 8;
    m_BufferProps.colorformat = IS_CM_MONO8;
    m_BufferProps.imgformat = QImage::Format_Invalid;
    m_BufferProps.pRgbTable = NULL;
    m_BufferProps.tableentries = 0;

    for (int i = 0; i < 256; i++)
        m_table[i] = qRgb (i, i, i);

    m_bDisplayImage = TRUE;
    m_bLive = FALSE;

    nReceived = 0;
    nDisplayed = 0;
    nFailed = 0;
    m_pLastBuffer = NULL;

    /* create the event threads */
    m_pEvDevremove = new EventThread();
    m_pEvDevnew = new EventThread();
    m_pEvDevStateChanged = new EventThread();
    m_pEvFrame = new EventThread();
    m_pEvTransferFailed = new EventThread();
    m_pEvPMCImageChanged = new EventThread();
    m_pEvDeviceReconnected = new EventThread();

    /* connect the EventThread signals */
    connect(m_pEvFrame, SIGNAL(eventhappen(int)), this, SLOT(eventreceived(int)));
    connect(m_pEvTransferFailed, SIGNAL(eventhappen(int)), this, SLOT(eventreceived(int)));
    connect(m_pEvPMCImageChanged, SIGNAL(eventhappen(int)), this, SLOT(eventreceived(int)));
    connect(m_pEvDeviceReconnected, SIGNAL(eventhappen(int)), this, SLOT(eventreceived(int)));

    UpdateControls ();

    // create the imageinfo timer
    QTimer *pImageinfoTimer = new QTimer(this);
    connect(pImageinfoTimer, SIGNAL(timeout()), this, SLOT(onImageInfoUpdate()));
    pImageinfoTimer->start(IMAGEINFO_TIMEOUT);

    // if start ID is given with commandline then start
    // worker thread with initialzing the camera
    //if (strStartID != "")
    {
        // get the commandline start ID
        unsigned int camid = strStartID.toInt();

        // init the cameralist for getting device infos
        pDlgCamerList = new CameraList (this);
        //if (pDlgCamerList->SelectCameraByID(camid)==0)
        {
            /* pmc device is at the moment not available in the api. We wait for the first heartbeat */
            if (camid > 1500)
            {
               sleep(1);
            }
            m_CamListInfo = pDlgCamerList->GetCameraInfo ();

            // initialize and start the worker thread
            m_eWorkthreadJob = wj_cameraopenlive;
            m_workthreadReturn = IS_SUCCESS;
            m_workThread->start();
        }
        delete pDlgCamerList;
        pDlgCamerList = 0;
    }

    // properties dialog
    m_pPropertiesDlg = 0;

    m_pHistogramDlg = NULL;

    // read window state
    readSettings();

}

Mainview::~Mainview ()
{
    if (m_workThread)
    {
        if (m_workThread->wait (1000) == false)
            m_workThread->terminate ();
        delete m_workThread;
        m_workThread = NULL;
    }

    /* Signal stopping to all event threads => faster teardown */
    if (m_pEvDevnew)
        m_pEvDevnew->stop ();
    if (m_pEvDevremove)
        m_pEvDevremove->stop ();
    if (m_pEvDevStateChanged)
        m_pEvDevStateChanged->stop ();
    if (m_pEvFrame)
        m_pEvFrame->stop ();
    if (m_pEvTransferFailed)
        m_pEvTransferFailed->stop ();
    if (m_pEvPMCImageChanged)
        m_pEvPMCImageChanged->stop();
    if (m_pEvDeviceReconnected)
        m_pEvDeviceReconnected->stop();

    if (m_pEvDevnew)
    {
        if (m_pEvDevnew->wait (2 * EVENTTHREAD_WAIT_TIMEOUT) == false)
            m_pEvDevnew->terminate ();
        delete m_pEvDevnew;
        m_pEvDevnew = NULL;
    }
    if (m_pEvDevremove)
    {
        if (m_pEvDevremove->wait (2 * EVENTTHREAD_WAIT_TIMEOUT) == false)
            m_pEvDevremove->terminate ();
        delete m_pEvDevremove;
        m_pEvDevremove = NULL;
    }
    if (m_pEvDevStateChanged)
    {
        if (m_pEvDevStateChanged->wait (2 * EVENTTHREAD_WAIT_TIMEOUT) == false)
            m_pEvDevStateChanged->terminate ();
        delete m_pEvDevStateChanged;
        m_pEvDevStateChanged = NULL;
    }
    if (m_pEvFrame)
    {
        if (m_pEvFrame->wait (2 * EVENTTHREAD_WAIT_TIMEOUT) == false)
            m_pEvFrame->terminate ();
        delete m_pEvFrame;
        m_pEvFrame = NULL;
    }
    if (m_pEvTransferFailed)
    {
        if (m_pEvTransferFailed->wait (2 * EVENTTHREAD_WAIT_TIMEOUT) == false)
            m_pEvTransferFailed->terminate ();
        delete m_pEvTransferFailed;
        m_pEvTransferFailed = NULL;
    }
    if (m_pEvPMCImageChanged)
    {
        if (m_pEvPMCImageChanged->wait (2 * EVENTTHREAD_WAIT_TIMEOUT) == false)
            m_pEvPMCImageChanged->terminate ();
        delete m_pEvPMCImageChanged;
        m_pEvPMCImageChanged = NULL;
    }
    if (m_pEvDeviceReconnected)
    {
        if (m_pEvDeviceReconnected->wait (2 * EVENTTHREAD_WAIT_TIMEOUT) == false)
            m_pEvDeviceReconnected->terminate ();
        delete m_pEvDeviceReconnected;
        m_pEvDeviceReconnected = NULL;
    }
    if (m_pImageInfoDlg)
    {
        delete m_pImageInfoDlg;
        m_pImageInfoDlg = NULL;
    }
    if(m_pHistogramDlg)
    {
        delete m_pHistogramDlg;
        m_pHistogramDlg = NULL;
    }
    if (m_hCamera)
        is_ExitCamera (m_hCamera);
    m_hCamera = (HIDS)0;

    m_nHotpixelListSize = 0;
    delete[] m_nHotpixelList;

    // save window state
    writeSettings();
}

void Mainview::writeSettings()
{
    // saves window positions and states
    QSettings settings("CPS", "cpsclient");

    settings.beginGroup("mainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState());
    settings.endGroup();
}

void Mainview::readSettings()
{
    // reads window positions and states
    QSettings settings("CPS", "cpsclient");

    settings.beginGroup("mainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());
    settings.endGroup();
}

void Mainview::about ()
{
    QMessageBox::about (this, "About CPS Client",
                        QString("cps client application %1\n"
                        "robot ").arg(CLIENT_VERSION_STR));
}

void Mainview::createStatusBar ()
{
    textStatus = new QLabel (" CPS Status: ");
    textStatus->setAlignment (Qt::AlignLeft);

    textStatus_2 = new QLabel ();
    textStatus_3_0 = new QLabel ();
    textStatus_3_1 = new QLabel ();
    textStatus_3_2 = new QLabel ();
    textStatus_3_3 = new QLabel();
    textStatus_3_3->setVisible(false);
    textStatus_4 = new QLabel ();

    statusbar->addWidget (textStatus);
    statusbar->addWidget (textStatus_2);
    statusbar->addWidget (textStatus_3_0);
    statusbar->addWidget (textStatus_3_1);
    statusbar->addWidget (textStatus_3_2);
    statusbar->addWidget (textStatus_3_3);
    statusbar->addWidget (textStatus_4);
}


void Mainview::closeEvent (QCloseEvent * e)
{
    if (m_bLive)
        onLive ();

    // delete propertiesDlg
    delete m_pPropertiesDlg;
    m_pPropertiesDlg = 0;

    // hide optionally opened imageinfo dialog
    m_pImageInfoDlg->hide();

    e->accept ();
}

void Mainview::paintEvent (QPaintEvent *e)
{
    Q_UNUSED(e);
    m_bReady = TRUE;
}

int Mainview::GetBitsPerPixel(int colormode)
{
    switch (colormode)
    {
    default:
    case IS_CM_MONO8:
    case IS_CM_SENSOR_RAW8:
        return 8;   // occupies 8 Bit
    case IS_CM_MONO12:
    case IS_CM_MONO16:
    case IS_CM_SENSOR_RAW12:
    case IS_CM_SENSOR_RAW16:
    case IS_CM_BGR5_PACKED:
    case IS_CM_BGR565_PACKED:
    case IS_CM_UYVY_PACKED:
    case IS_CM_CBYCRY_PACKED:
        return 16;  // occupies 16 Bit
    case IS_CM_RGB8_PACKED:
    case IS_CM_BGR8_PACKED:
        return 24;
    case IS_CM_RGBA8_PACKED:
    case IS_CM_BGRA8_PACKED:
    case IS_CM_RGBY8_PACKED:
    case IS_CM_BGRY8_PACKED:
    case IS_CM_RGB10_PACKED:
    case IS_CM_BGR10_PACKED:
        return 32;
    }
}

void Mainview::DrawImage (char *pBuffer)
{
    int nNum;
    INT ret = IS_SUCCESS;
    if (!m_hCamera || !m_bDisplayImage)
        return;

    if (!pBuffer || m_BufferProps.width < 1 || m_BufferProps.height < 1)
        return;

    nNum = _GetImageNum (pBuffer);
    ret = is_LockSeqBuf (m_hCamera, nNum, pBuffer);

    QVector<QRgb> colors;
    for (int i = 0; i < m_BufferProps.tableentries; ++i)
    {
        colors.append(m_BufferProps.pRgbTable[i]);
    }
    QImage image((uchar*)pBuffer,
                 m_BufferProps.width,
                 m_BufferProps.height,
                 m_BufferProps.width * m_BufferProps.bitspp / 8,
                 m_BufferProps.imgformat);
    image.setColorTable(colors);

    // get current colormode
    int colormode = is_SetColorMode(m_hCamera, IS_GET_COLOR_MODE);
    switch (colormode)
    {
    default:
        break;
    case IS_CM_BGR8_PACKED:
        //swap the R and B color parts, cause QImage::Format_RGB888 takes order RGB
        image = image.rgbSwapped();
        break;
    }

    bool resizeAble = false;

    // switch view size
    double scaleFactor = 1.0;

    switch (m_eDispMode)
    {
    default:
    case display_fit:
        resizeAble = true;
        scaleFactor = 1.0;
        break;
    case display_original:
        resizeAble = false;
        scaleFactor = 1.0;
        break;
    case display_half:
        resizeAble = false;
        scaleFactor = 0.5;
        break;
    case display_quarter:
        resizeAble = false;
        scaleFactor = 0.25;
        break;
    case display_double:
        resizeAble = false;
        scaleFactor = 2.0;
        break;
    }
    m_scrollArea->setWidgetResizable(resizeAble);
    m_imageLabel->setImage(image, scaleFactor, resizeAble);

    if(m_pHistogramDlg != 0)
    {
        m_pHistogramDlg->updateHistogram(m_hCamera, pBuffer);
    }


    ret |= is_UnlockSeqBuf (m_hCamera, nNum, pBuffer);
}

void Mainview::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
}

void Mainview::onOpenCamera()
{
    openCamera(false);
}

void Mainview::onOpenCameraLive()
{
    openCamera(true);
}

void Mainview::onCloseCamera()
{
    if (m_hCamera == IS_INVALID_HIDS)
        return;

    /* Signal stopping event threads here (so we have not wait too long) */

    // stop frame event thread
    m_pEvFrame->stop ();
    // stop transfer failed event thread
    m_pEvTransferFailed->stop ();
    m_pEvPMCImageChanged->stop();

    m_pEvDeviceReconnected->stop();

    if (m_bLive)
        onLive ();

    _FreeImages ();


    /* wait for event threads termination */
    if (m_pEvFrame->wait (2 * EVENTTHREAD_WAIT_TIMEOUT) == FALSE)
        m_pEvFrame->terminate ();

    if (m_pEvTransferFailed->wait (2 * EVENTTHREAD_WAIT_TIMEOUT) == FALSE)
        m_pEvTransferFailed->terminate ();
    if (m_pEvPMCImageChanged->wait (2 * EVENTTHREAD_WAIT_TIMEOUT) == FALSE)
        m_pEvPMCImageChanged->terminate ();
    if (m_pEvDeviceReconnected->wait (2 * EVENTTHREAD_WAIT_TIMEOUT) == FALSE)
        m_pEvDeviceReconnected->terminate ();

    // Disable camera property
    // Evenet have to been closed before  is_ExitCamera is called
    delete m_pPropertiesDlg;
    m_pPropertiesDlg = 0;

    is_ExitCamera (m_hCamera);
    //printf ("ExitCamera returned %d\n", ret);
    QString strWindowTitle = QString ("cps client %1").arg(CLIENT_VERSION_STR);
    setWindowTitle (strWindowTitle);

    ZeroMemory (&m_SensorInfo, sizeof(SENSORINFO));
    ZeroMemory (&m_CamInfo, sizeof(CAMINFO));
    ZeroMemory (&m_CamListInfo, sizeof(UEYE_CAMERA_INFO));

    m_BufferProps.width = 0;
    m_BufferProps.height = 0;
    m_BufferProps.bitspp = 8;
    m_BufferProps.colorformat = IS_CM_MONO8;
    m_BufferProps.imgformat = QImage::Format_Invalid;
    m_BufferProps.pRgbTable = NULL;
    m_BufferProps.tableentries = 0;

    m_hCamera = (HIDS)0;

    textStatus_2->setText (QString ("S/N: %1 - Model: %2 (%3x%4)").arg(m_CamListInfo.SerNo)
                           .arg(m_CamListInfo.Model).arg(m_BufferProps.width).arg(m_BufferProps.height));

    nReceived = 0;
    nDisplayed = 0;
    nFailed = 0;
    m_eDispMode = display_fit;
    m_bDisplayImage = true;

    actionView_Fit->setChecked(true);
    m_pLastBuffer = NULL;

    // hide additional image info dlg
    m_pImageInfoDlg->hide();
    m_pImageInfoDlg->ResetCounters();
    m_pImageInfoDlg->ClearControls();
    actionImageinfo->setChecked(false);

    UpdateControls ();
    ClearStatusbarInfo();

    // draw old cps image
    m_imageLabel->setPixmap(QPixmap(":/new/prefix1/images/ueye_background_logo.png"));
//    UpdateFps (0.0);
//    UpdateCounters ();
}

int Mainview::openCamera (bool bStartLive)
{
    // use CameraList when more then 1 camera is plugged
    int camID = 0;
    bool bForceList = actionForceCameralist->isChecked();
    pDlgCamerList = new CameraList (this);

    /* connect the event thread signals */
    connect (m_pEvDevremove, SIGNAL(eventhappen(int)), this, SLOT(eventreceived(int)));
    connect (m_pEvDevnew, SIGNAL(eventhappen(int)), this, SLOT(eventreceived(int)));
    connect (m_pEvDevStateChanged, SIGNAL(eventhappen(int)), this, SLOT(eventreceived(int)));

    /* start the event threads */
    m_pEvDevremove->start (0, IS_SET_EVENT_REMOVAL);
    m_pEvDevnew->start (0, IS_SET_EVENT_NEW_DEVICE);
    m_pEvDevStateChanged->start (0, IS_SET_EVENT_STATUS_CHANGED);

    int cameraCount = pDlgCamerList->Size ();
    /* call the user dialog to select a camera */
    if (cameraCount > 1 || bForceList)
    {
        pDlgCamerList->exec ();
    }
    else
    {
        // first camera have to be selected
        pDlgCamerList->SelectCamera(0);
    }
    m_CamListInfo = pDlgCamerList->GetCameraInfo ();
    camID = m_CamListInfo.dwDeviceID;
    delete pDlgCamerList;
    pDlgCamerList = 0;

    // disconnect the event thread signals
    disconnect (m_pEvDevremove);
    disconnect (m_pEvDevnew);
    disconnect (m_pEvDevStateChanged);

    // stops the event threads
    m_pEvDevremove->stop ();
    m_pEvDevnew->stop();
    m_pEvDevStateChanged->stop();

    if (camID < 0)
        return -1;

    if (m_CamListInfo.dwInUse != 0)
    {
        return -1;
    }

    // start worker thread with initializing the camera
    if (bStartLive)
    {
        m_eWorkthreadJob = wj_cameraopenlive;
    }
    else
    {
        m_eWorkthreadJob = wj_cameraopen;
    }

    // disable buttons to open new cameras
    actionOpenCamera->setEnabled (false);
    actionOpenCameraStop->setEnabled (false);

    // start worker thread to open camera
    m_workthreadReturn = IS_SUCCESS;
    m_workThread->start();

    return 0;
}

int Mainview::reOpenCamera()
{
    m_bReopenLive = m_bLive || m_bReopenLive;

    UEYE_CAMERA_INFO camInfo = m_CamListInfo;
    onCloseCamera();
    m_CamListInfo = camInfo;

    // start worker thread with initializing the camera
    m_eWorkthreadJob = wj_camerareopen;

    // disable buttons to open new cameras
    actionOpenCamera->setEnabled (false);
    actionOpenCameraStop->setEnabled (false);

    // start worker thread to open camera
    m_workthreadReturn = IS_SUCCESS;
    m_workThread->start();

    return 0;
}

int Mainview::CameraInitialized()
{
    int ret = m_workthreadReturn;
    unsigned int uInitialParameterSet = IS_CONFIG_INITIAL_PARAMETERSET_NONE;

    if (ret != IS_SUCCESS)
    {
        m_hCamera = (HIDS)0;
        if (ret == IS_STARTER_FW_UPLOAD_NEEDED)
        {
            int buttonPressed = QMessageBox::information (this, "Firmware Error", QString("Upload of new Starter firmware is needed.\n"
                    "This will take about 25 seconds"), QMessageBox::Ok | QMessageBox::Cancel);
            if (buttonPressed == QMessageBox::Ok)
            {
                // start work thread with fw upload
                m_bAutomaticSEStarterUpload = true;
                m_workThread->start();
            }
        }
        else
        {
            QMessageBox::information (this, "Error !!!", QString("Open camera failed (Code: %1)").arg(ret), 0);
        }
    }
    else
    {
        if ((ret=is_GetCameraInfo (m_hCamera, &m_CamInfo)) != IS_SUCCESS)
        {
            QMessageBox::information (this, "Error !!!", "GetCameraInfo failed", 0);
        }
        else if ((ret=is_GetSensorInfo (m_hCamera, &m_SensorInfo)) != IS_SUCCESS)
        {
            QMessageBox::information (this, "Error !!!", "GetSensorInfo failed", 0);
        }
        else if ((ret = is_Configuration(IS_CONFIG_INITIAL_PARAMETERSET_CMD_GET, &uInitialParameterSet, sizeof(unsigned int))) != IS_SUCCESS)
        {
            QMessageBox::information(this, "Error !!!", "querying 'initial parameter set' failed", 0);
        }
        else
        {
            //m_nWidth = m_SensorInfo.nMaxWidth;
            //m_nHeight = m_SensorInfo.nMaxHeight;

            // restore all defaults
            // do this only if there is no 'initial parameter set' installed.
            // if an 'initial parameter set' is installed we must not overwrite this setup!
            if (uInitialParameterSet == IS_CONFIG_INITIAL_PARAMETERSET_NONE)
            {
                ret = is_ResetToDefault (m_hCamera);
            }

            int colormode = 0;
            if (m_SensorInfo.nColorMode >= IS_COLORMODE_BAYER)
            {
                colormode = IS_CM_BGRA8_PACKED;
            }
            else
            {
                colormode = IS_CM_MONO8;
            }

            if (is_SetColorMode (m_hCamera, colormode) != IS_SUCCESS)
            {
                QMessageBox::information (this, "Error !!!", "SetColorMode failed", 0);
            }

            /* get some special camera properties */
            ZeroMemory (&m_CameraProps, sizeof(m_CameraProps));

            // If the camera does not support a continuous AOI -> it uses special image formats
            m_CameraProps.bUsesImageFormats = false;
            INT nAOISupported = 0;
            if (is_ImageFormat(m_hCamera, IMGFRMT_CMD_GET_ARBITRARY_AOI_SUPPORTED, (void*)&nAOISupported,
                               sizeof(nAOISupported)) == IS_SUCCESS)
            {
                m_CameraProps.bUsesImageFormats = (nAOISupported == 0);
            }

            /* set the default image format, if used */
            if (m_CameraProps.bUsesImageFormats)
            {
                // search the default formats
                m_CameraProps.nImgFmtNormal  = searchDefImageFormats(CAPTMODE_FREERUN | CAPTMODE_SINGLE);
                m_CameraProps.nImgFmtDefaultNormal = m_CameraProps.nImgFmtNormal;
                m_CameraProps.nImgFmtTrigger = searchDefImageFormats(CAPTMODE_TRIGGER_SOFT_SINGLE);
                m_CameraProps.nImgFmtDefaultTrigger = m_CameraProps.nImgFmtTrigger;
                // set the default formats
                if ((ret=is_ImageFormat(m_hCamera, IMGFRMT_CMD_SET_FORMAT, (void*)&m_CameraProps.nImgFmtNormal,
                                   sizeof(m_CameraProps.nImgFmtNormal))) == IS_SUCCESS)
                {
                    //m_nImageFormat = nFormat;
                    //bRet = TRUE;
                }
            }

            /* setup the capture parameter */
            SetupCapture();

            ret = is_EnableEvent (m_hCamera, IS_SET_EVENT_FRAME);
            m_pEvFrame->start (m_hCamera, IS_SET_EVENT_FRAME);
            m_pEvTransferFailed->start(m_hCamera, IS_SET_EVENT_CAPTURE_STATUS);

            ret = is_EnableEvent(m_hCamera, IS_SET_EVENT_PMC_IMAGE_PARAMS_CHANGED);
            m_pEvPMCImageChanged->start(m_hCamera, IS_SET_EVENT_PMC_IMAGE_PARAMS_CHANGED);

            ret = is_EnableEvent(m_hCamera, IS_SET_EVENT_DEVICE_RECONNECTED);
            m_pEvDeviceReconnected->start(m_hCamera, IS_SET_EVENT_DEVICE_RECONNECTED);

            SetupTitles();
        }

        nReceived = 0;
        nDisplayed = 0;
        nFailed = 0;
        m_eDispMode = display_fit;
        m_bDisplayImage = true;
        m_pLastBuffer = NULL;

        UpdateFps (0.0);
        UpdateCounters ();
    }

    UpdateControls ();

    return ret;
}

void Mainview::SetupTitles()
{
    QString strWindowTitle = QString ("cps client %1 - %2 (%3x%4) - ID: %5 - SerNo: %6")
        .arg(CLIENT_VERSION_STR).arg(m_CamListInfo.Model).arg(m_BufferProps.width).arg(m_BufferProps.height)
        .arg(m_CamListInfo.dwCameraID).arg(m_CamListInfo.SerNo);
    setWindowTitle (strWindowTitle);

    textStatus_2->setText (QString ("S/N: %1 - Model: %2 (%3x%4)").arg(m_CamListInfo.SerNo)
                               .arg(m_CamListInfo.Model).arg(m_BufferProps.width).arg(m_BufferProps.height));
}

/*! \brief search the right default image format of a camera
 *
 * \param suppportMask
 * \return found imageformat
 */
int Mainview::searchDefImageFormats(int suppportMask)
{
    int ret = IS_SUCCESS;
    int nNumber;
    int format = 0;
    IMAGE_FORMAT_LIST *pFormatList;
    IS_RECT rectAOI;

    if ((ret=is_ImageFormat(m_hCamera, IMGFRMT_CMD_GET_NUM_ENTRIES, (void*)&nNumber, sizeof(nNumber))) == IS_SUCCESS &&
        (ret=is_AOI(m_hCamera, IS_AOI_IMAGE_GET_AOI, (void*)&rectAOI, sizeof(rectAOI))) == IS_SUCCESS)
    {
        int i = 0;
        int nSize = sizeof(IMAGE_FORMAT_LIST) + (nNumber - 1) * sizeof(IMAGE_FORMAT_LIST);
        pFormatList = (IMAGE_FORMAT_LIST*)(new char[nSize]);
        pFormatList->nNumListElements = nNumber;
        pFormatList->nSizeOfListEntry = sizeof(IMAGE_FORMAT_INFO);

        if((ret=is_ImageFormat(m_hCamera, IMGFRMT_CMD_GET_LIST, (void*)pFormatList, nSize)) == IS_SUCCESS)
        {
            for(i=0; i<nNumber; i++)
            {
                if ((pFormatList->FormatInfo[i].nSupportedCaptureModes & suppportMask) &&
                     pFormatList->FormatInfo[i].nHeight == (UINT)rectAOI.s32Height &&
                     pFormatList->FormatInfo[i].nWidth  == (UINT)rectAOI.s32Width)
                {
                    format = pFormatList->FormatInfo[i].nFormatID;
                    break;
                }
            }
        }
        else
        {
            qDebug("error: is_ImageFormat returned %d", ret);
        }

        delete (pFormatList);
    }
    else
    {
        qDebug("error: is_ImageFormat returned %d", ret);
    }
    qDebug("searchDefImageFormats returned %d [mask: 0x%X]\n", format, suppportMask);
    return format;
}

int Mainview::SetupCapture()
{
    int width, height;
    // init the memorybuffer properties
    ZeroMemory (&m_BufferProps, sizeof(m_BufferProps));

    IS_RECT rectAOI;
    INT nRet = is_AOI(m_hCamera, IS_AOI_IMAGE_GET_AOI, (void*)&rectAOI, sizeof(rectAOI));

    if (nRet == IS_SUCCESS)
    {
        width  = rectAOI.s32Width;
        height = rectAOI.s32Height;

        // get current colormode
        int colormode = is_SetColorMode(m_hCamera, IS_GET_COLOR_MODE);
        qDebug("SDK colormode: %d", colormode);
        
        if(colormode == IS_CM_BGR5_PACKED)
        {
            is_SetColorMode(m_hCamera, IS_CM_BGR565_PACKED);
            colormode = IS_CM_BGR565_PACKED;
            QMessageBox::information(this, "Info!", QString("Colorformat 'IS_CM_BGR5_PACKED' actually not supported from client, patched to 'IS_CM_BGR565_PACKED'\n"));
            
        }

        // fill memorybuffer properties
        ZeroMemory (&m_BufferProps, sizeof(m_BufferProps));
        m_BufferProps.width  = width;
        m_BufferProps.height = height;
        m_BufferProps.colorformat = colormode;
        m_BufferProps.bitspp = GetBitsPerPixel(colormode);

        switch (colormode)
        {
        default:
        case IS_CM_MONO8:
        case IS_CM_SENSOR_RAW8:
            m_BufferProps.pRgbTable = m_table;
            m_BufferProps.tableentries = 256;
            m_BufferProps.imgformat = QImage::Format_Indexed8;
            break;
        case IS_CM_BGR565_PACKED:
            m_BufferProps.imgformat = QImage::Format_RGB16;
            break;
        case IS_CM_RGB8_PACKED:
        case IS_CM_BGR8_PACKED:
            m_BufferProps.imgformat = QImage::Format_RGB888;
            break;
        case IS_CM_RGBA8_PACKED:
        case IS_CM_BGRA8_PACKED:
            m_BufferProps.imgformat = QImage::Format_RGB32;
            break;
        }

        // Reallocate image buffers
        _AllocImages();

        qDebug("Capture ready set up.");
    }
    return 0;
}

void Mainview::startLive()
{
    m_bLive = TRUE;
    m_bReady = TRUE;
    is_CaptureVideo (m_hCamera, IS_DONT_WAIT);
    actionLive->setChecked (true);
}

void Mainview::stopLive()
{
    INT nRet = 1;

    QTime timer;
    timer.start();

    while(nRet != IS_SUCCESS && timer.elapsed() <= 2 * 1000 )
    {
        nRet = is_StopLiveVideo (m_hCamera, IS_WAIT);
    }

    if(nRet == IS_SUCCESS)
    {
        m_bLive = FALSE;
        actionLive->setChecked (false);
    }
    else
    {
        QMessageBox::information (this, "Error !!!", QString("StopLiveVideo failed (Code: %1)").arg(nRet), 0);
    }
}

void Mainview::onLive ()
{
    if (!m_bLive)
    {
        startLive();
    }
    else
    {
        stopLive();
    }
    if (m_pPropertiesDlg != 0)
        m_pPropertiesDlg->OnUpdateLive (m_bLive);

    UpdateControls ();
    UpdateCounters ();

}

void Mainview::onSnapshot ()
{
    INT ret;

    if (m_hCamera)
    {
        if (!m_bLive)
        {
            ret = is_FreezeVideo (m_hCamera, IS_WAIT);

            if (ret != IS_SUCCESS)
            {
                QMessageBox::information (this, "Error !!!", QString("FreezeVideo failed (Code: %1)").arg(ret), 0);
            }
        }
    }
    else
        QMessageBox::information (this, "Error !!!", "Camera not open", 0);

}

void Mainview::onSaveImage ()
{
    if (m_hCamera)
    {
        QFileDialog* fd = new QFileDialog (this, "Save the current image" "/home", "image.bmp");
        fd->setAcceptMode(QFileDialog::AcceptSave);
        //fd->setMode (QFileDialog::AnyFile);
        QStringList namefilters;
        namefilters << "Bitmap images (*.bmp)";
        namefilters << "JPEG images (*.jpg)";
        namefilters << "PNG images (*.png)";

        fd->setNameFilters(namefilters);
        fd->selectNameFilter("Bitmap images (*.bmp)");

        QString fileName;
        if (fd->exec () == QDialog::Accepted)
        {
            fileName = fd->selectedFiles().at(0);
        }

        INT ret = IS_SUCCESS;
        if (!fileName.isEmpty ())
        {
            // set Bitmap params as default
            int fileformat = IS_IMG_BMP;
            int fileparam = 0; // !only for IS_IMG_JPEG
            QString selnamefilter = fd->selectedNameFilter();
            qDebug("selected name filter : %s", selnamefilter.toStdString().c_str());
            if (fd->selectedNameFilter() == "JPEG images (*.jpg)")
            {
                fileformat = IS_IMG_JPG;
                fileparam = 0; // JPEG quality 1-100%
            }
            else if (fd->selectedNameFilter() == "PNG images (*.png)")
            {
                fileformat = IS_IMG_PNG;
            }

            IMAGE_FILE_PARAMS ImageFileParams;

            wchar_t FilePath[255];
            memset(FilePath, 0, sizeof(FilePath));
            wcscpy(FilePath, fileName.toStdWString().c_str());

            ImageFileParams.pwchFileName = FilePath;
            ImageFileParams.pnImageID = NULL;
            ImageFileParams.ppcImageMem = NULL;
            ImageFileParams.nQuality = fileparam;
            ImageFileParams.nFileType = fileformat;

            ret = is_ImageFile(m_hCamera, IS_IMAGE_FILE_CMD_SAVE, (void*)&ImageFileParams, sizeof(ImageFileParams));
            if (ret != IS_SUCCESS)
            {
                QMessageBox::critical(this, "Error!", QString("Saving image failed with code %1!").arg(ret), QMessageBox::Ok, 0);
            }
        }
    }
    else
    {
        QMessageBox::information (this, "Error !!!", "Camera not open", 0);
    }
}

void Mainview::onLoadImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "/home",
                                                    tr("Images (*.png *.bmp)"));

    if (!fileName.isEmpty())
    {
        // stop live video
        if (m_bLive)
        {
            m_bLive = false;
            is_StopLiveVideo(m_hCamera, IS_WAIT);

            UpdateControls ();
        }

    	IMAGE_FILE_PARAMS ImageFileParams;

        wchar_t FilePath[255];
        memset(FilePath, 0, sizeof(FilePath));
        wcscpy(FilePath, fileName.toStdWString().c_str());

        ImageFileParams.pwchFileName = FilePath;
    	ImageFileParams.pnImageID = NULL;
    	ImageFileParams.ppcImageMem = NULL;
    	ImageFileParams.nQuality = 0;

    	int nRet = is_ImageFile(m_hCamera, IS_IMAGE_FILE_CMD_LOAD, (void*)&ImageFileParams, sizeof(ImageFileParams));
        if (nRet != IS_SUCCESS)
        {
            char *pErr = 0;
            is_GetError(m_hCamera, &nRet, &pErr);
            QString strErr(pErr);
            QMessageBox::information(this, "Error!", QString("Loading the image failed with error code: %1\n").arg(nRet) + strErr);
            qDebug() << "Error: Mainview --> onLoadImage() --> is_LoadImage() : " << nRet;
        }
        // Redraw frame
        ProcessFrame ();
    }
}

void Mainview::onResetCamera ()
{
    if(m_pPropertiesDlg != 0)
    {
        m_pPropertiesDlg->OnButtonDefault();
        UpdateControls ();
    }
}

void Mainview::onPrepareLoadParameter()
{
    qDebug("prepare loading camera parameter");
    bool bEnableParam = true;

    UINT nNumber = 0;
    int nRet = is_ParameterSet(m_hCamera, IS_PARAMETERSET_CMD_GET_NUMBER_SUPPORTED, (void*)&nNumber, sizeof(nNumber));
    if (IS_SUCCESS != nRet || nNumber != 1)
    {
        bEnableParam = false;
    }

    if (bEnableParam)
    {
         nRet = is_ParameterSet(m_hCamera, IS_PARAMETERSET_CMD_GET_HW_PARAMETERSET_AVAILABLE, (void*)&nNumber, sizeof(nNumber));
         if (IS_SUCCESS != nRet || nNumber != 1)
         {
             bEnableParam = false;
         }
    }

    // enable loading parameter sets
    actionLoadParameterSet->setEnabled(bEnableParam);
}

void Mainview::onLoadParameter(QAction* action)
{
	int ret;

    qDebug() << "onLoadParameter " << action->iconText();
    bool bStartLive = false;
    QFileDialog* fd = new QFileDialog (this, "Load camera parameter file" "/home");
    fd->setAcceptMode(QFileDialog::AcceptOpen);
    fd->setNameFilter("Ini Files (*.ini)");
    QString fileName ="";

    if (action != actionLoadParameterSet)
    {
        if (fd->exec () == QDialog::Accepted)
            fileName = fd->selectedFiles().at(0);
    }

    if (!fileName.isEmpty() || action == actionLoadParameterSet)
    {
        // stop optional live capture
        if (m_bLive)
        {
            is_StopLiveVideo (m_hCamera, IS_WAIT);
            m_bLive = FALSE;
            bStartLive = true; // restart again
        }
        // call cps function
        if( action == actionLoadParameterSet)
        {
            ret = is_ParameterSet( m_hCamera, IS_PARAMETERSET_CMD_LOAD_EEPROM, NULL, 0 );
        	fileName = "Cam-EEPROM";
        }
        else
        {
            ret = is_ParameterSet( m_hCamera, IS_PARAMETERSET_CMD_LOAD_FILE, (wchar_t*)fileName.toStdWString().c_str (), 0 );
        }
        if (ret == IS_SUCCESS)
        {
            // refresh property window
            m_pPropertiesDlg->RefreshPage();

            qDebug() << "camera config file " << fileName << " loaded successfully!";
            SetupCapture();
            /* update window title because the size may have changed */
            SetupTitles();

            // get current colormode
            int colormode = is_SetColorMode(m_hCamera, IS_GET_COLOR_MODE);
       
            // Get convert mode
            int nCurrentConvertMode, nDefaultConvertMode, nSupportedConvertModes = 0;
            is_GetColorConverter(m_hCamera, colormode, &nCurrentConvertMode, &nDefaultConvertMode, &nSupportedConvertModes);
        
        }
        // start optional live capture
        if (bStartLive)
        {
            m_bLive = TRUE;
            m_bReady = TRUE;
            is_CaptureVideo (m_hCamera, IS_DONT_WAIT);
        }
        // error checking
        if (ret != IS_SUCCESS)
        {
            if (ret == IS_INVALID_CAMERA_TYPE)
            {
                QMessageBox::information (this, "Load camera parameter file", "The configuration file\n"
                                          + fileName + "\ncannot be applied to the selected camera!", 0);
            }
            else
            {
                QString msg = QString("Error loading camerafile (error %1)").arg(ret);
                QMessageBox::warning (this, "Load camera parameter file", msg);
            }
        }
    }
}

void Mainview::onSaveParameter(QAction* action)
{
	int ret;

    qDebug() << "onSaveParameter " << action->iconText();
    QString filehint = "camconfig.ini";
    if (m_hCamera)
    {
        filehint = QString("%1_conf.ini").arg(QString(m_CamListInfo.Model));
    }
    QFileDialog* fd = new QFileDialog (this, "Save camera parameter file" "/home", filehint );
    fd->setAcceptMode(QFileDialog::AcceptSave);
    fd->setNameFilter("Ini Files (*.ini)");
    fd->setDefaultSuffix("ini");

    QString fileName ="";

    if (action != actionSaveParameterSet)
    {
        if (fd->exec () == QDialog::Accepted)
            fileName = fd->selectedFiles().at(0);
    }

    if (!fileName.isEmpty () || action == actionSaveParameterSet)
    {
        // call cps function
    	if(action == actionSaveParameterSet)
    	{
            ret = is_ParameterSet( m_hCamera, IS_PARAMETERSET_CMD_SAVE_EEPROM, NULL, 0);
        	fileName = "Cam-EEPROM";
    	}
    	else
    	{
            ret = is_ParameterSet( m_hCamera, IS_PARAMETERSET_CMD_SAVE_FILE, (wchar_t*)fileName.toStdWString().c_str (), 0 );
    	}
        if (ret == IS_SUCCESS)
        {
            qDebug() << "camera config file " << fileName << " saved successfully!";
        }
        else
        {
            QString msg = QString("Error saving camerafile (error %1)").arg(ret);
            QMessageBox::warning (this, "Load camera parameter file", msg);
        }
    }

}

void Mainview::onProperties ()
{
    //TODO modeless dlg
    // Properties dialog

    if(m_pPropertiesDlg != 0)
    {
        m_pPropertiesDlg->show ();
        UpdateControls ();
    }


}

void Mainview::onUseTrigger ()
{
    if (actionTrigger->isChecked ())
    {
        /* 1013/1008 only could use software trigger */
        if (m_SensorInfo.SensorID == IS_SENSOR_UI1013XC || m_SensorInfo.SensorID == IS_SENSOR_UI1008_C)
        {
            int nRet = 0;

            nRet = is_SetExternalTrigger (m_hCamera, IS_SET_TRIGGER_SOFTWARE);
            if (nRet == IS_SUCCESS)
            {
                actionLive->setEnabled(false);

                /* use software trigger image format */
                if ((nRet = is_ImageFormat(m_hCamera, IMGFRMT_CMD_SET_FORMAT, (void*)&m_CameraProps.nImgFmtTrigger, sizeof(m_CameraProps.nImgFmtTrigger))) != IS_SUCCESS)
                {
                    QMessageBox::warning(this, "Error!", QString("Could not set image format. Failed with %1").arg(nRet));
                }
                else
                {
                    SetupCapture();
                    SetupTitles();
                }
            }
        }
        else
        {
            is_SetExternalTrigger (m_hCamera, IS_SET_TRIGGER_HI_LO);
        }
    }
    else
    {
        int nRet = 0;
        nRet = is_SetExternalTrigger (m_hCamera, IS_SET_TRIGGER_OFF);
        if (nRet == IS_SUCCESS)
        {
            actionLive->setEnabled(true);

            if (m_SensorInfo.SensorID == IS_SENSOR_UI1013XC || m_SensorInfo.SensorID == IS_SENSOR_UI1008_C)
            {
                /* use live video image format */
                if ((nRet = is_ImageFormat(m_hCamera, IMGFRMT_CMD_SET_FORMAT, (void*)&m_CameraProps.nImgFmtNormal, sizeof(m_CameraProps.nImgFmtNormal))) != IS_SUCCESS)
                {
                    QMessageBox::warning(this, "Error!", QString("Could not set image format. Failed with %1").arg(nRet));
                }
                else
                {
                    SetupCapture();
                    SetupTitles();
                }
            }
        }
    }
}

void Mainview::onDisplay ()
{
    if (actionDisplay->isChecked ())
    {
        m_bDisplayImage = false;
        m_eDispMode = display_no;
        m_scrollArea->setWidgetResizable(true);
        m_scrollArea->setAlignment(Qt::AlignCenter);
        m_imageLabel->clear(true);
    }
    else if (actionOriginal_size->isChecked ())
    {
        m_bDisplayImage = true;
        m_eDispMode = display_original;
        m_scrollArea->setAlignment(Qt::AlignCenter);
    }
    else if (actionHalf_size->isChecked ())
    {
        m_bDisplayImage = true;
        m_eDispMode = display_half;
        m_scrollArea->setAlignment(Qt::AlignCenter);
    }
    else if (actionQuarter_size->isChecked ())
    {
        m_bDisplayImage = true;
        m_eDispMode = display_quarter;
        m_scrollArea->setAlignment(Qt::AlignCenter);
    }
    else if (actionDouble_size->isChecked ())
    {
        m_bDisplayImage = true;
        m_eDispMode = display_double;
        m_scrollArea->setAlignment(Qt::AlignCenter);
    }
    else /* actionView_Fit isChecked */
    {
        m_bDisplayImage = true;
        m_eDispMode = display_fit;
        m_scrollArea->setAlignment(Qt::AlignCenter);
    }
}

void Mainview::onApiErrorReport()
{
    if (actionUseErrorReport->isChecked())
        is_SetErrorReport(m_hCamera, IS_ENABLE_ERR_REP);
    else
        is_SetErrorReport(m_hCamera, IS_DISABLE_ERR_REP);
}

void Mainview::onImageInfo()
{
    if (m_pImageInfoDlg->isHidden())
    {
        m_pImageInfoDlg->show();
        actionImageinfo->setChecked(true);
    }
    else
    {
        m_pImageInfoDlg->hide();
        actionImageinfo->setChecked(false);
    }
}

void Mainview::onImageInfoUpdate()
{
    if ((m_hCamera != IS_INVALID_HIDS) && m_pImageInfoDlg->isVisible())
    {
        // Only for Gigabit Ethernet cameras and USB3 CP Rev. 2
        bool bAdditionalImageInfo = false;

        if ((is_GetCameraType(m_hCamera) & IS_INTERFACE_TYPE_ETH) != 0)
        {
            bAdditionalImageInfo = true;
        }
        else if ((is_GetCameraType(m_hCamera) & IS_INTERFACE_TYPE_USB3) != 0)
        {
            int nSupportedModes = 0;
            is_DeviceFeature(m_hCamera, IS_DEVICE_FEATURE_CMD_GET_SUPPORTED_FEATURES,
                             &nSupportedModes, sizeof(nSupportedModes));

            if (nSupportedModes & IS_DEVICE_FEATURE_CAP_MEMORY_MODE)
            {
                bAdditionalImageInfo = true;
            }
        }

        if(bAdditionalImageInfo)
        {
            char *pLast = NULL;
            char *pMem  = NULL;
            int dummy = 0;

            // Get the active buffer
            if (is_GetActSeqBuf (m_hCamera, &dummy, &pMem, &pLast) == IS_SUCCESS)
            {
                if (pLast != NULL)
                {
                    int nImageID = _GetImageID( pLast );
                    UEYEIMAGEINFO ImageInfo;

                    if (is_GetImageInfo (m_hCamera, nImageID, &ImageInfo, sizeof (ImageInfo)) == IS_SUCCESS)
                    {
                        m_pImageInfoDlg->UpdateControls(&ImageInfo);
                    }
                }
            }
        }
    }
}


void Mainview::closeHistogram()
{
    actionHistogram->setChecked(false);
}

void Mainview::onHistogram(bool show)
{
    if (show)
    {
        if (m_pHistogramDlg == NULL)
        {
            m_pHistogramDlg = new Histogram(this);
        }
        m_pHistogramDlg->show();
    }
    else
    {
        if (m_pHistogramDlg != NULL)
        {
            m_pHistogramDlg->hide();
            delete m_pHistogramDlg;
            m_pHistogramDlg = NULL;
        }
    }
}

void Mainview::setMarkHotpixel(bool b)
{
    m_bMarkHotpixel = b;
    delete[] m_nHotpixelList;

    INT nNumber = 0;
    INT nRet = is_HotPixel(m_hCamera, IS_HOTPIXEL_GET_MERGED_CAMERA_LIST_NUMBER, (void*)&nNumber , sizeof(nNumber));
    if (nRet == IS_SUCCESS)
    {
        m_nHotpixelListSize = 1 + 2 * nNumber;
        m_nHotpixelList = new WORD[m_nHotpixelListSize];
        nRet = is_HotPixel(m_hCamera, IS_HOTPIXEL_GET_MERGED_CAMERA_LIST, (void*)m_nHotpixelList, m_nHotpixelListSize * sizeof(WORD));
        if (nRet != IS_SUCCESS)
        {
            m_nHotpixelListSize = 0;
            delete[] m_nHotpixelList;
        }
    }
}

INT Mainview::getCurrentAOI(IS_RECT* rectAOI)
{
    if (rectAOI != NULL)
    {
        return is_AOI(m_hCamera, IS_AOI_IMAGE_GET_AOI, (void*)rectAOI, sizeof(*rectAOI));
    }
    return IS_NO_SUCCESS;
}

DWORD Mainview::getMaxSensorWidth()
{
    return m_SensorInfo.nMaxWidth;
}

DWORD Mainview::getMaxSensorHeight()
{
    return m_SensorInfo.nMaxHeight;
}

bool Mainview::getMarkHotpixel()
{
    return m_bMarkHotpixel;
}

WORD* Mainview::getHotpixelList()
{
    return m_nHotpixelList;
}

UINT Mainview::getHotpixelListSize()
{
    return m_nHotpixelListSize;
}

void Mainview::eventreceived (int event)
{
    bool bUpdateCameraList = false;
    switch (event)
    {
    case IS_SET_EVENT_REMOVAL:
        printf ("device removed\n");
        bUpdateCameraList = true;
        break;
    case IS_SET_EVENT_NEW_DEVICE:
        printf ("device added\n");
        bUpdateCameraList = true;
        break;
    case IS_SET_EVENT_STATUS_CHANGED:
        printf ("cameralist changed\n");
        bUpdateCameraList = true;
        break;

    case IS_SET_EVENT_CAPTURE_STATUS:
        UpdateCounters();
        m_pImageInfoDlg->UpdateCounters();
        break;
    case IS_SET_EVENT_FRAME:
        //printf ("New Frame received\n");
        if (!m_hCamera)
        {
            break;
        }
        ProcessFrame ();
        break;
    case IS_SET_EVENT_PMC_IMAGE_PARAMS_CHANGED:
        {
            reOpenCamera();
        }
        break;
    case IS_SET_EVENT_DEVICE_RECONNECTED:
        {
            if (m_pImageInfoDlg)
            {
                m_pImageInfoDlg->IncrementReconnectCounter();
                m_pImageInfoDlg->UpdateCounters();
            }
        }
        break;
    default:
        break;
    }
    if ((pDlgCamerList != 0) && bUpdateCameraList)
        pDlgCamerList->Update ();
}

void Mainview::ClearStatusbarInfo()
{
    textStatus_4->clear();
    textStatus_3_0->clear();
    textStatus_3_1->clear();
    textStatus_3_2->clear();
    textStatus_2->clear();
}

void Mainview::UpdateFps (double fps)
{
    static double meanfps = 0.0;

    if (meanfps == 0.0)
        meanfps = fps;
    else
        meanfps = (meanfps * 15 + fps) / 16;

    textStatus_4->setText (QString (" FPS: %1").arg (meanfps, 0, 'f', 2));
}

void Mainview::UpdateCounters ()
{
    m_nAdaptiveHotpixelNumberDetected = 0;
    m_nAdaptiveHotpixelPercentDetected = 0.0;
    if (m_hCamera)
    {
        INT stateAdaptiveHotpixelCorrection = 0;

        is_HotPixel(m_hCamera, IS_HOTPIXEL_ADAPTIVE_CORRECTION_GET_ENABLE, (void*)&stateAdaptiveHotpixelCorrection, sizeof(stateAdaptiveHotpixelCorrection));
        if (stateAdaptiveHotpixelCorrection)
        {
            is_HotPixel(m_hCamera, IS_HOTPIXEL_ADAPTIVE_CORRECTION_GET_NUMBER_DETECTED, (void*)&m_nAdaptiveHotpixelNumberDetected, sizeof(m_nAdaptiveHotpixelNumberDetected));

            IS_RECT rectAOI;
            if (getCurrentAOI(&rectAOI) == IS_SUCCESS)
            {
                unsigned int area = std::max(rectAOI.s32Width * rectAOI.s32Height, 1);
                m_nAdaptiveHotpixelPercentDetected = static_cast<double>(m_nAdaptiveHotpixelNumberDetected) / static_cast<double>(area) * 100.0;
            }

            emit adaptiveHotpixelNumberDetectedChanged(m_nAdaptiveHotpixelNumberDetected);
            emit adaptiveHotpixelPercentDetectedChanged(m_nAdaptiveHotpixelPercentDetected);
        }
    }

    if (m_hCamera)
    {
        UEYE_CAPTURE_STATUS_INFO CaptureStatusInfo;
        INT nRet = is_CaptureStatus(m_hCamera,
                                    IS_CAPTURE_STATUS_INFO_CMD_GET,
                                    (void*)&CaptureStatusInfo,
                                    sizeof(CaptureStatusInfo)
                                   );

        if (nRet == IS_SUCCESS)
        {
            nFailed = CaptureStatusInfo.dwCapStatusCnt_Total;
        }
    }
    textStatus_3_0->setText (QString (" Frames: %1").arg (nReceived, 0, 10));
    textStatus_3_1->setText (QString (" Display: %1").arg (nDisplayed, 0, 10));
    textStatus_3_2->setText (QString (" Failed: %1").arg (nFailed, 0, 10));

    if (actionTrigger->isChecked())
    {
        /* External Trigger is used => show Trigger Missed cnt */
        textStatus_3_3->setVisible(true);
        ULONG nTriggerMissedCnt = is_CameraStatus(m_hCamera, IS_TRIGGER_MISSED, IS_GET_STATUS);
        textStatus_3_3->setText(QString (" Missed: %1").arg (nTriggerMissedCnt, 0, 10));
    }
    else
    {
        textStatus_3_3->setVisible(false);
    }
}

void Mainview::UpdateControls ()
{
    actionOpenCamera->setEnabled (m_hCamera == IS_INVALID_HIDS);
    actionOpenCameraStop->setEnabled (m_hCamera == IS_INVALID_HIDS);
    actionCloseCamera->setEnabled (m_hCamera != IS_INVALID_HIDS);

    if (m_bLive)
    {
        actionLive->setText ("Stop live");
        actionLive->setIcon(QIcon(":/new/prefix1/images/media_stop.png"));
        actionLive->setStatusTip ("stop live capturing camera");
        actionLive->setToolTip ("Stop live capture");
        actionLive->setChecked(true);
    }
    else
    {
        actionLive->setText ("Start live");
        actionLive->setIcon(QIcon(":/new/prefix1/images/media_play.png"));
        actionLive->setStatusTip ("start live capturing camera");
        actionLive->setToolTip ("Start live capture");
        actionLive->setChecked(false);
    }

    if (m_hCamera == IS_INVALID_HIDS)
    {
        actionLive->setEnabled (false);
        actionLive->setChecked (false);
    }
    else
    {
        /* live mode is not supported if camera is a XC and trigger mode is active */
        if (m_SensorInfo.SensorID != IS_SENSOR_UI1013XC || is_SetExternalTrigger(m_hCamera, IS_GET_EXTERNALTRIGGER) == IS_SET_TRIGGER_OFF)
        {
            actionLive->setEnabled (true);
        }
    }



    actionSnapshot->setEnabled ((m_hCamera != IS_INVALID_HIDS) && (!m_bLive));
    actionResetCamera->setEnabled (m_hCamera != IS_INVALID_HIDS);
    menuLoad_parameter->setEnabled (m_hCamera == IS_INVALID_HIDS);
    menuSave_parameter->setEnabled (m_hCamera == IS_INVALID_HIDS);
    actionSave_image->setEnabled (m_hCamera == IS_INVALID_HIDS);
    actionLoad_image->setEnabled (m_hCamera == IS_INVALID_HIDS);
    actionHistogram->setEnabled (m_hCamera == IS_INVALID_HIDS);

    /* 1013/1008 only support software trigger */
    if (m_SensorInfo.SensorID == IS_SENSOR_UI1013XC || m_SensorInfo.SensorID == IS_SENSOR_UI1008_C)
    {
        actionTrigger->setText("use SW trigger");
    }
    else
    {
        actionTrigger->setText("use HW trigger");
    }
    actionTrigger->setEnabled ((m_hCamera != IS_INVALID_HIDS) && (!m_bLive));
    actionProperties->setEnabled (m_hCamera == IS_INVALID_HIDS);

    actionDisplay->setEnabled (m_hCamera == IS_INVALID_HIDS);
    actionView_Fit->setEnabled (m_hCamera == IS_INVALID_HIDS);
    actionOriginal_size->setEnabled (m_hCamera == IS_INVALID_HIDS);
    actionHalf_size->setEnabled (m_hCamera == IS_INVALID_HIDS);
    actionQuarter_size->setEnabled (m_hCamera == IS_INVALID_HIDS);
    actionDouble_size->setEnabled (m_hCamera == IS_INVALID_HIDS);

    actionTrigger->setChecked ((m_hCamera != IS_INVALID_HIDS)
            && (is_SetExternalTrigger (m_hCamera, IS_GET_EXTERNALTRIGGER) != IS_SET_TRIGGER_OFF));

    if (m_hCamera == IS_INVALID_HIDS)
    {
        m_imageLabel->setScaledContents(false);
        m_imageLabel->setPixmap(QPixmap(":/new/prefix1/images/ueye_background_logo.png"));
        m_imageLabel->setAlignment(Qt::AlignCenter);
        m_scrollArea->setWidgetResizable(true);
    }
    else
    {
        m_imageLabel->setScaledContents(true);
    }
}

void Mainview::ProcessFrame ()
{
    INT dummy = 0;
    bool bDraw = true;
    double fps;
    char *pLast = NULL, *pMem = NULL;

    is_GetActSeqBuf (m_hCamera, &dummy, &pMem, &pLast);
    m_pLastBuffer = pLast;

    if (is_GetFramesPerSecond (m_hCamera, &fps) == IS_SUCCESS)
        UpdateFps (fps);
    nReceived++;
    UpdateCounters ();

    if (m_bReady)
    {
        m_bReady = FALSE;
        update();
        if (m_pLastBuffer && m_bDisplayImage)
        {
            int nTicks = 0;
            // Frame rate limit ?
            nTicks = m_Time.elapsed();

            if (bDraw)
            {
                nDisplayed++;
                m_Time.restart();
                DrawImage (m_pLastBuffer);
            }
        }
    }
}

INT Mainview::_GetImageID (char* pbuf)
{
    if (!pbuf)
        return 0;

    for (unsigned int i = 0; i < sizeof(m_Images) / sizeof(m_Images[0]); i++)
        if (m_Images[i].pBuf == pbuf)
            return m_Images[i].nImageID;

    return 0;
}

INT Mainview::_GetImageNum (char* pbuf)
{
    if (!pbuf)
        return 0;

    for (unsigned int i = 0; i < sizeof(m_Images) / sizeof(m_Images[0]); i++)
        if (m_Images[i].pBuf == pbuf)
            return m_Images[i].nImageSeqNum;

    return 0;
}

void Mainview::_FreeImages ()
{
    m_pLastBuffer = NULL;
    //printf ("freeing image buffers\n");
    for (unsigned int i = 0; i < sizeof(m_Images) / sizeof(m_Images[0]); i++)
    {
        if (m_Images[i].pBuf)
        {
            is_FreeImageMem (m_hCamera, m_Images[i].pBuf, m_Images[i].nImageID);
            m_Images[i].pBuf = NULL;
            m_Images[i].nImageID = 0;
        }
    }
}

void Mainview::_EmptyImages ()
{
    INT ret = IS_SUCCESS;
    m_pLastBuffer = NULL;
    for (unsigned int i = 0; i < sizeof(m_Images) / sizeof(m_Images[0]); i++)
    {
        if (m_Images[i].pBuf)
        {
            ret = is_LockSeqBuf (m_hCamera, m_Images[i].nImageSeqNum, m_Images[i].pBuf);
            ZeroMemory (m_Images[i].pBuf, m_Images[i].nBufferSize);
            ret |= is_UnlockSeqBuf (m_hCamera, m_Images[i].nImageSeqNum, m_Images[i].pBuf);
        }
    }
}

bool Mainview::_AllocImages ()
{
    m_pLastBuffer = NULL;
    int nWidth = 0;
    int nHeight = 0;

    UINT nAbsPosX;
    UINT nAbsPosY;

    m_imageLabel->clear(false);

    is_AOI(m_hCamera, IS_AOI_IMAGE_GET_POS_X_ABS, (void*)&nAbsPosX , sizeof(nAbsPosX));
    is_AOI(m_hCamera, IS_AOI_IMAGE_GET_POS_Y_ABS, (void*)&nAbsPosY , sizeof(nAbsPosY));

    is_ClearSequence (m_hCamera);
    for (unsigned int i = 0; i < sizeof(m_Images) / sizeof(m_Images[0]); i++)
    {
        if (m_Images[i].pBuf)
        {
            is_FreeImageMem (m_hCamera, m_Images[i].pBuf, m_Images[i].nImageID);
            m_Images[i].pBuf = NULL;
            m_Images[i].nImageID = 0;
        }

        nWidth = m_BufferProps.width;
        nHeight = m_BufferProps.height;

        if (nAbsPosX)
        {
            m_BufferProps.width = nWidth = m_SensorInfo.nMaxWidth;
        }
        if (nAbsPosY)
        {
            m_BufferProps.height = nHeight = m_SensorInfo.nMaxHeight;
        }

        if (is_AllocImageMem (m_hCamera, nWidth, nHeight, m_BufferProps.bitspp, &m_Images[i].pBuf,
                              &m_Images[i].nImageID) != IS_SUCCESS)
            return FALSE;
        if (is_AddToSequence (m_hCamera, m_Images[i].pBuf, m_Images[i].nImageID) != IS_SUCCESS)
            return FALSE;

        m_Images[i].nImageSeqNum = i + 1;
        m_Images[i].nBufferSize = nWidth * nHeight * m_BufferProps.bitspp / 8;
    }

    return TRUE;
}

bool Mainview::prepareWorkThread()
{
  if(!m_workThread) m_workThread = new WorkerThread(this);
  if(m_workThread->isRunning()) return false; // Not
  connect(m_workThread, SIGNAL(started()), this, SLOT(workThreadStarted()));
  connect(m_workThread, SIGNAL(finished()), this, SLOT(workThreadFinished()), Qt::QueuedConnection);
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  connect(m_workThread, SIGNAL(terminated()), this, SLOT(workThreadFinished()), Qt::QueuedConnection);
#endif
  return true;
}

void Mainview::workThreadStarted()
{
    // prepare the statusbar to show camera will be initialized

    switch(m_eWorkthreadJob)
    {
    case wj_cameraopen:
    case wj_cameraopenlive:
        m_pProgBar = new QProgressBar();
        if (m_bAutomaticSEStarterUpload == true)
        {
            m_pProgText = new QLabel("    upload firmware...");
            m_pProgBar->setRange(0,50);  // 25 secs
            m_pTimer->start(500);
        }
        else
        {
            m_pProgText = new QLabel("    open the camera...");
            m_pProgBar->setRange(0,0);
        }
        statusbar->addPermanentWidget(m_pProgText);
        m_pProgBar->setFixedHeight(14);
        m_pProgBar->setFixedWidth(150);
        m_pProgBar->setAccessibleDescription("init camera...");
        m_pProgBar->setTextVisible(true);
        statusbar->addPermanentWidget(m_pProgBar);
        break;
    case wj_camerareopen:
        break;
    default:
        break;
    }
}

void Mainview::workThreadFinished()
{
    int ret = IS_SUCCESS;
    statusbar->clearMessage();
    switch(m_eWorkthreadJob)
    {
    case wj_cameraopen:
    case wj_cameraopenlive:
        //enable camera property
        m_pPropertiesDlg = new properties(m_hCamera, m_CamInfo, m_SensorInfo, m_bLive, this);
        m_pTimer->stop();
        statusbar->removeWidget(m_pProgBar);
        statusbar->removeWidget(m_pProgText);
        if (m_pProgBar != NULL)
        {
            delete m_pProgBar;
            m_pProgBar = NULL;
        }
        ret = CameraInitialized();
        if (ret != IS_SUCCESS)
            break;

        m_bAutomaticSEStarterUpload = false;

        if (m_eWorkthreadJob==wj_cameraopenlive)
        {
            //start camera live mode
            onLive();
        }
        else
        {
            // take only a snapshot

            onSnapshot();
        }
        //enable camera property
        //m_pPropertiesDlg = new properties(m_hCamera, m_CamInfo, m_SensorInfo, m_bLive, this);

        break;
    case wj_camerareopen:
        m_pTimer->stop();

        //enable camera property
        m_pPropertiesDlg = new properties(m_hCamera, m_CamInfo, m_SensorInfo, m_bLive, this);
        ret = CameraInitialized();
        if (ret != IS_SUCCESS)
            break;

        m_bAutomaticSEStarterUpload = false;
        if (m_bReopenLive)
        {
            m_bLive = false;
            onLive();
            m_bReopenLive = false;
        }


        break;
    default:
        break;
    }
    //m_eWorkthreadJob = wj_none;
    m_workthreadReturn = 0;
}

void Mainview::run()
{
  switch(m_eWorkthreadJob)
  {
  case wj_cameraopen:
  case wj_cameraopenlive:
  case wj_camerareopen:
      // open the camera
      m_hCamera = (HIDS) (m_CamListInfo.dwDeviceID | IS_USE_DEVICE_ID); // open next camera
      if (m_bAutomaticSEStarterUpload)
      {
          m_hCamera |= IS_ALLOW_STARTER_FW_UPLOAD;
      }
      m_workthreadReturn = is_InitCamera (&m_hCamera, 0);

      break;
  default:
      break;
  }
}

void Mainview::updateProgress()
{
    if (m_pProgBar != NULL)
    {
        int progress = m_pProgBar->value();
        m_pProgBar->setValue(progress +1);
        if (progress >= 50)
            m_pTimer->stop();
    }
    else
        m_pTimer->stop();
}

void Mainview::initPassiveMulticast()
{
    BOOL enable = FALSE;

    int nRet = is_Multicast(0, IS_PMC_CMD_SYSTEM_GET_ENABLE, &enable, sizeof(enable));
    if(nRet == IS_SUCCESS)
    {
        if(enable == TRUE)
        {
            nRet = is_Multicast(0, IS_PMC_CMD_INITIALIZE, NULL, 0);
            if(nRet == IS_SUCCESS)
            {
                nRet = is_Multicast(0, IS_PMC_CMD_LOADDEVICES, NULL, 0);
            }
        }
    }
}
