#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    // STYLE BEGIN

    qApp->setStyle(QStyleFactory::create("Fusion"));
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText, Qt::gray);
    darkPalette.setColor(QPalette::Base, QColor(35,35,35));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::black);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::Light, Qt::transparent);
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(100,100,100));
    darkPalette.setColor(QPalette::Button, QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(100, 100, 100));
    darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(100, 100, 100));
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(160,200,200)/*QColor(42, 130, 218)*/);
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    qApp->setPalette(darkPalette);
    qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");

    qApp->setStyleSheet("QTableView { gridline-color: #fff; }");

   #ifdef Q_OS_MAC
    QFont f = qApp->font();
    //f.setFamily("Monaco");
    f.setPointSize(11);
    qApp->setFont(f);
   #endif

    // STYLE END


    MainWindow w;
    w.show();
    w.loadWindowSettings();
    return a.exec();
}
