#ifndef VALUEINDICATOR_H
#define VALUEINDICATOR_H

#include <QObject>
#include <QWidget>

class ValueIndicator : public QWidget
{
    Q_OBJECT
public:
    explicit ValueIndicator(QWidget *parent = nullptr);

signals:

};

#endif // VALUEINDICATOR_H
