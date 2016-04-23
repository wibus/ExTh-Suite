#include "ExRenderClientView.h"

#include <PropRoom2D/Team/AbstractTeam.h>
#include <PropRoom2D/Team/ArtDirector/GlArtDirector.h>

#include <PropRoom3D/Team/AbstractTeam.h>
#include <PropRoom3D/Team/ArtDirector/RaytracerState.h>
#include <PropRoom3D/Team/ArtDirector/ArtDirectorClient.h>
#include <PropRoom3D/Team/ArtDirector/ArtDirectorServer.h>

#include <Scaena/Play/Play.h>

#include <ui_ExRenderClientGui.h>

using namespace cellar;
using namespace prop3;


ExRenderClientView::ExRenderClientView(Ui::ExRenderClientGui* ui) :
    scaena::QGlWidgetView("Experimental Render Client View"),
    _ui(ui)
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
    _ui->tcpPortEdit->setText(QString::number(
        ArtDirectorServer::DEFAULT_TCP_PORT));

    ipAddress(_ui->ipAddressEdit->text());
    tcpPort(_ui->tcpPortEdit->text());
    updateEachTile(_ui->updateEachTileCheck->isChecked());

    connect(_ui->ipAddressEdit, &QLineEdit::textChanged,
            this, &ExRenderClientView::ipAddress);

    connect(_ui->tcpPortEdit, &QLineEdit::textChanged,
            this, &ExRenderClientView::tcpPort);

    connect(_ui->updateEachTileCheck, &QCheckBox::toggled,
            this, &ExRenderClientView::updateEachTile);
}

void ExRenderClientView::ipAddress(const QString& ip)
{
    _artDirectorClient->setServerIpAddress(ip.toStdString());
}

void ExRenderClientView::tcpPort(const QString& port)
{
    _artDirectorClient->setServerTcpPort(port.toStdString());
}

void ExRenderClientView::updateEachTile(bool update)
{
    _artDirectorClient->raytracerState()->setUpdateEachTile(update);
}

void ExRenderClientView::connectToServer(bool connect)
{
    if(connect)
        _artDirectorClient->connectToServer();
    else
        _artDirectorClient->deconnectFromServer();
}

