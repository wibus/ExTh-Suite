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

    void generateComposites();

    void closeEvent(QCloseEvent* e);

private:
    Ui::ExCompositorWindow *ui;
    std::shared_ptr<Processor> _processor;
    std::vector<std::shared_ptr<Section>> _sections;
    std::shared_ptr<Section> _currentSection;
    QVBoxLayout* _sectionLayout;
    QPixmap* _currentPixmap;
    QLabel* _imageLabel;
};

#endif // EXCOMPOSITORWINDOW_H
