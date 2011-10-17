#-------------------------------------------------
#
# Project created by QtCreator 2011-09-18T20:09:12
#
#-------------------------------------------------

QT       += core gui opengl xml

TARGET = PointCloud
TEMPLATE = app

LIBS += -L/Users/oonomakoto/OpenCV-2.3.0/lib -lopencv_core -lopencv_imgproc -lopencv_highgui  -lopencv_calib3d -lopencv_video -lopencv_features2d -lopencv_flann \
        -L/Users/oonomakoto/cvblob/lib -lcvblob \
                -L/Users/oonomakoto/klt/ -lklt



INCLUDEPATH += /Library/Frameworks/QGLViewer.framework/Headers

LIBS += -L/Library/Frameworks -framework QGLViewer


INCLUDEPATH += /Users/oonomakoto/OpenCV-2.3.0/include \
                /Users/oonomakoto/klt/ \

SOURCES += main.cpp\
        MainWindow.cpp \
    PointCloudView.cpp

HEADERS  += MainWindow.h \
    PointCloudView.h


