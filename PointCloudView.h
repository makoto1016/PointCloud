#ifndef POINTCLOUDVIEW_H
#define POINTCLOUDVIEW_H

#include <QGLWidget>
#include <opencv2/opencv.hpp>
#include <qglviewer.h>

using namespace cv;
class Viewer : public QGLViewer
{

public:
    Viewer(Mat *rgb, Mat *cloud) {rgbImg = rgb, pointCloud = cloud; }
protected :
    virtual void draw();
    virtual void init();
    virtual void animate();

private:
    GLuint id, DisplayList;
    QTimer timer;
    Mat *rgbImg, *pointCloud;
    VideoCapture capture;
    qglviewer::ManipulatedFrame* light1;
    qglviewer::ManipulatedFrame* light2;

};


#endif // POINTCLOUDVIEW_H
