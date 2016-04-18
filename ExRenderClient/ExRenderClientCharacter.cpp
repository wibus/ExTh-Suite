#include "ExRenderClientCharacter.h"

using namespace scaena;


ExRenderClientCharacter::ExRenderClientCharacter() :
    Character("Experimental Render Client Character")
{
}

void ExRenderClientCharacter::enterStage()
{
}

void ExRenderClientCharacter::beginStep(const StageTime& time)
{
}

void ExRenderClientCharacter::draw(const std::shared_ptr<View>& view,
                                   const StageTime& time)
{
}

void ExRenderClientCharacter::exitStage()
{
}
