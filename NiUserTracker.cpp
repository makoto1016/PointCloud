#include "NiUserTracker.h"
#include <QDebug>
// Callback: New user was detected

#include <map>
std::map<XnUInt32, std::pair<XnCalibrationStatus, XnPoseDetectionStatus> > m_Errors;
void XN_CALLBACK_TYPE MyCalibrationInProgress(xn::SkeletonCapability& capability, XnUserID id, XnCalibrationStatus calibrationError, void* pCookie)
{
    m_Errors[id].first = calibrationError;
}
void XN_CALLBACK_TYPE MyPoseInProgress(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID id, XnPoseDetectionStatus poseError, void* pCookie)
{
    m_Errors[id].second = poseError;
}

void XN_CALLBACK_TYPE User_NewUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
    NiUserTracker *ut = (NiUserTracker *)pCookie;

    qWarning() << "new";
    printf("New User %d\n", nId);
    // New user found
    if (ut->g_bNeedPose)
    {
        ut->g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(ut->g_strPose, nId);
    }
    else
    {
        ut->g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
    }
}
// Callback: An existing user was lost
void XN_CALLBACK_TYPE User_LostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
    qWarning() << "lost";
    printf("Lost user %d\n", nId);
}
// Callback: Detected a pose
void XN_CALLBACK_TYPE UserPose_PoseDetected(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie)
{
    NiUserTracker *ut = (NiUserTracker *)pCookie;

    qWarning() << "pose";
    printf("Pose %s detected for user %d\n", strPose, nId);
    ut->g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(nId);
    ut->g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
}
// Callback: Started calibration
void XN_CALLBACK_TYPE UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, XnUserID nId, void* pCookie)
{
    printf("Calibration started for user %d\n", nId);
}
// Callback: Finished calibration
void XN_CALLBACK_TYPE UserCalibration_CalibrationEnd(xn::SkeletonCapability& capability, XnUserID nId, XnBool bSuccess, void* pCookie)
{
    NiUserTracker *ut = (NiUserTracker *)pCookie;

    if (bSuccess)
    {
        // Calibration succeeded
        printf("Calibration complete, start tracking user %d\n", nId);
        ut->g_UserGenerator.GetSkeletonCap().StartTracking(nId);
    }
    else
    {
        // Calibration failed
        printf("Calibration failed for user %d\n", nId);
        if (ut->g_bNeedPose)
        {
            ut->g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(ut->g_strPose, nId);
        }
        else
        {
            ut->g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
        }
    }
}

void XN_CALLBACK_TYPE UserCalibration_CalibrationComplete(xn::SkeletonCapability& capability, XnUserID nId, XnCalibrationStatus eStatus, void* pCookie)
{
    NiUserTracker *ut = (NiUserTracker *)pCookie;
    if (eStatus == XN_CALIBRATION_STATUS_OK)
    {
        // Calibration succeeded
        printf("Calibration complete, start tracking user %d\n", nId);
        ut->g_UserGenerator.GetSkeletonCap().StartTracking(nId);
    }
    else
    {
        // Calibration failed
        printf("Calibration failed for user %d\n", nId);
        if (ut->g_bNeedPose)
        {
            ut->g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(ut->g_strPose, nId);
        }
        else
        {
            ut->g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
        }
    }
}

