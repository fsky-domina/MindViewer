#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initUi();//初始化界面
    initConnections();//初始化信号槽

    ui->actionGrap->setChecked(true);//默认使用图形模式
    resize(800,820);

    mBuff.clear();
}

MainWindow::~MainWindow()
{
    if(simGen!=nullptr){//停止模拟数据
        delete simGen;
    }
    delete retriverWgt;
    delete ui;
}

//使用状态机解析原始数据
int MainWindow::parserData(QByteArray ba, _eegPkt &pkt)
{
    //输入的数据ba只包含一个有效包
    //qDebug()<<"parse start";

    if(ba.isEmpty()) return -1;//如果没有数据就直接退出

    //qDebug()<<"add to buff."<<ba.size();
    QByteArray buff = ba;//将数据添加到处理缓冲区

    /////////////////处理开始/////////////////////////////////////
    int cnt=0;
    uchar state=PARSER_STATE_SYNC;//起始状态
    uchar payloadLength=0;
    uchar payloadSum=0;
    while(buff.size()){
        //qDebug()<<buff;
        //状态机处理
        switch(state){
        case PARSER_STATE_SYNC://第一个同步字节
            if((uchar)buff[0]==PARSER_SYNC_BYTE){
                //qDebug()<<"parser first aa "<<i<<(uchar)buff[i];
                state=PARSER_STATE_SYNC_CHECK;//等待第2个0xaa
            }else{
                state = PARSER_SYNC_BYTE;//如果第一个字节不是0xaa，就从头开始
            }
            buff.remove(0,1);
            break;
        case PARSER_STATE_SYNC_CHECK://第二个同步字节
            if((uchar)buff[0]==PARSER_SYNC_BYTE){//包第二个0xaa
                //qDebug()<<"parser second aa "<<i<<(uchar)buff[i];
                state=PARSER_STATE_PAYLOAD_LENGTH;//准备解析负载长度
            }else{
                state = PARSER_STATE_SYNC;//如果第二个字节不是0xaa，就从头开始
            }
            buff.remove(0,1);
            break;
        case PARSER_STATE_PAYLOAD_LENGTH:
            payloadLength=(uchar)buff[0];//接下来是长度
            //qDebug()<<"payloadlength"<<payloadLength;
            if(payloadLength>=170 || payloadLength<=0){//如果长度大于170就丢弃此包并查找下一个0xaa 0xaa
                qDebug()<<"this package payloadLength(the 3rd value) is wrong";
                state = PARSER_STATE_SYNC;
            }else{
                //qDebug()<<"parser payload length "<<i<<(uchar)buff[i];
                state=PARSER_STATE_CHKSUM;//准备解析有效数据
            }
            buff.remove(0,1);
            break;
        case PARSER_STATE_CHKSUM:
        {
            //qDebug()<<"check sum"<<payloadLength+1<<buff.size();
            //如果剩余大小不够 1是末尾的校验值
            if(payloadLength+1 >buff.size()){
                qDebug()<<"pkg is not valid.";
                state = PARSER_STATE_SYNC;
                buff.remove(0,1);
                break;
            }
            //执行到这里表示此时包是完整的，后续解析其实不需要判断包大小
            //首先校验数据是否有效
            payloadSum=0;
            for(int j=0;j<payloadLength;j++){
                //qDebug()<<(uchar)buff[j];
                payloadSum+=(uchar)buff[j];
            }
            payloadSum &= 0xff;
            payloadSum = ~payloadSum & 0xff;

            //qDebug()<<"add"<<payloadSum<<(uchar)buff[payloadLength];
            if(payloadSum!=(uchar)buff[payloadLength]){
                //如果与校验值不同就丢弃此包数据
                qDebug()<<"Checksum failed.";
                return -1;
            }
            //qDebug()<<"get data check sum is: "<<z<<(uchar)buff[z];
            //qDebug()<<"parser check sum "<<i;
            state=PARSER_STATE_PAYLOAD;
            //到此可以正常解析数据了
            break;
        }
        case PARSER_STATE_PAYLOAD://解析数据
        {
            //qDebug()<<"payload"<<buff;
            if(cnt==payloadLength){
                buff.remove(0,1);//如果数据已经没了，那么还剩下一个校验值
                break;
            }
            if((uchar)buff[0]==0x01){//电源值，最大为127
                //如果缓冲区大小小于2位
                if(buff.size() < 2){
                    pkt.isPowerValid=false;
                    state = PARSER_STATE_SYNC;
                    break;
                }
                pkt.isPowerValid=true;
                pkt.power=(uchar)buff[1];
                state=PARSER_STATE_PAYLOAD;
                buff.remove(0,2);
                cnt+=2;
            }else if((uchar)buff[0]==0x02){//数据信号强度值
                //如果缓冲区大小小于2位
                if(buff.size() < 2){
                    pkt.isSignalValid=false;
                    state = PARSER_STATE_SYNC;
                    break;
                }
                //qDebug()<<"signal value "<<(uchar)buff[1];
                pkt.isSignalValid=true;
                pkt.signal=(uchar)buff[1];
                state=PARSER_STATE_PAYLOAD;
                buff.remove(0,2);
                cnt+=2;
            }else if((uchar)buff[0]==0x03){
                //ego上的心跳强度0-255
            }else if((uchar)buff[0]==0x04){//注意力值
                //如果缓冲区大小小于2位
                if(buff.size() < 2){
                    pkt.isAttentionValid=false;
                    state = PARSER_STATE_SYNC;
                    break;
                }
                //qDebug()<<"attention value "<<(uchar)buff[1];
                pkt.isAttentionValid=true;
                pkt.attention=(uchar)buff[1];
                state=PARSER_STATE_PAYLOAD;
                buff.remove(0,2);
                cnt+=2;
            }else if((uchar)buff[0]==0x05){//冥想值
                //如果缓冲区大小小于2位
                if(buff.size() < 2){
                    pkt.isMeditationValid=false;
                    state = PARSER_STATE_SYNC;
                    break;
                }
                //qDebug()<<"meditation value "<<(uchar)buff[1];
                pkt.isMeditationValid=true;
                pkt.meditation=(uchar)buff[1];
                state=PARSER_STATE_PAYLOAD;
                buff.remove(0,2);
                cnt+=2;
            }else if((uchar)buff[0]==0x06){//8bit raw value
            }else if((uchar)buff[0]==0x07){
                //raw_marker 固定值为0
            }else if((uchar)buff[0]==0x80 && (uchar)buff[1]==0x02){//16位原始数据
                //qDebug()<<"raw data";
                //如果缓冲区大小小于5位
                if(buff.size() < 5){
                    pkt.isRawValid=false;
                    state = PARSER_STATE_SYNC;
                    break;
                }
                pkt.isRawValid=true;
                pkt.raw=((uchar)buff[2]<<8)|(uchar)buff[3];
                buff.remove(0,5);//4个数据以及最后的校验值
                state = PARSER_STATE_NULL;
                break;
            }else if((uchar)buff[0]==0x81 && (uchar)buff[1]==0x20){
                //eeg_power 8个大端四字节
            }else if((uchar)buff[0]==0x83 && (uchar)buff[1]==0x18){//eeg数据部分
                //0x83标志eeg部分开始，下一位表示为eeg部分程度默认为0x18,8个大端三字节
                //qDebug()<<"parser eeg data "<<i<<buff[i];
                //qDebug()<<"parser eeg length "<<(uchar)buff[i+1];
                //如果缓冲区大小小于24位
                if(buff.size() < 26){
                    pkt.isEEGValid=false;
                    state = PARSER_STATE_SYNC;
                    break;
                }
                pkt.isEEGValid=true;
                //tgam数据默认为大端
                pkt.delta =((uint)buff[2]<<16)|((uint)buff[3]<<8)|((uint)buff[4]);
                pkt.theta =((uint)buff[5]<<16)|((uint)buff[6]<<8)|((uint)buff[7]);
                pkt.lowAlpha =((uint)buff[8]<<16)|((uint)buff[9]<<8)|((uint)buff[10]);
                pkt.highAlpha =((uint)buff[11]<<16)|((uint)buff[12]<<8)|((uint)buff[13]);
                pkt.lowBeta =((uint)buff[14]<<16)|((uint)buff[15]<<8)|((uint)buff[16]);
                pkt.highBeta =((uint)buff[17]<<16)|((uint)buff[18]<<8)|((uint)buff[19]);
                pkt.lowGamma =((uint)buff[20]<<16)|((uint)buff[21]<<8)|((uint)buff[22]);
                pkt.midGamma =((uint)buff[23]<<16)|((uint)buff[24]<<8)|((uint)buff[25]);
                state=PARSER_STATE_PAYLOAD;
                buff.remove(0,26);//24 + 2
                cnt+=26;
            }else if((uchar)buff[0]==0x86 && (uchar)buff[1]==0x02){
                //两个大端字节表示R峰的间隔
            }
            break;
        }
        case PARSER_STATE_NULL:
            break;
        default:
            break;
        }
    }

    //qDebug()<<"parse end.";

    return 0;
}

