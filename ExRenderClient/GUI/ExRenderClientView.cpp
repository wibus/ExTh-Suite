#include "ExRenderClientView.h"

#include <PropRoom2D/Team/AbstractTeam.h>
#include <PropRoom2D/Team/ArtDirector/GlArtDirector.h>

#include <PropRoom3D/Team/AbstractTeam.h>
#include <PropRoom3D/Team/ArtDirector/ArtDirectorClient.h>

#include <Scaena/Play/Play.h>

using namespace cellar;


ExRenderClientView::ExRenderClientView() :
    scaena::QGlWidgetView("Experimental Render Client View")
{
}

ExRenderClientView::~ExRenderClientView()
{

}

void ExRenderClientView::notify(cellar::CameraMsg& msg)
{
    if(msg.change == CameraMsg::EChange::VIEWPORT)
    {
        setFixedSize(msg.camera.viewport().x,
                     msg.camera.viewport().y);
    }
}

void ExRenderClientView::installArtDirectors(scaena::Play& play)
{
    _artDirector2D.reset(new prop2::GlArtDirector());
    play.propTeam2D()->addArtDirector(_artDirector2D);

    _artDirectorClient.reset(new prop3::ArtDirectorClient());
    play.propTeam3D()->addArtDirector(_artDirectorClient);
    _artDirector3D = _artDirectorClient;

    _artDirectorClient->camera()->registerObserver(*this);
    _artDirectorClient->camera()->updateViewport(800, 600);
}

void ExRenderClientView::setup()
{

}
