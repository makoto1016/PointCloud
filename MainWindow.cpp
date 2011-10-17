#include "MainWindow.h"
#include <QSplitter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    capture.open(CV_CAP_OPENNI);
    Mat bgr;
    capture.grab();
    capture.retrieve(bgr, CV_CAP_OPENNI_BGR_IMAGE);
    capture.retrieve(pointCloud, CV_CAP_OPENNI_POINT_CLOUD_MAP);
    cvtColor(bgr, rgbImg, CV_BGR2RGB);
    QSplitter *main = new QSplitter(Qt::Vertical);

    main->addWidget(new Viewer(&rgbImg, &pointCloud));
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateImage()));
    timer.start(20);
    this->setCentralWidget(main);
    this->resize(1920,1440);
}


void MainWindow::updateImage()
{
    Mat bgr;
    capture.grab();
    capture.retrieve(bgr, CV_CAP_OPENNI_BGR_IMAGE);
    capture.retrieve(pointCloud, CV_CAP_OPENNI_POINT_CLOUD_MAP);
    cvtColor(bgr, rgbImg, CV_BGR2RGB);
}

MainWindow::~MainWindow()
{

}
