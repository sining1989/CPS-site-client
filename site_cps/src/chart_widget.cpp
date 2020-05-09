#include "chart_widget.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

CChartWidget::CChartWidget(QList<ScanRecord*> listRecord, QWidget *parent) :
	DropShadowWidget(parent)
{
	this->GetChartData(listRecord);
	this->setFixedSize(700, 500);
	this->initTitle();
	this->initCenter();

	QVBoxLayout *main_layout = new QVBoxLayout();
	main_layout->addLayout(title_layout);
	main_layout->addWidget(m_chartView);

	main_layout->setSpacing(0);
	main_layout->setContentsMargins(SHADOW_WIDTH, SHADOW_WIDTH, SHADOW_WIDTH, SHADOW_WIDTH);

	setLayout(main_layout);
	this->translateLanguage();
	
}

CChartWidget::~CChartWidget()
{

}

void CChartWidget::translateLanguage()
{
	m_titleLabel->setText(tr("统计表"));
	close_button->setToolTip(tr("关闭"));
}

void CChartWidget::paintEvent(QPaintEvent *event)
{
	DropShadowWidget::paintEvent(event);
	QString skin_name = Util_Tool::getSkinName();
	int height = 35;
	QPainter painter(this);
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::white);
	painter.drawPixmap(QRect(SHADOW_WIDTH, SHADOW_WIDTH, this->width() - 2 * SHADOW_WIDTH, this->height() - 2 * SHADOW_WIDTH), QPixmap(skin_name));
	painter.drawRect(QRect(SHADOW_WIDTH, height, this->width() - 2 * SHADOW_WIDTH, this->height() - height - SHADOW_WIDTH));
}

void CChartWidget::initTitle()
{
	m_titleLabel = new QLabel();
	m_titleIconLabel = new QLabel();
	close_button = new PushButton();
	close_button->setPicName(QString(":/Resources/images/sysButton/close"));

	m_titleLabel->setFixedHeight(30);
	m_titleLabel->setObjectName("whiteLabel");
	QPixmap pixmap(":/Resources/icon");
	m_titleIconLabel->setPixmap(pixmap);
	m_titleIconLabel->setFixedSize(16, 16);
	m_titleIconLabel->setScaledContents(true);

	title_layout = new QHBoxLayout();
	title_layout->addWidget(m_titleIconLabel, 0, Qt::AlignVCenter);
	title_layout->addWidget(m_titleLabel, 0, Qt::AlignVCenter);
	title_layout->addStretch();
	title_layout->addWidget(close_button, 0, Qt::AlignTop);
	title_layout->setSpacing(5);
	title_layout->setContentsMargins(10, 0, 5, 0);

	connect(close_button, SIGNAL(clicked()), this, SLOT(hide()));
}