//接收原始数据
void MainWindow::sltReceiveData(QByteArray ba)
{
    if(ba.size()<=0) return;

    //16进制模式，直接添加到编辑框里面
    if(ui->actionHex->isChecked()){
        QString s;
        for(int i=0;i<ba.size();i++){
            QString ss,sss;
            sss=ss.asprintf("0x%02X ",(unsigned char)ba.at(i));
            s+=sss;
        }
        ui->textHex->appendPlainText(s);
    }else{//图形模式
        //把新收到的数据填充到缓冲区
        mBuff.append(ba);
        //qDebug()<<"receive 2"<<ba;
        //qDebug()<<"buff "<<mBuff;

        if(mBuff.size()<=5){
            //qDebug()<<"buffer size is small then min size";
            //一个包最起码包含一个有效数据类型0xaa 0xaa 0x02 0xaa 0xaa
            return;//此时包肯定不完整，就结束
        }else{//有可能一次收的数据不完整先判断
            //qDebug()<<"buffer size is valid"<<mBuff.size();
            //TODO 可能出现的问题是，如果还没有处理完当前包，下一包就来了
            while(mBuff.size()>=5){//提取有效数据
                //qDebug()<<"buffer size is valid2"<<mBuff.size();
                //qDebug()<<mBuff;
                if((uchar)mBuff[0]==0xAA && (uchar)mBuff[1]==0xAA){//先找包头
                    //qDebug()<<"valid header";
                    //包大小
                    int pkgSize = mBuff[2];
                    if(pkgSize + 2 + 1+ 1 > mBuff.size()){
                        qDebug()<<"pkg is less than given size.";
                        return;
                    }else{
                        //此时继续解析
                        //有一种特殊情况 0xaa 0xaa 0xaa
                        if((uchar)mBuff[0]==0xaa && (uchar)mBuff[1]==0xaa && (uchar)mBuff[2]==0xaa){
                            //qDebug()<<"0xaa 0xaa 0xaa";
                            mBuff.remove(0,1);
                            continue;
                        }
                        if((uchar)mBuff[0]!=(uchar)0xAA || (uchar)mBuff[1]!=(uchar)0xAA ){
                            //qDebug()<<"next pkg"<<mBuff;
                            //删除一个直到符合
                            mBuff.remove(0,1);
                            continue;
                        }
                        //第3个字节是长度
                        int length = mBuff[2];
                        QByteArray tmpBA = mBuff.mid(0,length+2+1+1);//0xaa 0xaa 长度1 length 校验1
                        //qDebug()<<"valid pkg"<<tmpBA;
                        //从缓冲区删除已经解析的包
                        //qDebug()<<"before delete"<<mBuff;
                        mBuff.remove(0,length+4);
                        //qDebug()<<"after delete"<<mBuff;
                        //解析函数一次只解析一个包
                        {
                            struct _eegPkt pkt;
                            pkt.init();
                            if(parserData(tmpBA,pkt)!=0){
                                qDebug()<<"Cannot parse data.";
                                return;
                            }
                            //qDebug()<<"parsered";
                            //信号强度
                            if(pkt.isSignalValid){
                                ui->widgetSignal->setValue(pkt.signal);
                            }
                            //电源
                            if(pkt.isPowerValid){
                                ui->widgetPower->setValue(pkt.power);
                            }
                            //注意力
                            if(pkt.isAttentionValid){
                                ui->widgetAttention->setValue(pkt.attention);
                            }
                            //冥想值
                            if(pkt.isMeditationValid){
                                ui->widgetMeditation->setValue(pkt.meditation);
                            }

                            if(pkt.isEEGValid){
                                //8个数据
                                QVector<double> eegData;
                                eegData.append(pkt.delta);
                                eegData.append(pkt.theta);
                                eegData.append(pkt.lowAlpha);
                                eegData.append(pkt.highAlpha);
                                eegData.append(pkt.lowBeta);
                                eegData.append(pkt.highBeta);
                                eegData.append(pkt.lowGamma);
                                eegData.append(pkt.midGamma);
                                eegData.append(pkt.delta);
                                ui->widgetEEG->updateEEGData(pkt);
                            }

                            if(pkt.isRawValid){
                                ui->widgetEEG->updateRawData(pkt.raw);
                            }
                        }
                    }
                }else{
                    //如果前两个不是0xAA 0xAA就向后移一位
                    mBuff.remove(0,1);
                }
            }//while buff size >5
        }//if buff size <=5
    }//if action hex is checked
}
//使用测试模拟数据
void MainWindow::sltActionTest(bool checked)
{
    if(checked){
        if(ui->actionHex->isChecked()){
            ui->stackedWidget->setCurrentIndex(1);
        }
        if(ui->actionGrap->isChecked()){
            ui->stackedWidget->setCurrentIndex(0);
        }
        ui->actionCOM->setChecked(false);
        sltActionCOM(false);
        simGen=new SimGen();
        connect(simGen,&SimGen::sendData,this,&MainWindow::sltReceiveData);//模拟数据
    }else{
        if(simGen){
            disconnect(simGen,&SimGen::sendData,this,&MainWindow::sltReceiveData);
            delete simGen;
            simGen=nullptr;
        }
    }
}

