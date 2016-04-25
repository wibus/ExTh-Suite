#include "ExRenderClientGui.h"
#include "ui_ExRenderClientGui.h"

#include <QDesktopWidget>

#include <Scaena/Play/Play.h>

#include "ExRenderClientView.h"

using namespace prop3;


ExRenderClientGui::ExRenderClientGui(const std::shared_ptr<scaena::Play>& play) :
    _ui(new Ui::ExRenderClientGui),
    _play(play)
{
    _ui->setupUi(this);
    QPoint topRight = QApplication::desktop()->availableGeometry(this).topRight();
    move(topRight.x()-width(), topRight.y()-height());

    _view.reset(new ExRenderClientView(_ui));
    _ui->viewScrollArea->setWidget(_view.get());
    _play->addView(_view);
}

ExRenderClientGui::~ExRenderClientGui()
{
    delete _ui;
}
