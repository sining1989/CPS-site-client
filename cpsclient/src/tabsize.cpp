/*!
 * \file    tabsize.cpp
 * \author  zhusizhi
 *
 * \brief   properties tab widget for camera size operations
 *
 */

#include "properties.h"

void properties::initTabSize()
{
    m_maxWidth = m_SensorInfo.nMaxWidth;
    m_minWidth = 64;
    m_incWidth = 16;
    m_nWidth = m_SensorInfo.nMaxWidth;
    m_maxHeight = m_SensorInfo.nMaxHeight;
    m_minHeight = 4;
    m_incHeight = 2;
    m_nHeight = m_SensorInfo.nMaxHeight;
    m_maxXPos = 0;
    m_minXPos = 0;
    m_incXPos = 0;
    m_nXPos = 0;
    m_maxYPos = 0;
    m_minYPos = 0;
    m_incYPos = 0;
    m_nYPos = 0;

    // create extended Size Sliders
    m_pSliderWidthEx = new QSliderEx(Qt::Horizontal, this, 500);
    m_pSliderWidthEx->setTracking(FALSE);
    layoutWidth->addWidget(m_pSliderWidthEx,0,0, 1, -1 );
    connect(m_pSliderWidthEx, SIGNAL(sliderMoved(int)), this, SLOT(OnSliderChangeImageWidth(int)));
    connect(m_pSliderWidthEx, SIGNAL(valueChangedEx(int)), this, SLOT(OnImageWidthChanged(int)));
    m_pSliderHeightEx = new QSliderEx(Qt::Horizontal, this, 500);
    m_pSliderHeightEx->setTracking(FALSE);
    layoutHeight->addWidget(m_pSliderHeightEx,0,0, 1, -1 );
    connect(m_pSliderHeightEx, SIGNAL(sliderMoved(int)), this, SLOT(OnSliderChangeImageHeigth(int)));
    connect(m_pSliderHeightEx, SIGNAL(valueChangedEx(int)), this, SLOT(OnImageHeightChanged(int)));
    m_pSliderPosXEx = new QSliderEx(Qt::Horizontal, this, 500);
    m_pSliderPosXEx->setTracking(FALSE);
    layoutPosX->addWidget(m_pSliderPosXEx,0,0, 1, -1 );
    connect(m_pSliderPosXEx, SIGNAL(sliderMoved(int)), this, SLOT(OnSliderChangeImageXPos(int)));
    connect(m_pSliderPosXEx, SIGNAL(valueChangedEx(int)), this, SLOT(OnImageXPosChanged(int)));
    m_pSliderPosYEx = new QSliderEx(Qt::Horizontal, this, 500);
    m_pSliderPosYEx->setTracking(FALSE);
    layoutPosY->addWidget(m_pSliderPosYEx,0,0, 1, -1 );
    connect(m_pSliderPosYEx, SIGNAL(sliderMoved(int)), this, SLOT(OnSliderChangeImageYPos(int)));
    connect(m_pSliderPosYEx, SIGNAL(valueChangedEx(int)), this, SLOT(OnImageYPosChanged(int)));

    // color keeping
    INT binningtype = is_SetBinning (m_hCamera, IS_GET_BINNING_TYPE);
    INT subsamplingtype = is_SetSubSampling (m_hCamera, IS_GET_SUBSAMPLING_TYPE);

    switch (binningtype)
    {
    case IS_BINNING_MONO:
        if (m_SensorInfo.nColorMode == IS_COLORMODE_BAYER)
        {
            textLabelBinningH->setText (textLabelBinningH->text () + " (M)");
            textLabelBinningV->setText (textLabelBinningV->text () + " (M)");
        }
        break;
    case IS_BINNING_COLOR:
        if (m_SensorInfo.nColorMode == IS_COLORMODE_MONOCHROME)
        {
            textLabelBinningH->setText (textLabelBinningH->text () + " (C)");
            textLabelBinningV->setText (textLabelBinningV->text () + " (C)");
        }
        break;
    default:
        break;
    }

    switch (subsamplingtype)
    {
    case IS_SUBSAMPLING_MONO:
        if (m_SensorInfo.nColorMode == IS_COLORMODE_BAYER)
        {
            textLabelSubsamplingH->setText (textLabelSubsamplingH->text () + " (M)");
            textLabelSubsamplingV->setText (textLabelSubsamplingV->text () + " (M)");
        }
        break;
    case IS_SUBSAMPLING_COLOR:
        if (m_SensorInfo.nColorMode == IS_COLORMODE_MONOCHROME)
        {
            textLabelSubsamplingH->setText (textLabelSubsamplingH->text () + " (C)");
            textLabelSubsamplingV->setText (textLabelSubsamplingV->text () + " (C)");
        }
        break;
    default:
        break;
    }



}

