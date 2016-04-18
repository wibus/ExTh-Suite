#ifndef TIMELINEMANAGER_H
#define TIMELINEMANAGER_H

#include <memory>

#include <QWidget>

#include <PropRoom3D/Team/ArtDirector/ArtDirectorServer.h>


namespace Ui
{
    class ExAnimatorGui;
}

class TheFruitChoreographer;


class TimelineManager : public QObject
{
    Q_OBJECT

public:
    TimelineManager(Ui::ExAnimatorGui* ui);
    virtual ~TimelineManager();

    virtual void setChoreographer(
        const std::shared_ptr<TheFruitChoreographer>& choreographer);


signals:
    void startSoundtrack(double time);
    void stopSoundtrack();


public slots:
    virtual void onPathChanged();


private slots:
    virtual void animTimeOffset(double offset);
    virtual void animFps(int fps);
    virtual void animFrame(int frame);
    virtual void maxFrame(int frame);
    virtual void resetAnim(bool unsused);
    virtual void recordAnim(bool record);
    virtual void playAnim(bool play);
    virtual void fastPlay(bool fast);

    virtual void animFrameFromChoreographer(int frame);
    virtual void animPlayFromChoreographer(bool play);

protected:
    virtual void updateTimeMeter();
    virtual void manageSoundtrack();
    virtual int computeAnimFrameCount();
    virtual double computeTimeOffset();
    virtual double computeCurrentTime();

private:
    Ui::ExAnimatorGui* _ui;
    std::shared_ptr<TheFruitChoreographer> _choreographer;
};

#endif // TIMELINEMANAGER_H
