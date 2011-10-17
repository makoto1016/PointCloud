#include "PointCloudView.h"
#include <QDebug>

using namespace std;

float flabs(float p)
{
    return p > 0 ? p : -p;
}

void Viewer::draw()
{
    glEnable(GL_TEXTURE_2D);
    float dx = 1/(float)rgbImg->cols;
    float dy = 1/(float)rgbImg->rows;
    Point3f p3, p4;

    for(int y = 0; y < pointCloud->rows-1; y++)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for(int x = 0; x < pointCloud->cols; x++)
        {
            Point3f p1 = pointCloud->at<Point3f>(y, x);
            Point3f p2 = pointCloud->at<Point3f>(y+1, x);
            if(p3.z == 0 && p4.z == 0) {p3 = p1; p4 = p2;}
            float dis = 0.1;
            if(p1.z == 0 || p2.z == 0 || flabs(p1.z-p2.z) > dis || flabs(p2.z-p3.z) > dis || flabs(p3.z-p4.z) > dis || flabs(p1.z-p4.z) > dis)
            {
                glEnd();
                glBegin(GL_TRIANGLE_STRIP);
                p3 = p1;
                p4 = p2;
                continue;
            }
            glTexCoord2d(dx*x, dy*y);
            glVertex3f(p1.x, p1.y, -p1.z+3);
            glTexCoord2d(dx*x, dy*(y+1));
            glVertex3f(p2.x, p2.y, -p2.z+3);
            p3 = p1;
            p4 = p2;
        }
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
}


void Viewer::animate()
{
    deleteTexture(id);
    id = bindTexture(QImage(rgbImg->ptr(), rgbImg->cols, rgbImg->rows, QImage::Format_RGB888), GL_TEXTURE_2D, GL_RGB, QGLContext::LinearFilteringBindOption);
}


void Viewer::init()
{
    setBackgroundColor(Qt::darkGray);
    id = 0;
    bindTexture(QImage(rgbImg->ptr(), rgbImg->cols, rgbImg->rows, QImage::Format_RGB888), GL_TEXTURE_2D, GL_RGB, QGLContext::LinearFilteringBindOption);
    restoreStateFromFile();
    startAnimation();
}


