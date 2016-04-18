#include "CameraManager.h"

#include <GLM/gtc/matrix_transform.hpp>

#include <QKeyEvent>

#include <PropRoom3D/Team/ArtDirector/RaytracerState.h>

#include "ui_ExAnimatorGui.h"


CameraManager::CameraManager(Ui::ExAnimatorGui* ui) :
    _ui(ui),
    _fullscreenWindow(nullptr)
{
    _ui->fieldOfViewSlider->setMinimum(_ui->fieldOfViewSpin->minimum());
    _ui->fieldOfViewSlider->setMaximum(_ui->fieldOfViewSpin->maximum());
    _ui->fieldOfViewSlider->setValue(_ui->fieldOfViewSpin->value());
}

CameraManager::~CameraManager()
{    
    _ui->raytracedScrollView->setWidget(_view);
    delete _fullscreenWindow;
}

void CameraManager::setView(QWidget* view)
{
    _view = view;

    _ui->raytracedScrollView->setWidget(_view);

    _fullscreenWindow = new QWidget();
    _fullscreenWindow->installEventFilter(this);
    QLayout* fullscreenLayout = new QHBoxLayout();
    fullscreenLayout->setAlignment(Qt::AlignCenter);
    _fullscreenWindow->setLayout(fullscreenLayout);
    QPalette background(QPalette::Background, Qt::black);
    _fullscreenWindow->setPalette(background);

    _ui->colorOutputCombo->addItem(prop3::RaytracerState::COLOROUTPUT_ALBEDO.c_str());
    _ui->colorOutputCombo->addItem(prop3::RaytracerState::COLOROUTPUT_WEIGHT.c_str());
    _ui->colorOutputCombo->addItem(prop3::RaytracerState::COLOROUTPUT_DIVERGENCE.c_str());
    _ui->colorOutputCombo->addItem(prop3::RaytracerState::COLOROUTPUT_VARIANCE.c_str());
    _ui->colorOutputCombo->addItem(prop3::RaytracerState::COLOROUTPUT_PRIORITY.c_str());
    _ui->colorOutputCombo->addItem(prop3::RaytracerState::COLOROUTPUT_REFERENCE.c_str());
    _ui->colorOutputCombo->addItem(prop3::RaytracerState::COLOROUTPUT_COMPATIBILITY.c_str());

    connect(_ui->customOutputDimensionsRadioBtn, &QRadioButton::toggled,
            _ui->customOutputDimensionsWidget,   &QWidget::setEnabled);

    connect(_ui->matchCaptureSizeRadioBtn, &QRadioButton::toggled,
            this, &CameraManager::outputMatchCaptureDimensions);

    connect(_ui->captureDimensionsSpinX,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &CameraManager::captureDimensionsChanged);

    connect(_ui->captureDimensionsSpinY,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &CameraManager::captureDimensionsChanged);

    connect(_ui->outputDimensionsSpinX,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &CameraManager::outputDimensionsChanged);

    connect(_ui->outputDimensionsSpinY,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &CameraManager::outputDimensionsChanged);

    connect(_ui->fieldOfViewSlider,
            static_cast<void (QSlider::*)(int)>(&QSlider::valueChanged),
            _ui->fieldOfViewSpin,   &QSpinBox::setValue);

    connect(_ui->fieldOfViewSpin,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            _ui->fieldOfViewSlider, &QSlider::setValue);

    connect(_ui->fieldOfViewSpin,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &CameraManager::fieldOfViewChanged);

    connect(_ui->dofDistanceSpin,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &CameraManager::dofDistanceChanged);

    connect(_ui->dofApertureSpin,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &CameraManager::dofApertureChanged);


    connect(_ui->fullscreenButton, &QPushButton::clicked,
            this,                  &CameraManager::enterFullscreen);

    connect(_ui->updateEachTileCheck, &QCheckBox::toggled,
            this, &CameraManager::updateEachTileCheckChanged);

    connect(_ui->colorOutputCombo,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &CameraManager::colorOutputTypeChanged);

    outputDimensionsChanged(0);
    outputMatchCaptureDimensions(_ui->matchCaptureSizeRadioBtn->isChecked());
}

