#include "Serializer.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

#include "Section.h"

using namespace std;


Serializer::Serializer()
{

}

bool Serializer::write(const std::vector<std::shared_ptr<Section>>& sections, const QString& file)
{
    QFile jsonFile(file);
    if(!jsonFile.open(QFile::WriteOnly))
        return false;

    QJsonDocument doc;

    QJsonArray array;
    for(shared_ptr<Section> s : sections)
    {
        QJsonObject obj;
        obj["Name"] = s->name;
        obj["File"] = s->file;
        obj["begining"] = s->beginingTime;
        obj["fade-in"] = s->fadeInTime;
        obj["duration"] = s->durationTime;
        obj["fade-out"] = s->fadeOutTime;
        obj["in-color"] = toArray(s->inColor);
        obj["out-color"] = toArray(s->outColor);

        array.append(obj);
    }

    doc.setArray(array);
    jsonFile.write(doc.toJson());

    return true;
}

bool Serializer::read(std::vector<std::shared_ptr<Section>>& sections, const QString& file)
{
    QFile jsonFile(file);
    if(!jsonFile.open(QFile::ReadOnly | QFile::Text))
       return false;

    QJsonDocument doc = QJsonDocument::fromJson(jsonFile.readAll());

    QJsonArray array = doc.array();
    for(QJsonValueRef val : array)
    {
        QJsonObject obj = val.toObject();
        std::shared_ptr<Section> s(new Section());
        s->name = obj["Name"].toString();
        s->file = obj["File"].toString();
        s->beginingTime = obj["begining"].toDouble();
        s->fadeInTime = obj["fade-in"].toDouble();
        s->durationTime = obj["duration"].toDouble();
        s->fadeOutTime = obj["fade-out"].toDouble();
        s->inColor = toColor(obj["in-color"]);
        s->outColor = toColor(obj["out-color"]);

        sections.push_back(s);
    }

    return true;
}

QJsonValue Serializer::toArray(const QColor& color)
{
    QJsonArray array;
    array.append(color.red());
    array.append(color.green());
    array.append(color.blue());
    return array;
}

QColor Serializer::toColor(const QJsonValue& array)
{
    QJsonArray a = array.toArray();
    return QColor(a[0].toInt(), a[1].toInt(), a[2].toInt());
}