NiUserTracker::NiUserTracker()
    : g_bNeedPose(FALSE),
      g_bDrawBackground(TRUE),
      g_bDrawPixels(TRUE),
      g_bDrawSkeleton(TRUE),
      g_bPrintID(TRUE),
      g_bPrintState(TRUE),
      g_bPause(false),
      g_bRecord(false),
      g_bQuit(false)
{
    XnStatus nRetVal = XN_STATUS_OK;
    g_strPose[0] = '\0';

    nRetVal = g_Context.InitFromXmlFile(SAMPLE_XML_PATH);

    nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_IMAGE, g_ImageGenerator);
    nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
    qWarning() << "Find depth generator";
    g_DepthGenerator.GetAlternativeViewPointCap().SetViewPoint(g_ImageGenerator);
    nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_USER, g_UserGenerator);
    if (nRetVal != XN_STATUS_OK)
    {
        nRetVal = g_UserGenerator.Create(g_Context);
        qWarning() << "Find user generator";
    }

    XnCallbackHandle hUserCallbacks, hCalibrationStart, hCalibrationComplete, hPoseDetected, hCalibrationInProgress, hPoseInProgress;
    if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON))
    {
        printf("Supplied user generator doesn't support skeleton\n");
    }
    nRetVal = g_UserGenerator.RegisterUserCallbacks(User_NewUser, User_LostUser, this, hUserCallbacks);
    qWarning() << "Register to user callbacks";
    nRetVal = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationStart(UserCalibration_CalibrationStart, this, hCalibrationStart);
    qWarning() << "Register to calibration start";
    nRetVal = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationComplete(UserCalibration_CalibrationComplete, this, hCalibrationComplete);
    qWarning() << "Register to calibration complete";

    if (g_UserGenerator.GetSkeletonCap().NeedPoseForCalibration())
    {
        g_bNeedPose = TRUE;
        if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
        {
            printf("Pose required, but not supported\n");
        }
        nRetVal = g_UserGenerator.GetPoseDetectionCap().RegisterToPoseDetected(UserPose_PoseDetected, this, hPoseDetected);
        qWarning() << "Register to Pose Detected";
        g_UserGenerator.GetSkeletonCap().GetCalibrationPose(g_strPose);
    }

    g_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);

    nRetVal = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationInProgress(MyCalibrationInProgress, this, hCalibrationInProgress);
    qWarning() << "Register to calibration in progress";

    nRetVal = g_UserGenerator.GetPoseDetectionCap().RegisterToPoseInProgress(MyPoseInProgress, this, hPoseInProgress);
    qWarning() << "Register to pose in progress";

    nRetVal = g_Context.StartGeneratingAll();
    qWarning() << "StartGenerating";

    xn::DepthMetaData depthMD;
    grab();
    g_DepthGenerator.GetMetaData(depthMD);

    uSize = depthMD.XRes();
    vSize = depthMD.YRes();
    uCenter = uSize / 2;
    vCenter = vSize / 2;
    pointCloud.create(vSize, uSize, CV_32FC3);
    XnDouble pixelSize;
    XnUInt64 focalLength;
    g_DepthGenerator.GetIntProperty ("ZPD", focalLength );
    g_DepthGenerator.GetRealProperty ("ZPPS", pixelSize );
    P2R = pixelSize * ( 1280 / uSize ) / (float)focalLength;

}


NiUserTracker::~NiUserTracker()
{
    g_scriptNode.Release();
    g_DepthGenerator.Release();
    g_UserGenerator.Release();
    g_ImageGenerator.Release();
    g_Player.Release();
    g_Context.Release();

}

void NiUserTracker::depth2point( Mat &img )
{

    XnDepthPixel *depthMap = depthMD.WritableData();
    float dist;
    for (int v = 0; v < vSize; ++v)
    {
        for (int u = 0; u < uSize; ++u)
        {
            dist = (*depthMap) * 0.001f; //mm -> m

            Point3f p;
            p.x = dist * P2R * ( uCenter - u );
            p.y = dist * P2R * ( vCenter - v );
            p.z = dist;
            pointCloud.at<Point3f>(v, u)  = p;
            ++depthMap;
        }
    }
    pointCloud.copyTo(img);
}

void NiUserTracker::renderUsers(Mat &img)
{

    static const unsigned char Col[][3] =
    {
        {0,255,255},
        {0,0,255},
        {0,255,0},
        {255,255,0},
        {255,0,0},
        {255,128,0},
        {128,255,0},
        {0,128,255},
        {128,0,255},
        {255,255,128},
        {255,255,255}
    };

    int g_nYRes = sceneMD.YRes();
    int g_nXRes = sceneMD.XRes();
    Mat m(g_nYRes, g_nXRes, CV_8UC3);
    const XnLabel *pLabels = sceneMD.Data();

    for (int nY=0; nY<g_nYRes; nY++)
    {
        for (int nX=0; nX < g_nXRes; nX++)
        {
            Vec3b vec;
            vec[0] = 0;
            vec[1] = 0;
            vec[2] = 0;

            if (*pLabels != 0)
            {
                XnLabel label = *pLabels;
                XnUInt32 nColorID = label % nColors;
                if (label == 0)
                {
                    nColorID = nColors;
                }

                vec[0] = Col[nColorID][0];
                vec[1] = Col[nColorID][1];
                vec[2] = Col[nColorID][2];

            }
            m.at<Vec3b>(nY, nX) = vec;
            pLabels++;
        }
    }
    m.copyTo(img);
}

