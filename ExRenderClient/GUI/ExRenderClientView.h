#ifndef EXRENDERCLIENTVIEW_H
#define EXRENDERCLIENTVIEW_H

#include <CellarWorkbench/Camera/Camera.h>

#include <CellarWorkbench/DesignPattern/SpecificObserver.h>

#include <Scaena/ScaenaApplication/QGlWidgetView.h>


namespace prop3
{
    class ArtDirectorClient;
}

namespace Ui
{
    class ExRenderClientGui;
}


class ExRenderClientView :
        public scaena::QGlWidgetView,
        public cellar::SpecificObserver<cellar::CameraMsg>
{
public:
    ExRenderClientView(Ui::ExRenderClientGui* ui);
    virtual ~ExRenderClientView();


    virtual void notify(cellar::CameraMsg& msg) override;

protected:
    virtual void installArtDirectors(scaena::Play& play) override;
    virtual void setup() override;


protected slots:
    virtual void ipAddress(const QString& ip);
    virtual void tcpPort(const QString& port);
    virtual void updateEachTile(bool update);
    virtual void connectToServer(bool connect);


private:
    Ui::ExRenderClientGui* _ui;
    std::shared_ptr<prop3::ArtDirectorClient> _artDirectorClient;
};

#endif // EXRENDERCLIENTVIEW_H
