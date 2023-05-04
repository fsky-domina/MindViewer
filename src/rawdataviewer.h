///////////////////////
/// JackeyLea
/// 20230428
/// 用于显示原始数据折线
///////////////////////

#ifndef RAWDATAVIEWER_H
#define RAWDATAVIEWER_H

#include <QObject>
#include <QWidget>
#include <QPaintEvent>
#include <QPointF>

#include "qwt_plot.h"
#include "qwt_plot_canvas.h"
#include "qwt_plot_curve.h"
#include "qwt_plot_layout.h"
#include "QwtScaleMap"

class RawDataViewer : public QWidget
{
    Q_OBJECT
public:
    explicit RawDataViewer(QWidget *parent = nullptr);

    void setMaxDataCnt(uint cnt);

    void appendData(double data);//添加新数据

protected:
    void paintEvent(QPaintEvent *e) override;

signals:

private:
    QwtPlotCurve curveRaw;//折线

    QVector<double> m_xdata;//x坐标
    //曲线数据
    QVector<double> m_data;

    QwtScaleMap xMap,yMap;

    uint m_cnt;//最大数据量
};

#endif // RAWDATAVIEWER_H