void MainWindow::sltActionCOM(bool checked)
{
    if(checked){
        if(ui->actionHex->isChecked()){
            ui->stackedWidget->setCurrentIndex(1);
        }
        if(ui->actionGrap->isChecked()){
            ui->stackedWidget->setCurrentIndex(0);
        }
        ui->actionTest->setChecked(false);
        sltActionTest(false);
        retriverWgt->showWgt();
    }else{
        retriverWgt->stopCOM();
    }
}

void MainWindow::sltActionExit()
{
    exit(0);
}

void MainWindow::sltActionHex(bool checked)
{
    if(checked){//文本模式
        ui->stackedWidget->setCurrentIndex(1);
        ui->actionGrap->setChecked(false);
        ui->textHex->clear();
    }else{
        ui->stackedWidget->setCurrentIndex(0);
        ui->actionGrap->setChecked(true);
        ui->textHex->clear();
        }
}

void MainWindow::sltActionGraph(bool checked)
{
    if(checked){
        ui->stackedWidget->setCurrentIndex(0);
        ui->actionHex->setChecked(false);
    }else{
        ui->stackedWidget->setCurrentIndex(1);
        ui->actionHex->setChecked(true);
    }
}

void MainWindow::sltActionAbout()
{
    QString msg = QString("<h1>MindViewer</h1>"
                          "<h2>TGAM module tools</h2>"
                          "<h3>Author: JackeyLea</h3>"
                          "<h3>E-mail: 1768478912@qq.com</h3>"
                          "<h3>GitHub: https://github.com/JackeyLea/MindViewer</h3>"
                          "<h3>Gitee: https://gitee.com/JackeyLea/MindViewer</h3>"
                          "<h4>Do what you want but please obey the LGPL3 rules</h4>"
                          "<h4>And keep those message within application</h4>");
    QMessageBox::information(this, tr("About"), msg, QMessageBox::Ok);
}

