//////////////////////////
/// JackeyLea
/// 20230505
/// 显示信号强度，只有三格信号
//////////////////////////

#ifndef SIGNALDATAVIEWER_H
#define SIGNALDATAVIEWER_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>

//信号强度
enum SignalStrength{
    Zero = 0,
    One,
    Two,
    Three,
    Four
};

class SignalDataViewer : public QWidget
{
    Q_OBJECT
public:
    explicit SignalDataViewer(QWidget *parent = nullptr);

    void setValue(short v);
    //设置信号强度
    void setSignalStrength(uchar s);

protected:
    void paintEvent(QPaintEvent *e);

signals:

private:
    SignalStrength m_signalStrength;
};

#endif // SIGNALDATAVIEWER_H
