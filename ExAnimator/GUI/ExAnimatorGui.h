#ifndef EXANIMATORGUI_H
#define EXANIMATORGUI_H

#include <QMainWindow>

#include <Scaena/Play/Play.h>

namespace Ui
{
    class ExAnimatorGui;
}

class ExAnimatorView;
class CameraManager;
class AnimationManager;
class TimelineManager;
class PostProdManager;
class PathManager;


class ExAnimatorGui : public QMainWindow
{
    Q_OBJECT

public:
    explicit ExAnimatorGui(const std::shared_ptr<scaena::Play>& play);

    ~ExAnimatorGui();

private:
    Ui::ExAnimatorGui* _ui;
    std::shared_ptr<scaena::Play> _play;
    std::shared_ptr<ExAnimatorView> _view;
    std::shared_ptr<CameraManager> _cameraManager;
    std::shared_ptr<AnimationManager> _animationManager;
    std::shared_ptr<TimelineManager> _timelineManager;
    std::shared_ptr<PostProdManager> _postProdManager;
    std::shared_ptr<PathManager> _pathManager;
};

#endif // EXANIMATORGUI_H
