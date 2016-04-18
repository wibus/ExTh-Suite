#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <memory>

#include <QWidget>

#include <CellarWorkbench/Camera/Camera.h>

#include <PropRoom3D/Team/ArtDirector/ArtDirectorServer.h>

namespace Ui
{
    class RaytracerGui;
}


class CameraManager : public QObject
{
public:
    CameraManager(Ui::RaytracerGui* ui);
    virtual ~CameraManager();

    virtual void setView(QWidget* view);
    virtual void setRaytracer(const std::shared_ptr<prop3::ArtDirectorServer>& raytracer);

public slots:
    virtual void onFreeCamera(bool isFree);

protected slots:
    virtual void captureDimensionsChanged(int unused);
    virtual void outputDimensionsChanged(int unused);
    virtual void outputMatchCaptureDimensions(bool match);
    virtual void fieldOfViewChanged(int unused);
    virtual void dofDistanceChanged(double unused);
    virtual void dofApertureChanged(double unused);
    virtual void enterFullscreen();
    virtual void exitFullscreen();
    virtual void updateEachTileCheckChanged(bool unused);
    virtual void colorOutputTypeChanged(int unused);

protected:
    virtual void updateCameraProjection();
    virtual bool eventFilter(QObject* obj, QEvent* event) override;

private:
    Ui::RaytracerGui* _ui;
    QWidget* _view;
    QWidget* _fullscreenWindow;
    std::shared_ptr<cellar::Camera> _camera;
    std::shared_ptr<prop3::ArtDirectorServer> _raytracer;
};

#endif // CAMERAMANAGER_H
