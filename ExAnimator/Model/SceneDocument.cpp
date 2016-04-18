#include "SceneDocument.h"

#include <QString>


SceneDocument& getSceneDocument()
{
    return *SceneDocument::getInstance();
}

SceneDocument::SceneDocument() :
    _rootDir("Animations/"),
    _sceneName("Scene"),
    _stageSetName("StageSet"),
    _outputFilmDirectory("films"),
    _outputFrameDirectory("frames"),
    _outputFrameFormat(".png"),
    _includeSampleCountInFrame(true),
    _includeRenderTimeInFrame(true),
    _includeDivergenceInFrame(true),
    _animationTimeOffset(0.0)
{

}

SceneDocument::~SceneDocument()
{

}

void SceneDocument::setDocumentRootDirectory(const std::string& root)
{
    _rootDir = root;
}

void SceneDocument::setSceneName(const std::string& name)
{
    _sceneName = name;
}

void SceneDocument::setStageSetName(const std::string& name)
{
    _stageSetName = name;
}

void SceneDocument::setSoundtrackName(const std::string& name)
{
    _soundtrackName = name;
}

void SceneDocument::setOutputFilmDirectory(const std::string& directory)
{
    _outputFilmDirectory = directory;
}

void SceneDocument::setOutputFrameDirectory(const std::string& directory)
{
    _outputFrameDirectory = directory;
}

void SceneDocument::setOutputFrameFormat(const std::string& format)
{
    _outputFrameFormat = format;
}

void SceneDocument::setIncludeSampleCountInFrame(bool include)
{
    _includeSampleCountInFrame = include;
}

void SceneDocument::setIncludeRenderTimeInFrame(bool include)
{
    _includeRenderTimeInFrame = include;
}

void SceneDocument::setIncludeDivergenceInFrame(bool include)
{
    _includeDivergenceInFrame = include;
}

void SceneDocument::setAnimationTimeOffset(double offset)
{
    _animationTimeOffset = offset;
}

std::string SceneDocument::getStageSetFilePath() const
{
    return _rootDir + _sceneName + "/" + _stageSetName + ".prop3";
}

std::string SceneDocument::getAnimationPathsFilePath() const
{
    return _rootDir + _sceneName + "/Paths.pth";
}

std::string SceneDocument::getSoundtrackFilePath() const
{
    return _rootDir + _sceneName + "/audio/" + _soundtrackName;
}

std::string SceneDocument::getTextureRootDirectory() const
{
    return _rootDir + _sceneName + "/textures/";
}

std::string SceneDocument::getAnimationFilmsDirectory() const
{
    return _rootDir + _sceneName + "/" + _outputFilmDirectory + "/";
}

std::string SceneDocument::getAnimationFramesDirectory() const
{
    return _rootDir + _sceneName + "/" + _outputFrameDirectory + "/";
}

const double SEC_IN_MIN = 60.0;
const double CEN_IN_SEC = 100.0;
std::string SceneDocument::timeToString(double time)
{
    int minutes = int(time / SEC_IN_MIN);
    double minToSec = minutes * SEC_IN_MIN;
    int seconds = int(time - minToSec);
    int centisec = round((time - minToSec - seconds)*CEN_IN_SEC);
    seconds += centisec / int(CEN_IN_SEC);
    centisec -= (centisec / int(CEN_IN_SEC)) * CEN_IN_SEC;

    QString str = QString("%1m%2s%3c")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'))
        .arg(centisec, 2, 10, QChar('0'));
    return str.toStdString();
}
