#include <QVBoxLayout>
#include <QImage>
#include <QPixmap>
#include <iostream>

#include "Bino/App/MainWindow.hpp"


MainWindow::MainWindow(CameraManager* cam, QWidget* parent)
    : QMainWindow(parent), camera(cam) {

    auto* w = new QWidget;
    auto* layout = new QVBoxLayout;

    leftLabel = new QLabel;
    rightLabel = new QLabel;
    rgbLabel = new QLabel;

    layout->addWidget(leftLabel);
    layout->addWidget(rightLabel);
    layout->addWidget(rgbLabel);

    w->setLayout(layout);
    setCentralWidget(w);

    camera->setCallback([this](const SyncedFrameSet& f) {
        QMetaObject::invokeMethod(this, [this, f]() { onFrame(f); });
    });
}

void MainWindow::onFrame(const SyncedFrameSet& f) {
    std::cout << "Received frame !" << std::endl;
    
    if (f.hasLeft) {
        QImage img(f.left.data, f.left.cols, f.left.rows, f.left.step, QImage::Format_Grayscale8);
        leftLabel->setPixmap(QPixmap::fromImage(img.copy()));
        std::cout << "Has left !" << std::endl;
    }

    if (f.hasRight) {
        QImage img(f.right.data, f.right.cols, f.right.rows, f.right.step, QImage::Format_Grayscale8);
        rightLabel->setPixmap(QPixmap::fromImage(img.copy()));
        std::cout << "Has right !" << std::endl;
    }

    if (f.hasRgb) {
        QImage img(f.rgb.data, f.rgb.cols, f.rgb.rows, f.rgb.step, QImage::Format_RGB888);
        rgbLabel->setPixmap(QPixmap::fromImage(img.copy()));
        std::cout << "Has RGB !" << std::endl;
    }
}
