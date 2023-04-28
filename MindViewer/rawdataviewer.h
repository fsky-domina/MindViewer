///////////////////////
/// JackeyLea
/// 20230428
/// 用于显示原始数据折线
///////////////////////

#ifndef RAWDATAVIEWER_H
#define RAWDATAVIEWER_H

#include <QObject>
#include <QWidget>

class RawDataViewer : public QWidget
{
    Q_OBJECT
public:
    explicit RawDataViewer(QWidget *parent = nullptr);

signals:

};

#endif // RAWDATAVIEWER_H
