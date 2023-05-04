#include "mainwindow.h"
#include "ui_mainwindow.h"

///////////////////////////以下内容来自官方参考代码//////////////////////////////////////

/* Decoder states (Packet decoding) */
#define PARSER_STATE_NULL           0x00  /* NULL state */
#define PARSER_STATE_SYNC           0x01  /* Waiting for SYNC byte */
#define PARSER_STATE_SYNC_CHECK     0x02  /* Waiting for second SYNC byte */
#define PARSER_STATE_PAYLOAD_LENGTH 0x03  /* Waiting for payload[] length */
#define PARSER_STATE_CHKSUM         0x04  /* Waiting for chksum byte */
#define PARSER_STATE_PAYLOAD        0x05  /* Waiting for next payload[] byte */

/* Decoder states (2-byte raw decoding) */
#define PARSER_STATE_WAIT_HIGH      0x06  /* Waiting for high byte */
#define PARSER_STATE_WAIT_LOW       0x07  /* High r'cvd.  Expecting low part */

/* Other constants */
#define PARSER_SYNC_BYTE            0xAA  /* Syncronization byte */
#define PARSER_EXCODE_BYTE          0x55  /* EXtended CODE level byte */

/////////////////////////////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->actionGraph->setChecked(true);//默认使用图形模式
    resize(800,820);

    mBuff.clear();

    retriverWgt = new Retriver;

    connect(retriverWgt,&Retriver::rawData,this,&MainWindow::sltReceiveData);//实际串口数据
    //每30ms刷新一次界面
    QTimer timer;
    timer.setInterval(30);
    connect(&timer,&QTimer::timeout,[=](){
        update();
    });

    ui->graphEEG->setGeometry(0,0,800,532);
    ui->graphCommon->setGeometry(0,532,800,266);
}

MainWindow::~MainWindow()
{
    if(gen!=nullptr){//停止模拟数据
        delete gen;
    }
    delete retriverWgt;
    delete ui;
}

