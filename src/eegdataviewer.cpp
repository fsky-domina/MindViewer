#include "eegdataviewer.h"

//const QwtInterval s_radialInterval(0,10);
const QwtInterval s_radialInterval(-32768,32767);
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

    //数据
    data = new EEGData();

    //曲线
    m_curve = new QwtPolarCurve();
    m_curve->setStyle(QwtPolarCurve::Lines);
    m_curve->setPen(QPen(Qt::green,2));
    m_curve->setSymbol( new QwtSymbol( QwtSymbol::Rect,
         QBrush( Qt::cyan ), QPen( Qt::white ), QSize( 3, 3 ) ) );
    m_curve->setData(data);
    m_curve->attach(this);
}

void EEGDataViewer::updateData(QVector<double> eeg)
{
    data->updateData(eeg);
}
