#ifndef NIUSERTRACKER_H
#define NIUSERTRACKER_H

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <opencv2/opencv.hpp>
#include <QObject>
#include <QMap>
#include <QList>


using namespace cv;
typedef QMap<unsigned int, Point3f> Skelton;
#define SAMPLE_XML_PATH "/Users/oonomakoto/kinect/OpenNI-Bin-MacOSX-v1.3.2.1/Samples/Config/SamplesConfig.xml"
//#define SAMPLE_XML_PATH "C:\Program Files\OpenNI\Data\SamplesConfig.xml"


class NiUserTracker : public QObject
{
    Q_OBJECT
public:
    NiUserTracker();
    ~NiUserTracker();
    xn::Context g_Context;
    xn::ScriptNode g_scriptNode;
    xn::DepthGenerator g_DepthGenerator;
    xn::UserGenerator g_UserGenerator;
    xn::ImageGenerator g_ImageGenerator;
    xn::Player g_Player;
    XnBool g_bNeedPose;
    XnChar g_strPose[20];
    XnBool g_bDrawBackground;
    XnBool g_bDrawPixels;
    XnBool g_bDrawSkeleton;
    XnBool g_bPrintID;
    XnBool g_bPrintState;
    XnBool g_bPause;
    XnBool g_bRecord;
    xn::SceneMetaData sceneMD;
    xn::DepthMetaData depthMD;
    xn::ImageMetaData imageMD;
    int uSize;
    int vSize;
    float uCenter;
    float vCenter;
    float P2R;
    Mat pointCloud;
    static const int nColors  = 10;

    XnBool g_bQuit;

    enum IMAGETYPE {
        RGB8U,
        DEPTH8U,
        DEPTH16U,
        POINTCLOUD,
        USERLABELS
    };

    void grab() { g_Context.WaitAnyUpdateAll();}
    void retrieve(Mat &img, IMAGETYPE type);
    void retrieveSkeltons(QMap<unsigned int, Skelton > &skeltons);
private:
    Point3f Pos(XnUserID player,XnSkeletonJoint ejoint);
    void depth2point( Mat &img );
    void renderUsers( Mat &img );

};

#endif // NIUSERTRACKER_H
