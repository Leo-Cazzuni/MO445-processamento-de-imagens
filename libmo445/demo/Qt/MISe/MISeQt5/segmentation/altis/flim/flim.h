#ifndef FLIM_H
#define FLIM_H

#include <QObject>
#include <ift.h>
#include <QDir>
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QTextStream>

class FLIM : public QObject
{
    Q_OBJECT
public:
    FLIM(iftFLIMArch *arch, QTemporaryDir *origDir, QDir *network);
    ~FLIM();

    Q_DECL_DEPRECATED iftImage *generateImageGradient();
    iftMImage *extractFeatures();
signals:

public slots:

private:
    iftFLIMArch *arch;
    QTemporaryDir *origDir;
    QDir *network;
};

#endif // FLIM_H
