#ifndef CHART_WIDGET_H
#define CHART_WIDGET_H

#include <QLabel>
#include <QPushButton>
#include <QtWidgets/QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QVBoxLayout>
#include "push_button.h"
#include "drop_shadow_widget.h"
#include "bean/scanrecord.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

QT_CHARTS_USE_NAMESPACE

class CChartWidget : public DropShadowWidget
{
	Q_OBJECT

public:
	explicit CChartWidget(QList<ScanRecord*> listRecord, QWidget *parent = 0);
	~CChartWidget();

	void translateLanguage();

protected:
	void paintEvent(QPaintEvent *event);
	void initTitle();
	void initCenter();
	void GetChartData(QList<ScanRecord*> listRecord);
private:
	QHBoxLayout *title_layout; 
	QLabel *m_titleLabel; 
	QLabel *m_titleIconLabel;
	PushButton *close_button;

	QBarSet *m_set0;
	QBarSet *m_set1;
	QBarSet *m_set2;
	QBarSet *m_set3;
	QBarSet *m_set4;
	QBarCategoryAxis *m_axis;
	QBarSeries *m_series;
	QChart *m_chart;
	QChartView *m_chartView;

	std::map<QString, std::vector<ScanRecord*>> m_mapRecord;

	QStringList m_strListCate;
	QList<qreal> m_dListSet0;
	QList<qreal> m_dListSet1;
	QList<qreal> m_dListSet2;
	QList<qreal> m_dListSet3;
	QList<qreal> m_dListSet4;
};
#endif // CHART_WIDGET_H