// -------------------  UpdateWindowSizeCtrls  -----------------------
//
void properties::UpdateWindowSizeCtrls ()
{

    if (m_bInit || !m_hCamera)
        return;

    QString strTemp;

    qDebug("Properties -> UpdateWindowSizeCtrls");

    // get values
    IS_RECT rectAOI;
    INT nRet = is_AOI(m_hCamera, IS_AOI_IMAGE_GET_AOI, (void*)&rectAOI, sizeof(rectAOI));

    if (nRet == IS_SUCCESS)
    {
      m_nXPos = rectAOI.s32X;
      m_nYPos = rectAOI.s32Y;
      m_nWidth = rectAOI.s32Width;
      m_nHeight = rectAOI.s32Height;
    }

    IS_SIZE_2D sizeTemp;

    is_AOI(m_hCamera, IS_AOI_IMAGE_GET_SIZE_MAX, (void*)&sizeTemp, sizeof(sizeTemp));
    m_maxWidth = sizeTemp.s32Width;
    m_maxHeight = sizeTemp.s32Height;

    is_AOI(m_hCamera, IS_AOI_IMAGE_GET_SIZE_MIN, (void*)&sizeTemp, sizeof(sizeTemp));
    m_minWidth = sizeTemp.s32Width;
    m_minHeight = sizeTemp.s32Height;

    is_AOI(m_hCamera, IS_AOI_IMAGE_GET_SIZE_INC, (void*)&sizeTemp, sizeof(sizeTemp));
    m_incWidth = sizeTemp.s32Width;
    m_incHeight = sizeTemp.s32Height;

    IS_POINT_2D pointTemp;

    is_AOI(m_hCamera, IS_AOI_IMAGE_GET_POS_MAX, (void*)&pointTemp, sizeof(pointTemp));
    m_maxXPos = pointTemp.s32X;
    m_maxYPos = pointTemp.s32Y;

    is_AOI(m_hCamera, IS_AOI_IMAGE_GET_POS_MIN, (void*)&pointTemp, sizeof(pointTemp));
    m_minXPos = pointTemp.s32X;
    m_minYPos = pointTemp.s32Y;

    is_AOI(m_hCamera, IS_AOI_IMAGE_GET_POS_INC, (void*)&pointTemp, sizeof(pointTemp));
    m_incXPos = pointTemp.s32X;
    m_incYPos = pointTemp.s32Y;

    // set text
    strTemp.setNum (m_minWidth, 10);
    textMinWidth->setText (strTemp);
    strTemp.setNum (m_maxWidth, 10);
    textMaxWidth->setText (strTemp);
    strTemp.setNum (m_minHeight, 10);
    textMinHeight->setText (strTemp);
    strTemp.setNum (m_maxHeight, 10);
    textMaxHeight->setText (strTemp);
    strTemp.setNum (m_minXPos, 10);
    textMinXPos->setText (strTemp);
    strTemp.setNum (m_maxXPos, 10);
    textMaxXPos->setText (strTemp);
    strTemp.setNum (m_minYPos, 10);
    textMinYPos->setText (strTemp);
    strTemp.setNum (m_maxYPos, 10);
    textMaxYPos->setText (strTemp);

    m_bInit = TRUE;
    // width
    m_pSliderWidthEx->EnableSignaling(false);
    m_pSliderWidthEx->setMinimum (m_minWidth);
    m_pSliderWidthEx->setMaximum (m_maxWidth);
    m_pSliderWidthEx->setSingleStep (m_incWidth);
    m_pSliderWidthEx->setPageStep (4* m_incWidth );
    m_pSliderWidthEx->setValue (m_nWidth);
    m_pSliderWidthEx->EnableSignaling(true);
    spinBoxWidth->setMinimum (m_minWidth);
    spinBoxWidth->setMaximum (m_maxWidth);
    spinBoxWidth->setSingleStep (m_incWidth);
    spinBoxWidth->setValue (m_nWidth);
    // height
    m_pSliderHeightEx->EnableSignaling(false);
    m_pSliderHeightEx->setMinimum (m_minHeight);
    m_pSliderHeightEx->setMaximum (m_maxHeight);
    m_pSliderHeightEx->setSingleStep (m_incHeight);
    m_pSliderHeightEx->setPageStep (4* m_incHeight );
    m_pSliderHeightEx->setValue (m_nHeight);
    m_pSliderHeightEx->EnableSignaling(true);
    spinBoxHeight->setMinimum (m_minHeight);
    spinBoxHeight->setMaximum (m_maxHeight);
    spinBoxHeight->setSingleStep (m_incHeight);
    spinBoxHeight->setValue (m_nHeight);
    // xpos
    m_pSliderPosXEx->EnableSignaling(false);
    m_pSliderPosXEx->setMinimum (m_minXPos);
    m_pSliderPosXEx->setMaximum (m_maxXPos);
    m_pSliderPosXEx->setSingleStep (m_incXPos);
    m_pSliderPosXEx->setPageStep (4* m_incXPos );
    m_pSliderPosXEx->setValue (m_nXPos);
    m_pSliderPosXEx->EnableSignaling(true);
    spinBoxXPos->setMinimum (m_minXPos);
    spinBoxXPos->setMaximum (m_maxXPos);
    spinBoxXPos->setSingleStep (m_incXPos);
    spinBoxXPos->setValue (m_nXPos);
    // ypos
    m_pSliderPosYEx->EnableSignaling(false);
    m_pSliderPosYEx->setMinimum (m_minYPos);
    m_pSliderPosYEx->setMaximum (m_maxYPos);
    m_pSliderPosYEx->setSingleStep (m_incYPos);
    m_pSliderPosYEx->setPageStep (4* m_incYPos );
    m_pSliderPosYEx->setValue (m_nYPos);
    m_pSliderPosYEx->EnableSignaling(true);
    spinBoxYPos->setMinimum (m_minYPos);
    spinBoxYPos->setMaximum (m_maxYPos);
    spinBoxYPos->setSingleStep (m_incYPos);
    spinBoxYPos->setValue (m_nYPos);

    /*! \note if camera uses IMAGE FORMATS instead of continuous AOI size  we have to disable the
     * normal size controls */
    INT nAOISupported = 0;
    INT ret = 0;
	if ((ret=is_ImageFormat(m_hCamera, IMGFRMT_CMD_GET_ARBITRARY_AOI_SUPPORTED, (void*)&nAOISupported,
					   sizeof(nAOISupported))) != IS_SUCCESS)
	{
		qDebug("ERROR: Properties -> UpdateWindowSizeCtrls: is_ImageFormat returns %d", ret);
	}
	if (nAOISupported == 0)
	{
		// width
		m_pSliderWidthEx->setEnabled (false);
		spinBoxWidth->setEnabled (false);
		// height
		m_pSliderHeightEx->setEnabled (false);
		spinBoxHeight->setEnabled (false);
		// xpos
		m_pSliderPosXEx->setEnabled (false);
		spinBoxXPos->setEnabled (false);
		// ypos
		m_pSliderPosYEx->setEnabled (false);
		spinBoxYPos->setEnabled (false);
	}
	else
	{
		// width
		m_pSliderWidthEx->setEnabled (m_maxWidth >= (m_minWidth + m_incWidth));
		spinBoxWidth->setEnabled (m_maxWidth >= (m_minWidth + m_incWidth));
		// height
		m_pSliderHeightEx->setEnabled (m_maxHeight >= (m_minHeight + m_incHeight));
		spinBoxHeight->setEnabled (m_maxHeight >= (m_minHeight + m_incHeight));
		// xpos
		m_pSliderPosXEx->setEnabled (m_maxXPos >= (m_minXPos + m_incXPos));
		spinBoxXPos->setEnabled (m_maxXPos >= (m_minXPos + m_incXPos));
		// ypos
		m_pSliderPosYEx->setEnabled (m_maxYPos >= (m_minYPos + m_incYPos));
		spinBoxYPos->setEnabled (m_maxYPos >= (m_minYPos + m_incYPos));
	}

    m_pDlgMainView->SetupTitles();
    m_bInit = FALSE;
}