void NiUserTracker::retrieve(Mat &img, IMAGETYPE type)
{


    switch(type)
    {
        case RGB8U:
        {
            g_ImageGenerator.GetMetaData(imageMD);
            Mat m(imageMD.YRes(), imageMD.XRes(), CV_8UC3, imageMD.WritableRGB24Data());
            m.copyTo(img);
            break;
        }
        case DEPTH8U:
        {
            Mat m(depthMD.YRes(), depthMD.XRes(), CV_16UC1, depthMD.WritableData());
            m.convertTo(img, CV_8UC1, 0.04);
            break;
        }
        case DEPTH16U:
        {
            Mat m(depthMD.YRes(), depthMD.XRes(), CV_16UC1, depthMD.WritableData());
            m.copyTo(img);
            break;
        }
        case POINTCLOUD:
        {
            g_DepthGenerator.GetMetaData(depthMD);
            depth2point(img);
            break;
        }
        case USERLABELS:
        {
            g_UserGenerator.GetUserPixels(0, sceneMD);
            renderUsers(img);
            break;
        }
    }

}

Point3f NiUserTracker::Pos(XnUserID player,XnSkeletonJoint ejoint)
{
    XnSkeletonJointPosition jointx;
    /**playerのejointの情報をjointxに入れる*/
    g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player,ejoint,jointx);


    /**jointxの位置情報をpointで指定したアドレスに入れる*/
    float dist = jointx.position.Z * 0.001f; //mm -> m

    Point3f p;
    p.x = 0.001 * -jointx.position.X;
    p.y = 0.001 * jointx.position.Y;
    p.z = dist;
    return p;
}

void NiUserTracker::retrieveSkeltons(QMap<unsigned int, Skelton> &skeltons)
{
    XnUserID aUsers[15];
    XnUInt16 nUsers = 15;
    g_UserGenerator.GetUsers(aUsers,nUsers);//ユーザIDと認識中の人数の取得

    for (int i = 0; i < nUsers; ++i)//ユーザの人数だけ繰り返す
    {
        /**骨格のトラッキング中かどうか*/
        if(g_UserGenerator.GetSkeletonCap().IsTracking(aUsers[i])){
            /*各間接位置の取得**/
            Skelton s;

            s.insert(XN_SKEL_TORSO, Pos(aUsers[i],XN_SKEL_TORSO));
            s.insert(XN_SKEL_NECK, Pos(aUsers[i],XN_SKEL_NECK));
            s.insert(XN_SKEL_HEAD, Pos(aUsers[i],XN_SKEL_HEAD));
            s.insert(XN_SKEL_LEFT_SHOULDER, Pos(aUsers[i],XN_SKEL_LEFT_SHOULDER));
            s.insert(XN_SKEL_LEFT_ELBOW, Pos(aUsers[i],XN_SKEL_LEFT_ELBOW));
            s.insert(XN_SKEL_LEFT_HAND, Pos(aUsers[i],XN_SKEL_LEFT_HAND));
            s.insert(XN_SKEL_RIGHT_SHOULDER, Pos(aUsers[i],XN_SKEL_RIGHT_SHOULDER));
            s.insert(XN_SKEL_RIGHT_ELBOW, Pos(aUsers[i],XN_SKEL_RIGHT_ELBOW));
            s.insert(XN_SKEL_RIGHT_HAND, Pos(aUsers[i],XN_SKEL_RIGHT_HAND));
            s.insert(XN_SKEL_LEFT_HIP, Pos(aUsers[i],XN_SKEL_LEFT_HIP));
            s.insert(XN_SKEL_LEFT_KNEE, Pos(aUsers[i],XN_SKEL_LEFT_KNEE));
            s.insert(XN_SKEL_LEFT_FOOT, Pos(aUsers[i],XN_SKEL_LEFT_FOOT));
            s.insert(XN_SKEL_RIGHT_HIP, Pos(aUsers[i],XN_SKEL_RIGHT_HIP));
            s.insert(XN_SKEL_RIGHT_KNEE, Pos(aUsers[i],XN_SKEL_RIGHT_KNEE));
            s.insert(XN_SKEL_RIGHT_FOOT, Pos(aUsers[i],XN_SKEL_RIGHT_FOOT));

            skeltons.insert(aUsers[i], s);

        }
    }
}



