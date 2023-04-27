////////////////////////
/// JackeyLea
/// 20230427
/// 指示器类，类似于汽车速度仪表盘，用于显示注意力和冥想值
////////////////////////

#ifndef VALUEINDICATOR_H
#define VALUEINDICATOR_H

#include <QObject>
#include <QWidget>
#include <QString>
#include <qwt_dial.h>

class ValueIndicator : public QwtDial
{
    Q_OBJECT
public:
    explicit ValueIndicator(QWidget *parent = nullptr);

    void setLabel(const QString &);//设置标签
    QString label() const;//获取标签

protected:
  virtual void drawScaleContents( QPainter* painter,
      const QPointF& center, double radius ) const override;

signals:

private:
    QString m_label;//用于显示类型

};

#endif // VALUEINDICATOR_H
