#include "TheFruitChoreographer.h"

#include <GLM/gtc/matrix_transform.hpp>

#include <QDir>

#include <CellarWorkbench/Misc/Log.h>
#include <CellarWorkbench/Image/Image.h>
#include <CellarWorkbench/GL/GlToolkit.h>

#include <PropRoom3D/Node/StageZone.h>
#include <PropRoom3D/Node/Prop/Prop.h>
#include <PropRoom3D/Node/Prop/Surface/Surface.h>
#include <PropRoom3D/Node/Debug/DebugLineStrip.h>
#include <PropRoom3D/Node/Light/LightBulb/LightBulb.h>
#include <PropRoom3D/Node/Light/Backdrop/ProceduralSun.h>
#include <PropRoom3D/Team/ArtDirector/ArtDirectorServer.h>
#include <PropRoom3D/Team/ArtDirector/Film/Film.h>

#include "Model/PathModel.h"
#include "Model/SceneDocument.h"
#include "GUI/Managers/PathManager.h"

using namespace cellar;
using namespace prop3;


const std::string RECORD_OUPUT_PREFIX = "";


void TheFruitChoreographer::update(double dt)
{
    bool forcedUpdate = (dt == 0.0);

    if(forcedUpdate || (_animPlaying && (
       _animFastPlay || !_raytracerState->isRendering())))
    {
        if(_isRecording && !forcedUpdate && !_animFastPlay && !_cameraIsFree)
            saveCurrentFrame();

        if(!forcedUpdate)
        {
            if(!_animFastPlay)
            {
                ++_animCurrFrame;
                _animTime = _animCurrFrame / double(_animFps);
            }
            else
            {
                _animTime += dt;
                _animCurrFrame = _animTime * _animFps;
            }

            emit animFrameChanged(_animCurrFrame);
            if(_animCurrFrame > _animFrameCount)
            {
                _animPlaying = false;
                playStateChanged(_animPlaying);

                return;
            }
            else
            {
                _raytracerState->setFilmRawFilePath(currentFilm());
            }
        }

        double t = _animTime;

        double divThreshold = _pathModel->divThreshold->value(t) * 1.0e-3;
        _raytracerState->setDivergenceThreshold(divThreshold);


        if(!_cameraIsFree)
        {
            // Camera position
            const glm::dvec3 camUp(0, 0, 1);
            glm::dvec3 camTo = _pathModel->cameraTo->value(t);
            glm::dvec3 camEye = _pathModel->cameraEye->value(t);
            _camera->updateView(glm::lookAt(camEye, camTo, camUp));

            // Camera focus
            float dofDist = glm::length(camTo - camEye);
            float dofAper = dofDist + _camAperture + ArtDirectorServer::IMAGE_DEPTH;
            glm::mat4 projection =
                glm::perspectiveFov(
                    glm::radians((float)_pathModel->cameraFoV->value(t))/2,
                    (float) _camera->viewport().x,
                    (float) _camera->viewport().y,
                    dofDist, dofAper);
            _camera->updateProjection(projection);
        }

        // The Fruit's position
        glm::dvec3 theFruitNewPos = _pathModel->theFruitPos->value(t);
        glm::dvec3 theFruitDisplacement = theFruitNewPos - _theFruitPosition;
        _theFruitZone->translate(theFruitDisplacement);
        _theFruitPosition = theFruitNewPos;

        // The Fruit's size
        double theFruitNewHeight = _pathModel->theFruitHeight->value(t);
        double theFruitDHeight = theFruitNewHeight / _theFruitHeight;
        double theFruitDWidth = 1.0 / glm::sqrt(theFruitDHeight);
        Surface::transform(_theFruitSurf, glm::scale(glm::dmat4(), glm::dvec3(
            theFruitDWidth, theFruitDWidth, theFruitDHeight)));
        _theFruitHeight = theFruitNewHeight;


        // Clouds position
        glm::dvec3 cloudsNewPos = _pathModel->clouds->value(t);
        glm::dvec3 cloudsDisplacement = cloudsNewPos - _cloudsPosition;
        _cloudsZone->translate(cloudsDisplacement);
        _cloudsPosition = cloudsNewPos;


        // Sin direction
        double refHour = 17.73;
        glm::dvec3 lastPos(-2.5, -1.5, 0.8);
        glm::dvec3 lastHole(5.0, -4.6, 1.8);
        glm::dvec3 sunAxis = glm::normalize(glm::dvec3(1, 2.55, 0.2));
        glm::dvec4 lastDir(glm::normalize(lastPos - lastHole), 0.0);
        double sunRot = (_pathModel->dayTime->value(t) - refHour) * glm::pi<double>() / 12.0;
        glm::dvec3 sunDir = glm::dvec3(glm::rotate(glm::dmat4(), sunRot, sunAxis) * lastDir);
        _backdrop->setSunDirection( -sunDir );


        // Lights
        double hallIntensity= _pathModel->hallLight->value(t);
        _hallLight->setRadiantFlux(_fixtureRadiantFlux * hallIntensity);
        _hallLight->setIsOn(hallIntensity > 0.0);

        double backIntensity= _pathModel->backLight->value(t);
        _roomLightBack->setRadiantFlux(_fixtureRadiantFlux * backIntensity);
        _roomLightBack->setIsOn(backIntensity > 0.0);

        double frontIntensity= _pathModel->frontLight->value(t);
        _roomLightFront->setRadiantFlux(_fixtureRadiantFlux * frontIntensity);
        _roomLightFront->setIsOn(frontIntensity > 0.0);

        double lampIntensity= _pathModel->lampLight->value(t);
        _lampLight->setRadiantFlux(_lampRadiantFlux * lampIntensity);
        _lampLight->setIsOn(lampIntensity > 0.0);
    }
}

