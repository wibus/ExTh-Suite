#include "TheFruitChoreographer.h"

#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/random.hpp>

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
#include <PropRoom3D/Team/ArtDirector/ArtDirectorServer.h>

#include "Model/PathModel.h"
#include "Model/SceneDocument.h"

using namespace cellar;
using namespace prop3;


typedef std::shared_ptr<StageZone> pZone;
typedef std::shared_ptr<LightBulb> pLight;
typedef std::shared_ptr<Surface> pSurf;
typedef std::shared_ptr<Material> pMat;
typedef std::shared_ptr<Coating> pCoat;
typedef std::shared_ptr<Prop> pProp;


TheFruitChoreographer::TheFruitChoreographer(
        const std::shared_ptr<prop3::ArtDirectorServer>& raytracer) :
    _camAperture(0.8),
    _pathModel(new PathModel()),
    _raytracer(raytracer),
    _raytracerState(_raytracer->raytracerState()),
    _camMan(new CameraManFree(raytracer->camera(), false)),
    _camera(raytracer->camera()),
    _animFps(24),
    _animFrameCount(-1),
    _animCurrFrame(0),
    _animTime(0.0),
    _animPlaying(false),
    _animFastPlay(false),
    _cameraIsFree(false)
{
}

TheFruitChoreographer::~TheFruitChoreographer()
{
}

