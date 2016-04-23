#include "ServerManager.h"

#include <PropRoom3D/Team/ArtDirector/RaytracerState.h>
#include <PropRoom3D/Team/ArtDirector/ArtDirectorServer.h>

#include "ui_ExAnimatorGui.h"

using namespace prop3;


ServerManager::ServerManager(Ui::ExAnimatorGui* ui) :
    _ui(ui)
{
}

ServerManager::~ServerManager()
{
}

void ServerManager::setRaytracer(const std::shared_ptr<prop3::ArtDirectorServer>& server)
{
    _server = server;

    _ui->ipAddressEdit->setText(_server->ipAddress().c_str());

    QString port = QString::number(ArtDirectorServer::DEFAULT_TCP_PORT);
    _ui->tcpPortEdit->setText(port);
    tcpPort(port);

    runServer(_ui->runServerButton->isChecked());

    connect(_ui->ipAddressEdit, &QLineEdit::textChanged,
            this, &ServerManager::ipAddress);

    connect(_ui->tcpPortEdit, &QLineEdit::textChanged,
            this, &ServerManager::tcpPort);

    connect(_ui->runServerButton, &QPushButton::toggled,
            this, &ServerManager::runServer);
}

void ServerManager::ipAddress(const QString& ip)
{
    // IP Address is immutable
}

void ServerManager::tcpPort(const QString& port)
{
    _server->setTcpPort(port.toInt());
}

void ServerManager::runServer(bool run)
{
    if(run)
    {
        _server->turnOn();

        if(_server->isRunning())
            _ui->tcpPortEdit->setEnabled(false);
        else
            _ui->runServerButton->setChecked(false);
    }
    else
    {
        _ui->tcpPortEdit->setEnabled(true);
    }
}
