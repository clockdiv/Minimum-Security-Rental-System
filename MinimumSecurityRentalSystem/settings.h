#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QFileDialog>
#include "mainwindow.h"

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();
    void setMainWindow(MainWindow* mw);
    void setUiDatabaseFilename(const QString& filename);
    void setMailserver(QString emailAddress, QString password, QString server, int port);

private slots:
    void on_pushButton_settings_fileopenDatabase_clicked();
    void on_buttonBox_accepted();

private:
    Ui::Settings *ui;
    MainWindow* mainWindow;
    void saveSettings();
};

#endif // SETTINGS_H
