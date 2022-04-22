#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTableView>
#include <QDate>
#include <QtSql>
#include <QCompleter>
#include <QMap>
#include <QComboBox>
#include "tableitemdelegate.h"
#include "rentercompleter.h"
#include "dialoginventoryadd.h"


struct Item {
public:
    int ID;
    QString Name;
    QString Barcode;
    QString Manufacturer;
    QString StorageRoom;
    QString Description;
    QString DateRemoved;
    QString Accessoires;
    QDateTime Timestamp;
};
Q_DECLARE_METATYPE(Item);

struct User {
public:
    int ID;
    QString Name;
    QString Surname;
    QString Department;
    int Year;
    QString Email;
    QDateTime Timestamp;
};
Q_DECLARE_METATYPE(User);

struct Rental {
    int ID;
    int UserID;
    QDate DateBegin;
    QDate DateEnd;
    QString Room;
    QString Project;
    QString Comment;
    QString AdditionalItems;
    QDateTime Timestamp;
};
Q_DECLARE_METATYPE(Rental);



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void loadWindowSettings();

    const QString ORGANISATION = "HfS Ernst Busch";
    const QString APPNAME = "Minimum Security Rental System";
    const QString DATEFORMAT= "yyyy-MM-dd";
    const QString DATETIMEFORMAT = "yyyy-MM-dd hh:mm:ss";
    const QString DATEFORMATREADABLE = "ddd, dd.MM.yyyy";


protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::MainWindow *ui;
    DialogInventoryAdd* dialogInventoryAdd = nullptr;
    QStandardItemModel *calendarModel = nullptr, *rentalModel = nullptr, *inventoryModel = nullptr;
    QTableView *frozenInventoryTableView = nullptr, *frozenRentalTableView = nullptr;
//    QPixmap *appLogo = nullptr;
    QSqlDatabase m_db;
    QString filename_db;
    QString dataDirectory;

    QDate tableStartDate, tableEndDate;
    QDate rentalStartDate, rentalEndDate;
    TableItemDelegate *tableItemDelegate = nullptr;

    RenterCompleter *renterNameCompleter = nullptr, *renterSurnameCompleter = nullptr;
    QStandardItemModel *usersModel = nullptr;

    void fillRenterDialog(QMap<int, QVariant> items);

    void setCalendarDateRange();
    void initializeCalendarTable();
    void initializeInventoryTable();
    void initializeRentalTable();

    QDate getOldestRentalDate();

    void saveWindowSettings();
    void loadDatabaseSettings();
    void loadInventoryFromDB();

    QList<Rental> getRentalsForItem(int);
    void updateTableItems();
    void updateTableGeometry();
    void moveRow(QStandardItemModel* source, int rowIndex, QStandardItemModel* destination);

    void loadAllUsers();
    int saveUser();
    int updateUser();

    void saveRental();

    void showItemPreview(QStandardItem*);
    void showRentalPreview(QStandardItem*);

    QImage loadImage(QString);
    QColor getColorFromUserID(int userID);

private slots:
    void ItemSelectedInInventory(const QModelIndex& index);
    void ItemSelectedInRental(const QModelIndex& index);
    void RentalSelectedInInventoryCalendar(const QModelIndex& index);
    void RentalSelectedInRentalCalendar(const QModelIndex& index);
    void dateSelected(const QItemSelection &, const QItemSelection &);
    void moveItemToRental(const QModelIndex & index);
    void moveItemToInventory(const QModelIndex & index);
    void renterNameCompleterActivated(const QModelIndex &index);
    void renterSurnameCompleterActivated(const QModelIndex &index);
    void LineEdit_UserName_changed(const QString&text);
    void sortInventoryModelByIndex(int index);
    void searchItemInInventory(const QString &text);

    void on_actionSettings_triggered();
    void on_actionAddInventory_triggered();
    void on_pushButton_RenterAdd_clicked();
    void on_pushButton_RenterUpdate_clicked();
    void on_pushButton_RenterClear_clicked();

    void on_pushButton_ConfirmRental_clicked();

    void addItemToInventory(const InventoryObject&);
};
#endif // MAINWINDOW_H
