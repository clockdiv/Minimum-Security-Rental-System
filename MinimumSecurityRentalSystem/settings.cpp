#include "QSettings"
#include "settings.h"
#include "ui_settings.h"
#include <QDebug>


Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
}

Settings::~Settings()
{
    delete ui;
}

void Settings::setMainWindow(MainWindow *mw)
{
    mainWindow = mw;
}

void Settings::on_pushButton_settings_fileopenDatabase_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open file", "/home", "*.sqlite");
    ui->lineEdit_settings_databaseFile->setText(filename);
}


void Settings::on_buttonBox_accepted()
{
    saveSettings();
}

void Settings::saveSettings()
{
    QString filename = ui->lineEdit_settings_databaseFile->text();
    QSettings setting(mainWindow->ORGANISATION, mainWindow->APPNAME);
    setting.beginGroup("Database");
    setting.setValue("Filename", filename);
    setting.endGroup();
}

void Settings::setUiDatabaseFilename(const QString& filename)
{
    ui->lineEdit_settings_databaseFile->setText(filename);
}

