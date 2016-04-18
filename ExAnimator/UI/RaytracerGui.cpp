#include "RaytracerGui.h"
#include "ui_RaytracerGui.h"

#include <QSpinBox>
#include <QApplication>
#include <QDesktopWidget>

#include "Managers/CameraManager.h"
#include "Managers/AnimationManager.h"
#include "Managers/TimelineManager.h"
#include "Managers/PostProdManager.h"
#include "Managers/PathManager.h"
#include "RaytracedView.h"

using namespace scaena;


RaytracerGui::RaytracerGui(const std::shared_ptr<Play>& play) :
    _ui(new Ui::RaytracerGui),
    _play(play)
{
    _ui->setupUi(this);
    QPoint center = QApplication::desktop()->availableGeometry(this).center();
    move(center.x()-width()*0.5, center.y()-height()*0.5);

    _cameraManager.reset(new CameraManager(_ui));
    _postProdManager.reset(new PostProdManager(_ui));
    _animationManager.reset(new AnimationManager(_ui));
    _timelineManager.reset(new TimelineManager(_ui));
    _pathManager.reset(new PathManager(_ui));
    _raytracedView.reset(new RaytracedView(
         _cameraManager,
         _animationManager,
         _timelineManager,
         _postProdManager,
         _pathManager));

    _cameraManager->setView(_raytracedView.get());
    _play->addView(_raytracedView);
}

RaytracerGui::~RaytracerGui()
{
    delete _ui;
}
