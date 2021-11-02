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
    QSettings setting(mainWindow->ORGANISATION, mainWindow->APPNAME);
    setting.beginGroup("Database");
    setting.setValue("Filename", ui->lineEdit_settings_databaseFile->text());
    setting.endGroup();
    setting.beginGroup("MailServer");
    setting.setValue("EmailAddress", ui->lineEdit_Settings_EmailAddress->text());
    setting.setValue("Password", ui->lineEdit_Settings_Password->text());
    setting.setValue("Server", ui->lineEdit_Settings_Server->text());
    setting.setValue("Port", ui->spinBox_Settings_Port->value());
    setting.endGroup();
}

void Settings::setUiDatabaseFilename(const QString& filename)
{
    ui->lineEdit_settings_databaseFile->setText(filename);
}

void Settings::setMailserver(QString emailAddress, QString password, QString server, int port)
{
    ui->lineEdit_Settings_EmailAddress->setText(emailAddress);
    ui->lineEdit_Settings_Password->setText(password);
    ui->lineEdit_Settings_Server->setText(server);
    qDebug() << port;
    ui->spinBox_Settings_Port->setValue(port);
}