void TheFruitChoreographer::setup(const std::shared_ptr<StageSet>& stageSet)
{
    getSceneDocument().setSceneName("The Fruit");
    getSceneDocument().setStageSetName("StageSet");
    std::string textureRoot = getSceneDocument().getTextureRootDirectory();

    // Environment
    _backdrop.reset( new ProceduralSun() );
    stageSet->setBackdrop(std::shared_ptr<Backdrop>(_backdrop));

    glm::dvec3 focusPos = glm::dvec3(0, 0, 1.0);
    glm::dvec3 camPos = focusPos + glm::dvec3(8, -15, 5) * 1.3;
    glm::dvec3 dir = glm::normalize(focusPos - camPos);
    double tilt = glm::atan(dir.z, glm::length(glm::dvec2(dir.x, dir.y)));
    double pan = glm::atan(dir.y, dir.x);

    _camMan->setOrientation(pan, tilt);
    _camMan->setPosition(camPos);


    ///////////
    // Socle //
    ///////////
    glm::dvec3 socelMin(-10.0, -10.0, -0.2);
    glm::dvec3 socelMax( 10.0,  10.0, 0.0);
    glm::dvec3 socleDia = socelMax - socelMin;

    pSurf soceSurf = BoxTexture::boxCorners(
            socelMin, socelMax, // Corners
            socelMin,         // Tex Origin
            glm::dvec3(socleDia.x/7.0, 0.0, 0.0),
            glm::dvec3(0.0, socleDia.y/7.0, 0.0),
            true);

    auto socleCoat = new TexturedStdCoating();
    socleCoat->setRoughnessTexName(textureRoot + "Bathroom_Tiles_gloss.png");
    socleCoat->setDefaultRoughness(0.0);
    socleCoat->setPaintColorTexName(textureRoot + "Bathroom_Tiles_albedo.png");
    socleCoat->setDefaultPaintColor(glm::dvec4(color::white, 1.0));
    socleCoat->setPaintRefractiveIndex(1.55);
    socleCoat->setTexFilter(ESamplerFilter::LINEAR);
    socleCoat->setTexWrapper(ESamplerWrapper::REPEAT);

    pProp socleProp(new Prop("Socle"));
    soceSurf->setCoating(pCoat(socleCoat));
    socleProp->addSurface(soceSurf);
    stageSet->addProp(socleProp);


    ///////////
    // Stage //
    ///////////
    glm::dvec3 boxMin(-5, -5, 0.0);
    glm::dvec3 boxMax( 5,  5, 5.0);
    glm::dvec3 boxDia = boxMax - boxMin;
    glm::dvec3 boxCenter = boxMin + boxDia / 2.0;
    glm::dvec3 wallThickness = glm::dvec3(0.1);
    glm::dvec3 doorDim(0.85, 0.85, 2.0 * 2);

    glm::dvec3 hallMin(boxCenter.x + wallThickness.x, boxCenter.y + wallThickness.y, -1.0);
    glm::dvec3 hallMax(boxMax.x - wallThickness.x, boxMax.y - wallThickness.y, boxCenter.z - wallThickness.z);
    glm::dvec3 boudMin(boxCenter.x + wallThickness.x, boxMin.y - wallThickness.y, -1.0);
    glm::dvec3 boudMax(boxMax.x + wallThickness.x, boxCenter.y - wallThickness.y, boxMax.z - wallThickness.z);
    glm::dvec3 roomMin(boxMin.x + wallThickness.x, boxMin.y + wallThickness.y, -1.0);
    glm::dvec3 roomMax(boxCenter.x - wallThickness.x, boxMax.y - wallThickness.y, boxMax.z - wallThickness.z);

    pSurf box = Box::boxCorners( glm::dvec3(boxMin.x, boxMin.y, socelMin.z), boxMax);

    pSurf hall = !Box::boxCorners(hallMin, hallMax);
    pSurf room = !Box::boxCorners(roomMin, roomMax);
    pSurf boud = !Box::boxCorners(boudMin, boudMax);

    pSurf hallEntrance = !Box::boxPosDims(glm::dvec3(boxMax.x, boxMax.y/2.0, 0.0), doorDim);
    pSurf roomEntrance = !Box::boxPosDims(glm::dvec3(boxMin.y, boxMin.y/2.0, 0.0), doorDim);
    pSurf glassPassage = !Box::boxPosDims(glm::dvec3(boxMax.x/2.0, 0.0, 0.0), doorDim);
    pSurf roomPassage = !Box::boxPosDims(glm::dvec3(0.0, boxMax.y/2.0, 0.0), doorDim);

    pSurf crossWindowHole= !(
        Box::boxPosDims(
            glm::dvec3(0, boxMin.y/2.0, 2),
            glm::dvec3(wallThickness.x * 3.0, 1.0, 3.0)) |
        Box::boxPosDims(
            glm::dvec3(0, boxMin.y/2.0, 3.0),
            glm::dvec3(wallThickness.x * 3.0, 3.0, 1.0)));

    pSurf longWindowHole = !Box::boxPosDims(
            glm::dvec3(boxMin.y, 0, boxDia.z * 0.75),
            glm::dvec3(1, boxDia.y * 0.9, boxDia.z * 0.40));

    pSurf smallWindowHole1 = !Box::boxPosDims(
            glm::dvec3(boxMax.x - boxDia.x/8.0, boxMax.y, boxDia.z*0.3),
            glm::dvec3(boxDia.z/6.0, 1, boxDia.z/5.0));
    pSurf smallWindowHole2 = !Box::boxPosDims(
            glm::dvec3(boxMax.x - boxDia.x*3.0/8.0, boxMax.y, boxDia.z*0.3),
            glm::dvec3(boxDia.z/6.0, 1, boxDia.z/5.0));
    pSurf smallWindowHole3 = !Box::boxPosDims(
            glm::dvec3(boxMax.x - boxDia.x*5.0/8.0, boxMax.y, boxDia.z*0.3),
            glm::dvec3(boxDia.z/6.0, 1, boxDia.z/5.0));
    pSurf smallWindowHole4 = !Box::boxPosDims(
            glm::dvec3(boxMax.x - boxDia.x*7.0/8.0, boxMax.y, boxDia.z*0.3),
            glm::dvec3(boxDia.z/6.0, 1, boxDia.z/5.0));
    pSurf smallWindowHole5 = !Box::boxPosDims(
            glm::dvec3(boxMax.x - boxDia.x*5.0/8.0, boxMin.y, boxDia.z*0.3),
            glm::dvec3(boxDia.z/6.0, 1, boxDia.z/5.0));
    pSurf smallWindowHole6 = !Box::boxPosDims(
            glm::dvec3(boxMax.x - boxDia.x*7.0/8.0, boxMin.y, boxDia.z*0.3),
            glm::dvec3(boxDia.z/6.0, 1, boxDia.z/5.0));

    pSurf stageSurf = box & hall & room & boud &
        hallEntrance & roomEntrance & glassPassage & roomPassage &
        smallWindowHole1 & smallWindowHole2 & smallWindowHole3 &
        smallWindowHole4 & smallWindowHole5 & smallWindowHole6 &
        crossWindowHole & longWindowHole;

    pSurf stripWallSurf = createHoleStrippedWall(
        glm::dvec3(boxDia.x / 2.0, wallThickness.y, boxDia.z),
        0.35, 0.35, 0.35);

    pSurf ynegStripWall = Surface::shell(stripWallSurf);
    Surface::translate(ynegStripWall, glm::dvec3(boxMax.x/2.0, boxMin.y+wallThickness.y/2.0, 0));
    pSurf xposStripWall = Surface::shell(stripWallSurf);
    Surface::rotate(xposStripWall, -glm::pi<double>()/2.0, glm::dvec3(0, 0, 1));
    Surface::translate(xposStripWall, glm::dvec3(boxMax.x-wallThickness.x/2.0, boxMin.y/2.0, 0));

    pCoat stageCoat = coating::createClearCoat(1.0);
    stageSurf->setCoating(stageCoat);
    xposStripWall->setCoating(stageCoat);
    ynegStripWall->setCoating(stageCoat);
    pMat stageMat = material::createInsulator(glm::dvec3(0.7), 1.45, 1.0, 1.0);
    stageSurf->setInnerMaterial(stageMat);
    xposStripWall->setInnerMaterial(stageMat);
    ynegStripWall->setInnerMaterial(stageMat);


    pZone stageZone(new StageZone("Stage Zone"));
    stageSet->addSubzone(stageZone);

    pProp stageProp(new Prop("Stage"));
    stageProp->addSurface(stageSurf);
    stageZone->addProp(stageProp);

    pProp xStrippedProp(new Prop("X Stripped Wall"));
    xStrippedProp->addSurface(xposStripWall);
    pZone xStrippedZone(new StageZone("X Stripped Zone"));
    stageZone->addSubzone(xStrippedZone);
    xStrippedZone->setBounds( Box::boxCorners(
        glm::dvec3(boxMax.x - wallThickness.x, boxMin.y, boxMin.z),
        glm::dvec3(boxMax.x, 0.0, boxMax.z)));
    xStrippedZone->addProp(xStrippedProp);

    pProp yStrippedProp(new Prop("Y Stripped Wall"));
    yStrippedProp->addSurface(ynegStripWall);
    pZone yStrippedZone(new StageZone("Y Stripped Zone"));
    stageZone->addSubzone(yStrippedZone);
    yStrippedZone->setBounds( Box::boxCorners(
        glm::dvec3(0.0, boxMin.y, boxMin.z),
        glm::dvec3(boxMax.x, boxMin.y + wallThickness.y, boxMax.z)));
    yStrippedZone->addProp(yStrippedProp);


    //////////
    // Roof //
    //////////
    double roofSide = 0.2;
    glm::dvec3 roofMin(boxMin.x * 1.80, boxMin.y * 1.80, boxMax.z);
    glm::dvec3 roofMax(boxMax.x * 1.30, boxMax.y * 1.30, boxMax.z + roofSide);
    pSurf roofSurf = Box::boxCorners(roofMin, roofMax);

    double roofPillarSide = wallThickness.x * 2.0;
    double pillarBootSide = roofPillarSide * 1.5;
    glm::dvec2 pillarXnYn(roofMin.x+pillarBootSide, roofMin.y+pillarBootSide);
    glm::dvec2 pillarXpYn(roofMax.x-pillarBootSide, roofMin.y+pillarBootSide);
    glm::dvec2 pillarXnYp(roofMin.x+pillarBootSide, roofMax.y-pillarBootSide);
    glm::dvec2 pillarXpYp(roofMax.x-pillarBootSide, roofMax.y-pillarBootSide);
    pSurf roofPillardXnYn = Box::boxPosDims(
        glm::dvec3(pillarXnYn, boxMax.z/2.0), glm::dvec3(roofSide, roofSide, boxDia.z));
    pSurf roofPillardXpYn = Box::boxPosDims(
        glm::dvec3(pillarXpYn, boxMax.z/2.0), glm::dvec3(roofSide, roofSide, boxDia.z));
    pSurf roofPillardXnYp = Box::boxPosDims(
        glm::dvec3(pillarXnYp, boxMax.z/2.0), glm::dvec3(roofSide, roofSide, boxDia.z));
    pSurf roofPillardXpYp = Box::boxPosDims(
        glm::dvec3(pillarXpYp, boxMax.z/2.0), glm::dvec3(roofSide, roofSide, boxDia.z));

    pSurf pillarBootXnYn = Box::boxPosDims(
        glm::dvec3(pillarXnYn, pillarBootSide/2.0), glm::dvec3(pillarBootSide));
    pSurf pillarBootXpYn = Box::boxPosDims(
        glm::dvec3(pillarXpYn, pillarBootSide/2.0), glm::dvec3(pillarBootSide));
    pSurf pillarBootXnYp = Box::boxPosDims(
        glm::dvec3(pillarXnYp, pillarBootSide/2.0), glm::dvec3(pillarBootSide));
    pSurf pillarBootXpYp = Box::boxPosDims(
        glm::dvec3(pillarXpYp, pillarBootSide/2.0), glm::dvec3(pillarBootSide));

    pProp roofProp(new Prop("Roof"));
    roofProp->addSurface(roofSurf);
    roofProp->addSurface(roofPillardXnYn);
    roofProp->addSurface(roofPillardXpYn);
    roofProp->addSurface(roofPillardXnYp);
    roofProp->addSurface(roofPillardXpYp);
    roofProp->addSurface(pillarBootXnYn);
    roofProp->addSurface(pillarBootXpYn);
    roofProp->addSurface(pillarBootXnYp);
    roofProp->addSurface(pillarBootXpYp);
    stageSet->addProp(roofProp);

    pCoat roofCoat = coating::createClearCoat(0.2);
    roofProp->setInnerMaterial(material::TITANIUM);
    roofProp->setCoating(roofCoat);



    ////////////////////////
    // Ceiling decoration //
    ////////////////////////
    /*
    double ceilThickness = 0.2;
    glm::dvec3 ceilCenter(boxMax.x/2.0, boxMin.y/2.0, boxMax.z - wallThickness.z - ceilThickness/2.0);
    glm::dvec3 ceilDims((boxMax.x - wallThickness.x)*0.8, (boxMax.y - wallThickness.y)*0.8, ceilThickness);
    pSurf ceilBase = Box::boxPosDims(ceilCenter, ceilDims);
    pSurf ceilHole1 = Box::boxPosDims(ceilCenter, glm::dvec3(ceilDims.x*7/9.0, ceilDims.y*7/9.0, ceilThickness*2));
    pSurf ceilInner = Box::boxPosDims(ceilCenter, glm::dvec3(ceilDims.x*5/9.0, ceilDims.y*5/9.0, ceilThickness*2));
    pSurf ceilHole2 = Box::boxPosDims(ceilCenter, glm::dvec3(ceilDims.x*3/9.0, ceilDims.y*3/9.0, ceilThickness*2));
    pSurf ceilCore = Box::boxPosDims(ceilCenter, glm::dvec3(ceilDims.x*1/9.0, ceilDims.y*1/9.0, ceilThickness*2));
    pSurf ceilSurf = ceilBase & !(ceilHole1 & !(ceilInner & !(ceilHole2 & !(ceilCore))));

    auto ceilCoat = coating::createClearCoat(0.1);
    ceilSurf->setInnerMaterial(material::SILVER);
    ceilSurf->setCoating(ceilCoat);

    pProp ceilProp(new Prop("Ceiling"));
    ceilProp->addSurface(ceilSurf);

    pZone ceilZone(new StageZone("Ceiling Zone"));
    stageZone->addSubzone(ceilZone);
    ceilZone->setBounds( ceilBase );
    ceilZone->addProp(ceilProp);
    */


    /////////////
    // Posters //
    /////////////
    double posterLength = boxDia.z / 2 - wallThickness.z * 2.0;
    double posterEpsilon = 0.002;

    pSurf herbieSextantSurf = BoxTexture::boxPosDims(glm::dvec3(),
        glm::dvec3(posterLength, posterEpsilon, posterLength),
        glm::dvec3(posterLength/2.0, 0, -posterLength * 3.0/2.0),
        glm::dvec3(-posterLength*2.0, 0, 0),
        glm::dvec3(0, 0, posterLength*2.0),
        true);
    Surface::translate(herbieSextantSurf, glm::dvec3(
        boxMax.x / 2.0, boxMax.y + posterEpsilon * 10.0, boxMax.z * 0.72));

    pSurf herbieCrossingsSurf = BoxTexture::boxPosDims(glm::dvec3(),
        glm::dvec3(posterLength, posterEpsilon, posterLength),
        glm::dvec3(posterLength * 3.0/2.0, 0, -posterLength * 3.0/2.0),
        glm::dvec3(-posterLength*2.0, 0, 0),
        glm::dvec3(0, 0, posterLength*2.0),
        true);
    Surface::translate(herbieCrossingsSurf, glm::dvec3(
        boxMin.x / 2.0,  boxMax.y + posterEpsilon * 10.0,  boxMax.z * 0.72));

    pSurf bitchesBrewSurf = BoxTexture::boxPosDims(glm::dvec3(),
        glm::dvec3(posterLength*2.0, posterEpsilon, posterLength),
        glm::dvec3(-posterLength, 0, -posterLength / 2.0),
        glm::dvec3(posterLength*2.0, 0, 0),
        glm::dvec3(0, 0, posterLength*2.0),
        true);
    Surface::translate(bitchesBrewSurf, glm::dvec3(
        boxMin.x / 2.0,  boxMin.y - posterEpsilon * 10.0,  boxMax.z * 0.72));

    pSurf dessinCocoSurf = BoxTexture::boxPosDims(glm::dvec3(),
        glm::dvec3(posterLength*7.0/4, posterEpsilon, posterLength*3.0/4),
        glm::dvec3(posterLength/2.0, 0, -posterLength*3/8.0),
        glm::dvec3(-posterLength*3/4.0, 0, 0),
        glm::dvec3(0, 0, posterLength*3/4.0),
        true);
    Surface::translate(dessinCocoSurf, glm::dvec3(
        boxMin.x / 2.0,  boxMin.y + wallThickness.y + posterEpsilon * 10.0,  boxMax.z * 0.72));

    auto pPosterCoat = new TexturedStdCoating();
    pPosterCoat->setPaintColorTexName(textureRoot + "Fusion_Albums.png");
    pPosterCoat->setDefaultPaintColor(glm::dvec4(0.2, 0.2, 0.2, 1.0));
    pPosterCoat->setTexFilter(ESamplerFilter::LINEAR);
    pPosterCoat->setTexWrapper(ESamplerWrapper::CLAMP);
    pPosterCoat->setDefaultRoughness(1.0);
    pCoat posterCoat = pCoat(pPosterCoat);

    auto pDessinCocoCoat = new TexturedStdCoating();
    pDessinCocoCoat->setPaintColorTexName(textureRoot + "Dessin_Coco.png");
    pDessinCocoCoat->setDefaultPaintColor(glm::dvec4(0.2, 0.2, 0.2, 1.0));
    pDessinCocoCoat->setTexFilter(ESamplerFilter::LINEAR);
    pDessinCocoCoat->setTexWrapper(ESamplerWrapper::CLAMP);
    pDessinCocoCoat->setDefaultRoughness(1.0);
    pCoat dessinCocoCoat = pCoat(pDessinCocoCoat);

    pProp postersProp(new Prop("Posters"));
    postersProp->addSurface(herbieSextantSurf);
    herbieSextantSurf->setCoating(posterCoat);
    postersProp->addSurface(herbieCrossingsSurf);
    herbieCrossingsSurf->setCoating(posterCoat);
    postersProp->addSurface(bitchesBrewSurf);
    bitchesBrewSurf->setCoating(posterCoat);
    postersProp->addSurface(dessinCocoSurf);
    dessinCocoSurf->setCoating(dessinCocoCoat);

    stageSet->addProp(postersProp);


    ///////////
    // Fence //
    ///////////
    double fenceSide = 0.20;
    double fenceHeight = 1.0;
    double fenceCapSide = fenceSide * 1.33;
    double fenceCapRadius = fenceCapSide * 1.05;
    double fenceWallWidth = fenceSide * 0.6;
    double fenceWallHeight = fenceHeight * 0.8;
    glm::dvec3 sceneDim((socleDia.x-fenceCapSide) / 2.0,
                        (socleDia.y-fenceCapSide) / 2.0,
                        0.0);

    glm::dmat4 capXRot = glm::rotate(glm::dmat4(), glm::pi<double>() / 2.0, glm::dvec3(1, 0, 0));
    glm::dmat4 capYRot = glm::rotate(glm::dmat4(), glm::pi<double>() / 2.0, glm::dvec3(0, 1, 0));
    pSurf capXneg = Quadric::cylinder(fenceCapRadius, fenceCapRadius);
    Surface::transform(capXneg, glm::translate(capXRot, glm::dvec3(-fenceCapSide, 0, 0)));
    pSurf capXpos = Quadric::cylinder(fenceCapRadius, fenceCapRadius);
    Surface::transform(capXpos, glm::translate(capXRot, glm::dvec3( fenceCapSide, 0, 0)));
    pSurf capYneg = Quadric::cylinder(fenceCapRadius, fenceCapRadius);
    Surface::transform(capYneg, glm::translate(capYRot, glm::dvec3(0, -fenceCapSide, 0)));
    pSurf capYpos = Quadric::cylinder(fenceCapRadius, fenceCapRadius);
    Surface::transform(capYpos, glm::translate(capYRot, glm::dvec3(0,  fenceCapSide, 0)));
    pSurf capBox = Box::boxPosDims(glm::dvec3(0, 0, -fenceCapSide/2.0), glm::dvec3(fenceCapSide));
    pSurf capSphere = Sphere::sphere(glm::dvec3(0, 0, -fenceSide/2.0), fenceSide / 2.0);
    pSurf capSurf = capSphere | (capBox & !(capXneg | capXpos | capYneg | capYpos));
    Surface::translate(capSurf, glm::dvec3(0, 0.0, fenceHeight + fenceCapSide));

    pSurf fencePostBeam = Box::boxPosDims(glm::dvec3(0, 0, fenceHeight/2.0), glm::dvec3(fenceSide, fenceSide, fenceHeight));
    pSurf fencePost = fencePostBeam | capSurf;
    pCoat fenceCoat = stageCoat;
    fencePost->setCoating(fenceCoat);

    std::vector<glm::dvec3> postPos = {
        glm::dvec3(-1.0,     -1.0,     0.0),
        glm::dvec3(-1.0/3.0, -1.0,     0.0),
        glm::dvec3( 1.0/3.0, -1.0,     0.0),
        glm::dvec3( 1.0,     -1.0,     0.0),
        glm::dvec3( 1.0,     -1.0/3.0, 0.0),
        glm::dvec3( 1.0,      1.0/3.0, 0.0),
        glm::dvec3( 1.0,      1.0,     0.0),
        glm::dvec3( 1.0/3.0,  1.0,     0.0),
        glm::dvec3(-1.0/3.0,  1.0,     0.0),
        glm::dvec3(-1.0,      1.0,     0.0),
        glm::dvec3(-1.0,      1.0/3.0, 0.0),
        glm::dvec3(-1.0,     -1.0/3.0, 0.0)
    };

    pZone fenceZone(new StageZone("Fence Zone"));
    stageSet->addSubzone(fenceZone);
    for(const auto& pos : postPos)
    {
        glm::dvec3 postOffset = sceneDim * pos;

        pSurf postShell = Surface::shell(fencePost);
        Surface::translate(postShell, postOffset);

        pZone postZone(new StageZone("Post Zone"));
        fenceZone->addSubzone(postZone);
        postZone->setBounds(Box::boxPosDims(
            postOffset + glm::dvec3(0, 0, (fenceHeight + fenceCapSide)/2.0),
            glm::dvec3(fenceCapSide, fenceCapSide, fenceHeight + fenceCapSide)));

        pProp postProp(new Prop("Fence Post"));
        postProp->addSurface(postShell);
        postZone->addProp(postProp);
    }

    pProp fenceWallsProp(new Prop("Fence Walls"));

    pSurf fenceWallSurfXNeg = Box::boxPosDims(
        glm::dvec3(-sceneDim.x, 0, fenceWallHeight/2.0),
        glm::dvec3(fenceWallWidth, 2*sceneDim.y, fenceWallHeight));
    fenceWallSurfXNeg->setCoating(fenceCoat);
    fenceWallsProp->addSurface(fenceWallSurfXNeg);

    pSurf fenceWallSurfXPos = Box::boxPosDims(
        glm::dvec3(sceneDim.x, 0, fenceWallHeight/2.0),
        glm::dvec3(fenceWallWidth, 2*sceneDim.y, fenceWallHeight));
    fenceWallSurfXPos->setCoating(fenceCoat);
    fenceWallsProp->addSurface(fenceWallSurfXPos);

    pSurf fenceWallSurfYNeg = Box::boxPosDims(
        glm::dvec3(0, -sceneDim.y, fenceWallHeight/2.0),
        glm::dvec3(2*sceneDim.x, fenceWallWidth, fenceWallHeight));
    fenceWallSurfYNeg->setCoating(fenceCoat);
    fenceWallsProp->addSurface(fenceWallSurfYNeg);

    pSurf fenceWallSurfYPos = Box::boxPosDims(
        glm::dvec3(0, sceneDim.y, fenceWallHeight/2.0),
        glm::dvec3(2*sceneDim.x, fenceWallWidth, fenceWallHeight));
    fenceWallSurfYPos->setCoating(fenceCoat);
    fenceWallsProp->addSurface(fenceWallSurfYPos);

    fenceZone->addProp(fenceWallsProp);


    /////////////////////
    // Egg (ellipsoid) //
    /////////////////////
    double eggRadius = 0.3;
    double eggStandSide = eggRadius * 4;
    double eggStandHeight = eggStandSide * 0.15;
    glm::dvec3 eggPos(sceneDim.x / 4.0, -sceneDim.y / 4.0, 0.0);

    pSurf eggTop = Quadric::ellipsoid(eggRadius, eggRadius, 2*eggRadius);
    Surface::translate(eggTop, glm::dvec3(0, 0, 0));
    pSurf eggBottom = Sphere::sphere(glm::dvec3(), eggRadius);
    pSurf topCap = Plane::plane(glm::dvec3(0, 0, -1), glm::dvec3(0, 0, 0));
    pSurf bottomCap = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, 0));
    pSurf eggSurf = (eggTop & ~topCap) | (eggBottom & ~bottomCap);
    Surface::translate(eggSurf, glm::dvec3(eggPos.x, eggPos.y, eggRadius + eggStandHeight));

    pSurf eggStandSurf = Box::boxPosDims(
        glm::dvec3(eggPos.x, eggPos.y, eggStandHeight/2.0),
        glm::dvec3(eggStandSide, eggStandSide, eggStandHeight));

    pZone eggZone(new StageZone("Egg Zone"));
    stageZone->addSubzone(eggZone);
    eggZone->setBounds(Box::boxCorners(
        eggPos - glm::dvec3(eggStandSide/2, eggStandSide/2, 0.0),
        eggPos + glm::dvec3(eggStandSide/2, eggStandSide/2, eggStandHeight + 3 * eggRadius)));

    eggSurf->setInnerMaterial(material::GOLD);

    pCoat eggStandCoat = coating::createClearPaint(glm::dvec3(0.08, 0.08, 0.08), 0.0);
    eggStandSurf->setCoating(eggStandCoat);

    pProp eggProp(new Prop("Egg"));
    eggProp->addSurface(eggSurf);
    eggProp->addSurface(eggStandSurf);
    eggZone->addProp(eggProp);



    pZone workZone(new StageZone("Work Zone"));
    stageZone->addSubzone(workZone);

    ////////////////
    // Work table //
    ////////////////
    double workTableTopThick = 0.08;
    double workTableLegThick = 0.10;
    glm::dvec3 workTableDims(1.0, 2.0, 0.80);
    glm::dvec2 workTablePos(boxMin.x + workTableDims.x/2.0 + wallThickness.x + 0.05,
                        boxMax.y*0.85 - workTableDims.y/2.0 - wallThickness.y - 0.05);
    glm::dvec2 workTableLegOff((workTableDims.x-workTableLegThick)/2.0,
                           (workTableDims.y-workTableLegThick)/2.0);

    pSurf workTableTop = Box::boxPosDims(
        glm::dvec3(workTablePos, workTableDims.z - workTableTopThick/2.0),
        glm::dvec3(workTableDims.x, workTableDims.y, workTableTopThick));
    pSurf workTableLeg1 = Box::boxPosDims(
        glm::dvec3(workTablePos.x-workTableLegOff.x, workTablePos.y-workTableLegOff.y, (workTableDims.z - workTableTopThick)/2.0),
        glm::dvec3(0.1, 0.1, workTableDims.z - workTableTopThick));
    pSurf workTableLeg2 = Box::boxPosDims(
        glm::dvec3(workTablePos.x+workTableLegOff.x, workTablePos.y-workTableLegOff.y, (workTableDims.z - workTableTopThick)/2.0),
        glm::dvec3(0.1, 0.1, workTableDims.z - workTableTopThick));
    pSurf workTableLeg3 = Box::boxPosDims(
        glm::dvec3(workTablePos.x+workTableLegOff.x, workTablePos.y+workTableLegOff.y, (workTableDims.z - workTableTopThick)/2.0),
        glm::dvec3(0.1, 0.1, workTableDims.z - workTableTopThick));
    pSurf workTableLeg4 = Box::boxPosDims(
        glm::dvec3(workTablePos.x-workTableLegOff.x, workTablePos.y+workTableLegOff.y, (workTableDims.z - workTableTopThick)/2.0),
        glm::dvec3(0.1, 0.1, workTableDims.z - workTableTopThick));
    pSurf workTableSurf = workTableTop | workTableLeg1 | workTableLeg2 | workTableLeg3 | workTableLeg4;


    pProp workTableProp(new Prop("Work Table"));
    pMat workTableMat = material::createInsulator(glm::dvec3(0.4), 1.3, 1.0, 1.0);
    pCoat workTableCoat = coating::createClearCoat(1.0);
    workTableSurf->setInnerMaterial(workTableMat);
    workTableSurf->setCoating(workTableCoat);
    workTableProp->addSurface(workTableSurf);
    workZone->addProp(workTableProp);


    //////////
    // Bowl //
    //////////
    double bowlRadius = 0.18;
    pSurf bowlBase = Sphere::sphere(glm::dvec3(0, 0, bowlRadius*0.8), bowlRadius);
    pSurf bowlSurf = bowlBase &
        !(Sphere::sphere(glm::dvec3(0, 0, bowlRadius*0.8), bowlRadius*0.75) &
          Plane::plane(glm::dvec3(0,0,-1), glm::dvec3(0,0,bowlRadius*0.2)))&
        Plane::plane(glm::dvec3(0,0,1), glm::dvec3(0,0,bowlRadius*0.8)) &
        Plane::plane(glm::dvec3(0,0,-1), glm::dvec3(0,0,0.001));
    Surface::translate(bowlSurf,
        glm::dvec3(workTablePos.x, workTablePos.y - workTableDims.y / 4.0, workTableDims.z));

    pMat bowlMat = material::createInsulator(
        glm::dvec3(0.95, 0.75, 0.72),
        material::GLASS_REFRACTIVE_INDEX,
        0.975,
        0.0);

    bowlSurf->setInnerMaterial(bowlMat);

    pProp bowlProp(new Prop("Bowl"));
    bowlProp->addSurface(bowlSurf);

    pZone bowlZone(new StageZone("Bowl Zone"));
    workZone->addSubzone(bowlZone);
    bowlZone->setBounds(bowlBase);
    bowlZone->addProp(bowlProp);



    //////////
    // Lamp //
    //////////
    pSurf lampSurf;
    glm::dvec3 lampPos = glm::dvec3(workTablePos, 0.0) +
        glm::dvec3(0, workTableDims.y/4.0, workTableDims.z);

    // Head
    double headRot = glm::pi<double>() / 16;
    double headTwist = -glm::pi<double>() / 4;
    double elbowRot = glm::pi<double>() / 1.5;
    double shoulderRot = -glm::pi<double>() / 1.5;


    double headRad = 0.05;
    double headLen = 0.10;
    double headThick = 0.015;
    double neckRad = headRad * 0.5;
    double neckLen = headLen * 0.4;
    pSurf headOut = Quadric::paraboloid(1, 1);
    pSurf headIn = Quadric::paraboloid(1, 1);
    Surface::translate(headIn, glm::dvec3(0, 0, headThick));
    pSurf headCap = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, 1));
    pSurf headSurf = headCap & headOut & !headIn;
    Surface::transform(headSurf, glm::scale(glm::dmat4(), glm::dvec3(headRad, headRad, headLen)));
    pSurf neckCyl = Quadric::cylinder(neckRad, neckRad);
    pSurf neckTop = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, neckLen/2.0));
    pSurf neckBot = Plane::plane(glm::dvec3(0, 0,-1), glm::dvec3(0, 0,-neckLen/2.0));
    pSurf neckSurf = neckCyl & neckTop & neckBot;

    lampSurf = headSurf | neckSurf;
    Surface::rotate(lampSurf, glm::pi<double>(), glm::dvec3(0, 1, 0));
    Surface::rotate(lampSurf, headTwist, glm::dvec3(1, 0, 0));
    Surface::rotate(lampSurf, headRot, glm::dvec3(0, 1, 0));


    double forearmLen = 0.40;
    double forearmRad = 0.005;
    pSurf forearmCyl = Quadric::cylinder(forearmRad, forearmRad);
    pSurf forearmTop = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, 0));
    pSurf forearmBot = Plane::plane(glm::dvec3(0, 0,-1), glm::dvec3(0, 0, -forearmLen));
    pSurf foreArmSurf = forearmCyl & forearmTop & forearmBot;
    Surface::rotate(foreArmSurf, glm::pi<double>()/2.0, glm::dvec3(0, 1, 0));

    lampSurf = lampSurf | foreArmSurf;
    Surface::translate(lampSurf, glm::dvec3(forearmLen, 0, 0));


    double elbowRad = 0.02;
    double elbowWidth = 0.015;
    pSurf elbowCyl = Quadric::cylinder(elbowRad, elbowRad);
    pSurf elbowTop = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, elbowWidth/2.0));
    pSurf elbowBot = Plane::plane(glm::dvec3(0, 0,-1), glm::dvec3(0, 0, -elbowWidth/2.0));
    pSurf elbowSurf = elbowCyl & elbowTop & elbowBot;
    Surface::rotate(elbowSurf, glm::pi<double>()/2.0, glm::dvec3(1, 0, 0));

    lampSurf = lampSurf | elbowSurf;
    Surface::rotate(lampSurf, elbowRot, glm::dvec3(0, 1, 0));


    double armLen = forearmLen;
    double armRad = forearmRad;
    pSurf armCyl = Quadric::cylinder(armRad, armRad);
    pSurf armTop = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, 0));
    pSurf armBot = Plane::plane(glm::dvec3(0, 0,-1), glm::dvec3(0, 0, -armLen));
    pSurf armSurf = armCyl & armTop & armBot;
    Surface::rotate(armSurf, glm::pi<double>()/2.0, glm::dvec3(0, 1, 0));

    lampSurf = lampSurf | armSurf;
    Surface::translate(lampSurf, glm::dvec3(armLen, 0, 0));


    double shoulderRad = elbowRad * 0.8;
    double shoulderWidth = elbowWidth * 2.0;
    pSurf shoulderCyl = Quadric::cylinder(shoulderRad, shoulderRad);
    pSurf shoulderTop = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, shoulderWidth/2.0));
    pSurf shoulderBot = Plane::plane(glm::dvec3(0, 0,-1), glm::dvec3(0, 0, -shoulderWidth/2.0));
    pSurf shoulderSurf = shoulderCyl & shoulderTop & shoulderBot;
    Surface::rotate(shoulderSurf, glm::pi<double>()/2.0, glm::dvec3(1, 0, 0));

    lampSurf = lampSurf | shoulderSurf;
    Surface::rotate(lampSurf, shoulderRot, glm::dvec3(0, 1, 0));
    Surface::translate(lampSurf, glm::dvec3(0, 0, shoulderRad));


    double bodyRad = 0.10;
    double bodyHeight = 0.01;
    double shoulderOffset = -bodyRad * 0.65;
    double creaseHeight = bodyHeight * 0.35;
    double creaseOffset = bodyRad * 0.2;
    double creaseRad = bodyRad * 0.4;
    pSurf bodyCyl = Quadric::cylinder(bodyRad, bodyRad);
    pSurf bodyTop = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, 0));
    pSurf bodyBot = Plane::plane(glm::dvec3(0, 0,-1), glm::dvec3(0, 0, -bodyHeight));
    pSurf bodySurf = bodyCyl & bodyTop & bodyBot;
    pSurf creaseCone = Quadric::cone(creaseRad, creaseRad);
    pSurf creaseBottom = Plane::plane(glm::dvec3(0, 0, -1), glm::dvec3(0, 0, 1.0-creaseHeight));
    pSurf creaseSurf = creaseCone & creaseBottom;
    Surface::translate(creaseSurf, glm::dvec3(creaseOffset, 0, -1.0));
    bodySurf = bodySurf & !creaseSurf;

    Surface::translate(lampSurf, glm::dvec3(shoulderOffset, 0, 0));
    lampSurf = lampSurf | bodySurf;
    Surface::translate(lampSurf, glm::dvec3(0, 0, bodyHeight));

    pCoat lampCoat = coating::createClearCoat(0.25);
    lampSurf->setCoating(lampCoat);
    lampSurf->setInnerMaterial(material::TITANIUM);

    pProp lampProp(new Prop("Lamp"));
    lampProp->addSurface(lampSurf);

    double lampBoundsTop = armLen*1.05;
    pZone lampZone(new StageZone("Lamp Zone"));
    workZone->addSubzone(lampZone);
    lampZone->addProp(lampProp);
    lampZone->setBounds(Box::boxPosDims(
        glm::dvec3(-0.02, 0, armLen/2.0*1.05),
        glm::dvec3(armLen*1.35, bodyRad*2.25, lampBoundsTop)));
    lampZone->translate(lampPos);




    // Lamp bulb
    _lampRadiantFlux = color::tungsten40W * 40.0;
    double lamLightRad = headRad * 0.5;
    _lampLight.reset(new SphericalLight("Lamp Light",
        glm::dvec3(0, 0, headLen/3.0), lamLightRad));
    _lampLight->setRadiantFlux(_lampRadiantFlux);
    _lampLight->rotate(glm::pi<double>(), glm::dvec3(0, 1, 0));
    _lampLight->rotate(headTwist, glm::dvec3(1, 0, 0));
    _lampLight->rotate(headRot, glm::dvec3(0, 1, 0));
    _lampLight->translate(glm::dvec3(forearmLen, 0, 0));
    _lampLight->rotate(elbowRot, glm::dvec3(0, 1, 0));
    _lampLight->translate(glm::dvec3(armLen, 0, 0));
    _lampLight->rotate(shoulderRot, glm::dvec3(0, 1, 0));
    _lampLight->translate(glm::dvec3(0, 0, shoulderRad));
    _lampLight->translate(glm::dvec3(shoulderOffset, 0, 0));
    _lampLight->translate(glm::dvec3(0, 0, bodyHeight));
    _lampLight->translate(lampPos);
    lampZone->addLight(_lampLight);
    _lampLight->setIsOn(false);


    //Work zone bounds
    workZone->setBounds(Box::boxCorners(
        glm::dvec3(workTablePos.x - workTableDims.x/2, workTablePos.y - workTableDims.y/2, 0.0),
        glm::dvec3(workTablePos.x + workTableDims.x/2, workTablePos.y + workTableDims.y/2,
                   workTableDims.z + lampBoundsTop)));



    pZone sculptZone(new StageZone("Sculpture Zone"));
    stageZone->addSubzone(sculptZone);

    /////////////////////
    // Sculpture table //
    /////////////////////
    double scultpTableRadius = 0.30;
    double sculptTableHeight = 1.00;
    glm::dvec3 sculptTablePos(boxMin.x/2.0, 3*boxMin.y/4.0, 0);

    double sculptTableTopThick = 0.02;
    double sculptTableTopRadius = scultpTableRadius + sculptTableTopThick;

    pSurf sculptTableTop = Quadric::paraboloid(scultpTableRadius, scultpTableRadius);
    Surface::translate(sculptTableTop, glm::dvec3(0,0, -0.25));
    pSurf sculptTableBot = Quadric::paraboloid(scultpTableRadius, scultpTableRadius);
    Surface::rotate(sculptTableBot, glm::pi<double>(), glm::dvec3(1, 0, 0));
    Surface::translate(sculptTableBot, glm::dvec3(0,0, 0.25));
    pSurf sculptTopPlane = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, 0.75));
    pSurf sculptBotPlane = Plane::plane(glm::dvec3(0, 0,-1), glm::dvec3(0, 0,-0.75));
    pSurf sculptTableSurf = (sculptTableTop | sculptTableBot) & sculptBotPlane & sculptTopPlane;
    Surface::transform(sculptTableSurf, glm::scale(glm::dmat4(), glm::dvec3(1, 1, sculptTableHeight / 1.50)));
    Surface::translate(sculptTableSurf, glm::dvec3(0, 0, sculptTableHeight*0.5 + 0.001));
    Surface::translate(sculptTableSurf, sculptTablePos);

    sculptTableSurf->setInnerMaterial(material::GLASS);

    pSurf sculptTopCyl = Quadric::cylinder(sculptTableTopRadius, sculptTableTopRadius);
    pSurf sculptTopBot = Plane::plane(glm::dvec3(0, 0,-1), glm::dvec3(0,0, sculptTableHeight + 0.002));
    pSurf sculptTopTop = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0,0, sculptTableHeight + sculptTableTopThick));
    pSurf sculptTopSurf = sculptTopCyl & sculptTopBot & sculptTopTop;
    Surface::translate(sculptTopSurf, sculptTablePos);

    sculptTopSurf->setInnerMaterial(material::SILVER);

    pZone sculptTableZone(new StageZone("Sculpture Table Zone"));
    sculptZone->addSubzone(sculptTableZone);
    sculptTableZone->setBounds(Box::boxPosDims(
        sculptTablePos + glm::dvec3(0, 0, (sculptTableHeight + sculptTableTopThick) /2.0 + 0.001),
        glm::dvec3(scultpTableRadius*3, scultpTableRadius*3, sculptTableHeight+sculptTableTopThick)));

    pProp sculptTableProp(new Prop("Sculpture Table"));
    sculptTableProp->addSurface(sculptTableSurf);
    sculptTableProp->addSurface(sculptTopSurf);
    sculptTableZone->addProp(sculptTableProp);



    pCoat theFruitCoat = coating::createClearCoat(0.01);
    pMat theFruitMat = material::createInsulator(
        glm::dvec3(1.0, 0.6, 0.48), 1.3, 0.5, 0.1);

    pMat babyMat = material::SILVER;
    pCoat babyCoat = coating::createClearPaint(
        glm::dvec3(1.0), 0.0, 0.8);

    ///////////////
    // Sculpture //
    ///////////////
    double sculptScale = 0.17;
    pSurf flowerSurf;
    for(int i=0; i < 8; ++i)
    {
        pSurf outerSurf = Sphere::sphere(glm::dvec3(0, 0, -0.7), 1.0);
        pSurf innerSurf = Quadric::ellipsoid(0.985 - i/25.0, 2.0, 1.0);
        pSurf petalSurf = outerSurf & !innerSurf;

        Surface::scale(petalSurf, 1.0 - i / 25.0);
        Surface::rotate(petalSurf, glm::pi<double>() * i * 3.0 / 8.0, glm::dvec3(0, 0, 1));
        Surface::translate(petalSurf, glm::dvec3(0, 0, 1.5));

        flowerSurf = flowerSurf | petalSurf;
    }

    flowerSurf = flowerSurf & !Sphere::sphere(glm::dvec3(0.0, 0.0, 0.9), 0.65);
    flowerSurf = flowerSurf & Plane::plane(glm::dvec3(0, 0, -1), glm::dvec3(0, 0, 0.001));
    flowerSurf->setInnerMaterial(material::createInsulator(glm::dvec3(0.95, 0.60, 0.55), 1.2, 0.95, 1.0));
    flowerSurf->setCoating(coating::createClearCoat(1.0));


    pSurf pearlSurf = Sphere::sphere(glm::dvec3(0.0, 0.0, 0.9), 0.63);
    pearlSurf->setInnerMaterial(babyMat);
    pearlSurf->setCoating(babyCoat);

    pProp sculptProp(new Prop("Sculpture"));
    sculptProp->addSurface(flowerSurf);
    sculptProp->addSurface(pearlSurf);

    pZone sculptFlowerZone(new StageZone("Sculpture Flower Zone"));
    sculptZone->addSubzone(sculptFlowerZone);
    sculptFlowerZone->setBounds(Sphere::sphere(glm::dvec3(0, 0, 0.8), 1.0));
    sculptFlowerZone->addProp(sculptProp);
    sculptFlowerZone->scale(sculptScale);
    sculptFlowerZone->translate(sculptTablePos);
    sculptFlowerZone->translate(glm::dvec3(0, 0, sculptTableHeight+sculptTableTopThick+0.001));



    ////////////////////
    // Light Fixtures //
    ////////////////////
    double lightFixtureHeight = 0.30;
    double lightFixtureOutRad = 0.10;
    double lightFixtureInRad = 0.08;
    pSurf lightFixtureOut = Quadric::cylinder(lightFixtureOutRad, lightFixtureOutRad);
    pSurf lightFixtureIn = Quadric::cylinder(lightFixtureInRad, lightFixtureInRad);
    pSurf lightFixtureMid = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, 0));
    pSurf lightFixtureTop = Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, lightFixtureHeight/2.0));
    pSurf lightFixtureBot = Plane::plane(glm::dvec3(0, 0,-1), glm::dvec3(0, 0,-lightFixtureHeight/2.0));
    pSurf lightFixtureSurf = lightFixtureTop & lightFixtureBot &
                             lightFixtureOut & !(lightFixtureIn & ~lightFixtureMid);

    lightFixtureSurf->setInnerMaterial(material::TITANIUM);
    lightFixtureSurf->setCoating(lampCoat);

    glm::dvec3 fixturePositions[] = {
        glm::dvec3(boxMax.x*1/ 2.0, boxMax.y / 2.0, boxMax.z / 2.0 -  (lightFixtureHeight/2.0 + wallThickness.z) - 0.01), // Hall
        glm::dvec3(boxMin.x*1/ 4.0, boxMax.y / 2.0, boxMax.z * 0.75 - (lightFixtureHeight/2.0 + wallThickness.z) - 0.01), // Show room
        glm::dvec3(boxMin.x*3/ 4.0, boxMax.y / 2.0, boxMax.z * 0.75 - (lightFixtureHeight/2.0 + wallThickness.z) - 0.01), // Show room
        glm::dvec3(boxMin.x*1/ 4.0, boxMin.y / 2.0, boxMax.z * 0.75 - (lightFixtureHeight/2.0 + wallThickness.z) - 0.01), // Show room
        glm::dvec3(boxMin.x*3/ 4.0, boxMin.y / 2.0, boxMax.z * 0.75 - (lightFixtureHeight/2.0 + wallThickness.z) - 0.01), // Show room
    };

    _fixtureRadiantFlux = color::tungsten100W * 60.0;
    pMat cordMat = material::createInsulator(glm::dvec3(0.1), 1.44, 1.0, 1.0);
    for(const glm::dvec3& pos : fixturePositions)
    {
        const glm::dvec3 down(0, 0, -1);
        pLight fixtureBulb(new CircularLight("Fixture bulb",
            pos, down, lightFixtureInRad));
        fixtureBulb->setRadiantFlux(_fixtureRadiantFlux);

        pSurf surf = Surface::shell(lightFixtureSurf);
        Surface::translate(surf, pos);

        double cordTop;
        double cordSide = 0.004;
        if(pos.x < 0.0)
            cordTop = boxMax.z - wallThickness.z;
        else
            cordTop = boxMax.z/2.0 - wallThickness.z;
        pSurf cordSurf = Box::boxCorners(
            pos + glm::dvec3(-cordSide, -cordSide, lightFixtureHeight/2.0),
            pos + glm::dvec3( cordSide,  cordSide, cordTop - pos.z));
        cordSurf->setInnerMaterial(cordMat);

        pZone fixtureZone(new StageZone("Light Fixture Zone"));
        stageZone->addSubzone(fixtureZone);
        fixtureZone->setBounds(Box::boxCorners(
            pos + glm::dvec3(-lightFixtureOutRad, -lightFixtureOutRad, -lightFixtureHeight/2.0),
            pos + glm::dvec3( lightFixtureOutRad,  lightFixtureOutRad, cordTop - pos.z)));

        pProp lightFixturesProp(new Prop("Light Fixture"));
        lightFixturesProp->addSurface(surf);
        lightFixturesProp->addSurface(cordSurf);
        fixtureZone->addProp(lightFixturesProp);
        fixtureZone->addLight(fixtureBulb);
        fixtureBulb->setIsOn(false);


        if(pos == fixturePositions[0])
            _hallLight = fixtureBulb;
        else if(pos == fixturePositions[1])
            _roomLightFront = fixtureBulb;
        else if(pos == fixturePositions[4])
            _roomLightBack = fixtureBulb;
    }


    ////////////
    // Clouds //
    ////////////
    srand(3);
    pSurf cloudSurf;
    int cloudCount = 50.0;
    double minCloudRad = 3.0;
    double maxCloudRad = 12.0;
    glm::dvec3 cloudsDim = glm::dvec3(30, 60, 25.0);
    for(int i=0; i < cloudCount; ++i)
    {
        cloudSurf = cloudSurf | Sphere::sphere(
                    glm::ballRand(0.5) * cloudsDim,
                    glm::linearRand(minCloudRad, maxCloudRad));
    }
    cloudSurf = Surface::translate(cloudSurf, glm::dvec3(0, 0, cloudsDim.z/4.0));
    cloudSurf = cloudSurf & Plane::plane(glm::dvec3(0, 0, -1), glm::dvec3(0, 0, 0));
    cloudSurf->setCoating(coating::CLEAR_ROUGH);
    cloudSurf->setInnerMaterial(material::createInsulator(
        glm::dvec3(0.93), material::AIR_REFRACTIVE_INDEX, 0.4, 1.0));
    pProp cloudsProp(new Prop("Clouds"));
    cloudsProp->addSurface(cloudSurf);
    _cloudsZone.reset(new StageZone("Clouds Zone"));
    stageSet->addSubzone(_cloudsZone);
    _cloudsZone->addProp(cloudsProp);

    _cloudsZone->setBounds(Box::boxCorners(
        glm::dvec3(-cloudsDim.x/2.0 - maxCloudRad, -cloudsDim.y/2.0 - maxCloudRad, 0.0),
        glm::dvec3( cloudsDim.x/2.0 + maxCloudRad,  cloudsDim.y/2.0 + maxCloudRad, cloudsDim.z*0.75 + maxCloudRad)));
    _cloudsPosition = glm::dvec3(-200.0, -150.0, 50.0);
    _cloudsZone->translate(_cloudsPosition);



    ///////////////
    // The Fruit //
    ///////////////

    _theFruitHeight = 1.6;
    _theFruitPosition = glm::dvec3(-6.0, -7.0, 0.0);
    pSurf theFruitSurf = Sphere::sphere(glm::dvec3(0, 0, 0.5), 0.5);
    theFruitSurf->setInnerMaterial(theFruitMat);
    theFruitSurf->setCoating(theFruitCoat);

    pSurf theFruitBaby = Sphere::sphere(glm::dvec3(0, 0, 0.4), 0.075);
    theFruitBaby->setOuterMaterial(babyMat);
    theFruitBaby->setInnerMaterial(theFruitMat);
    theFruitBaby->setCoating(babyCoat);

    _theFruitSurf = Surface::shell(theFruitSurf);
    pProp theFruiProp(new Prop("The Fruit"));
    theFruiProp->addSurface(Surface::shell(_theFruitSurf) & !theFruitBaby);
    theFruiProp->transform(glm::scale(glm::dmat4(), glm::dvec3(0.7, 0.7, _theFruitHeight)));
    _theFruitZone.reset(new StageZone("The Fruit Zone"));
    _theFruitZone->addProp(theFruiProp);
    _theFruitZone->transform(glm::translate(glm::dmat4(), _theFruitPosition));
    stageSet->addSubzone(_theFruitZone);

