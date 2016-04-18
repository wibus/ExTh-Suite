#include "PathManager.h"

#include <fstream>

#include <QInputDialog>

#include <CellarWorkbench/Misc/Log.h>
#include <CellarWorkbench/Misc/StringUtils.h>

#include <CellarWorkbench/Path/PathVisitor.h>
#include <CellarWorkbench/Path/PointPath.h>
#include <CellarWorkbench/Path/LinearPath.h>
#include <CellarWorkbench/Path/CubicSplinePath.h>
#include <CellarWorkbench/Path/BasisSplinePath.h>
#include <CellarWorkbench/Path/PolynomialPath.h>
#include <CellarWorkbench/Path/CompositePath.h>

#include <PropRoom3D/Node/StageSet.h>

#include "ui_RaytracerGui.h"
#include "Model/PathModel.h"
#include "Model/SceneDocument.h"
#include "Animators/TheFruitChoreographer.h"

using namespace cellar;

Q_DECLARE_METATYPE(AbstractPath<double>*)
Q_DECLARE_METATYPE(AbstractPath<glm::dvec3>*)


Spin::Spin(double value)
{
    setDecimals(2);
    setMinimum(-400);
    setMaximum( 400);
    setSingleStep(0.1);
    setValue(value);
}

template<typename Data>
class TreeBuilder : public PathVisitor<Data>
{
public:
    TreeBuilder(std::vector<std::function<void(void)>>& displayPathCallback,
                std::vector<std::function<void(double)>>& durationChangedCallback,
                std::vector<std::string>& pathParentName,
                const std::function<void(void)>& refreshCallBack,
                Ui::RaytracerGui* ui,
                AbstractPath<Data>* path,
                const std::string& name,
                double beginTime);

    virtual ~TreeBuilder() {}


    virtual void visit(PointPath<Data>& path) override;

    virtual void visit(LinearPath<Data>& path) override;

    virtual void visit(CubicSplinePath<Data>& path) override;

    virtual void visit(BasisSplinePath<Data>& path) override;

    virtual void visit(PolynomialPath<Data>& path) override;

    virtual void visit(CompositePath<Data> &path) override;



    QStandardItem* getRoot() {return _last;}

protected:
    void putValue(QTableWidget* table, int row, double& value, AbstractPath<Data>& path, int column = 0);
    void putValue(QTableWidget* table, int row, glm::dvec3& value, AbstractPath<Data>& path);
    void setupTable(QTableWidget* table, int rowCount, bool isWeighted);

private:
    std::vector<std::function<void(void)>> & _displayPathCallback;
    std::vector<std::function<void(double)>>& _durationChangedCallback;
    std::vector<std::string>& _pathParentName;
    std::function<void(void)> _refreshCallBack;
    Ui::RaytracerGui* _ui;
    AbstractPath<Data>* _path;
    std::string _name;
    double _beginTime;

    QStandardItem* _last;
};


template<typename Data>
TreeBuilder<Data>::TreeBuilder(
        std::vector<std::function<void(void)>>& displayPathCallback,
        std::vector<std::function<void(double)>>& durationChangedCallback,
        std::vector<std::string>& pathParentName,
        const std::function<void(void)>& refreshCallBack,
        Ui::RaytracerGui* ui,
        AbstractPath<Data>* path,
        const std::string& name,
        double beginTime) :
    _displayPathCallback(displayPathCallback),
    _durationChangedCallback(durationChangedCallback),
    _pathParentName(pathParentName),
    _refreshCallBack(refreshCallBack),
    _ui(ui),
    _path(path),
    _name(name),
    _beginTime(beginTime)
{
    _path->accept(*this);
    _last->setText((QString(name.c_str()) +" [%1]").arg(
        SceneDocument::timeToString(_beginTime).c_str()));
}