void TheFruitChoreographer::forceUpdate()
{
    update(0.0);
}

int TheFruitChoreographer::animFrameCount() const
{
    return _animFrameCount;
}

std::string TheFruitChoreographer::currentFilm() const
{
    if(!_cameraIsFree)
    {
        std::string filmsDir = getSceneDocument().getAnimationFilmsDirectory() + "/";
        QString fileName = (RECORD_OUPUT_PREFIX + filmsDir).c_str();
        fileName += QString("%1").arg(_animCurrFrame, 4, 10, QChar('0'));

        return fileName.toStdString();
    }
    else
    {
        return RaytracerState::UNSPECIFIED_RAW_FILE;
    }
}

void TheFruitChoreographer::setAnimTimeOffset(double offset)
{
    // Anim time offset doesn't affect frame timings
}

void TheFruitChoreographer::setAnimFrameCount(int frame)
{
    _animFrameCount = frame;
    setAnimFrame(glm::min(_animCurrFrame, frame));
}

void TheFruitChoreographer::setAnimFps(int fps)
{
    _animFps = fps;
}

void TheFruitChoreographer::setAnimFrame(int frame)
{
    if(_animCurrFrame != frame)
    {
        _animCurrFrame = frame;
        _animTime = frame / double(_animFps);

        _raytracerState->setFilmRawFilePath(currentFilm());
        emit animFrameChanged(_animCurrFrame);
        forceUpdate();
    }
}

void TheFruitChoreographer::resetAnimation()
{
    setAnimFrame(0);
}

void TheFruitChoreographer::startRecording()
{
    _isRecording = true;

    QString outputFilmsDir((RECORD_OUPUT_PREFIX +
       getSceneDocument().sceneName() + "/" +
       getSceneDocument().outputFilmDirectory()).c_str());

    QString outputFramesDir((RECORD_OUPUT_PREFIX +
       getSceneDocument().sceneName() + "/" +
       getSceneDocument().outputFrameDirectory()).c_str());

    QDir dir = QDir::current();
    dir.mkpath(outputFilmsDir);
    dir.mkpath(outputFramesDir);
}

void TheFruitChoreographer::stopRecording()
{
    _isRecording = false;
}

void TheFruitChoreographer::playAnimation()
{
    _animPlaying = true;
}

void TheFruitChoreographer::pauseAnimation()
{
    _animPlaying = false;
}

void TheFruitChoreographer::setFastPlay(bool playFast)
{
    _animFastPlay = playFast;

    if(_animPlaying)
        forceUpdate();
}

void TheFruitChoreographer::bindCameraToPath()
{
    _cameraIsFree = false;
    _raytracerState->setFilmRawFilePath(currentFilm());
    _raytracer->film()->clearReferenceShot();
    forceUpdate();
}

void TheFruitChoreographer::freeCameraFromPath()
{
    _cameraIsFree = true;
    _raytracerState->setFilmRawFilePath(currentFilm());
    _raytracer->film()->clearReferenceShot();
    forceUpdate();
}

std::shared_ptr<PathModel> TheFruitChoreographer::pathModel() const
{
    return _pathModel;
}

void TheFruitChoreographer::saveCurrentFrame()
{
    std::string framesDir = getSceneDocument().getAnimationFramesDirectory() + "/";
    QString fileName = (RECORD_OUPUT_PREFIX + framesDir).c_str();

    fileName += QString("%1").arg(_animCurrFrame, 4, 10, QChar('0'));
    if(getSceneDocument().includeSampleCountInFrame())
        fileName += QString("_%1f").arg(_raytracerState->sampleCount());
    if(getSceneDocument().includeRenderTimeInFrame())
        fileName += QString("_%1").arg(SceneDocument::timeToString(_raytracerState->renderTime()).c_str());
    if(getSceneDocument().includeDivergenceInFrame())
        fileName += "_"+QString::number(_raytracerState->divergence(), 'f', 4)+"div";
    fileName += getSceneDocument().outputFrameFormat().c_str();
    std::string stdFileName = fileName.toStdString();

    cellar::Image screenshot;
    cellar::GlToolkit::takeFramebufferShot(screenshot);
    if(screenshot.save(stdFileName))
    {
        getLog().postMessage(new Message('I', false,
            stdFileName + " successfully recorded", "TheFruitChoreographer"));
    }
    else
    {
        getLog().postMessage(new Message('E', false,
            stdFileName + " couldn't be saved", "TheFruitChoreographer"));
    }

    std::string filmName = currentFilm();
    if(_raytracer->film()->saveRawFilm(filmName))
    {
        getLog().postMessage(new Message('I', false,
            filmName + " successfully recorded", "TheFruitChoreographer"));
    }
    else
    {
        getLog().postMessage(new Message('E', false,
            filmName + " couldn't be saved", "TheFruitChoreographer"));
    }
}
