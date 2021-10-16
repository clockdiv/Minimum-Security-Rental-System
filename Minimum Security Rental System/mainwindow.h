#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QDropEvent>
#include <QCamera>
#include <QCameraImageCapture>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void deleteItemFromInventory(QString ID);

private:
    Ui::MainWindow *ui;
    QSqlDatabase m_db;
    QString dataDirectory = "/Users/julian/Documents/Qt/MinimumSecurityRentalSystem/Minimum Security Rental System/data/";
    QString filename_db = dataDirectory + "inventory.sqlite";

    QImage itemImage;

    QStringListModel* nameListModel;
    QStringList nameList;

    QScopedPointer<QCamera> m_camera;
    QScopedPointer<QCameraImageCapture> m_imageCapture;
    QImageEncoderSettings m_imageSettings;


    void dropEvent(QDropEvent* event);
    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dragLeaveEvent(QDragLeaveEvent* event);


private slots:
    // Menü and Tab-Group
    // =========================
    void on_actionPreferences_triggered();
    void on_tabWidget_tabBarClicked(int tabID);

    // Tab "Rental"
    // =========================
    void on_pushButton_addUser_clicked();
    void on_lineEdit_userName_textChanged();
    void on_listView_users_doubleClicked(QModelIndex);
    // Tab "Reservation"
    // =========================

    // Tab "Return"
    // =========================

    // Tab "Overview"
    // =========================

    // Tab "Show Inventory"
    // =========================
    void on_pushButton_overviewInventory_Reload_clicked();
    void on_pushButton_temp_clicked();
    void clearInventoryOverview();

    // Tab "Add Inventory"
    // =========================
//    void on_pushButton_takePicture_clicked();
//    void on_pushButton_loadPicture_clicked();
    void on_pushButton_inventorySave_clicked();
    void on_pushButton_inventoryClear_clicked();
    void on_lineEdit_objectName_textChanged();
    void on_lineEdit_objectManufacturer_textChanged();
    void on_lineEdit_objectID_textChanged();

    // methods for camera:
    void setCamera(const QCameraInfo &cameraInfo);
    void readyForCapture(bool ready);
    void processCapturedImage(int requestId, const QImage& img);
    void imageSaved(int id, const QString &fileName);
    void displayCaptureError(int id, const QCameraImageCapture::Error error, const QString &errorString);
    void takeImage();

    // Common methods:
    // ===========================
    QImage loadImageFile(const QString &filename);
//    void setImage(const QImage &newImage);

    // unsorted / tests:
    void createButton();
    void showMessage();
    void on_pushButton_openDialog_clicked(); // non-dynamic signal-slot-connection ("auto connect"?)


};


#endif // MAINWINDOW_H
