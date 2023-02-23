#DEFINES += USE_QTAV
DEFINES += TESSERACT_V5

mac {
    T_PATH_OPENCV = $$system_path(${QT_PATH_OPENCV})
    T_PATH_TESSERACT = $$system_path(${QT_PATH_TESSERACT})
    T_PATH_LEPTONICA = $$system_path(${QT_PATH_LEPTONICA})

    contains(DEFINES,USE_QTAV) {
        T_PATH_QTAV_LIB = $$system_path(${QT_PATH_QTAV_LIB})
        T_PATH_FFMPEG = $$system_path(${QT_PATH_FFMPEG})

        LIBS += -F$${T_PATH_QTAV_LIB}
        LIBS += -framework QtAV
        INCLUDEPATH += $${T_PATH_QTAV_LIB}/QtAV.framework/Headers

        LIBS += -L$${T_PATH_QTAV_LIB}
        LIBS += -lcommon

        INCLUDEPATH += $${T_PATH_FFMPEG}/include
        LIBS += -L$${T_PATH_FFMPEG}/lib -lavcodec -lavfilter -lavdevice -lavutil -lswscale -lavformat -lswresample

        LIBS += -framework QtAVWidgets
        INCLUDEPATH += $${T_PATH_QTAV_LIB}/QtAVWidgets.framework/Headers
    }
    else {
    }

    INCLUDEPATH += $${T_PATH_OPENCV}/include/opencv4/opencv2
    INCLUDEPATH += $${T_PATH_OPENCV}/include/opencv4
    INCLUDEPATH += $${T_PATH_OPENCV}/include
    LIBS += -L$${T_PATH_OPENCV}/lib \
     -lopencv_core \
     -lopencv_highgui \
     -lopencv_imgproc \
     -lopencv_imgcodecs \
     -lopencv_videoio \

    INCLUDEPATH += $${T_PATH_TESSERACT}/include
    LIBS += -L$${T_PATH_TESSERACT}/lib -ltesseract

    INCLUDEPATH += $${T_PATH_LEPTONICA}/include
    LIBS += -L$${T_PATH_LEPTONICA}/lib -llept
}
win32 {
    contains(DEFINES,USE_QTAV) {
        T_PATH_QTAV_LIB = $$system_path(${QT_PATH_QTAV_LIB})
        LIBS += $${T_PATH_QTAV_LIB}/Qt5AV.lib
        LIBS += $${T_PATH_QTAV_LIB}/Qt5AVWidgets.lib
    }
    else {
    }

    T_PATH_OPENCV = $$system_path(${QT_PATH_OPENCV})
    CONFIG (debug, debug|release) {
        INCLUDEPATH += $${T_PATH_OPENCV}/Debug/include
        LIBS += $${T_PATH_OPENCV}/Debug/lib/opencv_core400d.lib
        LIBS += $${T_PATH_OPENCV}/Debug/lib/opencv_highgui400d.lib
        LIBS += $${T_PATH_OPENCV}/Debug/lib/opencv_imgcodecs400d.lib
        LIBS += $${T_PATH_OPENCV}/Debug/lib/opencv_imgproc400d.lib
        LIBS += $${T_PATH_OPENCV}/Debug/lib/opencv_videoio400d.lib
    }
    else {
        INCLUDEPATH += $${T_PATH_OPENCV}/Release/include
        LIBS += $${T_PATH_OPENCV}/Release/lib/opencv_world420.lib
    }
}
unix {
    contains(DEFINES,USE_QTAV) {
    }
    else {
    }

    INCLUDEPATH += /usr/include/opencv4/opencv2
    INCLUDEPATH += /usr/include/opencv4
    INCLUDEPATH += /usr/include
    LIBS += -L/opt/opencv/lib \
     -lopencv_core \
     -lopencv_highgui \
     -lopencv_imgproc \
     -lopencv_imgcodecs \
     -lopencv_videoio \

    INCLUDEPATH += /usr/include/tesseract
    LIBS += -L/usr/lib/aarch64-linux-gnu -ltesseract

    INCLUDEPATH += /usr/include/leptonica
    LIBS += -L/usr/lib/aarch64-linux-gnu -llept
}

QT       += core gui serialport qml multimedia multimediawidgets network

contains(DEFINES,USE_QTAV) {
    QT += av avwidgets
}

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

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    gamepadbtn.cpp \
    main.cpp \
    gamepad.cpp \
    minitool.cpp \
    qtavplayer.cpp \
    scriptengine.cpp \
    scriptengineevaluation.cpp \
    serialport.cpp \
    setting.cpp \
    socket.cpp \
    utils.cpp \
    videocapture.cpp

HEADERS += \
    gamepad.h \
    gamepadbtn.h \
    minitool.h \
    qtavplayer.h \
    scriptengine.h \
    scriptengineevaluation.h \
    serialport.h \
    setting.h \
    socket.h \
    utils.h \
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


mac {
    QMAKE_INFO_PLIST = Info.plist
#    QMAKE_TARGET_BUNDLE_PREFIX = cn.maaya
#    QMAKE_BUNDLE = chat
}

RESOURCES += \
    resource.qrc
