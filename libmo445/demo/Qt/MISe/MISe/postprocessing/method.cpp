#include "method.h"

Method::Method(QWidget *parent)
    : QWidget{parent}
{

}

Method::Method(const Method &method)
    : QWidget{method.parentWidget()}
{
    _name = method._name;
}

Method::~Method()
{

}

QString Method::toString() const
{
    return _name;
}
