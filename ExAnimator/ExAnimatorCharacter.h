#ifndef EXANIMATORCHARACTER_H
#define EXANIMATORCHARACTER_H

#include <memory>
#include <vector>

#include <GLM/glm.hpp>

#include <Scaena/Play/Character.h>

namespace cellar
{
    class CameraManFree;
}

namespace prop2
{
    class TextHud;
}

namespace prop3
{
    class Prop;
    class Surface;
    class Material;

    class ProceduralSun;
}


class ExAnimatorCharacter :
        public scaena::Character
{
public:
    ExAnimatorCharacter();

    virtual void enterStage() override;
    virtual void beginStep(const scaena::StageTime &time) override;
    virtual void draw(const std::shared_ptr<scaena::View> &view,
                      const scaena::StageTime&time) override;
    virtual void exitStage() override;

    virtual bool keyPressEvent(const scaena::KeyboardEvent& event) override;

protected:
    virtual void setupTheFruitStageSet();
    virtual void setupManufacturingStageSet();
    virtual void setupCornBoardStageSet();

private:
    double _camVelocity;
    std::shared_ptr<cellar::CameraManFree> _camMan;

    std::shared_ptr<prop3::Material> _glassMat;
    std::shared_ptr<prop3::Material> _fixtureMat;
    std::shared_ptr<prop3::Surface> _fixtureSurf;
    std::shared_ptr<prop3::Surface> _fixtureBounds;

    prop3::ProceduralSun* _backdrop;
};

#endif // EXANIMATORCHARACTER_H
