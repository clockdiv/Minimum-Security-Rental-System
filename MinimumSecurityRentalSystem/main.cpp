#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>
#include <QFile>
#include <QSslSocket>

int main(int argc, char *argv[])
{


    QApplication a(argc, argv);

    qDebug() << "SSL:";
    qDebug() << QSslSocket::supportsSsl() << QSslSocket::sslLibraryBuildVersionString() << QSslSocket::sslLibraryVersionString();

    qApp->setStyle(QStyleFactory::create("Fusion"));
    /*
    QPalette p = qApp->palette();
    p.setColor(QPalette::Window, QColor(53,53,53));
    p.setColor(QPalette::Button, QColor(53,53,53));
    p.setColor(QPalette::Highlight, QColor(142,45,197));
    p.setColor(QPalette::ButtonText, QColor(255,255,255));
    p.setColor(QPalette::WindowText, QColor(255,255,255));
    qApp->setPalette(p);
    */

    // use stylesheets:
    /*
    QFile stylesheetFile("/Users/julian/Desktop/qttest/data/Obit.qss");
    stylesheetFile.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(stylesheetFile.readAll());
    a.setStyleSheet(styleSheet);
    */

    MainWindow w;
    w.show();
    return a.exec();
}
