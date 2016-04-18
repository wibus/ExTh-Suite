#ifndef PATHMODEL_H
#define PATHMODEL_H

#include <map>
#include <memory>

#include <CellarWorkbench/Path/AbstractPath.h>

namespace prop3
{
    class StageSet;
}


class PathModel
{
public:
    PathModel();
    virtual ~PathModel();

    void init(const std::shared_ptr<prop3::StageSet>& stageSet);

    double animationLength() const;


    void refreshDebugLines();

    bool isDebugLineVisible(const std::string& name);

    void setDebugLineVisibility(const std::string& name, bool isVisible);


    std::string serialize() const;

    bool deserialize(const std::string& stream);


    std::shared_ptr<cellar::AbstractPath<glm::dvec3>> cameraEye;
    std::shared_ptr<cellar::AbstractPath<glm::dvec3>> cameraTo;
    std::shared_ptr<cellar::AbstractPath<double>> cameraFoV;
    std::shared_ptr<cellar::AbstractPath<glm::dvec3>> theFruitPos;
    std::shared_ptr<cellar::AbstractPath<double>> theFruitHeight;
    std::shared_ptr<cellar::AbstractPath<glm::dvec3>> clouds;
    std::shared_ptr<cellar::AbstractPath<double>> dayTime;
    std::shared_ptr<cellar::AbstractPath<double>> hallLight;
    std::shared_ptr<cellar::AbstractPath<double>> backLight;
    std::shared_ptr<cellar::AbstractPath<double>> frontLight;
    std::shared_ptr<cellar::AbstractPath<double>> lampLight;
    std::shared_ptr<cellar::AbstractPath<double>> divThreshold;


    static const std::string CAMERA_EYE_PATH_NAME;
    static const std::string CAMERA_TO_PATH_NAME;
    static const std::string CAMERA_FOV_PATH_NAME;
    static const std::string THE_FRUIT_POS_PATH_NAME;
    static const std::string THE_FRUIT_HEIGHT_PATH_NAME;
    static const std::string CLOUDS_PATH_NAME;
    static const std::string DAY_TIME_PATH_NAME;
    static const std::string HALL_LIGHT_PATH_NAME;
    static const std::string BACK_LIGHT_PATH_NAME;
    static const std::string FRONT_LIGHT_PATH_NAME;
    static const std::string LAMP_LIGHT_PATH_NAME;
    static const std::string DIV_THRESHOLD_PATH_NAME;

private:
    std::shared_ptr<prop3::StageSet> _stageSet;
    std::map<std::string, bool> _lineVisibility;
};

#endif // PATHMODEL_H
