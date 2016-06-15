#include "TimelineManager.h"

#include <QFileInfo>

#include <PropRoom3D/Team/ArtDirector/RaytracerState.h>
#include <PropRoom3D/Team/ArtDirector/Film/Film.h>

#include "PathManager.h"
#include "Model/PathModel.h"
#include "Model/SceneDocument.h"
#include "Animators/TheFruitChoreographer.h"

#include "ui_ExAnimatorGui.h"


TimelineManager::TimelineManager(Ui::ExAnimatorGui* ui) :
    _ui(ui)
{
    connect(_ui->animTimeOffsetMinSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &TimelineManager::animTimeOffset);
    connect(_ui->animTimeOffsetSecSpin, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &TimelineManager::animTimeOffset);

    connect(_ui->animFpsSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &TimelineManager::animFps);

    connect(_ui->animFrameSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &TimelineManager::animFrame);

    connect(_ui->maxFrameSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &TimelineManager::maxFrame);

    connect(_ui->animFrameSlider, &QSlider::valueChanged,
            this, &TimelineManager::animFrame);

    connect(_ui->resetAnimButton, &QPushButton::clicked,
            this, &TimelineManager::resetAnim);

    connect(_ui->recordAnimButton, &QPushButton::toggled,
            this, &TimelineManager::recordAnim);

    connect(_ui->playAnimButton, &QPushButton::toggled,
            this, &TimelineManager::playAnim);

    connect(_ui->fastAnimButton, &QPushButton::toggled,
            this, &TimelineManager::fastPlay);
}

TimelineManager::~TimelineManager()
{

}

void TimelineManager::setChoreographer(
    const std::shared_ptr<TheFruitChoreographer>& choreographer)
{
    _choreographer = choreographer;

    onPathChanged();
    animTimeOffset(0 /*unused*/);
    animFps(_ui->animFpsSpin->value());
    animFrame(_ui->animFrameSpin->value());
    fastPlay(_ui->fastAnimButton->isChecked());
    playAnim(_ui->playAnimButton->isChecked());

    connect(_choreographer.get(), &TheFruitChoreographer::animFrameChanged,
            this, &TimelineManager::animFrameFromChoreographer);

    connect(_choreographer.get(), &TheFruitChoreographer::playStateChanged,
            this, &TimelineManager::animPlayFromChoreographer);
}

void TimelineManager::animTimeOffset(double)
{
    double offset = computeTimeOffset();
    getSceneDocument().setAnimationTimeOffset(offset);
    _choreographer->setAnimTimeOffset(offset);
    updateTimeMeter();
}

void TimelineManager::animFps(int fps)
{
    int frameCount = computeAnimFrameCount();

    _choreographer->setAnimFps(fps);
    _choreographer->setAnimFrameCount(frameCount);
    _ui->maxFrameSpin->setMaximum(frameCount);
    updateTimeMeter();
}

void TimelineManager::animFrame(int frame)
{
    _choreographer->setAnimFrame(frame);
    _ui->animFrameSpin->setValue(frame);
    _ui->animFrameSlider->setValue(frame);
    updateTimeMeter();
}

void TimelineManager::maxFrame(int frame)
{
    _choreographer->setAnimFrameCount(frame);
    _ui->animFrameSpin->setMaximum(frame);
    _ui->animFrameSlider->setMaximum(frame);
    updateTimeMeter();
}

void TimelineManager::resetAnim(bool unsused)
{
    _choreographer->resetAnimation();
    manageSoundtrack();
}

void TimelineManager::recordAnim(bool record)
{
    if(record)
        _choreographer->startRecording();
    else
        _choreographer->stopRecording();

    _ui->playAnimButton->setChecked(record);
    _ui->animOutputGroup->setEnabled(!record);
}

void TimelineManager::playAnim(bool play)
{
    if(play)
    {
        _choreographer->playAnimation();
    }
    else
    {
        _choreographer->pauseAnimation();
        _ui->recordAnimButton->setChecked(false);
        _ui->fastAnimButton->setChecked(false);
    }

    manageSoundtrack();
}

void TimelineManager::fastPlay(bool fast)
{
    _choreographer->setFastPlay(fast);

    _ui->animFrameSpin->setEnabled(!fast);
    _ui->animFrameSlider->setEnabled(!fast);
    _ui->soundtrackNameEdit->setEnabled(!fast);
    manageSoundtrack();
}

void TimelineManager::animFrameFromChoreographer(int frame)
{
    _ui->animFrameSpin->setValue(frame);
    _ui->animFrameSlider->setValue(frame);
    updateTimeMeter();
}

void TimelineManager::animPlayFromChoreographer(bool play)
{
    _ui->playAnimButton->setChecked(play);
}

void TimelineManager::updateTimeMeter()
{
    double current = computeCurrentTime();
    double offset = computeTimeOffset();
    double endTime = offset + (_ui->maxFrameSpin->value() /
            double(_ui->animFpsSpin->value()));

    _ui->animTimeValue->setText(
        SceneDocument::timeToString(current).c_str());
    _ui->animTimeSuffix->setText(QString(" / %1")
        .arg(SceneDocument::timeToString(endTime).c_str()));


    int frameCount = computeAnimFrameCount();
    _ui->animFrameSpin->setSuffix(QString("/ %1").arg(frameCount));
    _ui->maxFrameSpin->setSuffix(QString("/ %1").arg(frameCount));
}

void TimelineManager::manageSoundtrack()
{
    if(_ui->playAnimButton->isChecked() &&
       _ui->fastAnimButton->isChecked())
    {
        emit startSoundtrack(computeCurrentTime());
    }
    else
    {
        emit stopSoundtrack();
    }
}

int TimelineManager::computeAnimFrameCount()
{
    return glm::ceil(_choreographer->pathModel()->animationLength() *
                     _ui->animFpsSpin->value());
}

double TimelineManager::computeTimeOffset()
{
    double offsetMin = _ui->animTimeOffsetMinSpin->value();
    double offsetSec = _ui->animTimeOffsetSecSpin->value();
    double offsetSecTot = offsetMin * 60.0 + offsetSec;
    return offsetSecTot;
}

double TimelineManager::computeCurrentTime()
{
    double offset = computeTimeOffset();
    double frame = _ui->animFrameSpin->value();
    double fps = _ui->animFpsSpin->value();
    return offset + frame/fps;
}

void TimelineManager::onPathChanged()
{
    int frameCount = computeAnimFrameCount();
    bool keepMax = _ui->maxFrameSpin->value() ==
            _ui->maxFrameSpin->maximum();

    _ui->maxFrameSpin->setMaximum(frameCount);
    if(keepMax)
    {
        _ui->maxFrameSpin->setValue(frameCount);
    }

    updateTimeMeter();
}
