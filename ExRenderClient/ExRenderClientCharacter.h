#ifndef EXRENDERCLIENTCHARACTER_H
#define EXRENDERCLIENTCHARACTER_H

#include <Scaena/Play/Character.h>


class ExRenderClientCharacter :
        public scaena::Character
{
public:
    ExRenderClientCharacter();

    virtual void enterStage() override;
    virtual void beginStep(const scaena::StageTime &time) override;
    virtual void draw(const std::shared_ptr<scaena::View> &view,
                      const scaena::StageTime&time) override;
    virtual void exitStage() override;
};

#endif // EXRENDERCLIENTCHARACTER_H
