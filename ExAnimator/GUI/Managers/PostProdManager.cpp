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

    connect(_ui->exposureGainSpin, (void (QDoubleSpinBox::*)(double)) &QDoubleSpinBox::valueChanged,
            this,                  &PostProdManager::exposureGainChanged);

    connect(_ui->middleGraySpin, (void (QDoubleSpinBox::*)(double)) &QDoubleSpinBox::valueChanged,
            this,                  &PostProdManager::middleGrayChanged);

    connect(_ui->contrastSpin, (void (QDoubleSpinBox::*)(double)) &QDoubleSpinBox::valueChanged,
            this,                &PostProdManager::contrastChanged);

    connect(_ui->acesTonemappingCheck, &QCheckBox::toggled,
            this,                  &PostProdManager::acesTonemmapingChanged);

    connect(_ui->gammaSpin, (void (QDoubleSpinBox::*)(double)) &QDoubleSpinBox::valueChanged,
            this,                  &PostProdManager::gammaChanged);

    connect(_ui->autoExposeButton, &QPushButton::clicked,
            this,            &PostProdManager::autoExpose);

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
    exposureGainChanged(_ui->exposureGainSpin->value());
    contrastChanged(_ui->contrastSpin->value());
    middleGrayChanged(_ui->middleGraySpin->value());
    acesTonemmapingChanged(_ui->acesTonemappingCheck->isChecked());
    gammaChanged(_ui->gammaSpin->value());
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

void PostProdManager::contrastChanged(double contrast)
{
    _unitBackend->setImageContrast(contrast);
}

void PostProdManager::middleGrayChanged(double middleGray)
{
    _unitBackend->setImageMiddleGray(middleGray);
}

void PostProdManager::acesTonemmapingChanged(bool isActive)
{
    _unitBackend->setAcesTonemappingActive(isActive);
}

void PostProdManager::exposureGainChanged(double exposure)
{
    _unitBackend->setExposureGain(glm::vec3(exposure));
}

void PostProdManager::gammaChanged(double gamma)
{
    _unitBackend->setImageGamma(gamma);
}

void PostProdManager::autoExpose()
{
    _ui->exposureGainSpin->setValue(
        _unitBackend->getAutoExposure(0.60));
}

void PostProdManager::resetHistogram()
{
    _ui->exposureGainSpin->setValue(1.0);
    _ui->middleGraySpin->setValue(0.5);
    _ui->contrastSpin->setValue(1);
}

void PostProdManager::saveOutputImage()
{
    _unitBackend->saveOutputImage();
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
