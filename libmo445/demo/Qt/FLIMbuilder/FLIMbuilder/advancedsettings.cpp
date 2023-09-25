#include "advancedsettings.h"
#include "ui_advancedsettings.h"

AdvancedSettings::AdvancedSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdvancedSettings)
{
    ui->setupUi(this);
    this->createWidgetsConnections();

    setWindowTitle("Advanced Settings");
}

AdvancedSettings::~AdvancedSettings()
{
    delete ui;
}

void AdvancedSettings::on_rbMarkerBasedNormalization_toggled(bool checked)
{
    Q_UNUSED(checked);
    if (this->ui->rbMarkerBasedNormalization->isChecked()){
        emit(this->toggledNormalization(1)); // 1 -> marker-based normalization
    } else if (this->ui->rbObjectBasedNormalization->isChecked()){
        emit(this->toggledNormalization(2)); // 2 -> object-based normalization
    }
}

void AdvancedSettings::on_pbOk_clicked()
{
    this->close();
}

void AdvancedSettings::createWidgetsConnections()
{
    connect(this->ui->rbMarkerBasedNormalization, SIGNAL(toggled(bool)), this, SLOT(on_rbMarkerBasedNormalization_toggled(bool)), Qt::UniqueConnection);

    connect(this->ui->pbOk, SIGNAL(clicked()), this, SLOT(on_pbOk_clicked()), Qt::UniqueConnection);
}
