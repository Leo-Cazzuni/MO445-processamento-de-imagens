#ifndef METHODCHOOSER_H
#define METHODCHOOSER_H

#include <QDialog>

namespace Ui {
class MethodChooser;
}

class MethodChooser : public QDialog
{
    Q_OBJECT

public:
    explicit MethodChooser(QWidget *parent, QStringList methods);
    ~MethodChooser();

    QString activeMethod();
private:
    Ui::MethodChooser *ui;
};

#endif // METHODCHOOSER_H
