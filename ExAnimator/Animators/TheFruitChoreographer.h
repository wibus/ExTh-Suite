#ifndef THEFRUIT_CHOREOGRAPHER_H
#define THEFRUIT_CHOREOGRAPHER_H

#include <QObject>

#include <CellarWorkbench/Camera/Camera.h>
#include <CellarWorkbench/Camera/CameraManFree.h>

#include <PropRoom3D/Team/ArtDirector/RaytracerState.h>
#include <PropRoom3D/Team/Choreographer/AbstractChoreographer.h>

namespace prop3
{
    class Prop;
    class Surface;
    class ProceduralSun;
    class LightBulb;
    class StageZone;

    class ArtDirectorServer;
}

class PathModel;
class PathManager;


class TheFruitChoreographer : public QObject, public prop3::AbstractChoreographer
{
    Q_OBJECT

public:
    TheFruitChoreographer(
            const std::shared_ptr<prop3::ArtDirectorServer>& raytracer);
    virtual ~TheFruitChoreographer();

    virtual void setup(const std::shared_ptr<prop3::StageSet>& stageSet) override;
    virtual void update(double dt) override;
    virtual void terminate() override;

    virtual void forceUpdate();
    virtual int animFrameCount() const;
    virtual std::string currentFilm() const;
    virtual void setAnimTimeOffset(double offset);
    virtual void setAnimFrameCount(int frame);
    virtual void setAnimFps(int fps);
    virtual void setAnimFrame(int frame);
    virtual void resetAnimation();
    virtual void startRecording();
    virtual void stopRecording();
    virtual void playAnimation();
    virtual void pauseAnimation();
    virtual void setFastPlay(bool playFast);

    virtual void bindCameraToPath();
    virtual void freeCameraFromPath();

    virtual std::shared_ptr<PathModel> pathModel() const;

    virtual void saveCurrentFrame();


signals:
    void animFrameChanged(int frameId);
    void playStateChanged(bool isPlaying);


protected:
    virtual std::shared_ptr<prop3::Surface> createHoleStrippedWall(
            const glm::dvec3& size,
            double stripeWidth,
            double holeWidth,
            double border,
            double patternOffset);

private:
    glm::dvec3 _cloudsPosition;
    std::shared_ptr<prop3::StageZone> _cloudsZone;

    double _theFruitHeight;
    glm::dvec3 _theFruitPosition;
    std::shared_ptr<prop3::Surface> _theFruitSurf;
    std::shared_ptr<prop3::StageZone> _theFruitZone;
    std::shared_ptr<prop3::ProceduralSun> _backdrop;

    glm::dvec3 _fixtureRadiantFlux;
    std::shared_ptr<prop3::LightBulb> _hallLight;
    std::shared_ptr<prop3::LightBulb> _roomLightBack;
    std::shared_ptr<prop3::LightBulb> _roomLightFront;
    glm::dvec3 _lampRadiantFlux;
    std::shared_ptr<prop3::LightBulb> _lampLight;

    std::shared_ptr<PathModel> _pathModel;
    std::shared_ptr<prop3::ArtDirectorServer> _raytracer;
    std::shared_ptr<prop3::RaytracerState> _raytracerState;
    std::shared_ptr<cellar::CameraManFree> _camMan;
    std::shared_ptr<cellar::Camera> _camera;
    double _camAperture;

    int _animFps;
    int _animFrameCount;
    int _animCurrFrame;
    double _animTime;
    bool _isRecording;
    bool _animPlaying;
    bool _animFastPlay;
    bool _cameraIsFree;
};

#endif // THEFRUIT_CHOREOGRAPHER_H
