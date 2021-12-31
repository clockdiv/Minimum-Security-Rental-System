#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>
#include <QFile>
#include <QSslSocket>

#include <InventoryWidgetSmall.h>

int main(int argc, char *argv[])
{
    //qRegisterMetaType<InventoryWidgetSmall>("InventoryWidgetSmall");

    QApplication a(argc, argv);

    qDebug() << "SSL:";
    qDebug() << QSslSocket::supportsSsl() << QSslSocket::sslLibraryBuildVersionString() << QSslSocket::sslLibraryVersionString();

    qApp->setStyle(QStyleFactory::create("Fusion"));
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(35,35,35));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::Light, Qt::transparent);
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(100,100,100));
    darkPalette.setColor(QPalette::Button, QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(100, 100, 100));
    darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(100, 100, 100));
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    qApp->setPalette(darkPalette);
    qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");

#ifdef Q_OS_MAC
    QFont f = qApp->font();
    //f.setFamily("Monaco");
    f.setPointSize(11);
    qApp->setFont(f);
#endif

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
