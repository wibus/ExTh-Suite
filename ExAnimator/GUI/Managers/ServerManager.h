#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <memory>

#include <QWidget>

namespace prop3
{
    class ArtDirectorServer;
}

namespace Ui
{
    class ExAnimatorGui;
}


class ServerManager : public QObject
{
public:
    ServerManager(Ui::ExAnimatorGui* ui);
    virtual ~ServerManager();

    virtual void setRaytracer(const std::shared_ptr<prop3::ArtDirectorServer>& server);

protected slots:
    virtual void ipAddress(const QString& ip);
    virtual void tcpPort(const QString& port);
    virtual void runServer(bool run);

private:
    Ui::ExAnimatorGui* _ui;
    std::shared_ptr<prop3::ArtDirectorServer> _server;
};

#endif // SERVERMANAGER_H
