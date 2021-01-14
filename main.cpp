#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("Fusion");

    QSettings settings("settings.ini",QSettings::IniFormat);
    if(settings.contains("App/darkMode")){
        bool darkModeOn = settings.value("App/darkMode").toBool();
        if(darkModeOn==true){
            QFile f(":/darkorange.qss");

            if (!f.exists()) {
                printf("Unable to set stylesheet, file not found\n");
            }
            else {
                f.open(QFile::ReadOnly | QFile::Text);
                QTextStream ts(&f);
                a.setStyleSheet(ts.readAll());
            }
        }
    }

    QCoreApplication::setOrganizationName("UTB");
    QCoreApplication::setApplicationName("Disc viewer");
    MainWindow w;

    if(QFile::exists("images/cd.bmp"))
        w.setWindowIcon(QIcon("images/cd.bmp"));

    w.show();
    return a.exec();
}
