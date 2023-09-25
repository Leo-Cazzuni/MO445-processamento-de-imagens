#ifndef FORESTWRAPPER_H
#define FORESTWRAPPER_H

#include <QObject>
#include <ift.h>
#include <global.h>

class ForestWrapper : public QObject
{
    Q_OBJECT
public:
    ForestWrapper(iftDynTrees *forest);
    ForestWrapper(iftImageForest   *forest);

    iftImage *label();
    iftImage *root();
    iftImage *pred();
    bool nonNull();
private:
    iftDynTrees *dynamic;
    iftImageForest   *watershed;

};

#endif // FORESTWRAPPER_H
