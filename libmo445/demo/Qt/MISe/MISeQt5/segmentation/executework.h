#ifndef EXECUTEWORK_H
#define EXECUTEWORK_H

#include <QObject>

#include "segmentation.h"

class ExecuteWork : public QObject {
    Q_OBJECT

public:
    ExecuteWork(Segmentation *method);
    virtual ~ExecuteWork() {}

public slots:
    void execute();

signals:
    void done();

private:
    Segmentation *method;
};

#endif // EXECUTEWORK_H
