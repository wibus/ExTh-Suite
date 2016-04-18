#ifndef EXRENDERCLIENTVIEW_H
#define EXRENDERCLIENTVIEW_H

#include <Scaena/ScaenaApplication/QGlWidgetView.h>


namespace prop3
{
    class ArtDirectorClient;
}


class ExRenderClientView : public scaena::QGlWidgetView
{
public:
    ExRenderClientView();
    virtual ~ExRenderClientView();


protected:
    virtual void installArtDirectors(scaena::Play& play) override;
    virtual void setup() override;


private:
    std::shared_ptr<prop3::ArtDirectorClient> _artDirectorClient;
};

#endif // EXRENDERCLIENTVIEW_H
