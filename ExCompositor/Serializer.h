#ifndef ExCompositor_SERIALIZER_H
#define ExCompositor_SERIALIZER_H

#include <memory>
#include <vector>

#include <QString>
#include <QColor>
#include <QJsonValue>

class Section;


class Serializer
{
public:
    Serializer();

    bool write(const std::vector<std::shared_ptr<Section>>& sections, const QString& file);
    bool read(std::vector<std::shared_ptr<Section>>& sections, const QString& file);

protected:
    QJsonValue toArray(const QColor& color);
    QColor toColor(const QJsonValue& array);
};

#endif //ExCompositor_SERIALIZER_H
