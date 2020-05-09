#include "histogram.h"
#include "ui_histogram.h"

#include <QPixmap>
#include <QPainter>
#include <QPen>
#include <QtCore/qmath.h>
#include <QMessageBox>
#include <QMap>

Histogram::Histogram(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Histogram)
{
    ui->setupUi(this);
    connect(ui->pushButtonClose, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->pushButtonClose, SIGNAL(clicked()), parent, SLOT(closeHistogram()));
    setWindowTitle("cps Histogram");
}

Histogram::~Histogram()
{
    delete ui;
}

bool Histogram::updateHistogram(HIDS hCam, char *pBuffer)
{
    uint32_t pixmapWidth = ui->labelHistogram->width();
    uint32_t pixmapHeight = ui->labelHistogram->height();
    QPixmap histogram(pixmapWidth, pixmapHeight);
    histogram.fill(Qt::lightGray);

    int nRet = IS_NO_SUCCESS, imageID = 0, colormode = 0;
    char* pImgMem = NULL;

    nRet = is_GetActSeqBuf(hCam, &imageID, 0, &pImgMem);
    if (nRet != IS_SUCCESS)
    {
        return false;
    }

    colormode = is_SetColorMode(hCam, IS_GET_COLOR_MODE);
    if (colormode == IS_CM_MONO8)
    {
        ui->checkBoxHistogramGreen->setEnabled(false);
        ui->checkBoxHistogramRed->setEnabled(false);
    }
    else
    {
        ui->checkBoxHistogramGreen->setEnabled(true);
        ui->checkBoxHistogramRed->setEnabled(true);
    }

    uint32_t steps = 0, channels = 3, valuesPerChannel = 256, maxValue = 0;
    uint32_t bgrBuffer[valuesPerChannel * channels];
    memset(bgrBuffer, 0, sizeof(bgrBuffer) / sizeof(bgrBuffer[0]));

    nRet = is_GetImageHistogram(hCam, imageID, colormode, bgrBuffer);
    if (nRet != IS_SUCCESS)
    {
        return false;
    }

    for (size_t channel = 0; channel < channels; channel++)
    {
        if ((channel == 0 && ui->checkBoxHistogramBlue->isChecked()) ||
            (channel == 1 && ui->checkBoxHistogramGreen->isChecked()) ||
            (channel == 2 && ui->checkBoxHistogramRed->isChecked()))
        {
            if (colormode == IS_CM_MONO8 && channel > 0)
            {
                continue;
            }

            uint32_t* buffer = bgrBuffer + (1 << 8) * channel;
            for (size_t i = 0; i < valuesPerChannel; i++)
            {
                if (buffer[i] > maxValue)
                {
                    maxValue = buffer[i];
                }
            }
        }
    }

    if (colormode == IS_CM_BGR565_PACKED)
    {
        ui->histogramLabel128->setText(QString::number((1 << 6) / 2));
        ui->histogramLabel255->setText(QString::number((1 << 6) - 1));
    }
    else
    {
        ui->histogramLabel128->setText("128");
        ui->histogramLabel255->setText("255");
    }

    QPainter painter(&histogram);
    QPen pen(Qt::DashLine);
    QPolygon polygon;
    std::vector<QColor> colors;

    colors.push_back(QColor("blue"));
    colors.push_back(QColor("green"));
    colors.push_back(QColor("red"));

    pen.setColor(Qt::gray);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.setBrush(Qt::lightGray);
    painter.drawRect(0, 0, pixmapWidth, pixmapHeight);

    steps = 1 << int(log(pixmapWidth / 10.0f) / log(2.0f));
    for (unsigned int i = 1; i < steps; ++i)
    {
        painter.drawLine((pixmapWidth * float(i) / steps), 1, (pixmapWidth * float(i) / steps), pixmapHeight - 1);
    }

    steps = 1 << int(log(pixmapHeight / 10.0f) / log(2.0f));
    for (unsigned int i = 1; i < steps; ++i)
    {
        painter.drawLine(1, pixmapHeight * float(i) / steps, pixmapWidth - 1, pixmapHeight * float(i) / steps);
    }

    painter.setRenderHint(QPainter::Antialiasing);
    pen.setStyle(Qt::SolidLine);

    qreal scaleWidth = pixmapWidth / double(valuesPerChannel - 1);
    qreal scaleHeight = pixmapHeight / double(maxValue);

    for (size_t channel = 0; channel < channels; channel++)
    {
        if ((channel == 0 && ui->checkBoxHistogramBlue->isChecked()) ||
            (channel == 1 && ui->checkBoxHistogramGreen->isChecked()) ||
            (channel == 2 && ui->checkBoxHistogramRed->isChecked()))
        {
            if (colormode == IS_CM_MONO8 && channel > 0)
            {
                continue;
            }
            if (colormode == IS_CM_BGR565_PACKED)
            {
                if (channel == 1)
                {
                    scaleWidth = pixmapWidth / double(256 / 8 * 6 - 1) * 3.0;
                }
                else
                {
                    scaleWidth = pixmapWidth / double(256 / 8 * 5 - 1) * 3.0;
                }
            }

            uint32_t* buffer = bgrBuffer + (1 << 8) * channel;

            pen.setColor(colors[channel]);
            painter.setPen(pen);
            painter.setBrush(Qt::transparent);

            polygon.clear();
            polygon << QPoint(pixmapWidth, pixmapHeight) << QPoint(0, pixmapHeight);

            for (size_t i = 0; i < valuesPerChannel; i++)
            {
                polygon << QPoint(scaleWidth * i, pixmapHeight - scaleHeight * buffer[i]);
            }

            painter.drawPolygon(polygon);
        }
    }

    ui->labelHistogram->setPixmap(histogram);

    return false;
}
