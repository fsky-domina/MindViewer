#include "powerviewer.h"

PowerViewer::PowerViewer(QWidget *parent)
    : QWidget(parent)
    , m_value(18)
{
}

void PowerViewer::setValue(double v)
{
    m_value = v;
}

void PowerViewer::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    //设置颜色
    QPen pen;

    //画最外侧的边框
    pen.setBrush(Qt::white);
    painter.setPen(pen);
    painter.drawRect(contentsRect());

    //画指示部分
    if(m_value>20){
        pen.setBrush(Qt::green);
        painter.setBrush(Qt::green);
    }else{
        pen.setBrush(Qt::red);
        painter.setBrush(Qt::red);
    }
    painter.setPen(pen);

    double left = width() - m_value / 100.0 * width();
    QRectF vRect(left,0,width(),height());
    painter.drawRect(vRect);

    //绘制文字
    pen.setBrush(Qt::black);
    painter.setPen(pen);
    painter.drawText(contentsRect(),Qt::AlignCenter,QString::number((int)m_value));
}
