#include "markersettings.h"
#include "ui_markersettings.h"

#include <views/view.h>

MarkerSettings *MarkerSettings::_instance = nullptr;

MarkerSettings::MarkerSettings(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MarkerSettings)
{
    ui->setupUi(this);
    createConnections();

    QGraphicsScene *scene = new QGraphicsScene;
    ui->gvBrush->setRenderHint(QPainter::Antialiasing);
    brushGraphicsItem = scene->addEllipse(QRect(), QPen(), QBrush(Qt::black));
    ui->gvBrush->setScene(scene);

    ui->gpBrush->hide();
}

MarkerSettings *MarkerSettings::instance(QWidget *parent)
{
    if (_instance == nullptr)
        _instance = new MarkerSettings(parent);
    return _instance;
}

MarkerSettings::~MarkerSettings()
{
    emit(HaltAnnotation());
    destroyConnections();
    emit(destroyed());
    delete ui;

    _instance = nullptr;
}


void MarkerSettings::fillMarkerData(int label, QColor color, QString name, float radius, bool visible)
{
    this->label = label;
    this->color = color;
    this->name = name;
    this->radius = radius;

    QPoint p(radius, radius);
    QRect r(QPoint(), 2*p);
    r.moveCenter(p);
    brushGraphicsItem->setRect(r);

    ui->sbLabel->setEnabled(label != 0);
    ui->sbLabel->setValue(label);
    ui->leObject->setText(name);
    QString s = "background-color:rgb(%1, %2, %3);";
    ui->wColor->setStyleSheet(s.arg(color.red()).arg(color.green()).arg(color.blue()));

    ui->sbSize->setValue(radius);

    setMarkerVisibility(label, visible);
}

void MarkerSettings::setMarkerVisibility(int label, bool visible)
{
    if (label == this->label) {
        disconnect(ui->cbVisible, SIGNAL(stateChanged(int)), nullptr, nullptr);

        Qt::CheckState state = visible? Qt::Checked : Qt::Unchecked;
        ui->cbVisible->setCheckState(state);

        connect(ui->cbVisible, SIGNAL(stateChanged(int)), this, SLOT(changeMarkerVisibility(int)));
    }
}

int MarkerSettings::getMarkerLabel()
{
    return this->label;
}

QString MarkerSettings::getMarkerName()
{
    return this->name;
}

QColor MarkerSettings::getMarkerColor()
{
    return this->color;
}

void MarkerSettings::updateMarkerColorOnForm(QColor color)
{
    this->color = color;
    QString s = "background-color:rgb(%1, %2, %3);";
    ui->wColor->setStyleSheet(s.arg(color.red()).arg(color.green()).arg(color.blue()));
}

void MarkerSettings::updateMarkerNameOnForm(QString name)
{
    this->name = name;
    ui->leObject->setText(name);
}

void MarkerSettings::changeMarkerVisibility(int state)
{
    bool visible = state == Qt::Checked ? true : false;
    int index = View::instance()->annotation->getIndexFromLabel(label);
    emit updateMarkerVisibility(index, visible);
}

void MarkerSettings::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    ui->pbFreeFormAnnotation->click();
}

void MarkerSettings::ChangeMarkerName()
{
    this->name = ui->leObject->text();
    emit (UpdateMarkerName(label,name));
}

void MarkerSettings::ChangeMarkerColor(iftColor *color)
{
    QColor c;
    QString s = "background-color:rgb(%1, %2, %3);";
    if (color == nullptr) {
        c = QColorDialog::getColor(this->color, this, "Select Color");
        if (!this->color.isValid())
            return;
    } else {
        iftColor rgb = iftYCbCrtoRGB(*color, 255);
        c.setRed(rgb.val[0]);
        c.setGreen(rgb.val[1]);
        c.setBlue(rgb.val[2]);
    }
    this->color.setRed(c.red());
    this->color.setGreen(c.green());
    this->color.setBlue(c.blue());
    ui->wColor->setStyleSheet(s.arg(this->color.red()).arg(this->color.green()).arg(this->color.blue()));
    emit(UpdateMarkerColor(this->label,this->color));
}

void MarkerSettings::ChangeMarkerLabel()
{
    int new_label = ui->sbLabel->value();

    if (new_label != label) {
        if (View::instance()->annotation->getMarkerInfoArray().hasLabel(new_label)) {
            QMessageBox::critical(this, "Error", "This label value already exists.");
            ui->sbLabel->setValue(label);
        } else if (label == 0) {
            QMessageBox::critical(this, "Error", "It is not possible to change the background label value.");
            ui->sbLabel->setValue(label);
        } else {
            View::instance()->annotation->setMarkerLabel(label, new_label);
            this->label = new_label;

            iftColor c = View::instance()->annotation->getMarkerColor(label);
            ChangeMarkerColor(&c);
        }
    }
}

void MarkerSettings::StartFreeFromAnnotationClicked()
{
    /*if (ui->pbFreeFormAnnotation->isChecked()){
        ui->pbEraseMarker->setChecked(false);
        ui->pbBoxAnnotation->setChecked(false);
        ui->pbFreeFormAnnotation->setChecked(false);
        emit(HaltAnnotation());
    } else {*/
        ui->pbBoxAnnotation->setChecked(false);
        ui->pbFreeFormAnnotation->setChecked(true);
        ui->pbEraseMarker->setChecked(false);
        emit(StartAnnotation(FREE_FORM_ANNOTATION));
        ui->gpBrush->show();
    //}
}