#if 0
void properties::OnImageChanged()
{
    bool bSizeChanged = false;
    bool bPosChanged  = false;

    if (m_bInit || !m_hCamera)
        return;
    // check width and height
    int width  = sliderWidth->value();
    int height = sliderHeight->value();
    int xpos   = sliderXPos->value();
    int ypos   = sliderYPos->value();

    /* to be sure that all values are only incremented valid uEye steps,
     * we change the user value to next lower valid increment value.
     * After that we check MIN/MAX values */
    // check width
    width -= width % m_incWidth;
    width = (width < m_minWidth) ? m_minWidth : width;
    width = (width > m_maxWidth) ? m_maxWidth : width;
    // check height
    height -= height % m_incHeight;
    height = (height < m_minHeight) ? m_minHeight : height;
    height = (height > m_maxHeight) ? m_maxHeight : height;
    // check xpos
    xpos -= xpos % m_incXPos;
    xpos = (xpos < m_minXPos) ? m_minXPos : xpos;
    xpos = (xpos > m_maxXPos) ? m_maxXPos : xpos;
    // check ypos
    ypos -= ypos % m_incYPos;
    ypos = (ypos < m_minYPos) ? m_minYPos : ypos;
    ypos = (ypos > m_maxYPos) ? m_maxYPos : ypos;

    // compare old with new values if changed
    if ((width != m_nWidth) || (height != m_nHeight))
        bSizeChanged = true;
    if ((xpos != m_nXPos) || (ypos != m_nYPos))
        bPosChanged = true;

    if(bSizeChanged || bPosChanged)
    {
        // stop live if needed
        if (m_bLive)
            is_StopLiveVideo (m_hCamera, IS_WAIT);
        // set new image size and position
        is_SetAOI (m_hCamera, IS_SET_IMAGE_AOI, &xpos, &ypos, &width, &height);

        if (bSizeChanged)
        {
            // change image buffers if needed
            m_pDlgMainView->ChangeImageSize(width, height);
        }
        // restart live if needed
        if (m_bLive)
            is_CaptureVideo (m_hCamera, IS_DONT_WAIT);
        // Update Size controls
        UpdateWindowSizeCtrls ();
    }
}
#endif

void properties::OnSliderChangeImageWidth (int value)
{
    spinBoxWidth->blockSignals(true);
    value -= value % m_incWidth;
    spinBoxWidth->setValue(value);
}

