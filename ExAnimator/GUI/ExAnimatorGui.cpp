#include "ExAnimatorGui.h"
#include "ui_ExAnimatorGui.h"

#include <QSpinBox>
#include <QApplication>
#include <QDesktopWidget>

#include "Managers/CameraManager.h"
#include "Managers/AnimationManager.h"
#include "Managers/TimelineManager.h"
#include "Managers/PostProdManager.h"
#include "Managers/ServerManager.h"
#include "Managers/PathManager.h"
#include "ExAnimatorView.h"

using namespace scaena;


ExAnimatorGui::ExAnimatorGui(const std::shared_ptr<Play>& play) :
    _ui(new Ui::ExAnimatorGui),
    _play(play)
{
    _ui->setupUi(this);
    QPoint center = QApplication::desktop()->availableGeometry(this).center();
    move(center.x()-width()*0.5, center.y()-height()*0.5);

    _cameraManager.reset(new CameraManager(_ui));
    _serverManager.reset(new ServerManager(_ui));
    _postProdManager.reset(new PostProdManager(_ui));
    _animationManager.reset(new AnimationManager(_ui));
    _timelineManager.reset(new TimelineManager(_ui));
    _pathManager.reset(new PathManager(_ui));
    _view.reset(new ExAnimatorView(
         _cameraManager,
         _animationManager,
         _timelineManager,
         _postProdManager,
         _serverManager,
         _pathManager));

    _cameraManager->setView(_view.get());
    _play->addView(_view);
}

ExAnimatorGui::~ExAnimatorGui()
{
    delete _ui;
}
