#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    size = -1;
}

MainWindow::~MainWindow() {
    delete ui;
}
//数据解析开始，先将数据头部的空格去除，然后每两个字母之间有一个空格。
QString MainWindow::toStandardText(QString source) {
    source.remove('\n'); //先将换行去除
    source.remove(' ');  //去除所有空格
    size = source.size();
    QString result;
    int     i = 0;
    while (i < size) {
        result += source.mid(i, 2) + ' ';
        i += 2;
    }
    return result;
}

QList<pkgPacket> MainWindow::parserData(QList<int> data) {
    QList<pkgPacket> result;

    unsigned char checksum = 0x0, PLENGTH;
    //数据包含三个部分，包头，有效数据（最多169位），检验值
    for (int i = 0; i < data.size(); i++) {
        pkgPacket packet;
        if (data.at(i) == 0xAA) { // 0
            i++;
            if (data.at(i) == 0xAA) { // 1
                i++;
                PLENGTH = (unsigned char)data.at(i); // 2
                if (PLENGTH > 170)
                    return result; // POOR_SIGNAL Quality 信号质量
                                   // 0-200，值越大，证明噪声越多，值为200时表示电极未与皮肤接触
                // 0x02
                for (int t = i + 1; t < PLENGTH + i; t++) {
                    checksum += data.at(t);
                }
                for (int j = i + 1; j <= PLENGTH + i;) // 3-34
                {
                    switch (data.at(j)) // 3
                    {
                        case 0x02:
                            packet.signal = data.at(++j); // 4
                            j++;                          // 5
                            break;
                        case 0x83:
                            if (data.at(++j) == 0x18)  {// 6
                                packet.delta     = data.at(j + 2);
                                packet.theta     = data.at(j + 5);
                                packet.lowAlpha  = data.at(j + 8);
                                packet.highAlpha = data.at(j + 11);
                                packet.lowBeta   = data.at(j + 14);
                                packet.highBeta  = data.at(j + 17);
                                packet.lowGamma  = data.at(j + 20);
                                packet.midGamma  = data.at(j + 23);
                                // ASIC_EEG_POWER 0x83
                                /* this is Data Value is output as a series of eight 3-byte unsigned
                                 integers in little-endian format. e eight EEG powers are output in
                                 the following order: delta (0.5 - 2.75Hz), theta (3.5 - 6.75Hz),
                                 low-alpha (7.5 - 9.25Hz), high-alpha (10 - 11.75Hz), low-beta (13
                                 - 16.75Hz), high-beta
                                    (18 - 29.75Hz), low-gamma (31 - 39.75Hz), and mid-gamma (41
                                 - 49.75Hz).*/
                                //单独数据没有意义
                            }
                            j = j + 25; // 31
                            break;
                        case 0x04:
                            packet.attention = data.at(++j); // 32
                            j++;                             // 33
                            break;
                            // eSenses (i.e. Attention, Meditation)注意度，冥想度
                            // 0-100 各8位 1次/s 0x04 0x05
                            // 0 未接触 1-20 非常低 20-40 低 40-60 中等 60-80 偏高 80-100 非常高
                        case 0x05:
                            packet.meditation = data.at(++j); // 34
                            j++;                              // 35
                            break;
                        case 0x16:
                            packet.blink = data.at(++j);
                            j++; // 37
                            break;
                            //眨眼强度 0-255
                            // 0x16
                        case 0x80:
                            if (data.at(++j) == 0x02) {
                                unsigned char low  = data.at(++j);
                                unsigned char high = data.at(++j);
                                packet.rawdata     = (low << 8) | high;
                                // RAW WAVE DATA 16位 -32768 to 32767 高低位，每位8位 512次/s 0x80
                                /*short raw = (Value[0]<<8) | Value[1];
                                where Value[0] is the high-order byte, and Value[1] is the low-order
                                byte. In systems or languages where bit operations are inconvenient,
                                the following arithmetic operations may be substituted instead: raw
                                = Value[0]*256 + Value[1]; if( raw >= 32768 ) raw = raw - 65536;*/
                            }
                            j++;
                            break;
                    }
                }
                checksum &= 0xFF;
                checksum = ~checksum & (0xFF);
                if (checksum == data.back()) {
                    packet.valid = true;
                } else {
                    packet.valid = false;
               }
            }
            result.append(packet);
        } else {
            continue;
        }
    }
    return result;
}

QList<pkgPacket> MainWindow::parserData(QByteArray data) {
    QList<int> dataList;
    for(int i=0;i<data.size();i++){
        dataList.append(data.at(i));
    }
    return parserData(dataList);
}

void MainWindow::on_actionOpen_triggered() {
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open"), QString());
    if (filePath.isEmpty()) {
        QMessageBox::information(this, tr("Warning"), tr("Nothing selected"));
        return;
    }
    QFile file(filePath);
    if (!file.exists()) {
        QMessageBox::information(this, tr("Warning"), tr("The file didn't exists"));
        return;
    }
    QString result;
    if (file.open(QIODevice::Text | QIODevice::ReadOnly)) {
        QTextStream readIn(&file);
        result = readIn.readAll();
    }

    QString standardText = toStandardText(result);

    QList<int> dataList;
    int        i = 0;
    while (i < result.size()) {
        QString str = result.mid(i, 2);
        qDebug() << str;
        dataList.append(str.toInt(nullptr, 16));
        i = i + 3;
    }
    qDebug() << "Parsered data count is: " << dataList.size();
    qDebug() << "The first data is: " << dataList.at(2);

    QList<pkgPacket> parserResult = parserData(dataList);
    int cnt = parserResult.size();
    qDebug()<<"The size of data parsered is: "<<cnt;
    if(cnt>=1){
        qDebug()<<"isValid"<<parserResult.at(0).valid;
    }
}

void MainWindow::on_actionExit_triggered() {
    qApp->exit(0);
}

void MainWindow::on_actionPause_triggered() {
}

void MainWindow::on_actionResume_triggered() {
}

void MainWindow::on_actionCapture_triggered() {
}

void MainWindow::on_actionClear_triggered() {
}

void MainWindow::on_actionAbout_triggered() {
    QString msg = QString("<h1>MindViewer</h1>"
                          "<h2>TGAM module tools</h2>"
                          "<h3>Author: JackeyLea</h3>"
                          "<h3>E-mail: 1768478912@qq.com</h3>"
                          "<h3>Phone: 13812991101</h3>"
                          "<h3>GitHub: https://github.com/JackeyLea/MindViewer</h3>"
                          "<h3>Gitee: https://gitee.com/JackeyLea/MindViewer</h3>"
                          "<h4>Do what you want but please obey the LGPL3 rules</h4>"
                          "<h4>And keep those message within application</h4>");
    QMessageBox::information(this, tr("About"), msg, QMessageBox::Ok);
}

void MainWindow::on_actionQt_triggered() {
    qApp->aboutQt();
}

void MainWindow::on_actionGithub_triggered() {
    QDesktopServices::openUrl(QUrl("https://github.com/JackeyLea/MindViewer"));
}
