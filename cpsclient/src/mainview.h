/*!
 * \file    mainview.h
 * \date    12.09.2018
 * \author  zhusizhi
 *
 * \brief   MainWindow class declaration of the Qt cps client
 *
 */

#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "ui_mainview.h"
#include "cps.h"
#include "properties.h"
#include "eventthread.h"
#include "cameralist.h"
#include "imageinfodlg.h"

#include "qtui.h"
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QImage>
#include <QPixmap>
#include <QString>
#include <QColor>
#include <QPainter>
#include <QRadioButton>
#include <QGroupBox>
#include <QSize>
#include <QPoint>
#include <QLayout>
#include <QThread>
#include <QMutex>
#include <QUrl>
#include <QRgb>
#include <QCloseEvent>
#include <QTimerEvent>
#include <QWidget>
#include <QProgressBar>
#include <QTimer>
#include <QTime>
#include <QScrollArea>
#include <QScrollBar>
#include <QVector>
#include "paintlabel.h"
#include "histogram.h"

class properties;
class PaintLabel;

class WorkerThread;

/*!
 * \defgroup MAINVIEW Mainview implementation
 * \{
 * \defgroup MAINVIEWCAMERAHANDLING cps camera handling
 * \defgroup MAINVIEWDIALOG Mainview dialog items
 * \defgroup EVENTHANDLING app eventhandling
 * \defgroup CPSEVENTHANDLING cps event handling
 * \defgroup WORKTRHEADHANDLING handling heavy duty work
 */

/*!
 * \brief Number of image buffer to alloc
 */
#define IMAGE_COUNT     5

/*!
 * \brief cps Image parameter structure
 */
typedef struct _UEYE_IMAGE
{
    char *pBuf;
    INT nImageID;
    INT nImageSeqNum;
    INT nBufferSize;
} UEYE_IMAGE, *PUEYE_IMAGE;

/*!
 * \brief supported display scale formats
 */
enum eDisplayMode
{
    display_no = 0, //!< display_no
    display_fit, //!< display_fit
    display_original,//!< display_original
    display_half, //!< display_half
    display_quarter, //!< display_quarter
    display_double
//!< display_double
};

/*!
 * \ingroup WORKTRHEADHANDLING
 * \brief worker thread job enumaration */
enum eWorkerJob
{
    wj_none = 0,
    wj_cameraopen = 1,
    wj_cameraopenlive = 2,
    wj_camerareopen
};

/*! \brief image buffer properties structure */
struct sBufferProps
{
    int width;
    int height;
    int colorformat;
    int bitspp;
    QImage::Format imgformat;
    QRgb *pRgbTable;
    int tableentries;
    int imageformat;

};

/*! \brief camera feature properties structure */
struct sCameraProps
{
    bool bUsesImageFormats;
    int nImgFmtNormal;
    int nImgFmtDefaultNormal;
    int nImgFmtTrigger;
    int nImgFmtDefaultTrigger;

};

/*!
 * \brief MainWindow class declaration
 */
class Mainview:public QMainWindow, public Ui::MainviewClass
{
Q_OBJECT

public:
    /*!
     * \brief standard constructor for the MainWindow object
     * \param parent        Qt parent object
     * \param strStartID    Select the device ID to open
     */
    Mainview (QWidget *parent = 0, QString strStartID = "");
    /*!
     * \brief standard destructor
     */
    virtual ~Mainview ();
    /*!
     * \ingroup WORKTRHEADHANDLING
     * \brief workerthread run function
     */
    void run();
    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief returns the number of an image buffer
     * \param pbuf image buffer pointer
     * \return according image number if successful, 0 otherwise
     */
    INT _GetImageNum (char* pbuf);

    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief frees all image buffers
     */
    void _FreeImages ();
    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief clears all image buffer contents
     */
    void _EmptyImages ();
    /*!
     * \brief returns the ID of an image buffer
     * \param pbuf image buffer pointer
     * \return according image ID if successful, 0 otherwise
     */
    INT _GetImageID (char* pbuf);
    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief creates all needed imagebuffers
     * \return  true if successful, false otherwise
     */
    bool _AllocImages ();
    /*!
     * \brief returns the curretn camera handle
     */
    HIDS getCameraHandle() { return m_hCamera; }
    /*! \brief returns the bits per pixel for each color format */
    int GetBitsPerPixel(int colormode);

    void startLive();
    void stopLive();

    WORD* getHotpixelList();
    UINT getHotpixelListSize();
    bool getMarkHotpixel();
    INT getCurrentAOI(IS_RECT* rectAOI);
    DWORD getMaxSensorWidth();
    DWORD getMaxSensorHeight();

signals:
    void adaptiveHotpixelNumberDetectedChanged(int);
    void adaptiveHotpixelPercentDetectedChanged(double);

    /*
     * public mainview class slots
     */
public slots:
    /*!
     * \brief shows the client about dialog
     */
    void about ();