/*
    ////////////////////////////
    // The rest of the family //
    ////////////////////////////
    glm::dvec3 geoffroyPos = glm::dvec3(6.3, 0.5, 0.0);
    pSurf geoffroySurf = Sphere::sphere(glm::dvec3(0, 0, 0.5), 0.5);
    geoffroySurf->setCoating(coating::createClearCoat(0.2));
    geoffroySurf->setInnerMaterial(material::createInsulator(
        glm::dvec3(0.4, 0.5, 0.9), 1.3, 0.5, 1.0));

    pProp geoffroyProp(new Prop("Geoffroy"));
    geoffroyProp->addSurface(geoffroySurf);
    geoffroyProp->transform(glm::scale(glm::dmat4(), glm::dvec3(0.7, 0.7, 1.8)));
    geoffroyProp->translate(geoffroyPos);
    stageSet->addProp(geoffroyProp);


    glm::dvec3 babyPos = glm::dvec3(7.2, 0.7, 0.5);
    glm::dvec3 babySize = glm::dvec3(0.5, 0.5, 0.5);
    pSurf babySurf = Sphere::sphere(glm::dvec3(0.0, 0.0, 0.5), 0.5);
    babySurf->setCoating(coating::createClearPaint(glm::dvec3(1.0), 0.0, 0.8));
    babySurf->setInnerMaterial(material::SILVER);

    pSurf pedestalSurf = Quadric::cylinder(babySize.x * 0.6, babySize.y * 0.6)
            & Plane::plane(glm::dvec3(0, 0, -1), glm::dvec3(0, 0, -babyPos.z))
            & Plane::plane(glm::dvec3(0, 0, 1), glm::dvec3(0, 0, 0.0));
    pedestalSurf->setInnerMaterial(material::createInsulator(glm::dvec3(0.4, 0.4, 0.1), 1.5, 1.0, 1.0));
    pedestalSurf->setCoating(coating::CLEAR_ROUGH);

    pProp babyProp(new Prop("Baby"));
    babyProp->addSurface(babySurf);
    babyProp->transform(glm::scale(glm::dmat4(), babySize));
    babyProp->addSurface(pedestalSurf);
    babyProp->translate(babyPos);
    stageSet->addProp(babyProp);
    */

    _pathModel->init(stageSet);
}

