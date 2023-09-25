#ifndef ARCWEIGHTFUNCTION_H
#define ARCWEIGHTFUNCTION_H

#include <QWidget>

#include <views/view.h>

class View;

class ArcWeightFunction : public QWidget
{
    Q_OBJECT
public:
    explicit ArcWeightFunction(QWidget *parent = nullptr);
    ArcWeightFunction(const ArcWeightFunction &);
    ArcWeightFunction &operator=(const ArcWeightFunction &) = default;
    QString name();
signals:
    void update();

public slots:
    virtual void generate() = 0;
    virtual void preprocess();

protected slots:
    virtual void updateArcWeightParams();

protected:
    View *view;
    QString _name;

    void loadDefault(float adjRelRadius);

};

Q_DECLARE_METATYPE(ArcWeightFunction*)

#endif // ARCWEIGHTFUNCTION_H
