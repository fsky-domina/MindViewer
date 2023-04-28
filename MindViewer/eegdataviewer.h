#ifndef EEGDATAVIEWER_H
#define EEGDATAVIEWER_H

#include <QObject>
#include <QWidget>

class EEGDataViewer : public QWidget
{
    Q_OBJECT
public:
    explicit EEGDataViewer(QWidget *parent = nullptr);

signals:

};

#endif // EEGDATAVIEWER_H
