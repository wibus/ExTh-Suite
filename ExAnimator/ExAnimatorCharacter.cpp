#include "ExAnimatorCharacter.h"

#include <iostream>

#include <GLM/gtc/matrix_transform.hpp>

#include <CellarWorkbench/Camera/Camera.h>
#include <CellarWorkbench/Camera/CameraManFree.h>
#include <CellarWorkbench/Misc/StringUtils.h>

#include <PropRoom2D/Prop/Hud/TextHud.h>
#include <PropRoom2D/Team/AbstractTeam.h>

#include <PropRoom3D/Node/StageSet.h>
#include <PropRoom3D/Node/Prop/Prop.h>
#include <PropRoom3D/Node/Prop/Surface/Box.h>
#include <PropRoom3D/Node/Prop/Surface/Sphere.h>
#include <PropRoom3D/Node/Prop/Surface/Plane.h>
#include <PropRoom3D/Node/Prop/Surface/Quadric.h>
#include <PropRoom3D/Node/Prop/Material/UniformStdMaterial.h>
#include <PropRoom3D/Node/Prop/Coating/UniformStdCoating.h>
#include <PropRoom3D/Node/Prop/Coating/TexturedStdCoating.h>
#include <PropRoom3D/Node/Light/Backdrop/ProceduralSun.h>
#include <PropRoom3D/Node/Light/LightBulb/CircularLight.h>
#include <PropRoom3D/Node/Light/LightBulb/SphericalLight.h>
#include <PropRoom3D/Team/AbstractTeam.h>

#include <Scaena/Play/Play.h>
#include <Scaena/Play/View.h>
#include <Scaena/StageManagement/Event/StageTime.h>
#include <Scaena/StageManagement/Event/SynchronousKeyboard.h>
#include <Scaena/StageManagement/Event/SynchronousMouse.h>
#include <Scaena/StageManagement/Event/KeyboardEvent.h>

#include "Model/SceneDocument.h"

using namespace cellar;
using namespace prop2;
using namespace prop3;
using namespace scaena;


typedef std::shared_ptr<StageZone> pZone;
typedef std::shared_ptr<LightBulb> pLight;
typedef std::shared_ptr<Surface> pSurf;
typedef std::shared_ptr<Material> pMat;
typedef std::shared_ptr<Coating> pCoat;
typedef std::shared_ptr<Prop> pProp;


ExAnimatorCharacter::ExAnimatorCharacter() :
    Character("Experimental Animator Character"),
    _camVelocity(4.1)
{
}

void ExAnimatorCharacter::enterStage()
{
    //* Choose and setup stageSet
    setupTheFruitStageSet();
    //setupManufacturingStageSet();
    //setupCornBoardStageSet();

    /*
    play().propTeam3D()->saveScene(
        getSceneDocument().getStageSetFilePath());
    play().propTeam3D()->loadScene(
        getSceneDocument().getStageSetFilePath());
    play().propTeam3D()->saveScene(
        getSceneDocument().getSceneRootDirectory() + "/StageCopy.prop3");
    */
}

void ExAnimatorCharacter::beginStep(const StageTime &time)
{
    float elapsedtime = time.elapsedTime();
    float velocity = _camVelocity * elapsedtime;
    const float turnSpeed = 0.3f * elapsedtime;

    SynchronousMouse& syncMouse = *play().synchronousMouse();
    SynchronousKeyboard& syncKeyboard = *play().synchronousKeyboard();


    if(syncKeyboard.isAsciiPressed('Q'))
    {
        _camVelocity = glm::max(0.1, _camVelocity - 0.2);
        std::cout << "Camera velocity: " << _camVelocity << "m/s" << std::endl;
    }
    if(syncKeyboard.isAsciiPressed('E'))
    {
        _camVelocity = glm::min(15.0, _camVelocity + 0.2);
        std::cout << "Camera velocity: " << _camVelocity << "m/s" << std::endl;
    }

    bool moved = false;
    if(syncKeyboard.isAsciiPressed('w'))
    {
        _camMan->forward(velocity);
        moved = true;
    }
    if(syncKeyboard.isAsciiPressed('s'))
    {
        _camMan->forward(-velocity);
        moved = true;
    }
    if(syncKeyboard.isAsciiPressed('a'))
    {
        _camMan->sideward(-velocity);
        moved = true;
    }
    if(syncKeyboard.isAsciiPressed('d'))
    {
        _camMan->sideward(velocity);
        moved = true;
    }

    if(syncMouse.displacement() != glm::ivec2(0, 0))
    {
        if(syncMouse.buttonIsPressed(EMouseButton::LEFT))
        {
            _camMan->pan( syncMouse.displacement().x * -turnSpeed);
            _camMan->tilt(syncMouse.displacement().y * -turnSpeed);
        }
        else if(syncMouse.buttonIsPressed(EMouseButton::RIGHT))
        {
            const glm::dvec3 ROT_AXIS = glm::normalize(glm::dvec3(1, 5, 0));
            const glm::dmat3 ROT_MAT = glm::dmat3(glm::rotate(glm::dmat4(), syncMouse.displacement().y / 100.0, ROT_AXIS));
            _backdrop->setSunDirection(ROT_MAT * _backdrop->sunDirection());
        }

    }

    if(moved)
    {
        glm::dvec3 p = _camMan->position();
        std::cout << "Pos: (" << p.x << ", " << p.y << ", " << p.z << "); ";
        std::cout << "Dist: " << glm::distance(p, glm::dvec3()) << "m" << std::endl;
    }
}

