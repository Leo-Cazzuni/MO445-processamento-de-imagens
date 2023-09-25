#include "forestwrapper.h"

ForestWrapper::ForestWrapper(iftDynTrees *forest)
{
    this->dynamic   = forest;
    this->watershed = nullptr;
}

ForestWrapper::ForestWrapper(iftImageForest *forest)
{
    this->dynamic   = nullptr;
    this->watershed = forest;
}

iftImage *ForestWrapper::label()
{
    if (dynamic) {
        return dynamic->label;
    } else if (watershed) {
        return watershed->label;
    } else {
        return nullptr;
    }
}

iftImage *ForestWrapper::root()
{
    if (dynamic) {
        return dynamic->root;
    } else if (watershed) {
        return watershed->root;
    } else {
        return nullptr;
    }
}

iftImage *ForestWrapper::pred()
{
    if (dynamic) {
        return dynamic->pred;
    } else if (watershed) {
        return watershed->pred;
    } else {
        return nullptr;
    }
}

bool ForestWrapper::nonNull()
{
    return dynamic != nullptr || watershed != nullptr;
}
