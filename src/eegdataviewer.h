///////////////////////
/// JackeyLea
/// 20230428
/// 用于显示EEG数据
///////////////////////

#ifndef EEGDATAVIEWER_H
#define EEGDATAVIEWER_H

#include <QObject>
#include <QWidget>
#include <QDebug>

#include "QwtPolarPlot"
#include "QwtPolarGrid"
#include <QwtPolarCurve>
#include <QwtSymbol>

const QVector<double> c_a = {0,45,90,135,180,225,270,315,360};
//数据
class EEGData : public QwtSeriesData<QwtPointPolar>
{
public:
    EEGData()
    {
        assert(c_a.size()==9);

        m_data.resize(9);
        //测试数据
        for(int i=0;i<9;i++){
            m_data[i]=5;
        }
    }

    size_t size() const override{
        return 9;
    }

    void updateData(QVector<double> eeg){
        //进行数据转换
    }

    QwtPointPolar sample(size_t i) const override{
        assert(i<9);
        const double a = c_a[i];
        const double r = m_data[i];

        return QwtPointPolar( a, r );
    }

    QRectF boundingRect() const override{
        if(cachedBoundingRect.width() < 0.0){
            cachedBoundingRect = qwtBoundingRect(*this);
        }
        return cachedBoundingRect;
    }

private:
    QVector<double> m_data;
};

class EEGDataViewer : public QwtPolarPlot
{
    Q_OBJECT
public:
    explicit EEGDataViewer(QWidget *parent = nullptr);

    void updateData(QVector<double> eeg);

signals:

private:
    QwtPolarGrid *m_grid;

    QwtPolarCurve *m_curve;

    EEGData *data;
};

#endif // EEGDATAVIEWER_H
