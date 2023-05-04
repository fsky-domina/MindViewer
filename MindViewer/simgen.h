#ifndef SIMGEN_H
#define SIMGEN_H

#include <QObject>

class SimGen : public QObject
{
    Q_OBJECT
public:
    explicit SimGen(QObject *parent = nullptr);

signals:

};

#endif // SIMGEN_H
