/////////////////////
/// JackeyLea
/// 20230429
/// 电源显示控件
/////////////////////

#ifndef POWERVIEWER_H
#define POWERVIEWER_H

#include <QObject>
#include <QWidget>
#include <QPaintEvent>
#include <QPainter>

class PowerViewer : public QWidget
{
    Q_OBJECT
public:
    explicit PowerViewer(QWidget *parent = nullptr);

    void setValue(double v);

protected:
    void paintEvent(QPaintEvent *);

signals:

private:
    double m_value;
};

#endif // POWERVIEWER_H
