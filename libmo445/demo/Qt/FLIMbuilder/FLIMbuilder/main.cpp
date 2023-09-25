#include "mainwindow.h"
#include "runguard.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    RunGuard guard( "flimbuilder_key" );
    if ( !guard.tryToRun() ){
        return 0;
    }

    QApplication::setDesktopSettingsAware(false);
    QApplication a(argc, argv);
    MainWindow w;
    QLocale::setDefault(QLocale::English);
    setlocale(LC_NUMERIC, "en_US.UTF-8");
    w.setWindowTitle("FLIMbuilder");
    w.show();

    return a.exec();
}
