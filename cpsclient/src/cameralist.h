/*!
 * \file    cameralist.h
 * \date    26.09.2018
 * \author  zhusizhi
 * \version 1.0.0
 * \version $Revision:$
 *
 * \brief   camera list widget declaration
 *
 */
#ifndef CAMERALIST_H
#define CAMERALIST_H

#include "qtui.h"
#include <QTableWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include "cps.h"

/*!
 * \defgroup CAMERALIST Camera list implementation
 * \{
 * \defgroup CAMERAHANDLING Camera information
 * \defgroup CAMERALISTDIALOG Camera list dialog items
 */

/*!
 * \brief Number of camera list columns
 */
#define TABLE_COLUMNS 4
/*!
 * \brief Camera list widget control
 */
class CameraList : public QDialog
{
Q_OBJECT

public:
    /*!
     * \brief Constructor
     * \param parent Qt parent object
     */
    CameraList (QWidget *parent = 0);
    /*!
     * \brief Destructor
     */
    ~CameraList ();

    /*!
     * \brief Get number of cameras
     * \return  Number of cameras
     */
    unsigned int Size ();
    /*!
     * \brief Select a camera from the camera list
     * \param iCam   Camera index
     * \return  Zero if successful, nonzero otherwise
     */
    int SelectCamera (unsigned int iCam);
    /*!
     * \brief Select a camera from the camera list by her ID
     * \param CamID   CameraID
     * \return  Zero if successful and camera exist, nonzero otherwise
     */
    int SelectCameraByID (unsigned int CamID);
    /*!
     * \brief Get camera info of selected camera
     * \return  Camera info structure
     * \see CameraList::SelectCamera
     */
    UEYE_CAMERA_INFO GetCameraInfo ()
    {
        return m_CameraInfo;
    }

public slots:
    /*!
     *  \brief Camera list widget close slot
     */
    void OnClose ();
    /*!
     * \brief Camera list widget update slot
     */
    void Update ();

private:
    /*!
     * \brief Initialize camera list
     * \return  true if successful, false otherwise
     */
    bool InitCameraList ();
    /*!
     * \brief Delete camera list
     */
    void DeleteCameraList ();
    /*!
     * \brief Build camera information table
     */
    void BuildTable ();
    /*!
     * \brief Add table row
     * \param items Number of items
     */
    void addRow (int items);
    /*!
     * \brief Clear the table
     */
    void clearTable ();

    /*!
     * \ingroup CAMERAHANDLING
     * \brief Camera list array
     */
    PUEYE_CAMERA_LIST m_pCamList;
    /*!
     * \ingroup CAMERAHANDLING
     * \brief Current camera info
     */
    UEYE_CAMERA_INFO m_CameraInfo;

    /*!
     * \ingroup CAMERALISTDIALOG
     * \brief Qt table widget for camera list display
     */
    QTableWidget* m_pTableWidget;
    /*!
     * \ingroup CAMERALISTDIALOG
     * \brief Qt button widget
     */
    QPushButton* m_pBut;
    /*!
     * \ingroup CAMERALISTDIALOG
     * \brief Qt box layout widget
     */
    QVBoxLayout *m_pLayout;

};

#endif // CAMERALIST_H
