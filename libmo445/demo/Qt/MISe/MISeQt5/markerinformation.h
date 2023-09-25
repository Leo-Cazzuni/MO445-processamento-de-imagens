#ifndef MARKERINFORMATION_H
#define MARKERINFORMATION_H

#include <QObject>
#include "ift.h"

class MarkerInformation : public QObject
{
    Q_OBJECT
public:
    MarkerInformation(int label, QString name = "");
    MarkerInformation(const MarkerInformation &other);
    MarkerInformation &operator =(const MarkerInformation &other);

    int label() const;
    iftColor color() const;
    bool isVisible() const;
    bool isActive() const;
    QString name() const;

    void setColor(iftColor color);
    void setLabel(int label);
    void activate();
    void deactivate();
    void setVisible(bool visibility);
    void setName(QString name);

    static iftColor defaultColor(int label);
signals:

private:
    int _label;
    iftColor _color;
    bool _visible, _active;
    QString _name;
    bool _useDefaultColor;
};

class MarkerInformationVector : public QVector<MarkerInformation> {
public:
    int lastActive() {
        return _lastActive;
    }

    void activate(int label) {
        for (MarkerInformation &markerInfo: *this) {
            if (label == markerInfo.label())
                markerInfo.activate();
            else
                markerInfo.deactivate();
        }
        _lastActive = label;
    }

    void deactivate() {
        for (MarkerInformation &markerInfo: *this) {
            markerInfo.deactivate();
        }
    }

    MarkerInformation getActive() {
        for (MarkerInformation &markerInfo: *this) {
            if (markerInfo.isActive())
                return markerInfo;
        }

        return MarkerInformation(-1);
    }

    bool hasLabel(int label) const {
        auto it = std::find_if(begin(),  end(),
                          [label] (const MarkerInformation& m) -> bool { return label == m.label(); });
        return it != end();
    }

    int maxLabel() const {
        int max = -1;
        for (MarkerInformation mi: *this) {
            if (mi.label() > max)
                max = mi.label();
        }
        return max;
    }

    iftColorTable *generateColorTable() {
        if (size() == 0)
            return nullptr;
        iftColorTable *t = iftCreateColorTable(this->maxLabel() + 1);
        for (int i = 0; i < t->ncolors; i++) {
            t->color[i] = iftRGBtoYCbCr({{255,255,255},0},255);
        }
        //TODO this returns all values, it shouldnt
        for (MarkerInformation &markerInfo: *this) {
            int l = markerInfo.label();
            t->color[l].val[0] = markerInfo.color().val[0];
            t->color[l].val[1] = markerInfo.color().val[1];
            t->color[l].val[2] = markerInfo.color().val[2];
        }

        return t;
    }

private:
    int _lastActive = 0;
};

#endif // MARKERINFORMATION_H
