#ifndef EXRENDERCLIENTVIEW_H
#define EXRENDERCLIENTVIEW_H

#include <CellarWorkbench/Camera/Camera.h>

#include <CellarWorkbench/DesignPattern/SpecificObserver.h>

#include <Scaena/ScaenaApplication/QGlWidgetView.h>


namespace prop3
{
    class ArtDirectorClient;
}


class ExRenderClientView :
        public scaena::QGlWidgetView,
        public cellar::SpecificObserver<cellar::CameraMsg>
{
public:
    ExRenderClientView();
    virtual ~ExRenderClientView();


    virtual void notify(cellar::CameraMsg& msg) override;


protected:
    virtual void installArtDirectors(scaena::Play& play) override;
    virtual void setup() override;


private:
    std::shared_ptr<prop3::ArtDirectorClient> _artDirectorClient;
};

#endif // EXRENDERCLIENTVIEW_H