void properties::OnImageWidthChanged (int value)
{
    spinBoxWidth->blockSignals(false);

    if (m_bInit || !m_hCamera)
        return;

    qDebug("Properties -> OnImageWidthChanged");
    // to be sure that all values are only incremented
    // valid uEye steps, we change the user value to next lower
    // valid increment value.
    value -= value % m_incWidth;

    // check value min/max range
    value = (value < m_minWidth) ? m_minWidth : value;
    value = (value > m_maxWidth) ? m_maxWidth : value;

    if (m_bLive)
        is_StopLiveVideo (m_hCamera, IS_WAIT);

    IS_RECT rect;
    rect.s32X = m_nXPos;
    rect.s32Y = m_nYPos;
    rect.s32Width = value;
    rect.s32Height = m_nHeight;

    is_AOI(m_hCamera, IS_AOI_IMAGE_SET_AOI, (void*)&rect, sizeof(rect));

    m_pDlgMainView->SetupCapture();

    if (m_bLive)
        is_CaptureVideo (m_hCamera, IS_DONT_WAIT);

    UpdateWindowSizeCtrls ();
}

void properties::OnSliderChangeImageHeigth (int value)
{
    spinBoxHeight->blockSignals(true);
    value -= value % m_incHeight;
    spinBoxHeight->setValue(value);
}

void properties::OnImageHeightChanged (int value)
{
    spinBoxHeight->blockSignals(false);

    if (m_bInit || !m_hCamera)
        return;

    qDebug("Properties -> OnImageHeightChanged");
    /* to be sure that all values are only incremented
     * valid uEye steps, we change the user value to next lower
     * valid increment value. */
    value -= value % m_incHeight;
    /* check value min/max range  */
    value = (value < m_minHeight) ? m_minHeight : value;
    value = (value > m_maxHeight) ? m_maxHeight : value;

    if (m_bLive)
        is_StopLiveVideo (m_hCamera, IS_WAIT);

    IS_RECT rect;
    rect.s32X = m_nXPos;
    rect.s32Y = m_nYPos;
    rect.s32Width = m_nWidth;
    rect.s32Height = value;

    is_AOI(m_hCamera, IS_AOI_IMAGE_SET_AOI, (void*)&rect, sizeof(rect));

    m_pDlgMainView->SetupCapture();

    if (m_bLive)
        is_CaptureVideo (m_hCamera, IS_DONT_WAIT);

    UpdateWindowSizeCtrls ();
}

void properties::OnSliderChangeImageXPos (int value)
{
    spinBoxXPos->blockSignals(true);
    value -= value % m_incXPos;
    spinBoxXPos->setValue(value);
}

void properties::OnImageXPosChanged (int value)
{
    spinBoxXPos->blockSignals(false);

    if (m_bInit || !m_hCamera)
        return;

    qDebug("Properties -> OnImageXPosChanged");
    /* to be sure that all values are only incremented
     * valid uEye steps, we change the user value to next lower
     * valid increment value. */
    value -= value % m_incXPos;

    value = (value < m_minXPos) ? m_minXPos : value;
    value = (value > m_maxXPos) ? m_maxXPos : value;

    if (m_bLive)
        is_StopLiveVideo (m_hCamera, IS_WAIT);

    IS_RECT rect;
    rect.s32X = value;
    rect.s32Y = m_nYPos;
    rect.s32Width = m_nWidth;
    rect.s32Height = m_nHeight;

    is_AOI(m_hCamera, IS_AOI_IMAGE_SET_AOI, (void*)&rect, sizeof(rect));

    if (m_bLive)
        is_CaptureVideo (m_hCamera, IS_DONT_WAIT);

    UpdateWindowSizeCtrls ();
}

void properties::OnSliderChangeImageYPos (int value)
{
    spinBoxYPos->blockSignals(true);
    value -= value % m_incYPos;
    spinBoxYPos->setValue(value);
}

void properties::OnImageYPosChanged (int value)
{
    spinBoxYPos->blockSignals(false);

    if (m_bInit || !m_hCamera)
        return;

    qDebug("Properties -> OnImageYPosChanged");
    /* to be sure that all values are only incremented
     * valid uEye steps, we change the user value to next lower
     * valid increment value. */
    value -= value % m_incYPos;

    value = (value < m_minYPos) ? m_minYPos : value;
    value = (value > m_maxYPos) ? m_maxYPos : value;

    if (m_bLive)
        is_StopLiveVideo (m_hCamera, IS_WAIT);

    IS_RECT rect;
    rect.s32X = m_nXPos;
    rect.s32Y = value;
    rect.s32Width = m_nWidth;
    rect.s32Height = m_nHeight;

    is_AOI(m_hCamera, IS_AOI_IMAGE_SET_AOI, (void*)&rect, sizeof(rect));

    if (m_bLive)
        is_CaptureVideo (m_hCamera, IS_DONT_WAIT);

    UpdateWindowSizeCtrls ();
}

