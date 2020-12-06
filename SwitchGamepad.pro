QT       += core gui serialport script multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

CONFIG (release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
}

#INCLUDEPATH += -I /usr/local/Cellar/python/3.7.6/Frameworks/Python.framework/Versions/3.7/Headers
#LIBS += -L /usr/local/Cellar/python/3.7.6/Frameworks/Python.framework/Versions/3.7/lib/python3.7/config-3.7m-darwin -lpython3.7

#INCLUDEPATH += /usr/local/Cellar/opencv/4.5.0_5/include/opencv4/opencv2
#INCLUDEPATH += /usr/local/Cellar/opencv/4.5.0_5/include/opencv4
#INCLUDEPATH += /usr/local/Cellar/opencv/4.5.0_5/include
#LIBS += -L/usr/local/Cellar/opencv/4.5.0_5/lib \
# -lopencv_core \
# -lopencv_highgui \
# -lopencv_imgproc \
# -lopencv_imgcodecs \
# -lopencv_videoio \

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    gamepadbtn.cpp \
    main.cpp \
    gamepad.cpp \
    minitool.cpp \
    scriptengine.cpp \
    scriptengineevaluation.cpp \
    serialport.cpp \
    setting.cpp \
    videocapture.cpp

HEADERS += \
    gamepad.h \
    gamepadbtn.h \
    minitool.h \
    scriptengine.h \
    scriptengineevaluation.h \
    serialport.h \
    setting.h \
    videocapture.h

FORMS += \
    gamepad.ui \
    minitool.ui

TRANSLATIONS += \
    SwitchGamepad_en_US.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
