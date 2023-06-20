####################
### JackeyLea
### 2023.04.27
### 用于图形化显示TGAM模块获取的数据
####################

QT       += core gui
QT       += serialport #COM支持

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#临时编译文件路径
OBJECTS_DIR = .obj
RCC_DIR = .rcc
UI_DIR = .ui
MOC_DIR = .moc

#Windows平台配置
#将qwt库文件和头文件路径添加到此处
win32:{
    INCLUDEPATH += $$PWD/qwt/include
    LIBS += -L$$PWD/qwt/lib -lqwt
}

#Linux下头文件以及库文件 qwt6.2.0
unix:{
    INCLUDEPATH += $$PWD/../qwt/include

    LIBS += -L$$PWD/../qwt/lib -lqwt
}

SOURCES += \
    curves.cpp \
    main.cpp \
    mainwindow.cpp \
    powerviewer.cpp \
    retriver.cpp \
    signaldataviewer.cpp \
    simgen.cpp \
    valueindicator.cpp

HEADERS += \
    common.h \
    curves.h \
    mainwindow.h \
    powerviewer.h \
    retriver.h \
    signaldataviewer.h \
    simgen.h \
    valueindicator.h

FORMS += \
    curves.ui \
    mainwindow.ui \
    retriver.ui

RESOURCES += \
    resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