void ExAnimatorCharacter::draw(const std::shared_ptr<scaena::View> &,
                                  const scaena::StageTime&time)
{
}

void ExAnimatorCharacter::exitStage()
{
    play().propTeam3D()->terminate();
}

bool ExAnimatorCharacter::keyPressEvent(const scaena::KeyboardEvent& event)
{
    return false;
}

void ExAnimatorCharacter::setupTheFruitStageSet()
{
    std::shared_ptr<Camera> camera = play().view()->camera3D();
    _camMan.reset(new CameraManFree(camera, false));

    //*
    glm::dvec3 focusPos = glm::dvec3(0, 0, 1.0);
    glm::dvec3 camPos = focusPos + glm::dvec3(8, -15, 5) * 1.3;
    /*/
    glm::dvec3 focusPos = glm::dvec3(-5, 5, 1.4);
    glm::dvec3 camPos = glm::dvec3(-0.5, -4, 1.7);
    //*/
    glm::dvec3 dir = glm::normalize(focusPos - camPos);
    double tilt = glm::atan(dir.z, glm::length(glm::dvec2(dir.x, dir.y)));
    double pan = glm::atan(dir.y, dir.x);

    _camMan->setOrientation(pan, tilt);
    _camMan->setPosition(camPos);


    // Bounding Hierarchy
    std::shared_ptr<StageSet> stageSet = play().propTeam3D()->stageSet();

    // Environment
    _backdrop = static_cast<ProceduralSun*>(stageSet->backdrop().get());
}


void ExAnimatorCharacter::setupManufacturingStageSet()
{
    // Setup Camera
    glm::dvec3 focusPos = glm::dvec3(0, 0, 0);
    glm::dvec3 camPos = glm::dvec3(4, -8, 3);
    glm::dvec3 dir = glm::normalize(focusPos - camPos);
    double tilt = glm::atan(dir.z, glm::length(glm::dvec2(dir.x, dir.y)));
    double pan = glm::atan(dir.y, dir.x);

    std::shared_ptr<Camera> camera = play().view()->camera3D();
    _camMan.reset(new CameraManFree(camera, false));
    _camMan->setOrientation(pan, tilt);
    _camMan->setPosition(camPos);


    std::shared_ptr<StageSet> stageSet = play().propTeam3D()->stageSet();

    _backdrop = new ProceduralSun();
    stageSet->setBackdrop(std::shared_ptr<Backdrop>(_backdrop));


    // Holes
    pSurf outer1 = Box::boxPosDims(glm::dvec3(0, 1.5, 0.5), glm::dvec3(1, 0.3, 0.99));
    pSurf inner1 = Box::boxPosDims(glm::dvec3(0, 1.5, 0.5), glm::dvec3(0.5, 0.5, 0.5));
    pSurf bloc1 = outer1;// & !inner1;
    bloc1->setInnerMaterial(material::GOLD);
    bloc1->setCoating(coating::createClearCoat(0.10));

    pSurf outer2 = Box::boxPosDims(glm::dvec3(0, 0.0, 0.5), glm::dvec3(1, 0.3, 0.99));
    pSurf inner2 = Box::boxPosDims(glm::dvec3(0, 0.0, 0.5), glm::dvec3(0.5, 0.5, 0.5));
    pSurf bloc2 = outer2;// & !inner2;
    bloc2->setInnerMaterial(material::GLASS);
    bloc2->setCoating(coating::createClearCoat(0.01));

    pSurf outer3 = Box::boxPosDims(glm::dvec3(0,-1.5, 0.5), glm::dvec3(1, 0.3, 0.99));
    pSurf inner3 = Box::boxPosDims(glm::dvec3(0,-1.5, 0.5), glm::dvec3(0.5, 0.5, 0.5));
    pSurf bloc3 = outer3;// & !inner3;
    bloc3->setInnerMaterial(material::createInsulator(glm::dvec3(0.3, 0.3, 0.3), 1.3, 0.8, 1.0));
    bloc3->setCoating(coating::createClearCoat(1.0));

    pProp blocsProp(new Prop("Blocs"));
    blocsProp->addSurface(bloc1);
    blocsProp->addSurface(bloc2);
    blocsProp->addSurface(bloc3);
    //stageSet->addProp(blocsProp);

    // Light
    pLight lampLight(new SphericalLight("Lamp Light",
        glm::dvec3(-1.25, 0.75, 0.4), 0.2));
    lampLight->setRadiantFlux(glm::dvec3(16.0));
    //stageSet->addLight(lampLight);

    // Box
    pSurf boxSurf = Box::boxPosDims(glm::dvec3(0, 0, -1.0), glm::dvec3(4.0, 4.0, 2.0));
    pCoat eggStandCoat = coating::createClearPaint(glm::dvec3(0.6), 0.0);
    boxSurf->setCoating(eggStandCoat);
    pProp boxProp(new Prop("Box"));
    boxProp->addSurface(boxSurf);
    stageSet->addProp(boxProp);

    pSurf fruitSurf = Sphere::sphere(glm::dvec3(), 1.0);
    Surface::transform(fruitSurf, glm::scale(glm::dmat4(), glm::dvec3(0.3, 0.5, 0.4)));
    Surface::transform(fruitSurf, glm::rotate(glm::dmat4(), glm::pi<double>()/2.0, glm::dvec3(1.0, 0, 0)));
    Surface::transform(fruitSurf, glm::translate(glm::dmat4(), glm::dvec3(0.0, 0, 1.0)));
    pProp fruitProp(new Prop("Fruit"));
    fruitProp->addSurface(fruitSurf);
    stageSet->addProp(fruitProp);
}