int properties::getSizeFactor(int *pVFactor, int *pHFactor)
{
    // get vertical size factor
    INT mode = is_SetBinning (m_hCamera, IS_GET_BINNING);
    if (mode & IS_BINNING_2X_VERTICAL)
    {
        *pVFactor = 2;
    }
    else if (mode & IS_BINNING_3X_VERTICAL)
    {
        *pVFactor = 3;
    }
    else if (mode & IS_BINNING_4X_VERTICAL)
    {
        *pVFactor = 4;
    }
    else if (mode & IS_BINNING_6X_VERTICAL)
    {
        *pVFactor = 6;
    }
    else if (mode & IS_BINNING_8X_VERTICAL)
    {
        *pVFactor = 8;
    }
    else
    {
        mode = is_SetSubSampling (m_hCamera, IS_GET_SUBSAMPLING);
        if (mode & IS_SUBSAMPLING_2X_VERTICAL)
        {
            *pVFactor = 2;
        }
        else if (mode & IS_SUBSAMPLING_3X_VERTICAL)
        {
            *pVFactor = 3;
        }
        else if (mode & IS_SUBSAMPLING_4X_VERTICAL)
        {
            *pVFactor = 4;
        }
        else if (mode & IS_SUBSAMPLING_5X_VERTICAL)
        {
            *pVFactor = 5;
        }
        else if (mode & IS_SUBSAMPLING_6X_VERTICAL)
        {
            *pVFactor = 6;
        }
        else if (mode & IS_SUBSAMPLING_8X_VERTICAL)
        {
            *pVFactor = 8;
        }
        else if (mode & IS_SUBSAMPLING_16X_VERTICAL)
        {
            *pVFactor = 16;
        }
        else
        {
            *pVFactor = 1;
        }
    }

    // get horizontal size factor
    mode = is_SetBinning (m_hCamera, IS_GET_BINNING);
    if (mode & IS_BINNING_2X_HORIZONTAL)
    {
        *pHFactor = 2;
    }
    else if (mode & IS_BINNING_3X_HORIZONTAL)
    {
        *pHFactor = 3;
    }
    else if (mode & IS_BINNING_4X_HORIZONTAL)
    {
        *pHFactor = 4;
    }
    else if (mode & IS_BINNING_6X_HORIZONTAL)
    {
        *pHFactor = 6;
    }
    else if (mode & IS_BINNING_8X_HORIZONTAL)
    {
        *pHFactor = 8;
    }
    else
    {
        mode = is_SetSubSampling (m_hCamera, IS_GET_SUBSAMPLING);
        if (mode & IS_SUBSAMPLING_2X_HORIZONTAL)
        {
            *pHFactor = 2;
        }
        else if (mode & IS_SUBSAMPLING_3X_HORIZONTAL)
        {
            *pHFactor = 3;
        }
        else if (mode & IS_SUBSAMPLING_4X_HORIZONTAL)
        {
            *pHFactor = 4;
        }
        else if (mode & IS_SUBSAMPLING_5X_HORIZONTAL)
        {
            *pHFactor = 5;
        }
        else if (mode & IS_SUBSAMPLING_6X_HORIZONTAL)
        {
            *pHFactor = 6;
        }
        else if (mode & IS_SUBSAMPLING_8X_HORIZONTAL)
        {
            *pHFactor = 8;
        }
        else if (mode & IS_SUBSAMPLING_16X_HORIZONTAL)
        {
            *pHFactor = 16;
        }
        else
        {
            *pHFactor = 1;
        }
    }
    qDebug("Properties -> getSizeFactor: size vFactor: %d, hFactor: %d", *pVFactor, *pHFactor);
    return 0;
}

