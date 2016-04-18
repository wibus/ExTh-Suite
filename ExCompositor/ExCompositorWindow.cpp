#include "ExCompositorWindow.h"
#include "ui_ExCompositorWindow.h"

#include <GLM/glm.hpp>

#include <QDir>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QColorDialog>
#include <QDebug>

#include "Section.h"
#include "Serializer.h"

using namespace std;


const QString ANIMATION_ROOT = "Animations/The Fruit/";
const QString ANIMATION_FILE = "[Animation]";
const QString COMPOSITION_FILE = "Composition.cmp";

ExCompositorWindow::ExCompositorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ExCompositorWindow),
    _currentSection(nullptr),
    _imageLabel(nullptr),
    _currentPixmap(nullptr)
{
    ui->setupUi(this);

    _sectionLayout = new QVBoxLayout();
    ui->sectionGroup->setLayout(_sectionLayout);
    _sectionLayout->addStretch();

    loadProject();
    _sections.front()->radio->setChecked(true);

    connect(ui->frameSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &ExCompositorWindow::frameChanged);
    connect(ui->frameSlider, &QSlider::valueChanged,
            this, &ExCompositorWindow::frameChanged);

    connect(ui->inColorButton, &QPushButton::clicked,
            this, &ExCompositorWindow::inColorButton);
    connect(ui->outColorButton, &QPushButton::clicked,
            this, &ExCompositorWindow::outColorButton);

    connect(ui->loadProjectButton, &QPushButton::clicked,
            this, &ExCompositorWindow::loadProject);
    connect(ui->saveProjectButton, &QPushButton::clicked,
            this, &ExCompositorWindow::saveProject);
    connect(ui->generateButton, &QPushButton::clicked,
            this, &ExCompositorWindow::generateComposites);

    _imageLabel = new QLabel();
    ui->imageScrollArea->setWidget(_imageLabel);

    frameChanged(0);
}

ExCompositorWindow::~ExCompositorWindow()
{
    delete ui;
}

void ExCompositorWindow::frameChanged(int frame)
{
    ui->frameSpin->setValue(frame);
    ui->frameSlider->setValue(frame);
    ui->timeLineMin->setValue((frame / 24.0)/60);
    ui->timeLineSec->setValue((frame / 24.0) - ui->timeLineMin->value()*60);

    double time = frame/ 24.0;

    shared_ptr<Section> section;
    for(shared_ptr<Section> s : _sections)
    {
        if(time < s->fadeOutTime)
        {
            section = s;
            break;
        }
    }

    if(section.get() == nullptr)
        return;

    delete _currentPixmap;
    _currentPixmap = nullptr;
    if(section->file != ANIMATION_FILE)
    {
        _currentPixmap = new QPixmap(ANIMATION_ROOT + "titles/"+section->file);
    }
    else
    {
        int animFirstFrame = section->beginingTime * 24;
        int animFrame = frame - animFirstFrame;

        if(animFrame > 0)
        {
            QStringList filters;
            filters << QString("%1*").arg(animFrame, 4, 10, QChar('0'));

            QDir dir(QDir::current());
            dir.cd("frames");
            QStringList dirs = dir.entryList(filters);

            if(dirs.size() > 0)
            {
                _currentPixmap = new QPixmap(ANIMATION_ROOT + "frames/"+dirs.at(0));
            }
            else
            {
                qDebug() << dir.absolutePath() << filters.at(0);
                _currentPixmap = new QPixmap(1280, 720);
            }
        }
        else
        {
            _currentPixmap = new QPixmap(1280, 720);
        }
    }

    if(time < section->fadeInTime)
    {
        QPainter p(_currentPixmap);
        p.setPen(QPen(section->inColor));
        p.setBrush(QBrush(section->inColor));
        p.setOpacity(1.0 - glm::smoothstep(section->beginingTime, section->fadeInTime, time));
        p.drawRect(_currentPixmap->rect());
    }
    else if(time < section->durationTime)
    {
        //nothing
    }
    else if(time < section->fadeOutTime)
    {
        QPainter p(_currentPixmap);
        p.setPen(QPen(section->outColor));
        p.setBrush(QBrush(section->outColor));
        p.setOpacity(glm::smoothstep(section->durationTime, section->fadeOutTime, time));
        p.drawRect(_currentPixmap->rect());
    }

    _imageLabel->setPixmap(*_currentPixmap);
}

