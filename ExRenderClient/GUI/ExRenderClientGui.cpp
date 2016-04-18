#include "ExRenderClientGui.h"
#include "ui_ExRenderClientGui.h"

#include <QDesktopWidget>

#include <Scaena/Play/Play.h>

#include "ExRenderClientView.h"


ExRenderClientGui::ExRenderClientGui(const std::shared_ptr<scaena::Play>& play) :
    _ui(new Ui::ExRenderClientGui),
    _play(play)
{
    _ui->setupUi(this);
    QPoint center = QApplication::desktop()->availableGeometry(this).center();
    move(center.x()-width()*0.5, center.y()-height()*0.5);

    _view.reset(new ExRenderClientView());
    _ui->viewScrollArea->setWidget(_view.get());
    _play->addView(_view);
}

ExRenderClientGui::~ExRenderClientGui()
{
    delete _ui;
}