void properties::UpdateSizeXSCtrls()
{
    // flip up down
    BOOL bChecked = is_SetRopEffect (m_hCamera, IS_GET_ROP_EFFECT, 0, 0) & IS_SET_ROP_MIRROR_UPDOWN;
    cB_Mirror_ud->setChecked (bChecked);
    // flip left right
    bChecked = is_SetRopEffect (m_hCamera, IS_GET_ROP_EFFECT, 0, 0) & IS_SET_ROP_MIRROR_LEFTRIGHT;
    cB_Mirror_lr->setChecked (bChecked);

}
void properties::UpdateSizeFormatCtrls ()
{
    if (m_bInit || !m_hCamera)
        return;

    qDebug("Properties -> UpdateSizeFormatCtrls");
    // get values
    INT binningMode = is_SetBinning (m_hCamera, IS_GET_BINNING);
    INT binningSupp = is_SetBinning (m_hCamera, IS_GET_SUPPORTED_BINNING);
    INT subsamplingMode = is_SetSubSampling (m_hCamera, IS_GET_SUBSAMPLING);
    INT subsamplingSupp = is_SetSubSampling (m_hCamera, IS_GET_SUPPORTED_SUBSAMPLING);

    m_bInit = TRUE;
    // horizontal format
    radioButtonHNormal->setChecked (((binningMode & IS_BINNING_MASK_HORIZONTAL)
            == IS_BINNING_DISABLE) && ((subsamplingMode & IS_SUBSAMPLING_MASK_HORIZONTAL)
            == IS_SUBSAMPLING_DISABLE));
    radioButton2xHBinning->setChecked (binningMode & IS_BINNING_2X_HORIZONTAL);
    radioButton3xHBinning->setChecked (binningMode & IS_BINNING_3X_HORIZONTAL);
    radioButton4xHBinning->setChecked (binningMode & IS_BINNING_4X_HORIZONTAL);
    radioButton5xHBinning->setChecked (binningMode & IS_BINNING_5X_HORIZONTAL);
    radioButton6xHBinning->setChecked (binningMode & IS_BINNING_6X_HORIZONTAL);
    radioButton8xHBinning->setChecked (binningMode & IS_BINNING_8X_HORIZONTAL);
    radioButton2xHSubsampling->setChecked (subsamplingMode & IS_SUBSAMPLING_2X_HORIZONTAL);
    radioButton3xHSubsampling->setChecked (subsamplingMode & IS_SUBSAMPLING_3X_HORIZONTAL);
    radioButton4xHSubsampling->setChecked (subsamplingMode & IS_SUBSAMPLING_4X_HORIZONTAL);
    radioButton5xHSubsampling->setChecked (subsamplingMode & IS_SUBSAMPLING_5X_HORIZONTAL);
    radioButton6xHSubsampling->setChecked (subsamplingMode & IS_SUBSAMPLING_6X_HORIZONTAL);
    radioButton8xHSubsampling->setChecked (subsamplingMode & IS_SUBSAMPLING_8X_HORIZONTAL);
    radioButton16xHSubsampling->setChecked (subsamplingMode & IS_SUBSAMPLING_16X_HORIZONTAL);
    // vertical format
    radioButtonVNormal->setChecked (
                                    ((binningMode & IS_BINNING_MASK_VERTICAL) == IS_BINNING_DISABLE)
                                            && ((subsamplingMode & IS_SUBSAMPLING_MASK_VERTICAL)
                                                    == IS_SUBSAMPLING_DISABLE));
    radioButton2xVBinning->setChecked (binningMode & IS_BINNING_2X_VERTICAL);
    radioButton3xVBinning->setChecked (binningMode & IS_BINNING_3X_VERTICAL);
    radioButton4xVBinning->setChecked (binningMode & IS_BINNING_4X_VERTICAL);
    radioButton5xVBinning->setChecked (binningMode & IS_BINNING_5X_VERTICAL);
    radioButton6xVBinning->setChecked (binningMode & IS_BINNING_6X_VERTICAL);
    radioButton8xVBinning->setChecked (binningMode & IS_BINNING_8X_VERTICAL);
    radioButton2xVSubsampling->setChecked (subsamplingMode & IS_SUBSAMPLING_2X_VERTICAL);
    radioButton3xVSubsampling->setChecked (subsamplingMode & IS_SUBSAMPLING_3X_VERTICAL);
    radioButton4xVSubsampling->setChecked (subsamplingMode & IS_SUBSAMPLING_4X_VERTICAL);
    radioButton5xVSubsampling->setChecked (subsamplingMode & IS_SUBSAMPLING_5X_VERTICAL);
    radioButton6xVSubsampling->setChecked (subsamplingMode & IS_SUBSAMPLING_6X_VERTICAL);
    radioButton8xVSubsampling->setChecked (subsamplingMode & IS_SUBSAMPLING_8X_VERTICAL);
    radioButton16xVSubsampling->setChecked (subsamplingMode & IS_SUBSAMPLING_16X_VERTICAL);

    // horizontal format
    radioButton2xHBinning->setEnabled (binningSupp & IS_BINNING_2X_HORIZONTAL);
    radioButton3xHBinning->setEnabled (binningSupp & IS_BINNING_3X_HORIZONTAL);
    radioButton4xHBinning->setEnabled (binningSupp & IS_BINNING_4X_HORIZONTAL);
    radioButton5xHBinning->setEnabled (binningSupp & IS_BINNING_5X_HORIZONTAL);
    radioButton6xHBinning->setEnabled (binningSupp & IS_BINNING_6X_HORIZONTAL);
    radioButton8xHBinning->setEnabled (binningSupp & IS_BINNING_8X_HORIZONTAL);
    radioButton2xHSubsampling->setEnabled (subsamplingSupp & IS_SUBSAMPLING_2X_HORIZONTAL);
    radioButton3xHSubsampling->setEnabled (subsamplingSupp & IS_SUBSAMPLING_3X_HORIZONTAL);
    radioButton4xHSubsampling->setEnabled (subsamplingSupp & IS_SUBSAMPLING_4X_HORIZONTAL);
    radioButton5xHSubsampling->setEnabled (subsamplingSupp & IS_SUBSAMPLING_5X_HORIZONTAL);
    radioButton6xHSubsampling->setEnabled (subsamplingSupp & IS_SUBSAMPLING_6X_HORIZONTAL);
    radioButton8xHSubsampling->setEnabled (subsamplingSupp & IS_SUBSAMPLING_8X_HORIZONTAL);
    radioButton16xHSubsampling->setEnabled (subsamplingSupp & IS_SUBSAMPLING_16X_HORIZONTAL);
    // vertical format
    radioButton2xVBinning->setEnabled (binningSupp & IS_BINNING_2X_VERTICAL);
    radioButton3xVBinning->setEnabled (binningSupp & IS_BINNING_3X_VERTICAL);
    radioButton4xVBinning->setEnabled (binningSupp & IS_BINNING_4X_VERTICAL);
    radioButton5xVBinning->setEnabled (binningSupp & IS_BINNING_5X_VERTICAL);
    radioButton6xVBinning->setEnabled (binningSupp & IS_BINNING_6X_VERTICAL);
    radioButton8xVBinning->setEnabled (binningSupp & IS_BINNING_8X_VERTICAL);
    radioButton2xVSubsampling->setEnabled (subsamplingSupp & IS_SUBSAMPLING_2X_VERTICAL);
    radioButton3xVSubsampling->setEnabled (subsamplingSupp & IS_SUBSAMPLING_3X_VERTICAL);
    radioButton4xVSubsampling->setEnabled (subsamplingSupp & IS_SUBSAMPLING_4X_VERTICAL);
    radioButton5xVSubsampling->setEnabled (subsamplingSupp & IS_SUBSAMPLING_5X_VERTICAL);
    radioButton6xVSubsampling->setEnabled (subsamplingSupp & IS_SUBSAMPLING_6X_VERTICAL);
    radioButton8xVSubsampling->setEnabled (subsamplingSupp & IS_SUBSAMPLING_8X_VERTICAL);
    radioButton16xVSubsampling->setEnabled (subsamplingSupp & IS_SUBSAMPLING_16X_VERTICAL);

    // get size factors
    getSizeFactor(&m_vFactor, &m_hFactor);

    // flip up down
    BOOL bChecked = is_SetRopEffect (m_hCamera, IS_GET_ROP_EFFECT, 0, 0) & IS_SET_ROP_MIRROR_UPDOWN;
    checkBoxUpDown->setChecked (bChecked);
    // flip left right
    bChecked = is_SetRopEffect (m_hCamera, IS_GET_ROP_EFFECT, 0, 0) & IS_SET_ROP_MIRROR_LEFTRIGHT;
    checkBoxLeftRight->setChecked (bChecked);

    m_bInit = FALSE;
}