    /*!
     * \ingroup CPSEVENTHANDLING
     * \brief cps event receive slot function
     * \param event been signaled
     */
    void eventreceived (int event);

    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief opens / closes the cps cameras
     */
    void onOpenCamera();
    void onOpenCameraLive();
    void onCloseCamera();

    /*! \brief setups the capture process */
    int SetupCapture();

    /*! \brief setup the Titles */
    void SetupTitles();

    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief starts/stops cps live capture
     */
    void onLive ();

    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief capture
     */
    void onSnapshot ();

    /*! \brief initialize new camera colormode and converter */
    //int ChangeColorFormat(int colormode, int colorconverter);

    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief save the actual cps image
     */
    void onSaveImage ();

    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief loads an image and display it
     */
    void onLoadImage();

    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief resets the camera to its default values
     */
    void onResetCamera();
    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief imports an camera parameter file
     * \param action triggered menu action
     */
    void onLoadParameter (QAction* action);

    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief exports the actual camera parameter to a file
     * \param action triggered menu action
     */
    void onSaveParameter (QAction* action);

    /*! ingroup MAINVIEWCAMERAHANDLING
     * \brief prepares the LoadParameter Menu
     */
    void onPrepareLoadParameter();

    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief shows the camera property dialog
     */
    void onProperties ();

    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief use external triggers for capture images
     */
    void onUseTrigger ();

    /*!
     * \brief changes the display scale format
     */
    void onDisplay ();

    /*!
     * \brief Enable/Disbale the API error report
     */
    void onApiErrorReport();

    /*!
     * \brief Show/hide the Image Info Dialog
     */
    void onImageInfo();

    /*!
     * \brief Imageinfo timeout slot
     */
    void onImageInfoUpdate();

    void closeHistogram();

    void onHistogram(bool show);

    void setMarkHotpixel(bool);

protected:
    void closeEvent (QCloseEvent * e);
    /*!
     * \ingroup EVENTHANDLING
     * \brief overloads parents 'paintEvent' function
     * \param e pointer to Qt paint event
     */
    void paintEvent (QPaintEvent *e);
    /*!
     * \ingroup MAINVIEWDIALOG
     * \brief clears the statusbar info
     */
    void ClearStatusbarInfo();
    /*!
     * \ingroup MAINVIEWDIALOG
     * \brief updates the fps display in the statusbar
     * \param fps new frames per second value
     */
    void UpdateFps (double fps);
    /*!
     * \ingroup MAINVIEWDIALOG
     * \brief updates the frame counter display in the statusbar
     */
    void UpdateCounters ();
    /*!
     * \ingroup MAINVIEWDIALOG
     * \brief updates the mainwindow control elements
     */
    void UpdateControls ();

private:
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    /*!
     * \ingroup MAINVIEWDIALOG
     * \brief creates the mainwindow status bar
     */
    void createStatusBar ();
    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief cps image process function
     */
    void ProcessFrame ();
    /*!
     * \brief cps image draw/display function
     * \param pBuffer pointer to image buffer to draw
     */
    void DrawImage (char *pBuffer);
    /*!
     * \ingroup WORKTRHEADHANDLING
     * \brief initialize the worker thread
     * \return true if success, otherwise false
     */
    bool prepareWorkThread();
    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief function to call when a camera was initialized through the work thread to
     *        finish the initializing work
     * \return ids error code (int)
     */
    int CameraInitialized();

    int openCamera (bool bStartLive);
    int reOpenCamera();

    int searchDefImageFormats(int suppportMask);
    /*!
     * \brief Saves the position and window states
     */
    void writeSettings();
    /*!
     * \brief Reeads the position and window states
     */
    void readSettings();
    /*!
     * \brief Initialize the Passive-Multicast
     */
    void initPassiveMulticast();
    /*
     * private mainview class slots
     */
private slots:
    /*!
     * \ingroup WORKTRHEADHANDLING
     * \brief callback slot when worker thread started
     */
    void workThreadStarted();
    /*!
     * \ingroup WORKTRHEADHANDLING
     * * \brief callback slot when worker thread stopped
     */
    void workThreadFinished();
    /*! \brief update progressbar slot */
    void updateProgress();

private:
    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief pointer on a camera list class object
     */
    CameraList *pDlgCamerList;
    /*!
     * \brief pointer on a imageinfodlg class object
     */
    imageinfodlg *m_pImageInfoDlg;
    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief cps frame buffer array
     */
    UEYE_IMAGE m_Images[IMAGE_COUNT];
    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief cps camera handle
     */
    HIDS m_hCamera;
    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief cps camera info structmember
     */
    CAMINFO m_CamInfo; // Camera (Board)info
    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief cps CameraList struct member
     */
    UEYE_CAMERA_INFO m_CamListInfo;
    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief cps sensor info struct member */
    SENSORINFO m_SensorInfo; // SensorInfo
    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief  holds the cps sensor width */
    int m_nWidth;
    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief holds the cps sensor height */
    int m_nHeight;
    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief holds the current display scale format */
    eDisplayMode m_eDispMode;
    bool m_bReady;
    char *m_pLastBuffer;

    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief used color mode */
    int m_nColorMode;
    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief Colorindex table for monochrom pictures */
    QRgb m_table[256];
    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \brief display images switch */
    bool m_bDisplayImage;

