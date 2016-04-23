#ifndef EXRENDERCLIENTGUI_H
#define EXRENDERCLIENTGUI_H

#include <QMainWindow>

#include <Scaena/Play/Play.h>

namespace Ui
{
    class ExRenderClientGui;
}

class ExRenderClientView;


class ExRenderClientGui : public QMainWindow
{
    Q_OBJECT

public:
    explicit ExRenderClientGui(const std::shared_ptr<scaena::Play>& play);
    virtual ~ExRenderClientGui();

private:
    Ui::ExRenderClientGui *_ui;
    std::shared_ptr<scaena::Play> _play;
    std::shared_ptr<ExRenderClientView> _view;
};

#endif // EXRENDERCLIENTGUI_H
