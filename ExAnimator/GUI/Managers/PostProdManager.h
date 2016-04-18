#ifndef POSTPRODMANAGER_H
#define POSTPRODMANAGER_H

#include <memory>

#include <QTableWidget>

namespace Ui
{
    class ExAnimatorGui;
}

namespace prop3
{
    class GlPostProdUnit;
}


class PostProdManager : public QObject
{
    Q_OBJECT

public:
    PostProdManager(Ui::ExAnimatorGui* ui);
    virtual ~PostProdManager();

    virtual void setPostProdUnit(
        const std::shared_ptr<prop3::GlPostProdUnit>& unitBackend);

private slots:
    virtual void activateLowPassChecked(int state);
    virtual void lowpassSizeChanged(int sizeIndex);
    virtual void lowpassVarianceChanged(double variance);
    virtual void useAdaptativeFilteringChecked(int state);
    virtual void adaptativeFilteringFactorChanged(int factor);
    virtual void temperatureChanged(int kelvin);
    virtual void temperatureDefaultClicked();
    virtual void contrastChanged(int contrast);
    virtual void luminosityChanged(int luminosity);
    virtual void equalizeHistogram();
    virtual void resetHistogram();
    virtual void saveOutputImage();

private:
    static float computeLuminosityFactor(int luminosity);
    static float computeContrastFactor(int contrast);
    static float computeAdaptativeFactor(int factor);
    static void updateLowpassKernelTable(QTableWidget* widget, const float* kernel);


private:
    Ui::ExAnimatorGui* _ui;
    std::shared_ptr<prop3::GlPostProdUnit> _unitBackend;
};

#endif // POSTPRODMANAGER_H