void properties::OnRadioHFormat ()
{
    if (m_bInit || !m_hCamera)
        return;

    INT binningMode = is_SetBinning (m_hCamera, IS_GET_BINNING) & IS_BINNING_MASK_VERTICAL;
    INT subsamplingMode = is_SetSubSampling (m_hCamera, IS_GET_SUBSAMPLING)
            & IS_SUBSAMPLING_MASK_VERTICAL;
    bool bBinning = false;
    bool bSubsampling = false;
    int  hFactor = 1;
    int  nMode = 0;

    if (radioButton2xHBinning->isChecked ())
    {
        bBinning = true;
        nMode = IS_BINNING_2X_HORIZONTAL;
        hFactor = 2;
    }
    else if (radioButton3xHBinning->isChecked ())
    {
        bBinning = true;
        nMode = IS_BINNING_3X_HORIZONTAL;
        hFactor = 3;
    }
    else if (radioButton4xHBinning->isChecked ())
    {
        bBinning = true;
        nMode = IS_BINNING_4X_HORIZONTAL;
        hFactor = 4;
    }
    else if (radioButton5xHBinning->isChecked ())
    {
        bBinning = true;
        nMode = IS_BINNING_5X_HORIZONTAL;
        hFactor = 5;
    }
    else if (radioButton6xHBinning->isChecked ())
    {
        bBinning = true;
        nMode = IS_BINNING_6X_HORIZONTAL;
        hFactor = 6;
    }
    else if (radioButton8xHBinning->isChecked ())
    {
        bBinning = true;
        nMode = IS_BINNING_8X_HORIZONTAL;
        hFactor = 8;
    }
    else if (radioButton2xHSubsampling->isChecked ())
    {
        bSubsampling = true;
        nMode = IS_SUBSAMPLING_2X_HORIZONTAL;
        hFactor = 2;
    }
    else if (radioButton3xHSubsampling->isChecked ())
    {
        bSubsampling = true;
        nMode = IS_SUBSAMPLING_3X_HORIZONTAL;
        hFactor = 3;
    }
    else if (radioButton4xHSubsampling->isChecked ())
    {
        bSubsampling = true;
        nMode = IS_SUBSAMPLING_4X_HORIZONTAL;
        hFactor = 4;
    }
    else if (radioButton5xHSubsampling->isChecked ())
    {
        bSubsampling = true;
        nMode = IS_SUBSAMPLING_5X_HORIZONTAL;
        hFactor = 5;
    }
    else if (radioButton6xHSubsampling->isChecked ())
    {
        bSubsampling = true;
        nMode = IS_SUBSAMPLING_6X_HORIZONTAL;
        hFactor = 6;
    }
    else if (radioButton8xHSubsampling->isChecked ())
    {
        bSubsampling = true;
        nMode = IS_SUBSAMPLING_8X_HORIZONTAL;
        hFactor = 8;
    }
    else if (radioButton16xHSubsampling->isChecked ())
    {
        bSubsampling = true;
        nMode = IS_SUBSAMPLING_16X_HORIZONTAL;
        hFactor = 16;
    }
    else
    {
        bSubsampling = true;
        bBinning = true;
        nMode = 0;
        hFactor = 1;
    }

    if (m_bLive)
        is_StopLiveVideo (m_hCamera, IS_WAIT);

    if (bBinning)
    {
        ApplyBinningSettings(binningMode, nMode);
    }

    if (bSubsampling)
    {
        ApplySubsamplingSettings(subsamplingMode, nMode);
    }

    UpdateSizeFormatCtrls ();
    UpdateWindowSizeCtrls ();

    m_pDlgMainView->SetupCapture();
    //m_pDlgMainView->ChangeImageSize(m_nWidth, m_nHeight);

    if (m_bLive)
        is_CaptureVideo (m_hCamera, IS_DONT_WAIT);

    // apply max exposure
    if (m_bMaxExposure)
    {
        double dblValue = 0.0;
        is_Exposure(m_hCamera, IS_EXPOSURE_CMD_SET_EXPOSURE, (void*)&dblValue, sizeof(dblValue));
    }
}

