#include "PointCloudView.h"
#include <QDebug>

void PointCloudViewer::draw()
{
    glCallList(background);

    glTranslatef(0, 0, 1.6);
    glScalef(0.4, 0.4, -0.4);

    drawPointCloud();
    drawUserSkeltons();
}

void PointCloudViewer::drawUserSkeltons()
{
    QMapIterator<unsigned int, Skelton> itr(*userSkeltons);

    while(itr.hasNext())
    {
        itr.next();
        QMap<unsigned int, Point3f> skelton(itr.value());
        Point3f p;
        glColor3b(0,0,0);
        glBegin(GL_LINE_LOOP);
        p = skelton.value(XN_SKEL_TORSO);
        glVertex3f(p.x, p.y, p.z);
        p = skelton.value(XN_SKEL_NECK);
        glVertex3f(p.x, p.y, p.z);
        p = skelton.value(XN_SKEL_HEAD);
        glVertex3f(p.x, p.y, p.z);
        glEnd();
        glBegin(GL_LINE_LOOP);
        p = skelton.value(XN_SKEL_LEFT_SHOULDER);
        glVertex3f(p.x, p.y, p.z);
        p = skelton.value(XN_SKEL_LEFT_ELBOW);
        glVertex3f(p.x, p.y, p.z);
        p = skelton.value(XN_SKEL_LEFT_HAND);
        glVertex3f(p.x, p.y, p.z);
        glEnd();
        glBegin(GL_LINE_LOOP);
        p = skelton.value(XN_SKEL_RIGHT_SHOULDER);
        glVertex3f(p.x, p.y, p.z);
        p = skelton.value(XN_SKEL_RIGHT_ELBOW);
        glVertex3f(p.x, p.y, p.z);
        p = skelton.value(XN_SKEL_RIGHT_HAND);
        glVertex3f(p.x, p.y, p.z);
        glEnd();
        glBegin(GL_LINE_LOOP);
        p = skelton.value(XN_SKEL_LEFT_HIP);
        glVertex3f(p.x, p.y, p.z);
        p = skelton.value(XN_SKEL_LEFT_KNEE);
        glVertex3f(p.x, p.y, p.z);
        p = skelton.value(XN_SKEL_LEFT_FOOT);
        glVertex3f(p.x, p.y, p.z);
        glEnd();
        glBegin(GL_LINE_LOOP);
        p = skelton.value(XN_SKEL_RIGHT_HIP);
        glVertex3f(p.x, p.y, p.z);
        p = skelton.value(XN_SKEL_RIGHT_KNEE);
        glVertex3f(p.x, p.y, p.z);
        p = skelton.value(XN_SKEL_RIGHT_FOOT);
        glVertex3f(p.x, p.y, p.z);
        glEnd();

    }
}

void PointCloudViewer::drawPointCloud()
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBufferARB(GL_ARRAY_BUFFER, buffers[0]);
    glBufferDataARB(GL_ARRAY_BUFFER, sizeof(float)*pointCloud->cols*pointCloud->rows*3, pointCloud->ptr(), GL_DYNAMIC_DRAW);
    glVertexPointer(3 , GL_FLOAT , 0 , 0);
    glBindBufferARB(GL_ARRAY_BUFFER, buffers[1]);
    glTexCoordPointer(2, GL_FLOAT, 0, 0);
    glEnable(GL_TEXTURE_2D);
    glDrawArrays(GL_POINTS, 0, pointCloud->cols * pointCloud->rows);
    glDisable(GL_TEXTURE_2D);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void PointCloudViewer::mouseMoveEvent(QMouseEvent *event)
{
    bool found;
    cursorPos = camera()->pointUnderPixel(event->pos(), found);
    QGLViewer::mouseMoveEvent(event);
    if(found)
    {
        cursorText = QString().sprintf("CursorPos:\tx→%+11.7f  y→%+11.7f  z→%+11.7f", cursorPos.x, cursorPos.y, cursorPos.z);
    }
    cameraPosText = QString().sprintf("CameraPos:\tx→%+11.7f  y→%+11.7f  z→%+11.7f", camera()->position().x, camera()->position().y, camera()->position().z);
    constructDebugText();

}
void PointCloudViewer::constructDebugText()
{
    QString text;
    text += QString::number(currentFPS(),'f', 1) + "FPS\n";
    text += cursorText;
    text += "\n";
    text += cameraPosText;
    emit debugTextChanged(text);

}

void PointCloudViewer::animate()
{

    deleteTexture(texid);
    texid = bindTexture(QImage(rgbImg->ptr(), rgbImg->cols, rgbImg->rows, QImage::Format_RGB888), GL_TEXTURE_2D, GL_RGB, QGLContext::LinearFilteringBindOption);

}

void PointCloudViewer::initBackground()
{
    background = glGenLists(1);
    glNewList(background, GL_COMPILE);
    ::glMatrixMode(GL_MODELVIEW);
    ::glPushMatrix();
    ::glLoadIdentity();
    ::glMatrixMode(GL_PROJECTION);
    ::glPushMatrix();
    ::glLoadIdentity();
    ::glDisable(GL_DEPTH_TEST);
    ::glBegin(GL_QUADS);
    ::glColor3d(0.5,0.7,0.7);
    ::glVertex3d(-1,-1,0);
    ::glVertex3d( 1,-1,0);
    ::glColor3d(1,1,1);
    ::glVertex3d( 1, 1,0);
    ::glVertex3d(-1, 1,0);
    ::glEnd();
    ::glEnable(GL_DEPTH_TEST);
    ::glMatrixMode(GL_PROJECTION);
    ::glPopMatrix();
    ::glMatrixMode(GL_MODELVIEW);
    ::glPopMatrix();
    glEndList();
}

void PointCloudViewer::initLights()
{

    float lighton[] = {1,1,1,1};
    float lightoff[] = {0,0,0,0};
    glLightfv(GL_LIGHT0, GL_AMBIENT, lighton);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightoff);
    //glLightfv(GL_LIGHT0, GL_SPECULAR, lightoff);

}

void PointCloudViewer::initBuffers()
{
    glGenBuffers(2, buffers);
    const float dx = 1/(float)pointCloud->cols;
    const float dy = 1/(float)pointCloud->rows;

    texcoord = new float[pointCloud->rows*pointCloud->cols*2];
    for(int y = 0; y < rgbImg->rows; y++)
    {
        for(int x = 0; x < pointCloud->cols; x++)
        {
            texcoord[y*pointCloud->cols*2 + x*2 + 0] = dx*x;
            texcoord[y*pointCloud->cols*2 + x*2 + 1] = dy*y;
        }
    }
    glBindBufferARB(GL_ARRAY_BUFFER, buffers[1]);
    glBufferDataARB(GL_ARRAY_BUFFER, sizeof(float) * pointCloud->cols*pointCloud->rows*2, texcoord, GL_STATIC_DRAW);
}

void PointCloudViewer::init()
{
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glPointSize(3);
    glLineWidth(10);
    setMouseTracking(true);


    texid = bindTexture(QImage(rgbImg->ptr(), rgbImg->cols, rgbImg->rows, QImage::Format_RGB888), GL_TEXTURE_2D, GL_RGB, QGLContext::LinearFilteringBindOption);
    initBackground();
    initLights();
    initBuffers();

    restoreStateFromFile();
    setAnimationPeriod(10);
    startAnimation();
}

PointCloudViewer::~PointCloudViewer()
{
    glDeleteBuffers(2, buffers);
    delete [] texcoord;
}
