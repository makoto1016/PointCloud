#include "MainWindow.h"
#include <QSplitter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      mode(RGBIMAGE)
{
    setupActions();
    updateImage();
    tracker.retrieve(depth, NiUserTracker::DEPTH8U);
    depth.copyTo(s);
    depth.copyTo(v);
    s.setTo(Scalar::all(255));
    v.setTo(Scalar::all(255));
    setupUI();

    connect(&timer, SIGNAL(timeout()), this, SLOT(updateImage()));
    timer.start(33);

}


void MainWindow::updateImage()
{
    mvec.clear();
    tracker.grab();
    tracker.retrieve(pointCloud, NiUserTracker::POINTCLOUD);

    switch(mode)
    {
        case RGBIMAGE:
        {
            tracker.retrieve(textureImg, NiUserTracker::RGB8U);
            break;
        }
        case DEPTHIMAGE:
        {
            tracker.retrieve(depth, NiUserTracker::DEPTH8U);

            mvec.push_back(depth);
            mvec.push_back(s);
            mvec.push_back(v);
            merge(mvec, hsv);
            cvtColor(hsv, textureImg, CV_HSV2RGB);
        }

    };
    if(userAction->isChecked())
    {
        Mat user, mask;
        tracker.retrieve(user, NiUserTracker::USERLABELS);
        cvtColor(user, mask, CV_RGB2GRAY);
        if(mode == DEPTHIMAGE)
        {
            std::vector<Mat> maskvec;
            maskvec.push_back(mask);
            maskvec.push_back(mask);
            maskvec.push_back(mask);
            merge(maskvec, user);
        }
        tracker.retrieveSkeltons(skeltons);
        user.copyTo(textureImg, mask);
    }

}

MainWindow::~MainWindow()
{
}


bool MainWindow::isCalibPose()
{
    return true;
}

void MainWindow::setupActions()
{
    mainbar = addToolBar("disp mode");
    rgbModeAction = mainbar->addAction("RGB", this, SLOT(setRGBMode()));
    depthModeAction = mainbar->addAction("DEPTH", this, SLOT(setDepthMode()));
    mainbar->addSeparator();
    userAction = mainbar->addAction("USER");
    userAction->setCheckable(true);

}

void MainWindow::setupUI()
{
    viewer = new PointCloudViewer(&textureImg, &pointCloud, &skeltons);
    viewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QAction *gridAction = mainbar->addAction("GRID", viewer, SLOT(toggleGridIsDrawn()));
    QAction *axisAction = mainbar->addAction("AXIS", viewer, SLOT(toggleAxisIsDrawn()));

    gridAction->setCheckable(true);
    axisAction->setCheckable(true);
    rgbModeAction->setCheckable(true);
    rgbModeAction->setChecked(true);
    depthModeAction->setCheckable(true);



    debugEdit.setReadOnly(true);
    debugEdit.setFixedWidth(640);
    debugEdit.setFixedHeight(100);
    debugEdit.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(viewer, SIGNAL(debugTextChanged(QString)), &debugEdit, SLOT(setText(QString)));

    QVBoxLayout *vlay = new QVBoxLayout;
    vlay->addWidget(viewer);
    vlay->addWidget(&debugEdit);

    QWidget *w = new QWidget;
    w->setLayout(vlay);

    this->setCentralWidget(w);
    this->resize(textureImg.cols,textureImg.rows);
}