void properties::OnRadioVFormat ()
{
    if (m_bInit || !m_hCamera)
        return;

    INT binningMode = is_SetBinning (m_hCamera, IS_GET_BINNING) & IS_BINNING_MASK_HORIZONTAL;
    INT subsamplingMode = is_SetSubSampling (m_hCamera, IS_GET_SUBSAMPLING)
            & IS_SUBSAMPLING_MASK_HORIZONTAL;
    bool bBinning = false;
    bool bSubsampling = false;
    int  vFactor = 1;
    int  nMode = 0;

    if (radioButton2xVBinning->isChecked ())
    {
        bBinning = true;
        nMode = IS_BINNING_2X_VERTICAL;
        vFactor = 2;
    }
    else if (radioButton3xVBinning->isChecked ())
    {
        bBinning = true;
        nMode = IS_BINNING_3X_VERTICAL;
        vFactor = 3;
    }
    else if (radioButton4xVBinning->isChecked ())
    {
        bBinning = true;
        nMode = IS_BINNING_4X_VERTICAL;
        vFactor = 4;
    }
    else if (radioButton5xVBinning->isChecked ())
    {
        bBinning = true;
        nMode = IS_BINNING_5X_VERTICAL;
        vFactor = 5;
    }
    else if (radioButton6xVBinning->isChecked ())
    {
        bBinning = true;
        nMode = IS_BINNING_6X_VERTICAL;
        vFactor = 6;
    }
    else if (radioButton8xVBinning->isChecked ())
    {
        bBinning = true;
        nMode = IS_BINNING_8X_VERTICAL;
        vFactor = 8;
    }
    else if (radioButton2xVSubsampling->isChecked ())
    {
        bSubsampling = true;
        nMode = IS_SUBSAMPLING_2X_VERTICAL;
        vFactor = 2;
    }
    else if (radioButton3xVSubsampling->isChecked ())
    {
        bSubsampling = true;
        nMode = IS_SUBSAMPLING_3X_VERTICAL;
        vFactor = 3;
    }
    else if (radioButton4xVSubsampling->isChecked ())
    {
        bSubsampling = true;
        nMode = IS_SUBSAMPLING_4X_VERTICAL;
        vFactor = 4;
    }
    else if (radioButton5xVSubsampling->isChecked ())
    {
        bSubsampling = true;
        nMode = IS_SUBSAMPLING_5X_VERTICAL;
        vFactor = 5;
    }
    else if (radioButton6xVSubsampling->isChecked ())
    {
        bSubsampling = true;
        nMode = IS_SUBSAMPLING_6X_VERTICAL;
        vFactor = 6;
    }
    else if (radioButton8xVSubsampling->isChecked ())
    {
        bSubsampling = true;
        nMode = IS_SUBSAMPLING_8X_VERTICAL;
        vFactor = 8;
    }
    else if (radioButton16xVSubsampling->isChecked ())
    {
        bSubsampling = true;
        nMode = IS_SUBSAMPLING_16X_VERTICAL;
        vFactor = 16;
    }
    else
    {
        bSubsampling = true;
        bBinning = true;
        nMode = 0;
        vFactor = 1;
    }

    if (m_bLive)
        is_StopLiveVideo (m_hCamera, IS_WAIT);

    if (bBinning)
    {
        ApplyBinningSettings(binningMode, nMode);
    }

    if (bSubsampling)
    {
        ApplySubsamplingSettings(subsamplingMode, nMode);
    }

    UpdateSizeFormatCtrls ();
    UpdateWindowSizeCtrls ();

    m_pDlgMainView->SetupCapture();
    //m_pDlgMainView->ChangeImageSize(m_nWidth, m_nHeight);

    if (m_bLive)
        is_CaptureVideo (m_hCamera, IS_DONT_WAIT);

    // apply max exposure
    if (m_bMaxExposure)
    {
        double dblValue = 0.0;
        is_Exposure(m_hCamera, IS_EXPOSURE_CMD_SET_EXPOSURE, (void*)&dblValue, sizeof(dblValue));
    }

}

void properties::OnButtonUpDown ()
{
    BOOL bEnable = checkBoxUpDown->isChecked ();
    is_SetRopEffect (m_hCamera, IS_SET_ROP_MIRROR_UPDOWN, bEnable, 0);
}

void properties::OnButtonLeftRight ()
{
    BOOL bEnable = checkBoxLeftRight->isChecked ();
    is_SetRopEffect (m_hCamera, IS_SET_ROP_MIRROR_LEFTRIGHT, bEnable, 0);
}