void CChartWidget::initCenter()
{
	//![1]
	m_set0 = new QBarSet(tr("扫描成功"));
	m_set1 = new QBarSet(tr("扫描异常"));
	m_set2 = new QBarSet(tr("上传成功"));
	m_set3 = new QBarSet(tr("上传失败"));
	m_set4 = new QBarSet(tr("未上传"));
	if (0 == m_dListSet0.size())
	{
		*m_set0 << 0 << 0 << 0 << 0 << 0 << 0 << 0;//扫描成功
	}
	else
	{
		for (size_t i = 0; i < m_dListSet0.size(); i++)
		{
			*m_set0 << m_dListSet0[i];//扫描成功
		}
	}
	if (0 == m_dListSet1.size())
	{
		*m_set1 << 0 << 0 << 0 << 0 << 0 << 0 << 0;
	}
	else
	{
		for (size_t i = 0; i < m_dListSet1.size(); i++)
		{
			*m_set1 << m_dListSet1[i];//扫描异常
		}
	}
	if (0 == m_dListSet2.size())
	{
		*m_set2 << 0 << 0 << 0 << 0 << 0 << 0 << 0;
	}
	else
	{
		for (size_t i = 0; i < m_dListSet2.size(); i++)
		{
			*m_set2<< m_dListSet2[i];//上传成功
		}
	}
	if (0 == m_dListSet3.size())
	{
		*m_set3 << 0 << 0 << 0 << 0 << 0 << 0 << 0;
	}
	else
	{
		for (size_t i = 0; i < m_dListSet3.size(); i++)
		{
			*m_set3 << m_dListSet3[i];//上传失败
		}
	}
	if (0 == m_dListSet4.size())
	{
		*m_set4 << 0 << 0 << 0 << 0 << 0 << 0 << 0;
	}
	else
	{
		for (size_t i = 0; i < m_dListSet4.size(); i++)
		{
			*m_set4 << m_dListSet4[i];//未上传
		}
	}
	//*m_set0 << m_dListSet0[0] << m_dListSet0[1] << m_dListSet0[2] << m_dListSet0[3] << m_dListSet0[4];//扫描成功
	//*m_set1 << m_dListSet1[0] << m_dListSet1[1] << m_dListSet1[2] << m_dListSet1[3] << m_dListSet1[4];//扫描异常
	//*m_set2 << m_dListSet2[0] << m_dListSet2[1] << m_dListSet2[2] << m_dListSet2[3] << m_dListSet2[4];//上传成功
	//*m_set3 << m_dListSet3[0] << m_dListSet3[1] << m_dListSet3[2] << m_dListSet3[3] << m_dListSet3[4];//上传失败
	//*m_set4 << m_dListSet4[0] << m_dListSet4[1] << m_dListSet4[2] << m_dListSet4[3] << m_dListSet4[4];//未上传

	//![2]
	m_series = new QBarSeries();
	m_series->append(m_set0);
	m_series->append(m_set1);
	m_series->append(m_set2);
	m_series->append(m_set3);
	m_series->append(m_set4);

	//![3]
	m_chart = new QChart();
	m_chart->addSeries(m_series);
	m_chart->setTitle(tr("最近一周统计表"));
	m_chart->setAnimationOptions(QChart::SeriesAnimations);
	//![3]

	//![4]
	m_axis = new QBarCategoryAxis();
	m_axis->append(m_strListCate);
	m_chart->createDefaultAxes();
	m_chart->setAxisX(m_axis, m_series);
	//![4]

	//![5]
	m_chart->legend()->setVisible(true);
	m_chart->legend()->setAlignment(Qt::AlignBottom);
	//![5]

	//![6]
	m_chartView = new QChartView(m_chart);
	m_chartView->setRenderHint(QPainter::Antialiasing);
	//![6]
}

void CChartWidget::GetChartData(QList<ScanRecord*> listRecord)
{
	m_mapRecord.clear();

	for (int i = 0; i < listRecord.size(); i++)
	{
		QString strDate = listRecord[i]->startTime.mid(0, listRecord[i]->startTime.indexOf(" "));
		//m_mapRecord.insertMulti(strDate, list[i]);

		std::map<QString, std::vector<ScanRecord*>>::iterator itFind = m_mapRecord.find(strDate);
		if (itFind != m_mapRecord.end())
		{
			itFind->second.push_back(listRecord[i]);
		}
		else
		{
			std::vector<ScanRecord*> vecTemp;
			vecTemp.clear();
			vecTemp.push_back(listRecord[i]);
			m_mapRecord.insert(std::make_pair(strDate, vecTemp));
		}
		//delete list[i];
	}
	
	m_strListCate.clear();
	m_dListSet0.clear();
	m_dListSet1.clear();
	m_dListSet2.clear();
	m_dListSet3.clear();
	m_dListSet4.clear();
	std::map<QString, std::vector<ScanRecord*>>::reverse_iterator itor = m_mapRecord.rbegin();
	for (int iIndex = 0; itor != m_mapRecord.rend() && iIndex < 7; ++itor, ++iIndex)
	{
		m_strListCate << itor->first;
		//按日期统计
		double dSet0 = 0;
		double dSet1 = 0;
		double dSet2 = 0;
		double dSet3 = 0;
		double dSet4 = 0;
		std::vector<ScanRecord*> vecRecord = itor->second;
		for (int iCount = 0; iCount < vecRecord.size(); ++iCount)
		{
			if (1 == vecRecord[iCount]->result)
			{
				++dSet0; //扫描成功
			}
			if (0 == vecRecord[iCount]->result)
			{
				++dSet1; //扫描失败
			}
			if (1 == vecRecord[iCount]->uploadStatus)
			{
				++dSet2; //上传成功
			}
			if (2 == vecRecord[iCount]->uploadStatus)
			{
				++dSet3; //上传失败
			}
			if (0 == vecRecord[iCount]->uploadStatus)
			{
				++dSet4; //未上传
			}
		}
		m_dListSet0 << dSet0; 
		m_dListSet1 << dSet1; 
		m_dListSet2 << dSet2; 
		m_dListSet3 << dSet3;
		m_dListSet4 << dSet4;
	}
}