#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QDialog>
#include "cps.h"

class Mainview;

namespace Ui {
class Histogram;
}

class Histogram : public QDialog
{
    Q_OBJECT

public:
    explicit Histogram(QWidget *parent = 0);
    ~Histogram();
    bool updateHistogram(HIDS hCam, char *pBuffer);

private:
    Ui::Histogram *ui;

    QVector<QVector<int> > generateHistogramData(char* pcMem, int imgWidth, int imgHeight, int bitsPixel, int bitsSkip, int bitsTotal, int genChannels) const;
    QPixmap calculateHistogramm(int* data, int values, int channels, QVector<QColor> colors, int imgWidth, int imgHeight, int maxHeight) const;
};

#endif // HISTOGRAM_H
