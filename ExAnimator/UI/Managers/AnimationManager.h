#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include <memory>

#include <QWidget>
#include <QMediaPlayer>

#include <CellarWorkbench/Camera/Camera.h>

#include <PropRoom3D/Team/ArtDirector/ArtDirectorServer.h>


namespace Ui
{
    class RaytracerGui;
}

class TheFruitChoreographer;


class AnimationManager : public QObject
{
    Q_OBJECT

public:
    AnimationManager(Ui::RaytracerGui* ui);
    virtual ~AnimationManager();

    virtual void setChoreographer(
        const std::shared_ptr<TheFruitChoreographer>& choreographer);
    virtual void setRaytracer(
            const std::shared_ptr<prop3::ArtDirectorServer>& raytracer);


public slots:
    virtual void startSoundtrack(double time);
    virtual void stopSoundtrack();


private slots:
    virtual void divThreshold(double div);
    virtual void maxSampleThreshold(int sampleCount);
    virtual void maxRenderTimeThreshold(int maxSeconds);
    virtual void surfaceVisibilityThreshold(int visibility);

    virtual void soundtrackName(QString name);
    virtual void soundtrackVolume(int volume);

    virtual void outputName(const QString& name);
    virtual void outputFormat(const QString& format);
    virtual void includeSampleCount(bool include);
    virtual void includeRenderTime(bool include);
    virtual void includeDivergence(bool include);

    virtual void saveReferenceShot();
    virtual void loadReferenceShot();
    virtual void clearReferenceShot();

private:
    Ui::RaytracerGui* _ui;
    QMediaPlayer _mediaPlayer;
    std::shared_ptr<prop3::ArtDirectorServer> _raytracer;
    std::shared_ptr<TheFruitChoreographer> _choreographer;

};

#endif // ANIMATIONMANAGER_H
