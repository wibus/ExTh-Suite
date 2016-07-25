#ifndef EXCOMPOSITORWINDOW_H
#define EXCOMPOSITORWINDOW_H

#include <memory>

#include <QLabel>
#include <QVBoxLayout>
#include <QMainWindow>


namespace Ui {
class ExCompositorWindow;
}

class Section;
class Processor;


class ExCompositorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ExCompositorWindow(QWidget *parent = 0);
    ~ExCompositorWindow();


protected slots:
    void frameChanged(int frame);
    void sectionChaged(bool);

    void beginingMin(int min);
    void beginingSec(double sec);
    void fadeInMin(int min);
    void fadeInSec(double sec);
    void durationMin(int min);
    void durationSec(double sec);
    void fadeOutMin(int min);
    void fadeOutSec(double sec);
    void inColorButton();
    void outColorButton();

    void loadProject();
    void saveProject();

protected:
    void addSection(QLayout* layout, Section& section);
    void updateTimeLine();

    void play();
    void generate();
    void renderFrame();

    void closeEvent(QCloseEvent* e);

private:
    Ui::ExCompositorWindow *ui;
    std::shared_ptr<Processor> _processor;
    std::vector<std::shared_ptr<Section>> _sections;
    std::shared_ptr<Section> _currentSection;
    QVBoxLayout* _sectionLayout;
    QLabel* _inputImageLabel;
    QLabel* _outputImageLabel;
    QPixmap _workingPixmap;
    QImage _tmpImage;

    int _currentFrame;
};

#endif // EXCOMPOSITORWINDOW_H
