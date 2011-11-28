#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "PointCloudView.h"
#include <opencv2/opencv.hpp>
//#include "NiUserTracker.h"
#include <QTimer>

using namespace cv;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    enum DispMode {
        RGBIMAGE,
        DEPTHIMAGE
    };

private slots:
    void updateImage();
    void setRGBMode() { mode = RGBIMAGE; depthModeAction->setChecked(false); rgbModeAction->setChecked(true); }
    void setDepthMode() { mode = DEPTHIMAGE; rgbModeAction->setChecked(false); depthModeAction->setChecked(true); }
private:
    NiUserTracker tracker;
    DispMode mode;
    QTextEdit debugEdit;
    VideoCapture capture;
    Mat textureImg, pointCloud, depth;
    QMap<unsigned int, Skelton > skeltons;
    Mat s, v, hsv;
    std::vector<Mat> mvec;
    PointCloudViewer *viewer;
    QTimer timer;
    QToolBar *mainbar;
    QAction *rgbModeAction, *depthModeAction;
    QAction *userAction;

    bool isCalibPose();
    void setupUI();
    void setupActions();
};

#endif // MAINWINDOW_H
