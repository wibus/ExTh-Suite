#ifndef ExCompositor_SECTION_H
#define ExCompositor_SECTION_H

#include <QObject>
#include <QString>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QRadioButton>


class Section : public QObject
{
    Q_OBJECT

public:
    Section() {}
    Section(const QString& name, const QString& file, double start, double end);

    void readTime(double& time, const QSpinBox& minutes, const QDoubleSpinBox& seconds);
    void writeTime(const double& time, QSpinBox& minutes, QDoubleSpinBox& seconds);

    QString name;
    QString file;
    QRadioButton* radio;

    double beginingTime;
    double fadeInTime;
    double durationTime;
    double fadeOutTime;
    QColor inColor;
    QColor outColor;
};

#endif //ExCompositor_SECTION_H
