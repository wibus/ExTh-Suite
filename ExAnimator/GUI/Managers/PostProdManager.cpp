#include "PostProdManager.h"

#include <cassert>

#include <GLM/gtc/constants.hpp>

#include <QShowEvent>
#include <QFileDialog>

#include <CellarWorkbench/Misc/StringUtils.h>
#include <CellarWorkbench/Misc/Log.h>
#include <CellarWorkbench/GL/GlInputsOutputs.h>

#include <PropRoom3D/Team/ArtDirector/GlPostProdUnit.h>
#include <PropRoom3D/Team/ArtDirector/ArtDirectorServer.h>

#include "../ExAnimatorGui.h"
#include "ui_ExAnimatorGui.h"

using namespace prop3;


PostProdManager::PostProdManager(Ui::ExAnimatorGui* ui) :
    _ui(ui)
{
    connect(_ui->activateLowpassCheck, &QCheckBox::stateChanged,
            this,                      &PostProdManager::activateLowPassChecked);

    connect(_ui->lowpassSizeCombo, (void (QComboBox::*)(int)) &QComboBox::currentIndexChanged,
            this,                  &PostProdManager::lowpassSizeChanged);

    connect(_ui->lowpassVarianceSpin, (void (QDoubleSpinBox::*)(double)) &QDoubleSpinBox::valueChanged,
            this,                     &PostProdManager::lowpassVarianceChanged);

    connect(_ui->useAdaptativeFilteringCheck, &QCheckBox::stateChanged,
            this,                             &PostProdManager::useAdaptativeFilteringChecked);

    connect(_ui->adaptativeFactorSlider, &QSlider::valueChanged,
            this,                        &PostProdManager::adaptativeFilteringFactorChanged);

    connect(_ui->temperatureSpin,   (void (QSpinBox::*)(int)) &QSpinBox::valueChanged,
            this,                   &PostProdManager::temperatureChanged);

    connect(_ui->temperatureDefaultBtn, &QPushButton::clicked,
            this,                       &PostProdManager::temperatureDefaultClicked);

    connect(_ui->contrastSlider, &QSlider::valueChanged,
            this,                &PostProdManager::contrastChanged);

    connect(_ui->luminositySlider, &QSlider::valueChanged,
            this,                  &PostProdManager::luminosityChanged);

    connect(_ui->equalizeHistogramButton, &QPushButton::clicked,
            this,            &PostProdManager::equalizeHistogram);

    connect(_ui->resetHistogramButton, &QPushButton::clicked,
            this,            &PostProdManager::resetHistogram);

    connect(_ui->saveButton, &QPushButton::clicked,
            this,            &PostProdManager::saveOutputImage);
}

PostProdManager::~PostProdManager()
{
}

void PostProdManager::setPostProdUnit(
        const std::shared_ptr<ArtDirectorServer>& raytracer)
{
    _unitBackend = raytracer->postProdUnit();

    lowpassSizeChanged(_ui->lowpassSizeCombo->currentIndex());
    lowpassVarianceChanged(_ui->lowpassVarianceSpin->value());
    adaptativeFilteringFactorChanged(_ui->adaptativeFactorSlider->value());
    // LowPassChecked must be done after low pass properties to make sur it
    // overwrites those properties in case low pass is deactivated.
    useAdaptativeFilteringChecked(_ui->useAdaptativeFilteringCheck->checkState());
    activateLowPassChecked(_ui->activateLowpassCheck->checkState());
    temperatureChanged(_ui->temperatureSpin->value());
    temperatureDefaultClicked();
    contrastChanged(_ui->contrastSlider->value());
    luminosityChanged(_ui->luminositySlider->value());
}

void PostProdManager::activateLowPassChecked(int state)
{
    bool isChecked = state;
    _ui->lowpassWidget->setEnabled(isChecked);
    _unitBackend->activateLowPassFilter(isChecked);

    if(isChecked)
    {
        updateLowpassKernelTable(_ui->kernelTable, _unitBackend->lowpassKernel());
    }
}

