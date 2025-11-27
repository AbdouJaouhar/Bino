#include "Bino/Camera/CameraManager.hpp"


CameraManager::CameraManager(const std::string& name, std::ifstream& configFile)
    : camera(name, configFile) {}

CameraManager::~CameraManager() {
    stop();
}

void CameraManager::start() {
    if (running) return;
    running = true;
    worker = std::thread(&CameraManager::run, this);
}

void CameraManager::stop() {
    running = false;
    if (worker.joinable()) worker.join();
}

void CameraManager::setCallback(FrameCallback cb) {
    callback = cb;
}

void CameraManager::run() {
    while (running) {
        if (!camera.areFramesAvailable()) continue;
        auto set = camera.getSyncedFrameSet();
        if (callback) callback(set);
    }
}
