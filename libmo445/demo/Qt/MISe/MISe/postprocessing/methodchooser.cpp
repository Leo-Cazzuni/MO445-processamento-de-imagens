#include "methodchooser.h"
#include "ui_methodchooser.h"

MethodChooser::MethodChooser(QWidget *parent, QStringList methods) :
    QDialog(parent),
    ui(new Ui::MethodChooser)
{
    ui->setupUi(this);
    ui->cbMethods->addItems(methods);
}

MethodChooser::~MethodChooser()
{
    delete ui;
}

QString MethodChooser::activeMethod()
{
    return ui->cbMethods->currentText();
}
