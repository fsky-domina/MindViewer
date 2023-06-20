# MindViewer

神念科技脑波模块TGAM图像化显示工具

官方说明手册翻译：<a href="https://feater.top/tgam/translation-of-tgam-development-doc">TGAM开发手册翻译</a>

TGAM连接及测试：<a href="https://feater.top/tgam/setup-and-installation-of-tgam-module">神念科技TGAM模块组装测试</a>

本软件开发流程和使用介绍：<a href="https://feater.top/qt/mindviewer-tgam-module-graphic-application">MindViewer-TGAM模块数据图形化软件</a>

配置好TGAM模块，连接好蓝牙，然后打开本软件读取端口数据就可以看见数据显示了。

Qt 5.15.3 + QWT6.2.0 + Linux Mint 21.1

## v3.0

此版本打算换一个抄

<img src="/img/mv3.webp" alt="新版本" />

提升科技感\^_\^，同时提高稳定性。

图片左侧的同心圆无法实现，只能两个结合。

<img src="/img/mymv3.webp" alt="v3.0" />

在2.0版本中位于下方的冥想+注意力+信号强度+电源指示折线图，变为冥想注意力指示器、电源变为手机电源格式、信号强度变为手机信号强度格式

## v2.0

勉强可用。

绘图效果：<img src="/img/mymv2.webp" alt="result" />

当时是抄的这个

<img src="/img/mv2.webp" alt="result" />

## v1.0

初步成型

## 说明

- 在官方的API中没有给出眨眼（blink）/走神程度的接口，所以其一直为0.
- 每发511个包之后会发送一个EEG包，如果每10毫秒发送一个包，那么就得5秒才会收到一个eeg包
- 在每个有效数据包末尾添加随机个数的随机数据作为干扰，经过测试没有问题，不知道一个TGAM数据包内会不会有干扰数据，未经测试，如果有，可以提出来
- TGAM模块是每秒发送512个数据包，也就是每2毫秒有一个数据包，经过测试，本程序可以正常显示，但是程序线程会飙到100%，风扇开始转。而且肉眼也跟不上数据，所以在界面上进行了滤波处理