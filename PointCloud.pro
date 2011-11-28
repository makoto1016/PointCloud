#-------------------------------------------------
#
# Project created by QtCreator 2011-09-18T20:09:12
#
#-------------------------------------------------

QT       += core gui opengl xml

TARGET = PointCloud
TEMPLATE = app

macx {
LIBS += -L/Users/oonomakoto/OpenCV-2.3.0/lib -lopencv_core -lopencv_imgproc -lopencv_highgui  -lopencv_calib3d -lopencv_video -lopencv_features2d -lopencv_flann \
        -L/Users/oonomakoto/cvblob/lib -lcvblob -L/Users/oonomakoto/kinect/OpenNI-Bin-MacOSX-v1.3.2.1/Lib -lOpenNI -lnimCodecs\
        -L/Library/Frameworks -framework QGLViewer



INCLUDEPATH += /Library/Frameworks/QGLViewer.framework/Headers /Users/oonomakoto/kinect/OpenNI-Bin-MacOSX-v1.3.2.1/Include /Users/oonomakoto/OpenCV-2.3.0/include
}

win32 {
LIBS += "C:\Program Files\OpenNI\Lib\*.lib" C:\OpenCV2.3\build\x86\vc9\lib\*.lib QGLViewer2.lib glew32.lib

INCLUDEPATH += C:\OpenCV2.3\build\include "C:\Program Files\OpenNI\Include" C:\libQGLViewer-2.3.10\QGLViewer
}

SOURCES += main.cpp\
        MainWindow.cpp \
    PointCloudView.cpp \
    NiUserTracker.cpp

HEADERS  += MainWindow.h \
    PointCloudView.h \
    NiUserTracker.h








