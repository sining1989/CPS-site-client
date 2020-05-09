/*!
 * \file    qsliderex.h
 * \date    02.09.2018
 * \author  zhusizhi
 *
 * \brief   extended QSlider class provides additional functionality
 *
 */
#ifndef QSLIDEREX_H_
#define QSLIDEREX_H_

#include <QObject>
#include <QSlider>
#include <QTimer>

/*! \brief Extended Slider class */
class QSliderEx:public QSlider
{
Q_OBJECT

public:
    /*! \brief standard constructor */
    QSliderEx(QWidget *parent = 0, int slack = 0);
    /*! \brief add constructor */
    QSliderEx(Qt::Orientation orientation, QWidget *parent = 0, int slack = 0);
    /*! \brief destructor */
    virtual ~QSliderEx();
    /*! \brief set slider slackness */
    void setSlackness(int slack);
    /*! \brief base class slider signaling */
    void EnableSignaling(bool bEnable);

signals:
    /*!
     * \brief slacked valueChange signal
     * \param value new slider value
     */
    void valueChangedEx (int value); // slacked valueChanged

private slots:
    /*! \brief slider init */
    void Init();

    /*!
     * \brief base class value has changed slot
     * \param value new base class value
     */
    void baseValueChanged(int value);
    /*! \brief slack timeout slot */
    void slacktimeout();

private:
    /*! \brief last slack emitted value */
    int m_lastvalue;
    /*! \brief current base class value */
    int m_value;
    /*! \brief signal slackness */
    int m_slack;
    /*! \brief slack timer */
    QTimer *m_pTimer;
};

#endif /* QSLIDEREX_H_ */