//使用状态机解析原始数据
int MainWindow::parserData(QByteArray ba, bool &raw, short &rawValue, bool &common, bool &eeg, struct _eegPkt &pkt)
{
    //输入的数据ba只包含一个有效包
    //qDebug()<<"parse start";
    raw=false;//此数据包是否包含原始数据
    eeg=false;//此数据包是否包含eeg数据
    common=false;//此数据包是否包含注意力/冥想等数据

    if(ba.isEmpty()) return -1;//如果没有数据就直接退出

    //qDebug()<<"add to buff."<<ba.size();
    QByteArray buff = ba;//将数据添加到处理缓冲区

    /////////////////处理开始/////////////////////////////////////
    int cnt=0;
    uchar state=PARSER_STATE_SYNC;
    uchar payloadLength=0;
    uchar payloadSum=0;
    while(buff.size()){
        //qDebug()<<buff;
        //状态机处理
        switch(state){
        case PARSER_STATE_SYNC://第一个同步字节
            if((uchar)buff[0]==PARSER_SYNC_BYTE){
                //qDebug()<<"parser first aa "<<i<<(uchar)buff[i];
                state=PARSER_STATE_SYNC_CHECK;
            }
            buff.remove(0,1);
            break;
        case PARSER_STATE_SYNC_CHECK:
            if((uchar)buff[0]==PARSER_SYNC_BYTE){//包第二个0xaa
                //qDebug()<<"parser second aa "<<i<<(uchar)buff[i];
                state=PARSER_STATE_PAYLOAD_LENGTH;//准备解析负载长度
                buff.remove(0,1);
            }
            break;
        case PARSER_STATE_PAYLOAD_LENGTH:
            payloadLength=(uchar)buff[0];//接下来是长度
            //qDebug()<<"payloadlength"<<payloadLength;
            if(payloadLength>=170 || payloadLength<=0){//如果长度大于170就丢弃此包并查找下一个0xaa 0xaa
                qDebug()<<"this package payloadLength(the 3rd value) is wrong";
                state = PARSER_STATE_SYNC;
                //如果是0xaa 0xaa 0xff，包头对了但是长度不对
            }else{
                //qDebug()<<"parser payload length "<<i<<(uchar)buff[i];
                state=PARSER_STATE_CHKSUM;//准备解析有效数据
            }
            buff.remove(0,1);
            break;
        case PARSER_STATE_CHKSUM:
        {
            //qDebug()<<"check sum"<<payloadLength+1<<buff.size();
            //如果剩余大小不够 1是末尾的校验值 3是包头和大小
            if(payloadLength+1 >buff.size()){
                qDebug()<<"pkg is not valid.";
                state = PARSER_STATE_SYNC;
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
                    common = false;
                    state = PARSER_STATE_SYNC;
                    break;
                }
                common=true;
                pkt.power=(uchar)buff[1];
                state=PARSER_STATE_PAYLOAD;
                buff.remove(0,2);
                cnt+=2;
            }else if((uchar)buff[0]==0x02){//数据信号强度值
                //如果缓冲区大小小于2位
                if(buff.size() < 2){
                    common= false;
                    state = PARSER_STATE_SYNC;
                    break;
                }
                //qDebug()<<"signal value "<<(uchar)buff[1];
                common=true;
                pkt.signal=(uchar)buff[1];
                state=PARSER_STATE_PAYLOAD;
                buff.remove(0,2);
                cnt+=2;
            }else if((uchar)buff[0]==0x03){
                //ego上的心跳强度0-255
            }else if((uchar)buff[0]==0x04){//注意力值
                //如果缓冲区大小小于2位
                if(buff.size() < 2){
                    common = false;
                    state = PARSER_STATE_SYNC;
                    break;
                }
                //qDebug()<<"attention value "<<(uchar)buff[1];
                common=true;
                pkt.attention=(uchar)buff[1];
                state=PARSER_STATE_PAYLOAD;
                buff.remove(0,2);
                cnt+=2;
            }else if((uchar)buff[0]==0x05){//冥想值
                //如果缓冲区大小小于2位
                if(buff.size() < 2){
                    common = false;
                    state = PARSER_STATE_SYNC;
                    break;
                }
                //qDebug()<<"meditation value "<<(uchar)buff[1];
                common=true;
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
                    raw = false;
                    state = PARSER_STATE_SYNC;
                    break;
                }
                raw=true;
                rawValue=((uchar)buff[2]<<8)|(uchar)buff[3];
                buff.remove(0,5);//4个数据以及最后的校验值
                break;
            }else if((uchar)buff[0]==0x81 && (uchar)buff[1]==0x20){
                //eeg_power 8个大端四字节
            }else if((uchar)buff[0]==0x83 && (uchar)buff[1]==0x18){//eeg数据部分
                //0x83标志eeg部分开始，下一位表示为eeg部分程度默认为0x18,8个大端三字节
                //qDebug()<<"parser eeg data "<<i<<buff[i];
                //qDebug()<<"parser eeg length "<<(uchar)buff[i+1];
                //如果缓冲区大小小于24位
                if(buff.size() < 26){
                    eeg = false;
                    state = PARSER_STATE_SYNC;
                    break;
                }
                eeg=true;
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

void MainWindow::resizeEvent(QResizeEvent */*event*/)
{
    int width = ui->stackedWidget->width();
    int height = ui->stackedWidget->height();
    if(height <700) return;//窗口必须大于700
    ui->graphEEG->setGeometry(0,0,width,height*2/3);
    ui->graphCommon->setGeometry(0,height*2/3,width,height/3);
    update();
}

//接收原始数据
void MainWindow::sltReceiveData(QByteArray ba)
{
    //qDebug()<<"receive 1 "<<ba;
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
            //一个包最起码包含一个有效数据类型0xaa 0xaa 0x02 0xaa 0xaa
            return;//此时包肯定不完整，就结束
        }else{//有可能一次收的数据不完整先判断
            while(mBuff.size()){
                if(mBuff[0]==0xAA && mBuff[1]==0xAA){//先找包头
                    //包大小
                    int pkgSize = mBuff[2];
                    if(pkgSize + 2 + 1+ 1 > mBuff.size()){
                        qDebug()<<"pkg is less than given size.";
                        return;
                    }else{
                        //此时继续解析
                        break;
                    }
                }else{
                    //如果前两个不是0xAA 0xAA就向后移一位
                    mBuff.remove(0,1);
                }
            }
        }

        //一包有效数据
        //qDebug()<<"valid pkg";

        //提取有效数据
        while(mBuff.size()>=5){
            //这里用while是考虑缓冲区可能有不止一个包
            //有一种特殊情况 0xaa 0xaa 0xaa
            if(mBuff[0]==0xaa && mBuff[1]==0xaa && mBuff[2]==0xaa){
                mBuff.remove(0,1);
                continue;
            }
            if(mBuff[0]!=(uchar)0xAA || mBuff[1]!=(uchar)0xAA ){
                //qDebug()<<"next pkg"<<mBuff;
                //删除一个直到符合
                mBuff.remove(0,1);
                continue;
            }
            //第3个字节是长度
            int length = mBuff[2];
            QByteArray tmpBA = mBuff.mid(0,length+2+1+1);
            //qDebug()<<"valid pkg"<<tmpBA;
            //从缓冲区删除已经解析的包
            //qDebug()<<"before delete"<<mBuff;
            mBuff.remove(0,length+4);
            //qDebug()<<"after delete"<<mBuff;
            //continue;
            //解析函数一次只解析一个包
            {
                bool raw,eeg,common;
                short rawValue;
                struct _eegPkt pkt;
                pkt.init();
                if(parserData(tmpBA,raw,rawValue,common,eeg,pkt)!=0){
                    qDebug()<<"Cannot parse data.";
                    return;
                }
                //qDebug()<<"parsered";
                if(common){
                    ui->graphCommon->updateCommonData(pkt);//将数据发送到界面
                    if(!isEEGResized){//绘图之后需要手动调整窗口大小，我们自己调整一下
                        resize(ui->stackedWidget->currentWidget()->size().width()+40,ui->stackedWidget->currentWidget()->size().height());
                        isEEGResized=true;
                    }
                }
                //qDebug()<<"eeg";
                if(eeg){
                    ui->graphEEG->updateEEGData(pkt);//将数据发送到界面
                    if(!isEEGResized){//绘图之后需要手动调整窗口大小，我们自己调整一下
                        resize(ui->stackedWidget->currentWidget()->size().width()+40,ui->stackedWidget->currentWidget()->size().height());
                        isEEGResized=true;
                    }
                }
                //qDebug()<<"raw";
                if(raw){
                    ui->graphEEG->updateRawData(rawValue);
                    if(!isCommonResized){
                        resize(ui->stackedWidget->currentWidget()->size().width()+40,ui->stackedWidget->currentWidget()->size().height());
                        isCommonResized=true;
                    }
                }
            }
        }
        //qDebug()<<"mBuff"<<mBuff;
    }
}
