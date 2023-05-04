#include "rawdataviewer.h"

#include <QPainter>

RawDataViewer::RawDataViewer(QWidget *parent)
    : QWidget(parent)
    , m_cnt(100)
{
    //x坐标
    for(int i=0;i<m_cnt;i++){
        m_xdata.append(i);
    }

    //所有值被限制在此范围内
    xMap.setScaleInterval(0,m_cnt);
    yMap.setScaleInterval(-32768,32767);

    curveRaw.setPen(Qt::black,2);
    curveRaw.setStyle(QwtPlotCurve::Lines);
    curveRaw.setRenderHint(QwtPlotItem::RenderAntialiased);
}

void RawDataViewer::setMaxDataCnt(uint cnt)
{
    m_cnt = cnt;
    update();
}

void RawDataViewer::appendData(double data)
{
    if(m_data.size() > m_cnt){
        m_data.pop_front();//删除第一个数
    }
    m_data.append(data);
    curveRaw.setSamples(m_xdata,m_data);
    update();
}

void RawDataViewer::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);

    QRect r= contentsRect();

    QPainter painter(this);

    xMap.setPaintInterval(r.left(),r.right());
    yMap.setPaintInterval(r.top(),r.bottom());
    painter.setRenderHint(QPainter::Antialiasing,
                          curveRaw.testRenderHint(QwtPlotItem::RenderAntialiased));
    curveRaw.draw(&painter,xMap,yMap,r);
}
