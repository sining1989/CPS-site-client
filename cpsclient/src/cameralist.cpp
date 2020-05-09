/*!
 * \file    cameralist.cpp
 * \date    02.10.2018
 * \author  zhusizhi
 * \version 1.0.0
 * \version $Revision:$
 *
 * \brief   camera list widget implementation
 *
 */

#include "cameralist.h"
#include <QBrush>
#include <QHeaderView>
#include <QScrollBar>

CameraList::CameraList (QWidget *parent) :
    QDialog (parent)
{
    m_pCamList = 0;
    m_pTableWidget = 0;

    // init the internal camera info structure
    ZeroMemory (&m_CameraInfo, sizeof(UEYE_CAMERA_INFO));

    // get the cameralist from SDK
    InitCameraList ();
    // setup dialog data
    m_pBut = new QPushButton ("select camera..");
    m_pTableWidget = new QTableWidget (0, TABLE_COLUMNS);

    m_pLayout = new QVBoxLayout ();
    m_pLayout->addWidget (m_pTableWidget);
    m_pLayout->addWidget (m_pBut);

    setLayout (m_pLayout);

    // connect the button with slot
    connect (m_pBut, SIGNAL(clicked()), this, SLOT(OnClose()));
    connect (m_pTableWidget, SIGNAL(cellClicked(int,int)), m_pTableWidget, SLOT(selectRow(int)));
    connect (m_pTableWidget, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(OnClose()));

    // build the new camera list table
    BuildTable ();
    m_CameraInfo.dwDeviceID = -1;

    setWindowTitle ("cps camera list");
}

CameraList::~CameraList ()
{
    DeleteCameraList ();
}

bool CameraList::InitCameraList ()
{
    /*!
     * \par Retrieve the camera list from libueye_api.so
     *
     * According to the libueye_api documentation, camera list query is performed in two steps:
     * \li Query the number of available cameras by calling is_GetCameralist() with one
     * UEYE_CAMERA_LIST structure as argument whose member dwCount ist zero. The number of cameras
     * may then be retrieved from the data returned by is_GetCameralist() by reading
     * m_pCamList->dwCount.
     * \li Allocate the number of elements of type UEYE_CAMERA_LIST and call is_GetCameraList()
     * again according to the uEye SDK documentation. The number of elements to retrieve must be
     * given in member m_pCamList->dwCount of the first array element.
     */
    bool ret;
    DeleteCameraList ();
    m_pCamList = new UEYE_CAMERA_LIST;

    m_pCamList->dwCount = 0;

    if (is_GetCameraList (m_pCamList) == IS_SUCCESS)
    {
        DWORD dw = m_pCamList->dwCount;
        delete m_pCamList;

        // Reallocate the required camera list size
        m_pCamList = (PUEYE_CAMERA_LIST)new char[sizeof(DWORD) + dw * sizeof(UEYE_CAMERA_INFO)];
        m_pCamList->dwCount = dw;

        // Get CameraList and store it ...
        if (is_GetCameraList (m_pCamList) == IS_SUCCESS)
        {
            //SelectCamera (0);
            ret = true;
        }
        else
            ret = false;
    }
    else
        ret = false;

    return ret;
}

void CameraList::DeleteCameraList ()
{
    if (m_pCamList)
        delete m_pCamList;
    m_pCamList = 0;

    ZeroMemory (&m_CameraInfo, sizeof(UEYE_CAMERA_INFO));
}

unsigned int CameraList::Size ()
{
    return (unsigned int)m_pCamList->dwCount;
}

int CameraList::SelectCamera (unsigned int cam)
{
    if (cam >= m_pCamList->dwCount)
        return -1;
    // copy current camera info
    memcpy (&m_CameraInfo, &m_pCamList->uci[cam], sizeof(UEYE_CAMERA_INFO));
    return 0;
}

int CameraList::SelectCameraByID (unsigned int CamID)
{
    unsigned int i;

    for(i=0; i<Size(); i++)
    {
        if(m_pCamList->uci[i].dwDeviceID == CamID)
        {
            // copy current camera info
            memcpy (&m_CameraInfo, &m_pCamList->uci[i], sizeof(UEYE_CAMERA_INFO));
            return 0;
        }
    }
    return -1;
}

void CameraList::OnClose ()
{
    // store the selected camera ID
    int row = m_pTableWidget->currentRow ();
    SelectCamera (row);
    // close the dialog
    close ();
}

void CameraList::addRow (int items)
{
    int row = m_pTableWidget->rowCount ();
    m_pTableWidget->insertRow (row);

    for (int i = 0; i < items; i++)
    {
        QTableWidgetItem *item = new QTableWidgetItem ();
        item->setTextAlignment (Qt::AlignLeft | Qt::AlignVCenter);
        m_pTableWidget->setItem (row, i, item);
    }
}

void CameraList::Update ()
{
    // clean the table
    clearTable ();

    // init the CameraList;
    InitCameraList ();

    // rebuild the table
    BuildTable ();
}

void CameraList::clearTable ()
{
    m_pTableWidget->clear ();
    m_pTableWidget->setRowCount (0);

}

void CameraList::BuildTable ()
{
    if (m_pCamList == 0)
        return;

    // build new table
    m_pTableWidget->setHorizontalHeaderLabels (QStringList () << "Modell" << "ID" << "use"
            << "Serial");

    for (unsigned int row = 0; row < m_pCamList->dwCount; row++)
    {
        addRow (TABLE_COLUMNS);
        QString strUse = (m_pCamList->uci[row].dwInUse != 0)?"used":"free";
        m_pTableWidget->item (row, 0)->setText (QString (m_pCamList->uci[row].Model));
        m_pTableWidget->item (row, 1)->setText (QString::number (m_pCamList->uci[row].dwCameraID));
        m_pTableWidget->item (row, 2)->setText (strUse);
        if (m_pCamList->uci[row].dwInUse != 0)
        {
            QBrush brush (Qt::red);
            m_pTableWidget->item (row, 2)->setBackground (brush);
        }
        m_pTableWidget->item (row, 3)->setText (QString (m_pCamList->uci[row].SerNo));

    }
    // set default row
    m_pTableWidget->selectRow (0);
    m_pTableWidget->setSelectionBehavior(QTableView::SelectRows);
    m_pTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_pTableWidget->resizeColumnsToContents ();
    m_pTableWidget->horizontalHeader()->setStretchLastSection(true);
}
