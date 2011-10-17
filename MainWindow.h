#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <PointCloudView.h>
#include <QTimer>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void updateImage();
private:
    VideoCapture capture;
    Mat rgbImg, pointCloud;
    QTimer timer;
};

#endif // MAINWINDOW_H