void CameraManager::setRaytracer(const std::shared_ptr<prop3::ArtDirectorServer>& raytracer)
{
    _raytracer = raytracer;
    updateEachTileCheckChanged(true);
    colorOutputTypeChanged(0);

    _camera = _raytracer->camera();
    captureDimensionsChanged(0);
}

void CameraManager::captureDimensionsChanged(int)
{
    _camera->updateViewport(
        _ui->captureDimensionsSpinX->value(),
        _ui->captureDimensionsSpinY->value());
    updateCameraProjection();
}

void CameraManager::outputDimensionsChanged(int)
{
    _view->setFixedSize(
        _ui->outputDimensionsSpinX->value(),
        _ui->outputDimensionsSpinY->value());
}

void CameraManager::outputMatchCaptureDimensions(bool match)
{
    if(match)
    {
        connect(_ui->captureDimensionsSpinX,
                static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                _ui->outputDimensionsSpinX, &QSpinBox::setValue);

        connect(_ui->captureDimensionsSpinY,
                static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                _ui->outputDimensionsSpinY, &QSpinBox::setValue);

        _ui->outputDimensionsSpinX->setValue(
            _ui->captureDimensionsSpinX->value());
        _ui->outputDimensionsSpinY->setValue(
            _ui->captureDimensionsSpinY->value());
    }
    else
    {
        disconnect(_ui->captureDimensionsSpinX,
                   static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                   _ui->outputDimensionsSpinX, &QSpinBox::setValue);

        disconnect(_ui->captureDimensionsSpinY,
                   static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                   _ui->outputDimensionsSpinY, &QSpinBox::setValue);
    }
}

void CameraManager::fieldOfViewChanged(int)
{
    updateCameraProjection();
}

void CameraManager::dofDistanceChanged(double unused)
{
    updateCameraProjection();
}

void CameraManager::dofApertureChanged(double unused)
{
    updateCameraProjection();
}

void CameraManager::enterFullscreen()
{
    _ui->raytracedScrollView->takeWidget();

    _fullscreenWindow->layout()->addWidget(_view);
    _fullscreenWindow->showFullScreen();
    _fullscreenWindow->setFocus();
}

void CameraManager::exitFullscreen()
{
    _fullscreenWindow->hide();
    _fullscreenWindow->layout()->takeAt(0);

    _ui->raytracedScrollView->setWidget(_view);
}

void CameraManager::updateEachTileCheckChanged(bool unused)
{
    _raytracer->raytracerState()->setUpdateEachTile(
        _ui->updateEachTileCheck->isChecked());
}

void CameraManager::colorOutputTypeChanged(int unused)
{
    _raytracer->raytracerState()->setColorOutputType(
        _ui->colorOutputCombo->currentText().toStdString());

    if(_raytracer.get() != nullptr)
        _raytracer->draw(prop3::ArtDirectorServer::FORCE_REFRESH_DT);
}

void CameraManager::updateCameraProjection()
{
    float dofDist = _ui->dofDistanceSpin->value();
    float dofAper = dofDist + _ui->dofApertureSpin->value() +
            prop3::ArtDirectorServer::IMAGE_DEPTH;

    glm::mat4 projection =
        glm::perspectiveFov(
            glm::radians((float)_ui->fieldOfViewSpin->value())/2,
            (float) _ui->captureDimensionsSpinX->value(),
            (float) _ui->captureDimensionsSpinY->value(),
            dofDist, dofAper);

    _camera->updateProjection(projection);
}

bool CameraManager::eventFilter(QObject* obj, QEvent* event)
{
    if(obj == _fullscreenWindow)
    {
        if(event->type() == QEvent::KeyPress)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if(keyEvent->key() == Qt::Key_Escape)
            {
                exitFullscreen();
                return true;
            }
        }

        return false;
    }
    else
    {
        return QObject::eventFilter(obj, event);
    }
}

void CameraManager::onFreeCamera(bool isFree)
{
    if(isFree)
        dofDistanceChanged(0.0);
}
