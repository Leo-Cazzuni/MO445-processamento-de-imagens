#include "markerinformation.h"

MarkerInformation::MarkerInformation(int label, QString name)
    : QObject()
{
    this->_label = label;
    if (name.isEmpty())
        if (label)
            this->_name = "obj"+QString::number(label);
        else
            this->_name = "background";
    else
        this->_name = name;
    this->_color = {{0,0,0},0};
    this->_visible = true;
    this->_useDefaultColor = true;
}

MarkerInformation::MarkerInformation(const MarkerInformation &other)
    : QObject()
{
    this->_color           = other._color;
    this->_label           = other._label;
    this->_useDefaultColor = other._useDefaultColor;
    this->_visible         = other._visible;
    this->_name            = other._name;
}

MarkerInformation &MarkerInformation::operator=(const MarkerInformation &)
{
    return *this;
}

int MarkerInformation::label() const
{
    return _label;
}

iftColor MarkerInformation::color() const
{
    if (_useDefaultColor)
        return defaultColor(_label);
    else
        return _color;
}

bool MarkerInformation::isVisible() const
{
    return _visible;
}

bool MarkerInformation::isActive() const
{
    return _active;
}

QString MarkerInformation::name() const
{
    return _name;
}

bool areColorsEqual(iftColor c1, iftColor c2) {
    // TODO remove workaround
    int value = pow(c1.val[0] - c2.val[0], 2) + pow(c1.val[1] - c2.val[1], 2) + pow(c1.val[2] - c2.val[2], 2);

    return value <= 3;
}

#include <QDebug>
void MarkerInformation::setColor(iftColor color)
{
    if (areColorsEqual(color, this->color())) {
        return;
    }
    qDebug() << "Color changed";
    _useDefaultColor = false;
    _color = color;
}

void MarkerInformation::setLabel(int label)
{
    _label = label;
}

void MarkerInformation::activate()
{
    _active = true;
}

void MarkerInformation::deactivate()
{
    _active = false;
}

void MarkerInformation::setVisible(bool visibility)
{
    _visible = visibility;
}

void MarkerInformation::setName(QString name)
{
    this->_name = name;
}

iftColor MarkerInformation::defaultColor(int label)
{
    iftColor color, ycbcr;
    if (label == 0) { // background is white
        color.val[0] = 0;
        color.val[1] = 0;
        color.val[2] = 255;
    } else {
        int h = label * 55 % 360;
        color.val[0] = h;
        color.val[1] = 255;
        color.val[2] = 255;
    }
    color = iftHSVtoRGB(color, 255);
    ycbcr = iftRGBtoYCbCr(color, 255);

    return ycbcr;
}


