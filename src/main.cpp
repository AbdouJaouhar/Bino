#include <QApplication>
#include <fstream>

#include "Bino/App/MainWindow.hpp"
#include "Bino/Camera/CameraManager.hpp"


int main(int argc, char** argv) {
    QApplication app(argc, argv);

    std::ifstream cfg("config/camera_config.json");
    CameraManager camera("OAK-D-Lite", cfg);
    camera.start();

    MainWindow w(&camera);
    w.show();

    int r = app.exec();
    camera.stop();
    return r;
}