void TheFruitChoreographer::terminate()
{

}

std::shared_ptr<Surface> TheFruitChoreographer::createHoleStrippedWall(
        const glm::dvec3& size,
        double stripeWidth,
        double holeWidth,
        double border)
{

    double zmin = -size.x/2.0 + border;
    double zmax = size.z + size.x/2.0 - border;
    const glm::dvec3 normalUp = glm::normalize(glm::dvec3(-1, 0, 1));
    const glm::dvec3 normalDn = glm::normalize(glm::dvec3( 1, 0,-1));

    double holeOff = holeWidth / normalUp.z;
    double patternWidth = (stripeWidth + holeWidth) / normalUp.z;
    int patternCount = glm::round((zmax - zmin) / patternWidth);

    pSurf holes;
    double stripZ = size.z / 2.0 - ((patternCount-1)/2.0) * patternWidth;
    for(int i=0; i < patternCount; ++i)
    {
        pSurf holeUp = Plane::plane(normalUp, glm::dvec3(0, 0, stripZ + holeOff/2.0));
        pSurf holeDn = Plane::plane(normalDn, glm::dvec3(0, 0, stripZ - holeOff/2.0));

        if(holes.get() == nullptr)
            holes = holeUp & holeDn;
        else
            holes = holes | (holeUp & holeDn);

        stripZ += patternWidth;
    }

    pSurf wallBase = Box::boxCorners(
            glm::dvec3(-size.x/2, -size.y/2, 0),
            glm::dvec3(size.x/2, size.y/2, size.z));

    pSurf bordeSurf = Box::boxCorners(
        glm::dvec3(-size.x/2 + border, -size.y/2 - 0.001, border),
        glm::dvec3( size.x/2 - border,  size.y/2 + 0.001, size.z- border));

    pSurf wallSurf = wallBase & !(bordeSurf & holes);
    return wallSurf;
}
