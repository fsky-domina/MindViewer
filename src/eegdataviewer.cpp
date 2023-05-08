#include "eegdataviewer.h"

//const QwtInterval s_radialInterval(0,1000);
const QwtInterval s_radialInterval(-16777216,16777215);
const QwtInterval s_azimuthInterval(0,360);

EEGDataViewer::EEGDataViewer(QWidget *parent)
    : QwtPolarPlot(parent)
{
    setScale(QwtPolar::Radius,s_radialInterval.minValue(),s_radialInterval.maxValue());
    setScale(QwtPolar::Azimuth,s_azimuthInterval.minValue(),s_azimuthInterval.maxValue(),s_azimuthInterval.width() / 8);

    //网格
    m_grid = new  QwtPolarGrid();
    m_grid->setPen(QPen(Qt::black));
    m_grid->showAxis( QwtPolar::AxisAzimuth, false );
    m_grid->showAxis( QwtPolar::AxisLeft, false );
    m_grid->showAxis( QwtPolar::AxisRight, false );
    m_grid->showAxis( QwtPolar::AxisTop, false );
    m_grid->showAxis( QwtPolar::AxisBottom, false );
    m_grid->showGrid( QwtPolar::Azimuth, true );
    m_grid->showGrid( QwtPolar::Radius, true );
    m_grid->attach( this );

    //用于指示
    baseData = new BaseData();
    m_baseCurve = new QwtPolarCurve();
    m_baseCurve->setStyle(QwtPolarCurve::Lines);
    m_baseCurve->setPen(QPen(Qt::red,1));
    m_baseCurve->setSymbol( new QwtSymbol( QwtSymbol::Rect,
         QBrush( Qt::cyan ), QPen( Qt::white ), QSize( 3, 3 ) ) );
    m_baseCurve->setData(baseData);
    m_baseCurve->attach(this);

    data = new EEGData();//数据
    //曲线
    m_curve = new QwtPolarCurve();
    m_curve->setStyle(QwtPolarCurve::Lines);
    m_curve->setPen(QPen(Qt::green,3));
    m_curve->setSymbol( new QwtSymbol( QwtSymbol::Rect,
         QBrush( Qt::cyan ), QPen( Qt::white ), QSize( 3, 3 ) ) );
    m_curve->setData(data);
    m_curve->attach(this);

    setAutoReplot(true);
}

void EEGDataViewer::updateData(QVector<double> eeg)
{
    assert(eeg.size()==9);
    data->updateData(eeg);
    m_curve->setData(data);
    m_curve->attach(this);
    replot();
}