template<>
void TreeBuilder<double>::setupTable(QTableWidget* table, int rowCount, bool isWeighted)
{
    table->clear();
    table->setRowCount(rowCount);
    if(isWeighted)
    {
        table->setColumnCount(2);
        table->setHorizontalHeaderLabels({"Value", "Weight"});
    }
    else
    {
        table->setColumnCount(1);
        table->setHorizontalHeaderLabels({"Value"});
    }
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

template<>
void TreeBuilder<glm::dvec3>::setupTable(QTableWidget* table, int rowCount, bool isWeighted)
{
    table->clear();
    if(isWeighted)
    {
        table->setColumnCount(4);
        table->setHorizontalHeaderLabels({"X", "Y", "Z", "Weight"});
    }
    else
    {
        table->setColumnCount(3);
        table->setHorizontalHeaderLabels({"Value"});
    }
    table->setRowCount(rowCount);
    table->setHorizontalHeaderLabels({});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

template<typename Data>
void TreeBuilder<Data>::putValue(QTableWidget* table, int row, double& value, AbstractPath<Data>& path, int column)
{
    Spin* spin =  new Spin(value);

    table->setCellWidget(row, column, spin);

    QObject::connect(spin, static_cast<void(Spin::*)(double)>(&Spin::valueChanged),
        [this, &value, &path](double nv){ value = nv; path.update(); _refreshCallBack();});
}

template<typename Data>
void TreeBuilder<Data>::putValue(QTableWidget* table, int row, glm::dvec3& value, AbstractPath<Data>& path)
{
    Spin* spinX =  new Spin(value.x);
    Spin* spinY =  new Spin(value.y);
    Spin* spinZ =  new Spin(value.z);

    table->setCellWidget(row, 0, spinX);
    table->setCellWidget(row, 1, spinY);
    table->setCellWidget(row, 2, spinZ);

    QObject::connect(spinX, static_cast<void(Spin::*)(double)>(&Spin::valueChanged),
        [this, &value, &path](double nv){ value.x = nv; path.update(); _refreshCallBack();});
    QObject::connect(spinY, static_cast<void(Spin::*)(double)>(&Spin::valueChanged),
        [this, &value, &path](double nv){ value.y = nv; path.update(); _refreshCallBack();});
    QObject::connect(spinZ, static_cast<void(Spin::*)(double)>(&Spin::valueChanged),
        [this, &value, &path](double nv){ value.z = nv; path.update(); _refreshCallBack();});
}

template<typename Data>
void TreeBuilder<Data>::visit(PointPath<Data>& path)
{
    _last = new QStandardItem(QString("Point [%1]").arg(
        SceneDocument::timeToString(_beginTime + path.duration()).c_str()));
    int callbackIdx = _displayPathCallback.size();
    _last->setData(QVariant(callbackIdx));

    _pathParentName.push_back(_name);
    _displayPathCallback.push_back([this, &path](){
        _ui->durationSpin->setValue(path.duration());

        setupTable(_ui->segmentTable, 1, false);
        putValue(_ui->segmentTable, 0, path.value(), path);
    });

    _durationChangedCallback.push_back([&path](double duration){
        path.setDuration(duration);
    });

    _beginTime += path.duration();
}

template<typename Data>
void TreeBuilder<Data>::visit(LinearPath<Data>& path)
{
    _last = new QStandardItem(QString("Linear [%1]").arg(
        SceneDocument::timeToString(_beginTime + path.duration()).c_str()));
    int callbackIdx = _displayPathCallback.size();
    _last->setData(QVariant(callbackIdx));

    _pathParentName.push_back(_name);
    _displayPathCallback.push_back([this, &path](){
        _ui->durationSpin->setValue(path.duration());

        setupTable(_ui->segmentTable, 2, false);
        putValue(_ui->segmentTable, 0, path.begin(), path);
        putValue(_ui->segmentTable, 1, path.end(), path);
    });

    _durationChangedCallback.push_back([&path](double duration){
        path.setDuration(duration);
    });

    _beginTime += path.duration();
}

template<typename Data>
void TreeBuilder<Data>::visit(CubicSplinePath<Data>& path)
{
    _last = new QStandardItem(QString("Cubic Spline [%1]").arg(
        SceneDocument::timeToString(_beginTime + path.duration()).c_str()));
    int callbackIdx = _displayPathCallback.size();
    _last->setData(QVariant(callbackIdx));

    _pathParentName.push_back(_name);
    _displayPathCallback.push_back([this, &path](){
        _ui->durationSpin->setValue(path.duration());

        setupTable(_ui->segmentTable, path.ctrlPts().size(), true);

        int row = 0;
        for(Data& pt : path.ctrlPts())
            putValue(_ui->segmentTable, row++, pt, path);

        row = 0;
        for(double& w : path.weights())
            putValue(_ui->segmentTable, row++, w, path, sizeof(Data)/sizeof(double));
    });

    _durationChangedCallback.push_back([&path](double duration){
        path.setDuration(duration);
    });

    _beginTime += path.duration();
}

template<typename Data>
void TreeBuilder<Data>::visit(BasisSplinePath<Data>& path)
{
    _last = new QStandardItem(QString("Basis Spline [%1]").arg(
        SceneDocument::timeToString(_beginTime + path.duration()).c_str()));
    int callbackIdx = _displayPathCallback.size();
    _last->setData(QVariant(callbackIdx));

    _pathParentName.push_back(_name);
    _displayPathCallback.push_back([this, &path](){
        _ui->durationSpin->setValue(path.duration());

        setupTable(_ui->segmentTable, path.ctrlPts().size(), false);

        int row = 0;
        for(Data& pt : path.ctrlPts())
            putValue(_ui->segmentTable, row++, pt, path);
    });

    _durationChangedCallback.push_back([&path](double duration){
        path.setDuration(duration);
    });

    _beginTime += path.duration();
}

template<typename Data>
void TreeBuilder<Data>::visit(PolynomialPath<Data>& path)
{
    _last = new QStandardItem(QString("Polynomial [%1]").arg(
        SceneDocument::timeToString(_beginTime + path.duration()).c_str()));
    int callbackIdx = _displayPathCallback.size();
    _last->setData(QVariant(callbackIdx));

    _pathParentName.push_back(_name);
    _displayPathCallback.push_back([this, &path](){
        _ui->durationSpin->setValue(path.duration());

        setupTable(_ui->segmentTable, path.ctrlPts().size(), true);

        int row = 0;
        for(Data& pt : path.ctrlPts())
            putValue(_ui->segmentTable, row++, pt, path);

        row = 0;
        for(double& w : path.weights())
            putValue(_ui->segmentTable, row++, w, path, sizeof(Data)/sizeof(double));
    });

    _durationChangedCallback.push_back([&path](double duration){
        path.setDuration(duration);
    });

    _beginTime += path.duration();
}


template<typename Data>
void TreeBuilder<Data>::visit(CompositePath<Data> &path)
{
    QStandardItem* item = new QStandardItem(QString("Composite [%1]").arg(
        SceneDocument::timeToString(_beginTime + path.duration()).c_str()));
    item->setSelectable(false);

    for(const auto& sub : path.paths())
    {
        sub->accept(*this);
        item->appendRow(_last);
    }

    _last  = item;
}


PathManager::PathManager(Ui::RaytracerGui* ui) :
    _ui(ui),
    _selectedPathId(-1)
{
    _doubleTreeBuilders.reserve(10);
    _dvec3TreeBuilders.reserve(10);

    _pathTreeModel = new QStandardItemModel();
    _ui->pathsTree->setModel(_pathTreeModel);

    connect(_ui->bindCameraToPathCheck, &QCheckBox::toggled,
            this, &PathManager::bindCameraToPath);

    connect(_ui->pathsTree->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this, &PathManager::selectionChanged);

    connect(_ui->displayDebugCheck, &QCheckBox::toggled,
            this, &PathManager::displayDebugToggled);

    connect(_ui->editNodesButton, &QPushButton::clicked,
            this, &PathManager::editPaths);

    connect(_ui->savePathsButton, &QPushButton::clicked,
            this, &PathManager::savePaths);

    connect(_ui->loadPathsButton, &QPushButton::clicked,
            this, &PathManager::loadPaths);

    connect(_ui->durationSpin, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &PathManager::durationChanged);
}

PathManager::~PathManager()
{
    delete _pathTreeModel;
}

void PathManager::setStageSet(
    const std::shared_ptr<prop3::StageSet>& stageSet)
{
    _stageSet = stageSet;
}

void PathManager::setChoreographer(
    const std::shared_ptr<TheFruitChoreographer>& choreographer)
{
    _choreographer = choreographer;
    loadPaths();

    bindCameraToPath(_ui->bindCameraToPathCheck->isChecked());
}

void PathManager::clearPaths()
{
    _pathTreeModel->clear();
    _ui->segmentTable->clear();
    _ui->segmentTable->setRowCount(0);
    _ui->segmentTable->setColumnCount(0);

    _durationChangedCallback.clear();
    _displayPathCallback.clear();
    _pathParentName.clear();

    _doubleTreeBuilders.clear();
    _dvec3TreeBuilders.clear();

    _selectedPathId = -1;
}

void PathManager::displayPaths()
{
    clearPaths();
    PathModel& pathModel = *_choreographer->pathModel();
    appendPath(pathModel.cameraEye,         PathModel::CAMERA_EYE_PATH_NAME);
    appendPath(pathModel.cameraTo,          PathModel::CAMERA_TO_PATH_NAME);
    appendPath(pathModel.cameraFoV,         PathModel::CAMERA_FOV_PATH_NAME);
    appendPath(pathModel.theFruitPos,       PathModel::THE_FRUIT_POS_PATH_NAME);
    appendPath(pathModel.theFruitHeight,    PathModel::THE_FRUIT_HEIGHT_PATH_NAME);
    appendPath(pathModel.clouds,            PathModel::CLOUDS_PATH_NAME);
    appendPath(pathModel.dayTime,           PathModel::DAY_TIME_PATH_NAME);
    appendPath(pathModel.hallLight,         PathModel::HALL_LIGHT_PATH_NAME);
    appendPath(pathModel.backLight,         PathModel::BACK_LIGHT_PATH_NAME);
    appendPath(pathModel.frontLight,        PathModel::FRONT_LIGHT_PATH_NAME);
    appendPath(pathModel.lampLight,         PathModel::LAMP_LIGHT_PATH_NAME);
    appendPath(pathModel.divThreshold,      PathModel::DIV_THRESHOLD_PATH_NAME);
    _ui->pathsTree->expandAll();

    emit pathChanged();
}

void PathManager::appendPath(const std::shared_ptr<AbstractPath<double> > &path, const std::string& name)
{
    _doubleTreeBuilders.emplace_back(
        _displayPathCallback,
        _durationChangedCallback,
        _pathParentName,
        std::bind(&PathManager::controlPointMoved, this),
        _ui, path.get(), name, getSceneDocument().animationTimeOffset());

    _pathTreeModel->appendRow(_doubleTreeBuilders.back().getRoot());
}

void PathManager::appendPath(const std::shared_ptr<AbstractPath<glm::dvec3>>& path, const std::string& name)
{
    _dvec3TreeBuilders.emplace_back(
        _displayPathCallback,
        _durationChangedCallback,
        _pathParentName,
        std::bind(&PathManager::controlPointMoved, this),
        _ui, path.get(), name, getSceneDocument().animationTimeOffset());

    _pathTreeModel->appendRow(_dvec3TreeBuilders.back().getRoot());
}

void PathManager::bindCameraToPath(bool bind)
{
    if(bind)
        _choreographer->bindCameraToPath();
    else
        _choreographer->freeCameraFromPath();

    emit freeCamera(!bind);
}

void PathManager::selectionChanged(const QItemSelection& selected,
                                   const QItemSelection& deselected)
{
    for(const QItemSelectionRange& range : selected)
    {
        const QAbstractItemModel* model = range.model();
        for(const QModelIndex& id :range.indexes())
        {
            QVariant var = model->itemData(id)[Qt::UserRole+1];

            _selectedPathId = var.toInt();
            _displayPathCallback[_selectedPathId]();
            _ui->displayDebugCheck->setChecked(
                _choreographer->pathModel()->isDebugLineVisible(
                    _pathParentName[_selectedPathId]));
        }
    }
}

void PathManager::displayDebugToggled(bool display)
{
    if(_selectedPathId >= 0)
    {
        _choreographer->pathModel()->setDebugLineVisibility(
            _pathParentName[_selectedPathId], display);
    }
}

void PathManager::durationChanged(double duration)
{
    if(_selectedPathId >= 0)
    {
        _durationChangedCallback[_selectedPathId](duration);
        emit pathChanged();
    }
}

void PathManager::controlPointMoved()
{
    if(_selectedPathId >= 0)
    {
        if(_choreographer->pathModel()->isDebugLineVisible(
                _pathParentName[_selectedPathId]))
        {
            _choreographer->pathModel()->refreshDebugLines();
        }
    }
}

void PathManager::editPaths()
{
    std::string input = _choreographer->pathModel()->serialize();

    bool ok = false;
    QString output = QInputDialog::getMultiLineText(
       _ui->editNodesButton, "Paths", "Path Tree", input.c_str(), &ok);

    if(ok)
    {
        std::string stream = output.toStdString();
        if(_choreographer->pathModel()->deserialize(stream))
        {
            getLog().postMessage(new Message('I', false,
                "New path tree successfully parsed", "PathManager"));
            displayPaths();
        }
        else
        {
            getLog().postMessage(new Message('E', false,
                "The edited path tree contained errors", "PathManager"));
        }
    }
}

void PathManager::savePaths()
{
    std::string output = _choreographer->pathModel()->serialize();

    std::ofstream fileStream;
    std::string pathsFile = getSceneDocument().getAnimationPathsFilePath();
    fileStream.open(pathsFile, std::ios_base::trunc);

    if(fileStream.is_open())
    {
        fileStream << output;
        fileStream.close();

        getLog().postMessage(new Message('I', false,
            pathsFile + " successfully saved", "PathManager"));
    }
    else
    {
        getLog().postMessage(new Message('E', false,
            "Could not save to " + pathsFile, "PathManager"));
    }
}

void PathManager::loadPaths()
{
    bool ok = false;
    std::string pathsFile = getSceneDocument().getAnimationPathsFilePath();
    std::string stream = cellar::fileToString(pathsFile, &ok);

    if(ok)
    {
        if(_choreographer->pathModel()->deserialize(stream))
        {
            getLog().postMessage(new Message('I', false,
                pathsFile + " successfully loaded", "PathManager"));
            displayPaths();
        }
        else
        {
            getLog().postMessage(new Message('E', false,
                "Could not load path from " + pathsFile, "PathManager"));
        }
    }
    else
    {
        getLog().postMessage(new Message('E', false,
            pathsFile + " is unreadable. Does it even exist?", "PathManager"));
    }
}
