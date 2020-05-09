/*!
 * \file    imageinfodlg.h
 * \date    29.09.2018
 * \author  zhusizhi
 * \version 1.0.0
 *
 * \brief   additional image information dialog
 *
 */
#ifndef IMAGEINFODLG_H
#define IMAGEINFODLG_H

#include <QDialog>
#include "ui_imageinfodlg.h"
#include "cps.h"

class Mainview;

/*! \brief imageinfo dialog class declaration */
class imageinfodlg : public QDialog
{
    Q_OBJECT

public:
    /*! \brief std constructor */
    imageinfodlg(QWidget *parent = 0);
    /*! \brief std destructor */
    ~imageinfodlg();
    /*!\brief clears the dialog image info controls */
    void ClearControls ();

    void ResetReconnectCounter();
    void IncrementReconnectCounter();
public slots:
    /*! \brief resets the dialogs counter control values */
    void ResetCounters();
    /*! \brief updates the dialogs counter control with current values */
    void UpdateCounters();
    /*! \brief updates the dialogs image parameter controls
     *  \param pImageinfo pointer to a imageinfo struct */
    void UpdateControls(UEYEIMAGEINFO *pImageinfo);

signals:
    /*! \brief close signal for the parent */
    void close();

protected:
    /*! \brief overloads parents 'closeEvent' function
     *  \param e pointer to Qt close event */
    void closeEvent (QCloseEvent * e);
    /*! \brief overloads parents 'showEvent' function
     *  \param e pointer to Qt show event */
    void showEvent (QShowEvent * e);

private:
    /*! \brief dialogs GUI member class, build by Qt */
    Ui::imageinfodlgClass ui;
    /*! \brief MainView parent ref */
    Mainview* m_pParent;

    int m_nReconnects;
};

#endif // IMAGEINFODLG_H
