#pragma once
#include <string>
#include <string_view>
#include <memory>
#include <vector>
#include <fstream>
#include <utility>
#include <chrono>

#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>
#include <depthai/depthai.hpp>

struct SyncedFrameSet {
    cv::Mat left, right, rgb;
    bool hasLeft, hasRight, hasRgb, hasImu;
    std::shared_ptr<dai::IMUData> imu;
};

struct Vector {
    float x, y, z;
};

struct RotVector {
    float i, j, k, real, accuracy;
};

struct IMUData {
    Vector acc;
    Vector gyro;
    Vector magField;
    RotVector rotVec;
};

class IStereoCamera {
public:
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual std::string_view getName() const = 0;
    virtual ~IStereoCamera() = default;
};

class DepthAIStereoCamera : public IStereoCamera {
public:
    explicit DepthAIStereoCamera(const std::string& mName, std::ifstream& mConfigFile)
        : name_(mName) {
        loadConfiguration(mConfigFile);

        try {
            // List connected devices for debugging
            auto devices = dai::Device::getAllAvailableDevices();
            std::cout << "Found " << devices.size() << " devices:" << std::endl;
            for(const auto& dev : devices) {
                std::cout << " - " << dev.toString() << std::endl;
            }

            // Try to connect with retries
            int maxRetries = 3;
            for(int i = 0; i < maxRetries; i++) {
                try {
                    // Create device first
                    device_ = std::make_shared<dai::Device>();
                    std::cout << "Device connected successfully!" << std::endl;
                    break;
                } catch(const std::exception& e) {
                    if (i == maxRetries - 1) throw; // Re-throw on last attempt
                    std::cout << "Connection attempt " << i+1 << " failed: " << e.what() << ". Retrying in 1s..." << std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
            }

            pipeline_ = std::make_unique<dai::Pipeline>(device_);

            setupPipeline();

            pipeline_->start();
            std::cout << "Pipeline started!" << std::endl;

            running_ = true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to initialize camera: " << e.what() << std::endl;
            throw;
        }
    }

    void start() override {
    }

    void stop() override {
        if (running_) {
            device_->close();
            running_ = false;
        }
    }

    ~DepthAIStereoCamera() override {
        stop();
    }

    std::string_view getName() const override {
        return name_;
    }

    bool areFramesAvailable() {
        return qSync_ && qSync_->has();
    }

    bool getSyncedFrames() {
        if (qSync_ && qSync_->has()) {
            syncedFrames_ = qSync_->get<dai::MessageGroup>();
            std::cout << "Received group size = " << syncedFrames_->getMessageNames().size() << std::endl;
            return static_cast<bool>(syncedFrames_);
        }
        return false;
    }

    SyncedFrameSet getSyncedFrameSet() {
        frameSet_.hasLeft = false;
        frameSet_.hasRight = false;
        frameSet_.hasRgb = false;
        frameSet_.hasImu = false;

        if (getSyncedFrames()) {
            if (showLeft_) {
                frameSet_.left = getImage("left");
                frameSet_.hasLeft = !frameSet_.left.empty();
            }
            if (showRight_) {
                frameSet_.right = getImage("right");
                frameSet_.hasRight = !frameSet_.right.empty();
            }
            if (showRgb_) {
                frameSet_.rgb = getImage("rgb");
                frameSet_.hasRgb = !frameSet_.rgb.empty();
            }
            if (showImu_) {
                frameSet_.imu = getIMU();
                frameSet_.hasImu = static_cast<bool>(frameSet_.imu);
            }
        }

        return frameSet_;
    }

private:
    std::string name_;

    std::shared_ptr<dai::Device> device_;
    std::unique_ptr<dai::Pipeline> pipeline_;
    std::shared_ptr<dai::MessageQueue> qSync_;
    std::shared_ptr<dai::MessageGroup> syncedFrames_;
    bool running_ = false;

    int width_ = 0;
    int height_ = 0;

    nlohmann::json config_;

    bool showLeft_ = false;
    bool showRight_ = false;
    bool showRgb_ = false;
    bool showImu_ = false;

    SyncedFrameSet frameSet_{};

    void loadConfiguration(std::ifstream& configFile) {
        if (!configFile.is_open()) {
            return;
        }
        config_ = nlohmann::json::parse(configFile, nullptr, false);
        if (config_.is_discarded()) {
            showLeft_ = true;
            showRight_ = true;
            showRgb_ = true;
            showImu_ = true;
            return;
        }

        showLeft_ = config_.value("left", false);
        showRight_ = config_.value("right", false);
        showRgb_ = config_.value("rgb", false);
        showImu_ = config_.value("imu", false);
    }

    void setupPipeline() {
        auto sync = pipeline_->create<dai::node::Sync>();
        sync->setSyncThreshold(std::chrono::milliseconds(10));

        if (showLeft_) {
            auto monoLeft = createMonoCamera(pipeline_, dai::CameraBoardSocket::CAM_B);
            monoLeft->out.link(sync->inputs["left"]);
        }
        if (showRight_) {
            auto monoRight = createMonoCamera(pipeline_, dai::CameraBoardSocket::CAM_C);
            monoRight->out.link(sync->inputs["right"]);
        }
        if (showRgb_) {
            auto rgbCam = createColorCamera(pipeline_);
            rgbCam->preview.link(sync->inputs["rgb"]);
        }
        if (showImu_) {
            auto imuNode = createIMU(pipeline_);
            imuNode->out.link(sync->inputs["imu"]);
        }

        qSync_ = sync->out.createOutputQueue(8, false);
    }

    std::shared_ptr<dai::node::Camera> createMonoCamera(std::unique_ptr<dai::Pipeline>& pipeline, dai::CameraBoardSocket socket) {
        auto monoCamera = pipeline->create<dai::node::Camera>();
        monoCamera->build(socket);
        monoCamera->setResolution(dai::MonoCameraProperties::SensorResolution::THE_480_P);
        monoCamera->setFps(60.0f);
        return monoCamera;
    }

    std::shared_ptr<dai::node::Camera> createColorCamera(std::unique_ptr<dai::Pipeline>& pipeline) {
        auto colorCamera = pipeline->create<dai::node::Camera>();
        colorCamera->build(dai::CameraBoardSocket::CAM_A);
        colorCamera->setResolution(dai::ColorCameraProperties::SensorResolution::THE_1080_P);
        colorCamera->setFps(30.0f);
        colorCamera->setVideoSize(1280, 720);
        colorCamera->setPreviewSize(1280, 720);
        colorCamera->setPreviewKeepAspectRatio(false);
        colorCamera->setPreviewType(dai::ImgFrame::Type::RGB888i);
        return colorCamera;
    }

    std::shared_ptr<dai::node::IMU> createIMU(std::unique_ptr<dai::Pipeline>& pipeline) {
        auto imu = pipeline->create<dai::node::IMU>();
        imu->enableIMUSensor(dai::IMUSensor::ACCELEROMETER_RAW, 480);
        imu->enableIMUSensor(dai::IMUSensor::GYROSCOPE_RAW, 400);
        imu->setBatchReportThreshold(1);
        imu->setMaxBatchReports(10);
        return imu;
    }

    cv::Mat getImage(std::string_view name) {
        if (!syncedFrames_) {
            return {};
        }

        if (name == "left" && showLeft_) {
            auto f = syncedFrames_->get<dai::ImgFrame>("left");
            if (!f) {
                return {};
            }
            width_ = f->getWidth();
            height_ = f->getHeight();
            return f->getCvFrame();
        }

        if (name == "right" && showRight_) {
            auto f = syncedFrames_->get<dai::ImgFrame>("right");
            if (!f) {
                return {};
            }
            width_ = f->getWidth();
            height_ = f->getHeight();
            return f->getCvFrame();
        }

        if (name == "rgb" && showRgb_) {
            auto f = syncedFrames_->get<dai::ImgFrame>("rgb");
            if (!f) {
                return {};
            }
            width_ = f->getWidth();
            height_ = f->getHeight();
            return f->getCvFrame();
        }

        return {};
    }

    std::shared_ptr<dai::IMUData> getIMU() {
        if (!syncedFrames_ || !showImu_) {
            return nullptr;
        }
        return syncedFrames_->get<dai::IMUData>("imu");
    }
};
