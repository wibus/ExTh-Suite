#ifndef PATHREADER_H
#define PATHREADER_H

#include <map>
#include <memory>

#include <QJsonDocument>

#include <CellarWorkbench/Path/AbstractPath.h>


class PathReader
{
public:
    PathReader();
    virtual ~PathReader();

    bool read(const QJsonDocument& jsonDoc);

    std::shared_ptr<cellar::AbstractPath<double>> doublePath(const std::string& name);

    std::shared_ptr<cellar::AbstractPath<glm::dvec3>> dvec3Path(const std::string& name);

protected:
    template<typename Data>
    Data readValue(const QJsonValue& value, bool& ok);

private:
    std::map<std::string, std::shared_ptr<cellar::AbstractPath<double>>> _doublePaths;
    std::map<std::string, std::shared_ptr<cellar::AbstractPath<glm::dvec3>>> _dvec3Paths;
};

#endif // PATHREADER_H
