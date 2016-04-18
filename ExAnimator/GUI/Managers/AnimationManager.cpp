#include "AnimationManager.h"

#include <QFileInfo>

#include <PropRoom3D/Team/ArtDirector/RaytracerState.h>
#include <PropRoom3D/Team/ArtDirector/Film/Film.h>

#include "PathManager.h"
#include "Model/PathModel.h"
#include "Model/SceneDocument.h"
#include "Animators/TheFruitChoreographer.h"

#include "ui_ExAnimatorGui.h"


AnimationManager::AnimationManager(Ui::ExAnimatorGui* ui) :
    _ui(ui)
{
    connect(_ui->maxSampleSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &AnimationManager::maxSampleThreshold);

    connect(_ui->maxTimeSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &AnimationManager::maxRenderTimeThreshold);

    connect(_ui->divThresholdSpin, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &AnimationManager::divThreshold);

    connect(_ui->visibilityThresholdSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &AnimationManager::surfaceVisibilityThreshold);


    connect(_ui->soundtrackNameEdit, &QLineEdit::textChanged,
            this, &AnimationManager::soundtrackName);

    connect(_ui->soundtrackVolumeSlider, &QSlider::valueChanged,
            this, &AnimationManager::soundtrackVolume);

    _ui->outputFormatCombo->addItem(".png");
    _ui->outputFormatCombo->addItem(".jpg");

    connect(_ui->animNameEdit, &QLineEdit::textChanged,
            this, &AnimationManager::outputName);

    connect(_ui->outputFormatCombo, &QComboBox::currentTextChanged,
            this, &AnimationManager::outputFormat);

    connect(_ui->animNameSampleCountCheck, &QCheckBox::toggled,
            this, &AnimationManager::includeSampleCount);

    connect(_ui->animNameRenderTimeCheck, &QCheckBox::toggled,
            this, &AnimationManager::includeRenderTime);

    connect(_ui->animNameDivergenceCheck, &QCheckBox::toggled,
            this, &AnimationManager::includeDivergence);


    connect(_ui->saveReferenceShotButton, &QPushButton::clicked,
            this, &AnimationManager::saveReferenceShot);

    connect(_ui->loadReferenceShotButton, &QPushButton::clicked,
            this, &AnimationManager::loadReferenceShot);

    connect(_ui->clearReferenceShotButton, &QPushButton::clicked,
            this, &AnimationManager::clearReferenceShot);
}

AnimationManager::~AnimationManager()
{

}

void AnimationManager::setChoreographer(
    const std::shared_ptr<TheFruitChoreographer>& choreographer)
{
    _choreographer = choreographer;

    soundtrackName(_ui->soundtrackNameEdit->text());
    soundtrackVolume(_ui->soundtrackVolumeSlider->value());
}

void AnimationManager::setRaytracer(
    const std::shared_ptr<prop3::ArtDirectorServer>& raytracer)
{
    _raytracer = raytracer;

    maxSampleThreshold(_ui->maxSampleSpin->value());
    maxRenderTimeThreshold(_ui->maxTimeSpin->value());
    divThreshold(_ui->divThresholdSpin->value());
    surfaceVisibilityThreshold(_ui->visibilityThresholdSpin->value());

    outputName(_ui->animNameEdit->text());
    outputFormat(_ui->outputFormatCombo->currentText());
    includeSampleCount(_ui->animNameSampleCountCheck->isChecked());
    includeRenderTime(_ui->animNameRenderTimeCheck->isChecked());
    includeDivergence(_ui->animNameDivergenceCheck->isChecked());
}

void AnimationManager::maxSampleThreshold(int sampleCount)
{
    _raytracer->raytracerState()->setSampleCountThreshold(sampleCount);
}

void AnimationManager::maxRenderTimeThreshold(int maxSeconds)
{
    _raytracer->raytracerState()->setRenderTimeThreshold(maxSeconds);
}

void AnimationManager::surfaceVisibilityThreshold(int visibility)
{
    _raytracer->raytracerState()->setSurfaceVisibilityThreshold(visibility);
}

void AnimationManager::divThreshold(double div)
{
    _raytracer->raytracerState()->setDivergenceThreshold(div);
}

void AnimationManager::soundtrackName(QString name)
{
    getSceneDocument().setSoundtrackName(name.toStdString());
    std::string relative = getSceneDocument().getSoundtrackFilePath();
    QString absolute = QFileInfo(relative.c_str()).absoluteFilePath();
    _mediaPlayer.setMedia(QUrl::fromLocalFile(absolute));
}

void AnimationManager::soundtrackVolume(int volume)
{
    _mediaPlayer.setVolume(volume);
}

void AnimationManager::outputName(const QString& name)
{
    getSceneDocument().setOutputFrameDirectory(name.toStdString());
}

void AnimationManager::outputFormat(const QString& format)
{
    getSceneDocument().setOutputFrameFormat(format.toStdString());
}

void AnimationManager::includeSampleCount(bool include)
{
    getSceneDocument().setIncludeSampleCountInFrame(include);
}

void AnimationManager::includeRenderTime(bool include)
{
    getSceneDocument().setIncludeRenderTimeInFrame(include);
}

void AnimationManager::includeDivergence(bool include)
{
    getSceneDocument().setIncludeDivergenceInFrame(include);
}

void AnimationManager::saveReferenceShot()
{
    _raytracer->film()->saveReferenceShot(
        getSceneDocument().getAnimationFilmsDirectory() + "/reference.film");
}

void AnimationManager::loadReferenceShot()
{
    _raytracer->film()->loadReferenceShot(
        getSceneDocument().getAnimationFilmsDirectory() + "/reference.film");
}

void AnimationManager::clearReferenceShot()
{
    _raytracer->film()->clearReferenceShot();
}

void AnimationManager::startSoundtrack(double time)
{
    qint64 pos = time * 1000.0;
    _mediaPlayer.setPosition(pos);
    _mediaPlayer.play();
}

void AnimationManager::stopSoundtrack()
{
    _mediaPlayer.pause();
}
