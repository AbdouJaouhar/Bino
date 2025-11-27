#pragma once
#include <thread>
#include <atomic>
#include <functional>

#include "DepthAIStereoCamera.hpp"


class CameraManager {
public:
    using FrameCallback = std::function<void(const SyncedFrameSet&)>;

    CameraManager(const std::string& name, std::ifstream& configFile);
    ~CameraManager();

    void start();
    void stop();
    void setCallback(FrameCallback cb);

private:
    void run();

    std::atomic<bool> running{false};
    std::thread worker;
    FrameCallback callback;
    DepthAIStereoCamera camera;
};
