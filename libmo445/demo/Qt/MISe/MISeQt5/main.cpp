#include "mainwindow.h"
#include <QApplication>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <postprocessing/method.h>

Q_DECLARE_METATYPE(QStringList)

QString processingMethodToString(Method* t) {
    return t->toString();
}

int main(int argc, char *argv[])
{
    QFile file (QDir::homePath()+"/mivlog.txt");
    file.remove();

    qRegisterMetaTypeStreamOperators<QStringList>("QStringList");
    QMetaType::registerConverter<Method*, QString>(processingMethodToString);


    QApplication app(argc, argv);
    QApplication::setWindowIcon(QIcon(":/Images/icons/segmentation.png"));
    QApplication::setApplicationName("MISe");
    QApplication::setOrganizationName("LIDS");
    QApplication::setOrganizationDomain("lids.ic.unicamp.br");
    QApplication::setApplicationVersion("1.0");

    QLocale::setDefault(QLocale::English);
    setlocale(LC_NUMERIC, "en_US.UTF-8");

    QCommandLineParser parser;
    parser.setApplicationDescription("Multidimensional Image Segmentation");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOptions({
        {{"i", "input-image"},
            QApplication::translate("main", "Load input image."),
            QApplication::translate("main", "image")},
        {{"l", "label-image"},
            QApplication::translate("main", "Load label image."),
            QApplication::translate("main", "label")},
                      });

    // Process the actual command line arguments given by the user
    parser.process(app);

    QString img_path = parser.value("input-image");
    QString label_path = parser.value("label-image");

    MainWindow w;
    w.show();

    if (!img_path.isNull())
        w.loadImageFromCommandLine(QStringList(img_path));
    if (!label_path.isNull())
        w.loadLabelFromCommandLine(label_path);

    return app.exec();


    /*
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
    */
}