void MainWindow::sltActionAboutQt()
{
    qApp->aboutQt();
}

void MainWindow::sltActionGithub()
{
    QDesktopServices::openUrl(QUrl("https://github.com/JackeyLea/MindViewer"));
}

void MainWindow::initUi()
{
    retriverWgt = new Retriver;

    ui->widgetAttention->setLabel("Attention");
    ui->widgetMeditation->setLabel("Meditation");
}

void MainWindow::initConnections()
{
    //实际串口数据
    connect(retriverWgt,&Retriver::rawData,this,&MainWindow::sltReceiveData);
    //模拟数据
    connect(ui->actionTest,&QAction::triggered,this,&MainWindow::sltActionTest);
    //串口连接
    connect(ui->actionCOM,&QAction::triggered,this,&MainWindow::sltActionCOM);
    //退出
    connect(ui->actionExit,&QAction::triggered,this,&MainWindow::sltActionExit);
    //16进制数据模式
    connect(ui->actionHex,&QAction::triggered,this,&MainWindow::sltActionHex);
    //图形模式
    connect(ui->actionGrap,&QAction::triggered,this,&MainWindow::sltActionGraph);
    //关于
    connect(ui->actionAbout,&QAction::triggered,this,&MainWindow::sltActionAbout);
    //关于qt
    connect(ui->actionAbout_Qt,&QAction::triggered,this,&MainWindow::sltActionAboutQt);
    //github
    connect(ui->actionGithub,&QAction::triggered,this,&MainWindow::sltActionGithub);
}

