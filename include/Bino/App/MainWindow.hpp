#pragma once
#include <QMainWindow>
#include <QLabel>

#include "../Camera/CameraManager.hpp"


class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(CameraManager* cam, QWidget* parent = nullptr);

private:
    CameraManager* camera;
    QLabel* leftLabel;
    QLabel* rightLabel;
    QLabel* rgbLabel;

    void onFrame(const SyncedFrameSet& f);
};