void ExCompositorWindow::sectionChaged(bool checked)
{
    if(checked)
    {
        ui->beginingMinSpin->disconnect(this);
        ui->beginingSecSpin->disconnect(this);

        ui->fadeInMinSpin->disconnect(this);
        ui->fadeInSecSpin->disconnect(this);

        ui->durationMinSpin->disconnect(this);
        ui->durationSecSpin->disconnect(this);

        ui->fadeOutMinSpin->disconnect(this);
        ui->fadeOutSecSpin->disconnect(this);

        for(shared_ptr<Section> s : _sections)
        {
            if(s->radio->isChecked())
            {
                _currentSection = s;
                break;
            }
        }

        _currentSection->writeTime(_currentSection->beginingTime,   *ui->beginingMinSpin,   *ui->beginingSecSpin);
        _currentSection->writeTime(_currentSection->fadeInTime,     *ui->fadeInMinSpin,     *ui->fadeInSecSpin);
        _currentSection->writeTime(_currentSection->durationTime,   *ui->durationMinSpin,   *ui->durationSecSpin);
        _currentSection->writeTime(_currentSection->fadeOutTime,    *ui->fadeOutMinSpin,    *ui->fadeOutSecSpin);

        connect(ui->beginingMinSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                this, &ExCompositorWindow::beginingMin);
        connect(ui->beginingSecSpin, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                this, &ExCompositorWindow::beginingSec);

        connect(ui->fadeInMinSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                this, &ExCompositorWindow::fadeInMin);
        connect(ui->fadeInSecSpin, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                this, &ExCompositorWindow::fadeInSec);

        connect(ui->durationMinSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                this, &ExCompositorWindow::durationMin);
        connect(ui->durationSecSpin, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                this, &ExCompositorWindow::durationSec);

        connect(ui->fadeOutMinSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                this, &ExCompositorWindow::fadeOutMin);
        connect(ui->fadeOutSecSpin, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                this, &ExCompositorWindow::fadeOutSec);
    }
}


void ExCompositorWindow::beginingMin(int min)
{
    _currentSection->readTime(
        _currentSection->beginingTime,
        *ui->beginingMinSpin,
        *ui->beginingSecSpin);
}

void ExCompositorWindow::beginingSec(double sec)
{
    _currentSection->readTime(
        _currentSection->beginingTime,
        *ui->beginingMinSpin,
        *ui->beginingSecSpin);
}

void ExCompositorWindow::fadeInMin(int min)
{
    _currentSection->readTime(
        _currentSection->fadeInTime,
        *ui->fadeInMinSpin,
        *ui->fadeInSecSpin);
}

void ExCompositorWindow::fadeInSec(double sec)
{
    _currentSection->readTime(
        _currentSection->fadeInTime,
        *ui->fadeInMinSpin,
        *ui->fadeInSecSpin);
}

void ExCompositorWindow::durationMin(int min)
{
    _currentSection->readTime(
        _currentSection->durationTime,
        *ui->durationMinSpin,
        *ui->durationSecSpin);
}

void ExCompositorWindow::durationSec(double sec)
{
    _currentSection->readTime(
        _currentSection->durationTime,
        *ui->durationMinSpin,
        *ui->durationSecSpin);
}

void ExCompositorWindow::fadeOutMin(int min)
{
    _currentSection->readTime(
        _currentSection->fadeOutTime,
        *ui->fadeOutMinSpin,
        *ui->fadeOutSecSpin);
    updateTimeLine();
}

void ExCompositorWindow::fadeOutSec(double sec)
{
    _currentSection->readTime(
        _currentSection->fadeOutTime,
        *ui->fadeOutMinSpin,
        *ui->fadeOutSecSpin);
    updateTimeLine();
}

void ExCompositorWindow::inColorButton()
{
    QColorDialog dialog(_currentSection->inColor);
    dialog.exec();
    _currentSection->inColor = dialog.currentColor();
}

void ExCompositorWindow::outColorButton()
{
    QColorDialog dialog(_currentSection->outColor);
    dialog.exec();
    _currentSection->outColor = dialog.currentColor();
}

void ExCompositorWindow::loadProject()
{
    _sections.clear();
    Serializer serializer;
    serializer.read(_sections, ANIMATION_ROOT + COMPOSITION_FILE);

    while(!_sectionLayout->isEmpty())
    {
        QLayoutItem* item =  _sectionLayout->takeAt(0);
        delete item->widget();
        delete item;
    }

    // Seriously WTF!
    QLayoutItem* item =  _sectionLayout->takeAt(0);
    delete item->widget();
    delete item;

    for(shared_ptr<Section> section : _sections)
        addSection(_sectionLayout, *section);
    _sectionLayout->addStretch();

    _sections.front()->radio->setChecked(true);

    updateTimeLine();
}

void ExCompositorWindow::saveProject()
{
    Serializer serializer;
    serializer.write(_sections, COMPOSITION_FILE);
}

void ExCompositorWindow::addSection(QLayout* layout, Section& section)
{
    section.radio = new QRadioButton(section.name);
    layout->addWidget(section.radio);

    connect(section.radio, &QRadioButton::toggled,
            this, &ExCompositorWindow::sectionChaged);
}

void ExCompositorWindow::updateTimeLine()
{
    double lastSec = _sections.back()->fadeOutTime;
    int frameCount = lastSec * 24;

    ui->frameSpin->setMaximum(frameCount);
    ui->frameSpin->setSuffix(QString("/%1").arg(frameCount));
    ui->frameSlider->setMaximum(frameCount);
}

void ExCompositorWindow::generateComposites()
{
    int frameCount = ui->frameSpin->maximum();
    for(int frame=1043; frame <= 1043; ++frame)
    {
        frameChanged(frame);

        if(_currentPixmap)
        {
            QFile file(QString("composites/%1.png").arg(frame, 4, 10, QChar('0')));
            file.open(QIODevice::WriteOnly);

            _currentPixmap->save(&file, "PNG", 80);

            QCoreApplication::processEvents();
        }
    }
}