void MarkerSettings::StartBoxAnnotationClicked()
{
    /*if (ui->pbBoxAnnotation->isChecked()){
        ui->pbEraseMarker->setChecked(false);
        ui->pbBoxAnnotation->setChecked(false);
        ui->pbFreeFormAnnotation->setChecked(false);
        emit(HaltAnnotation());
    } else {*/
        ui->pbBoxAnnotation->setChecked(true);
        ui->pbFreeFormAnnotation->setChecked(false);
        ui->pbEraseMarker->setChecked(false);
        emit(StartAnnotation(BOX_ANNOTATION));
        ui->gpBrush->hide();
    //}
}

void MarkerSettings::EraseMarkerClicked()
{
    /*if (ui->pbEraseMarker->isChecked()){
        ui->pbEraseMarker->setChecked(false);
        ui->pbBoxAnnotation->setChecked(false);
        ui->pbFreeFormAnnotation->setChecked(false);
        emit(HaltAnnotation());
    } else {*/
        ui->pbBoxAnnotation->setChecked(false);
        ui->pbFreeFormAnnotation->setChecked(false);
        ui->pbEraseMarker->setChecked(true);
        ui->gpBrush->hide();

        emit(eraseMarker());
    //}
}

void MarkerSettings::changeBrush(double value)
{
    this->radius = value;
    QPoint p(radius, radius);
    QRect r(QPoint(), 2*p);
    r.moveCenter(p);
    brushGraphicsItem->setRect(r);

    QGraphicsScene *scene = ui->gvBrush->scene();
    QRectF rect = scene->itemsBoundingRect();
    scene->setSceneRect(rect);

    emit(UpdateBrush(this->radius));
}

void MarkerSettings::changeSphericity()
{
    bool sphere = ui->cbSpheric->checkState() == Qt::Checked;
    View::instance()->annotation->setSphericity(sphere);
}

void MarkerSettings::OkButtonClicked()
{
    this->close();
}

void MarkerSettings::createConnections()
{
    connect(ui->leObject, SIGNAL(editingFinished()), this, SLOT(ChangeMarkerName()));
    connect(ui->sbLabel, SIGNAL(valueChanged(int)), this, SLOT(ChangeMarkerLabel()));

    connect(ui->pbFreeFormAnnotation, SIGNAL(clicked(bool)), this, SLOT(StartFreeFromAnnotationClicked()));
    connect(ui->pbBoxAnnotation, SIGNAL(clicked(bool)), this, SLOT(StartBoxAnnotationClicked()));
    connect(ui->pbEraseMarker, SIGNAL(clicked(bool)), this, SLOT(EraseMarkerClicked()));

    connect(ui->wColor, SIGNAL(WidgetDoubleClicked()), this, SLOT(ChangeMarkerColor()));

    connect(ui->cbVisible, SIGNAL(stateChanged(int)), this, SLOT(changeMarkerVisibility(int)));

    connect(ui->cbSpheric, SIGNAL(stateChanged(int)), this, SLOT(changeSphericity()));

    connect(ui->pbOk, SIGNAL(clicked(bool)), this, SLOT(OkButtonClicked()));

    connect(ui->sbSize, QOverload<double>::of(&QDoubleSpinBox::valueChanged), ui->hsSize, &QSlider::setValue);
    connect(ui->hsSize, &QSlider::valueChanged, ui->sbSize, &QDoubleSpinBox::setValue);
    connect(ui->sbSize, SIGNAL(valueChanged(double)), this, SLOT(changeBrush(double)));
}

void MarkerSettings::destroyConnections()
{
    disconnect(ui->leObject, SIGNAL(editingFinished()), nullptr,nullptr);
    disconnect(ui->sbLabel, SIGNAL(valueChanged(int)), nullptr,nullptr);

    disconnect(ui->pbFreeFormAnnotation, SIGNAL(clicked(bool)), nullptr,nullptr);
    disconnect(ui->pbBoxAnnotation, SIGNAL(clicked(bool)), nullptr,nullptr);
    disconnect(ui->pbEraseMarker, SIGNAL(clicked(bool)), nullptr,nullptr);

    disconnect(ui->wColor, SIGNAL(WidgetDoubleClicked()), nullptr,nullptr);

    disconnect(ui->cbSpheric, SIGNAL(stateChanged(int)), nullptr, nullptr);

    disconnect(ui->cbVisible, SIGNAL(stateChanged(int)), nullptr, nullptr);

    disconnect(ui->sbSize, QOverload<double>::of(&QDoubleSpinBox::valueChanged), nullptr, nullptr);
    disconnect(ui->hsSize, &QSlider::valueChanged, nullptr, nullptr);
    disconnect(ui->sbSize, SIGNAL(valueChanged(double)), nullptr, nullptr);
}

void MarkerSettings::increaseBrush()
{
    ui->sbSize->setValue(ui->sbSize->value() + 1);
}

void MarkerSettings::decreaseBrush()
{
    ui->sbSize->setValue(ui->sbSize->value() - 1);
}
