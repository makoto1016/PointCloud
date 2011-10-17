#include "PointCloudView.h"
#include <QDebug>



float flabs(float p)
{
    return p > 0 ? p : -p;
}

PointCloudView::PointCloudView()
    : QGLWidget(), mouseXAngle(0.0), mouseYAngle(0.0), mouseFlag(GL_FALSE), mouseXDis(0.0)
{
    setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));
    capture.open(CV_CAP_OPENNI);
    capture.grab();
    capture.retrieve(pointCloud, CV_CAP_OPENNI_POINT_CLOUD_MAP);
    capture.retrieve(bgrImg, CV_CAP_OPENNI_BGR_IMAGE);
    cvtColor(bgrImg, rgbImg, CV_BGR2RGB);
    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_BGR,rgbImg.cols,rgbImg.rows,GL_RGB,GL_UNSIGNED_BYTE,rgbImg.ptr<unsigned char>(0));
}


void PointCloudView::updateFrame()
{
    capture.grab();
    capture.retrieve(pointCloud, CV_CAP_OPENNI_POINT_CLOUD_MAP);
    capture.retrieve(bgrImg, CV_CAP_OPENNI_BGR_IMAGE);
    cvtColor(bgrImg, rgbImg, CV_BGR2RGB);
    glBindTexture(GL_TEXTURE_2D, texid);
    GLint num = gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB,rgbImg.cols,rgbImg.rows,GL_RGB,GL_UNSIGNED_BYTE,rgbImg.ptr<unsigned char>(0));


    updateGL();

}


void PointCloudView::PolarCoordinatesLookAt(double r, double theta, double phi, double cx, double cy, double cz)
{
    double x, y, z;

    //球座標から直交座標に変換
    x = r*sin(phi)*cos(theta) + cx;
    y = r*sin(phi)*sin(theta) + cy;
    z = r*cos(phi) + cz;

    //z軸が上方向
    gluLookAt(x, y, z, cx, cy, cz, 0.0, 1.0, 0.0);
}

void PointCloudView::initializeGL()
{
    qglClearColor(Qt::black);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void PointCloudView::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
}


void PointCloudView::draw()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    PolarCoordinatesLookAt(mouseXDis, mouseXAngle, mouseYAngle, 0.0, 0.0, 0.3);
    glEnable(GL_TEXTURE_2D);

    float dx = 1/(float)bgrImg.cols;
    float dy = 1/(float)bgrImg.rows;
    Point3f p3, p4;
    for(int y = 0; y < pointCloud.rows-1; y++)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for(int x = 0; x < pointCloud.cols; x++)
        {
            Point3f p1 = pointCloud.at<Point3f>(y, x);
            Point3f p2 = pointCloud.at<Point3f>(y+1, x);
            if(p3.z == 0 && p4.z == 0) {p3 = p1; p4 = p2;}
            float dis = 0.03;
            if(p1.z == 0 || p2.z == 0 || flabs(p1.z-p2.z) > dis || flabs(p2.z-p3.z) > dis || flabs(p3.z-p4.z) > dis || flabs(p1.z-p4.z) > dis)
            {
                glEnd();
                glBegin(GL_TRIANGLE_STRIP);
                p3 = p1;
                p4 = p2;
                continue;
            }
            glTexCoord2d(dx*x, dy*y);
            glVertex3f(p1.x, p1.y, p1.z);
            glTexCoord2d(dx*x, dy*(y+1));
            glVertex3f(p2.x, p2.y, p2.z);
            p3 = p1;
            p4 = p2;
        }
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);

}

void PointCloudView::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw();
}

void PointCloudView::mouseMoveEvent(QMouseEvent *event)
{

    int xdis, ydis;

    switch(mouseFlag){
        case 0x100: //GLUT_LEFT_BUTTON
            xdis = event->pos().x() - postpos.x();
            ydis = event->pos().y() - postpos.y();

            mouseXAngle -= (double)xdis * 0.002;
            mouseYAngle -= (double)ydis * 0.002;

            break;

        case 0x001: //GLUT_RIGHT_BUTTON
            xdis = event->pos().x() - postpos.x();

            mouseXDis += (double)xdis * 0.002;
            break;
    }

    postpos = event->pos();
    qWarning() << mouseXAngle << mouseYAngle << mouseXDis;


}

void PointCloudView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        mouseFlag = 0x100;
        postpos = event->pos();
    }
    else if(event->button() == Qt::RightButton)
    {
        mouseFlag = 0x001;
        postpos = event->pos();
    }


}

