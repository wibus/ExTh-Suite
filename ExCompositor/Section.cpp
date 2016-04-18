#include "Section.h"

Section::Section(const QString& name, const QString& file, double start, double end) :
      name(name),
      file(file),
      beginingTime(start),
      fadeInTime(start + (end-start)/3.0),
      durationTime(start + (end-start)*2.0/3.0),
      fadeOutTime(end),
      inColor(Qt::white),
      outColor(Qt::white)
{

}

void Section::readTime(double& time, const QSpinBox& minutes, const QDoubleSpinBox& seconds)
{
    time = seconds.value() + minutes.value() * 60.0;
}

void Section::writeTime(const double& time, QSpinBox& minutes, QDoubleSpinBox& seconds)
{
    minutes.setValue(time / 60);
    seconds.setValue(time - (minutes.value() * 60));
}
