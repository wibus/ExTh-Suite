#ifndef ExCompositor_SERIALIZER_H
#define ExCompositor_SERIALIZER_H

#include <map>
#include <vector>
#include <memory>

#include <QString>
#include <QColor>
#include <QJsonValue>

class Section;


class Serializer
{
public:
    Serializer();

    bool write(
            const std::vector<std::shared_ptr<Section>>& sections,
            const std::map<std::string, double>& shading,
            const QString& file);

    bool read(
            std::vector<std::shared_ptr<Section>>& sections,
            std::map<std::string, double>& shading,
            const QString& file);

protected:
    QJsonValue toArray(const QColor& color);
    QColor toColor(const QJsonValue& array);
};

#endif //ExCompositor_SERIALIZER_H