    /*!
     * \ingroup MAINVIEWCAMERAHANDLING
     * \defgroup CAPTURESTATUS capture status indicators
     * \{
     */
    bool m_bLive;               //!< \brief live indicator
    unsigned long nReceived;    //!< \brief current received frames
    unsigned long nFailed;      //!< \brief current count of failed transfers
    unsigned long nDisplayed;    //!< \brief current displayed images

    bool m_bReopenLive;            //!< \brief live indicator during reopen process
    /*!
     * \}
     */ /* end CAPTURESTATUS */

    /*!
     * \ingroup CPSEVENTHANDLING
     * \brief DEVICE_REMOVE Event waiting thread member
     */
    EventThread *m_pEvDevremove;
    /*!
     * \ingroup CPSEVENTHANDLING
     * \brief DEVICE_NEW Event waiting thread member
     */
    EventThread *m_pEvDevnew;
    /*!
     * \ingroup CPSEVENTHANDLING
     * \brief STATUS_CHANGED Event waiting thread member
     */
    EventThread *m_pEvDevStateChanged;
    /*!
     * \ingroup CPSEVENTHANDLING
     * \brief FRAME Event waiting thread member
     */
    EventThread *m_pEvFrame;
    /*!
     * \ingroup CPSEVENTHANDLING
     * \brief TRANSFER_FAILED Event waiting thread member
     */
    EventThread *m_pEvTransferFailed;

    /*!
     * \ingroup CPSEVENTHANDLING
     * \brief DEVICE_RECONNECTED Event waiting thread member
     */
    EventThread *m_pEvDeviceReconnected;

    /*!
     * \ingroup CPSEVENTHANDLING
     * \brief PMC Image Changed Event waiting thread member
     */
    EventThread *m_pEvPMCImageChanged;

    /*!
     * \ingroup MAINVIEWDIALOG
     * \brief pointer to the child view object
     */
    PaintLabel *m_imageLabel;
    QScrollArea *m_scrollArea;

    /*!
     * \ingroup MAINVIEWDIALOG
     * \defgroup STATUSBAR Status bar text labels
     * \brief statusbar text labels
     * \{
     */
    QLabel *textStatus;
    QLabel *textStatus_2;
    QLabel *textStatus_3_0;
    QLabel *textStatus_3_1;
    QLabel *textStatus_3_2;
    QLabel *textStatus_3_3;
    QLabel *textStatus_4;
    /*!
     * \}
     */ /* end STATUSBAR */

    /*!
     * \ingroup WORKTRHEADHANDLING
     * \brief workerthread jobnumber
     */
    int m_workthread_jobNumber;

    /*!
     * \ingroup WORKTRHEADHANDLING
     * \brief workerthread jobnumber
     */
    eWorkerJob m_eWorkthreadJob;

    /*!
         * \ingroup WORKTRHEADHANDLING
         * \brief workerthread return value
         */
    int m_workthreadReturn;

    /*!
     * \ingroup WORKTRHEADHANDLING
     * \brief worker thread member
     */
    WorkerThread *m_workThread;

    /*!
     * \ingroup WORKTRHEADHANDLING
     * \brief progress indicator
     */
    QProgressBar *m_pProgBar;

    /*!\ingroup WORKTRHEADHANDLING
     * \brief progress indicator text
     */
    QLabel *m_pProgText;

    /*! \brief flag to show if automatic fw upload is used */
    bool m_bAutomaticSEStarterUpload;

    /*! \brief misc timer */
    QTimer *m_pTimer;

    /*! \brief Property dialog */
    properties *m_pPropertiesDlg;

    /*! \brief misc time variable */
    QTime m_Time;


    Histogram* m_pHistogramDlg;

    bool m_bMarkHotpixel;
    WORD* m_nHotpixelList;
    UINT m_nHotpixelListSize;
    UINT m_nAdaptiveHotpixelNumberDetected = 0;
    double m_nAdaptiveHotpixelPercentDetected = 0;

public:
    struct sBufferProps m_BufferProps;
    struct sCameraProps m_CameraProps;
};


/*!
 * \brief workerthread class declaration
 */
class WorkerThread : public QThread
{
  Q_OBJECT
public:
  WorkerThread(Mainview *mainWindow)
    : mainWindow(mainWindow){};
  virtual ~WorkerThread(){};

  void run() {mainWindow->run();}
protected:
  Mainview *mainWindow;
};

#endif // MAINVIEW_H
