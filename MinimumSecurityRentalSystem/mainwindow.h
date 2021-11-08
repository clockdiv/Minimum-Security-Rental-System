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
    void addItemToRental(QString ID);
    void removeItemFromRental(QString ID);
    void returnItemFromRental(QString objectID);
    const QString ORGANISATION = "HfS Ernst Busch";
    const QString APPNAME = "Minimum Security Rental System";

private:
    Ui::MainWindow *ui;
    QSqlDatabase m_db;
    QString dataDirectory;
    QString filename_db;

    QImage itemImage;
    int statusBarTimeout = 2000;

    QStringListModel* nameListModel;
    QStringList nameList;
    QStringList itemRentalList;
    int userID;

    QString lastSearchItemID;

    QScopedPointer<QCamera> m_camera;
    QScopedPointer<QCameraImageCapture> m_imageCapture;
    QImageEncoderSettings m_imageSettings;


    struct Item {
        int ID;
        QString ObjectName;
        QString ObjectID;
        QString Manufacturer;
        QString StorageRoom;
        QString Description;
        int MarkedAsRemoved;
    };

    struct User {
        int ID;
        QString Name;
        QString Surname;
        QString Department;
        int Year;
        QString Email;
    };

    struct Rental {
        int ID;
        int UserID;
        QStringList Itemlist;
        QStringList ItemlistReturn;
        QDate DateBegin;
        QDate DateEnd;
        QString Comment;
    };

    QMap<int, int> nameListDatabaseIDs;
    QList<int> rentalFilterList;

    QList<Rental> allRentals;

    void dropEvent(QDropEvent* event);
    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dragLeaveEvent(QDragLeaveEvent* event);

    void loadWindowSettings();
    void saveWindowSettings();
    void loadDatabaseSettings();

    Item getItemFromDatabase(QSqlQuery q);
    User getUserFromDatabase(QSqlQuery q);
    Rental getRentalFromDatabase(QSqlQuery q);


private slots:
    // Menü and Tab-Group
    // =========================
    void on_actionPreferences_triggered();
    void on_tabWidget_tabBarClicked(int tabID);

    // Tab "Rental"
    // =========================
    int addUser();
    void on_lineEdit_userName_textChanged();
    void on_listView_users_doubleClicked(QModelIndex);
    void on_lineEdit_rentalSearchItem_textChanged();
    void on_lineEdit_rentalSearchItem_returnPressed();
    void clearInventorySearchResults();
    void on_pushButton_rentalSave_clicked();
    void on_calendarWidget_RentalStart_selectionChanged();
    void on_calendarWidget_RentalEnd_selectionChanged();

    // Tab "Reservation"
    // =========================

    // Tab "Return"
    // =========================

    // Tab "Overview"
    // =========================
    void loadRentalOverview(const QList<int>& rentalFilterList);
    void clearRentalOverview();
    void on_lineEdit_RentalOverview_textChanged();
    QList<int> searchByUser(QString searchString);
    QList<int> searchByItemID(QString searchString);
    int getRentalIDfromObjectID(QString objectID);
    void loadAllRentals();


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

    // methods for camera:
    void setCamera(const QCameraInfo &cameraInfo);
    void readyForCapture(bool ready);
    //void imageAvailable(int requestId, const QVideoFrame& preview);
    //void processCapturedImage(int requestId, const QImage& img);
    //void imageSaved(int id, const QString &fileName);
    void displayCaptureError(int id, const QCameraImageCapture::Error error, const QString &errorString);
    void takeImage();

    // Common methods:
    // ===========================
    QImage loadImageFile(const QString &filename);
//    void setImage(const QImage &newImage);

    // unsorted / tests:
//    void createButton();
//    void showMessage();
//    void on_pushButton_openDialog_clicked(); // non-dynamic signal-slot-connection ("auto connect"?)


    void on_pushButton_sendMail_clicked();
    void mailSent(QString status);
};


#endif // MAINWINDOW_H
