#ifndef MARKERSETTINGS_H
#define MARKERSETTINGS_H

#include <QMainWindow>
#include "global.h"

namespace Ui {
class MarkerSettings;
}

class MarkerSettings : public QMainWindow
{
    Q_OBJECT

public:
    static MarkerSettings* instance(QWidget *parent = nullptr);
    ~MarkerSettings();

    void fillMarkerData(int label, QColor color, QString name, float radius, bool visible);

    void setMarkerVisibility(int label, bool visible);

    int getMarkerLabel();
    QString getMarkerName();
    QColor getMarkerColor();

    void updateMarkerColorOnForm(QColor color);
    void updateMarkerNameOnForm(QString name);

    void createConnections();
    void destroyConnections();

    void increaseBrush();
    void decreaseBrush();

private slots:
    void ChangeMarkerName();
    void ChangeMarkerColor(iftColor *color = nullptr);
    void ChangeMarkerLabel();

    void EraseMarkerClicked();
    void StartFreeFromAnnotationClicked();
    void StartBoxAnnotationClicked();

    void changeBrush(double value);

    void changeSphericity();

    void OkButtonClicked();

    void changeMarkerVisibility(int state);
signals:
    void StartAnnotation(int);
    void HaltAnnotation();
    void UpdateBrush(float);
    void eraseMarker();
    void UpdateMarkerName(int,QString);
    void UpdateMarkerColor(int, QColor);
    void updateMarkerVisibility(int, bool);

private:
    Ui::MarkerSettings *ui;

    explicit MarkerSettings(QWidget *parent = 0);

    static MarkerSettings *_instance;

    QGraphicsEllipseItem *brushGraphicsItem = nullptr;

    void showEvent(QShowEvent *event) override;

    /*
     *  Variables
     */
    int label;
    float radius;
    QString name;
    QColor color;
};

#endif // MARKERSETTINGS_H