void ExAnimatorCharacter::setupCornBoardStageSet()
{
    // Setup Camera
    glm::dvec3 focusPos = glm::dvec3(0, 0, 0);
    glm::dvec3 camPos = glm::dvec3(10, -25, 8);
    glm::dvec3 dir = glm::normalize(focusPos - camPos);
    double tilt = glm::atan(dir.z, glm::length(glm::dvec2(dir.x, dir.y)));
    double pan = glm::atan(dir.y, dir.x);

    std::shared_ptr<Camera> camera = play().view()->camera3D();
    _camMan.reset(new CameraManFree(camera, false));
    _camMan->setOrientation(pan, tilt);
    _camMan->setPosition(camPos);


    std::shared_ptr<StageSet> stageSet = play().propTeam3D()->stageSet();

    _backdrop = new ProceduralSun;
    stageSet->setBackdrop(std::shared_ptr<Backdrop>(_backdrop));


    // Board
    glm::dvec3 boardMin = glm::dvec3(-10, -10, -0.1);
    glm::dvec3 boardMax = glm::dvec3( 10,  10,  0.0);
    pSurf boardSurf = Box::boxCorners(boardMin, boardMax) & !(
                          Plane::plane(glm::dvec3(1, 0, 0), glm::dvec3(0.0)) &
                          Plane::plane(glm::dvec3(0, 1, 0), glm::dvec3(0.0)) |
                          Plane::plane(glm::dvec3(1, 1, 0), boardMin*0.32));
    boardSurf->setInnerMaterial(material::createInsulator(glm::dvec3(0.8, 0.75, 0.3), 1.35, 0.95, 0.8));
    boardSurf->setCoating(coating::createClearCoat(0.7));
    pProp boardProp(new Prop("Board"));
    boardProp->addSurface(boardSurf);
    stageSet->addProp(boardProp);

    // Twin Towers
    pMat twinTowerMat = material::createMetal(glm::dvec3(0.9, 0.3, 0.3));
    pCoat twinTowerCoat = coating::CLEAR_POLISH;

    glm::dvec3 tallTowerPos = glm::dvec3(boardMax.x/2.0, boardMin.y/1.5, boardMax.z + 1.251);
    glm::dvec3 tallTowerDim = glm::dvec3(0.5,  0.5,  2.5);
    pSurf tallTowerSurf = Box::boxPosDims(tallTowerPos, tallTowerDim);
    tallTowerSurf->setInnerMaterial(twinTowerMat);
    tallTowerSurf->setCoating(twinTowerCoat);

    glm::dvec3 shortTowerPos = glm::dvec3(boardMax.x/1.6, boardMin.y/1.35, boardMax.z + 1.001);
    glm::dvec3 shortTowerDim = glm::dvec3(0.5,  0.5,  2.0);
    pSurf shortTowerSurf = Box::boxPosDims(shortTowerPos, shortTowerDim);
    shortTowerSurf->setInnerMaterial(twinTowerMat);
    shortTowerSurf->setCoating(twinTowerCoat);

    // Capacitor
    glm::dvec3 capaTowerPos = glm::dvec3(boardMax.x/3.0, boardMin.y/1.30, boardMax.z + 0.001);
    pSurf capaTowerSurf = Quadric::cylinder(0.4, 0.4) &
                          Plane::plane(glm::dvec3(0, 0,-1), glm::dvec3(0, 0, 0)) &
                          Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, 1.35));
    capaTowerSurf = Surface::translate(capaTowerSurf, capaTowerPos);
    capaTowerSurf->setInnerMaterial(material::createInsulator(glm::dvec3(0.2), 1.40, 1.0, 1.0));
    capaTowerSurf->setCoating(coating::createClearCoat(0.4));

    pProp towersProp(new Prop("Towers"));
    stageSet->addProp(towersProp);
    towersProp->addSurface(tallTowerSurf);
    towersProp->addSurface(shortTowerSurf);
    towersProp->addSurface(capaTowerSurf);
}
