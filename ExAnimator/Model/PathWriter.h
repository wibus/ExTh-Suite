#ifndef PATHWRITER_H
#define PATHWRITER_H

#include <string>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include <CellarWorkbench/Path/PathVisitor.h>
#include <CellarWorkbench/Path/CompositePath.h>
#include <CellarWorkbench/Path/CubicSplinePath.h>
#include <CellarWorkbench/Path/BasisSplinePath.h>
#include <CellarWorkbench/Path/PolynomialPath.h>
#include <CellarWorkbench/Path/LinearPath.h>


template<typename Data>
class PathWriter : public cellar::PathVisitor<Data>
{
public:
    PathWriter();
    virtual ~PathWriter();


    virtual QJsonValue write(const std::string &name,
                             cellar::AbstractPath<Data>& path);


    virtual void visit(cellar::PointPath<Data>& path) override;

    virtual void visit(cellar::LinearPath<Data>& path) override;

    virtual void visit(cellar::CubicSplinePath<Data>& path) override;

    virtual void visit(cellar::BasisSplinePath<Data>& path) override;

    virtual void visit(cellar::PolynomialPath<Data>& path) override;

    virtual void visit(cellar::CompositePath<Data>& path) override;

protected:
    virtual QJsonValue printValue(const Data& value);

private:
    QJsonObject _lastObj;
};



// IMPLEMENTATION //

template<typename Data>
PathWriter<Data>::PathWriter()
{

}

template<typename Data>
PathWriter<Data>::~PathWriter()
{

}

template<typename Data>
QJsonValue PathWriter<Data>::write(const std::string& name, cellar::AbstractPath<Data>& path)
{
    path.accept(*this);

    _lastObj["Name"] = name.c_str();
    if(sizeof(Data) == sizeof(double))
        _lastObj["Data"] = "double";
    else
        _lastObj["Data"] = "dvec3";

    return _lastObj;
}

template<typename Data>
void PathWriter<Data>::visit(cellar::PointPath<Data>& path)
{
    QJsonObject obj;
    obj["Type"] = "PointPath";
    obj["Duration"] = path.duration();

    QJsonArray ctrlPts;
    ctrlPts.append(printValue(path.value()));
    obj["CtrlPts"] = ctrlPts;

    _lastObj = obj;
}

template<typename Data>
void PathWriter<Data>::visit(cellar::LinearPath<Data>& path)
{
    QJsonObject obj;
    obj["Type"] = "LinearPath";
    obj["Duration"] = path.duration();

    QJsonArray ctrlPts;
    ctrlPts.append(printValue(path.begin()));
    ctrlPts.append(printValue(path.end()));
    obj["CtrlPts"] = ctrlPts;

    _lastObj = obj;
}

template<typename Data>
void PathWriter<Data>::visit(cellar::CubicSplinePath<Data>& path)
{
    QJsonObject obj;
    obj["Type"] = "CubicSplinePath";
    obj["Duration"] = path.duration();

    QJsonArray weights;
    for(const double& weight : path.weights())
        weights.append(QJsonValue(weight));
    obj["Weights"] = weights;

    QJsonArray ctrlPts;
    for(const Data& value : path.ctrlPts())
        ctrlPts.append(printValue(value));
    obj["CtrlPts"] = ctrlPts;

    _lastObj = obj;
}

template<typename Data>
void PathWriter<Data>::visit(cellar::BasisSplinePath<Data>& path)
{
    QJsonObject obj;
    obj["Type"] = "BasisSplinePath";
    obj["Duration"] = path.duration();
    obj["Degree"] = path.degree();

    QJsonArray ctrlPts;
    for(const Data& value : path.ctrlPts())
        ctrlPts.append(printValue(value));
    obj["CtrlPts"] = ctrlPts;

    _lastObj = obj;
}

template<typename Data>
void PathWriter<Data>::visit(cellar::PolynomialPath<Data>& path)
{
    QJsonObject obj;
    obj["Type"] = "PolynomialPath";
    obj["Duration"] = path.duration();

    QJsonArray weights;
    for(const double& weight : path.weights())
        weights.append(QJsonValue(weight));
    obj["Weights"] = weights;

    QJsonArray ctrlPts;
    for(const Data& value : path.ctrlPts())
        ctrlPts.append(printValue(value));
    obj["CtrlPts"] = ctrlPts;

    _lastObj = obj;
}

template<typename Data>
void PathWriter<Data>::visit(cellar::CompositePath<Data>& path)
{
    QJsonObject obj;
    obj["Type"] = "CompositePath";
    QJsonArray pathArray;

    for(const auto& sub : path.paths())
    {
        sub->accept(*this);
        pathArray.append(_lastObj);
    }

    obj["Paths"] = pathArray;
    _lastObj = obj;
}

template<>
QJsonValue PathWriter<double>::printValue(const double& value)
{
    return QJsonValue(value);
}

template<>
QJsonValue PathWriter<glm::dvec3>::printValue(const glm::dvec3& value)
{
    return QJsonArray({value.x, value.y, value.z});
}

#endif // PATHWRITER_H
