#ifndef POINTCLOUDVIEW_H
#define POINTCLOUDVIEW_H

#include <opencv2/opencv.hpp>
//#include <gl/glew.h>
#include <qglviewer.h>
#include <QMouseEvent>
#include "NiUserTracker.h"
#include <QMapIterator>

using namespace cv;
class PointCloudViewer : public QGLViewer
{
    Q_OBJECT
public:
    PointCloudViewer(Mat *rgb, Mat *cloud, QMap<unsigned int, Skelton> *skeltons)
        : rgbImg(rgb), pointCloud(cloud), userSkeltons(skeltons) {}
    ~PointCloudViewer();
protected:
    virtual void draw();
    virtual void init();
    virtual void animate();
    virtual void mouseMoveEvent(QMouseEvent *event);

signals:
    void debugTextChanged(QString text);

private:
    GLuint texid, background;
    GLuint buffers[2];
    float *texcoord;
    Mat *rgbImg, *pointCloud;
    QMap<unsigned int, Skelton> *userSkeltons;
    qglviewer::Vec cursorPos;
    QString cursorText, cameraPosText;

    void initBackground();
    void initLights();
    void initBuffers();
    void constructDebugText();
    void drawPointCloud();
    void drawUserSkeltons();

};


#endif // POINTCLOUDVIEW_H