void PostProdManager::lowpassSizeChanged(int sizeIndex)
{
    prop3::KernelSize size;
    if(sizeIndex == 0) size = prop3::KernelSize::SIZE_3x3;
    else if(sizeIndex == 1) size = prop3::KernelSize::SIZE_5x5;
    else assert(false /* Unsupported kernel size */);

    _unitBackend->setLowpassKernelSize(size);
    updateLowpassKernelTable(_ui->kernelTable, _unitBackend->lowpassKernel());
}

void PostProdManager::lowpassVarianceChanged(double variance)
{
    _unitBackend->setLowpassVariance(variance);
    updateLowpassKernelTable(_ui->kernelTable, _unitBackend->lowpassKernel());
}

void PostProdManager::useAdaptativeFilteringChecked(int state)
{
    bool isChecked = state;
    _ui->adaptativeLayout->setEnabled(isChecked);
    _unitBackend->activateAdaptativeFiltering(isChecked);
}

void PostProdManager::adaptativeFilteringFactorChanged(int factor)
{
    float zeroToOne = computeAdaptativeFactor(factor);
    _ui->adaptativeFactorLabel->setText(QString::number(zeroToOne*100) + '%');
    _unitBackend->setAdaptativeFilteringFactor(zeroToOne);
}

void PostProdManager::temperatureChanged(int kelvin)
{
    _ui->temperatureDefaultBtn->setEnabled(
        kelvin != prop3::GlPostProdUnit::DEFAULT_WHITE_TEMPERATURE);
    _unitBackend->setImageTemperature(kelvin);
}

void PostProdManager::temperatureDefaultClicked()
{
    _ui->temperatureSpin->setValue(
        prop3::GlPostProdUnit::DEFAULT_WHITE_TEMPERATURE);
}

void PostProdManager::contrastChanged(int contrast)
{
    float minusOneToOne = computeContrastFactor(contrast);
    _ui->contrastLabel->setText(
        QString::number((int)(minusOneToOne*100)) + '%');
    _unitBackend->setImageContrast(minusOneToOne);
}

void PostProdManager::luminosityChanged(int luminosity)
{
    float zeroToOne = computeLuminosityFactor(luminosity);
    _ui->luminosityLabel->setText(
        QString::number((double)zeroToOne, 'g', 2));
    _unitBackend->setImageLuminosity(zeroToOne);
}

void PostProdManager::equalizeHistogram()
{
    /*
    glm::dvec3 minComp;
    glm::dvec3 maxComp;
    _unitBackend->fetchImageMinAndMax(
        minComp, maxComp);

    double minVal = glm::max(glm::max(minComp.r, minComp.g), minComp.b);
    double maxVal = glm::min(glm::min(maxComp.r, maxComp.g), maxComp.b);
    maxVal = glm::min(maxVal, 2.0);

    double lumi =  0.5 - (minVal + maxVal)*0.5;
    double cont = 1.0 / (maxVal - minVal);
    */

    double lumi, cont;
    _unitBackend->getEqualizedImage(lumi, cont);


    _ui->luminositySlider->setValue(lumi * 50 + 50);
    _ui->contrastSlider->setValue(cont * 50);
}

void PostProdManager::resetHistogram()
{
    _ui->luminositySlider->setValue(50);
    _ui->contrastSlider->setValue(50);
}

void PostProdManager::saveOutputImage()
{
    _unitBackend->saveOutputImage();
}

float PostProdManager::computeLuminosityFactor(int luminosity)
{
    return (luminosity - 50) / 50.0f;
}

float PostProdManager::computeContrastFactor(int contrast)
{
    return contrast / 50.0f;
}

float PostProdManager::computeAdaptativeFactor(int factor)
{
    return factor / 100.0f;
}

void PostProdManager::updateLowpassKernelTable(QTableWidget* widget, const float* kernel)
{
    for(int j=0; j<5; ++j)
    {
        for(int i=0; i<5; ++i)
        {
            QString cellValue;
            float value = kernel[j*5 + i];
            if(value != 0) cellValue = QString::number(value, 'g', 2);
            widget->setCellWidget(i, j, new QLabel(cellValue));
        }
    }
}
