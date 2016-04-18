#include <Scaena/Play/Act.h>
#include <Scaena/Play/Play.h>
#include <Scaena/ScaenaApplication/Application.h>
#include <Scaena/ScaenaApplication/QGlWidgetView.h>

#include "ExAnimatorCharacter.h"
#include "UI/RaytracerGui.h"

using namespace std;
using namespace cellar;
using namespace prop3;
using namespace scaena;


int main(int argc, char* argv[])
{
    // Init application
    Application& app = getApplication();
    app.init(argc, argv);

    // Build the Play
    std::shared_ptr<Play> play(new Play("ExAnimator"));
    std::shared_ptr<Character> character(new ExAnimatorCharacter());
    std::shared_ptr<Act> act(new Act("Main Act"));
    act->addCharacter(character);
    play->appendAct(act);

    // Build GUI
    RaytracerGui* window = new RaytracerGui(play);
    window->show();

    app.setPlay(play);
    return app.execute();
}
