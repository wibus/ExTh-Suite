#ifndef RAYTRACERGUI_H
#define RAYTRACERGUI_H

#include <QMainWindow>

#include <Scaena/Play/Play.h>

namespace Ui
{
    class RaytracerGui;
}

class RaytracedView;
class CameraManager;
class AnimationManager;
class TimelineManager;
class PostProdManager;
class PathManager;


class RaytracerGui : public QMainWindow
{
    Q_OBJECT

public:
    explicit RaytracerGui(const std::shared_ptr<scaena::Play>& play);

    ~RaytracerGui();

private:
    Ui::RaytracerGui* _ui;
    std::shared_ptr<scaena::Play> _play;
    std::shared_ptr<RaytracedView> _raytracedView;
    std::shared_ptr<CameraManager> _cameraManager;
    std::shared_ptr<AnimationManager> _animationManager;
    std::shared_ptr<TimelineManager> _timelineManager;
    std::shared_ptr<PostProdManager> _postProdManager;
    std::shared_ptr<PathManager> _pathManager;
};

#endif // RAYTRACERGUI_H
