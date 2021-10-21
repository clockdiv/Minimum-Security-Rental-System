#include <QMessageBox>
#include <QtSql>
#include <QCameraInfo>
#include <QMimeData>
#include <QFileDialog>
#include <QImage>
#include <QImageReader>
#include <QListView>
#include <QStringListModel>
#include <QStringList>

//#include <QCameraViewfinder>
//#include <QMediaMetaData>

#include <QStyleFactory>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "InventoryWidget.h"
#include "InventoryWidgetSmall.h"
#include "RentalWidget.h"
#include "settings.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setAcceptDrops(true);

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(filename_db);
    m_db.open();



    const QList<QCameraInfo> availableCameras = QCameraInfo::availableCameras();
    qDebug() << "available cameras: ";
    for (const QCameraInfo &cameraInfo : availableCameras)
    {
        qDebug() << cameraInfo.description();
    }
    //setCamera(QCameraInfo::defaultCamera());

    /*
    for (const QCameraInfo &cameraInfo : availableCameras) {
        QAction *videoDeviceAction = new QAction(cameraInfo.description(), videoDevicesGroup);
        videoDeviceAction->setCheckable(true);
        videoDeviceAction->setData(QVariant::fromValue(cameraInfo));
        if (cameraInfo == QCameraInfo::defaultCamera())
            videoDeviceAction->setChecked(true);

        qDebug() << videoDeviceAction;
        //ui->menuDevices->addAction(videoDeviceAction);
    }
    */


    //connect(ui->pushButton_openDialog_2, SIGNAL(clicked()), this, SLOT(createButton()));
    //connect(ui->pushButton_inventorySave, SIGNAL(clicked()), this, SLOT(saveInventoryToDB()));
    //connect(ui->comboBox_userName, SIGNAL(currentTextChanged(const QString &)), this, SLOT(fillComboBox_userName(const QString &)));

    ui->comboBox_storageRoom->addItem("1.74");
    ui->comboBox_storageRoom->addItem("Ladenlokal");

    ui->comboBox_userDepartment->addItem("");
    ui->comboBox_userDepartment->addItem("Zeitgenössische Puppenspielkunst");
    ui->comboBox_userDepartment->addItem("Regie");
    ui->comboBox_userDepartment->addItem("Dramaturgie");
    ui->comboBox_userDepartment->addItem("Spiel && Objekt");
    ui->comboBox_userDepartment->addItem("Choreographie");
    ui->comboBox_userDepartment->addItem("Schauspiel");
    ui->comboBox_userDepartment->addItem("Technik");
    ui->comboBox_userDepartment->addItem("Verwaltung");

    nameListModel = new QStringListModel(this);
    ui->listView_users->setModel(nameListModel);

    ui->calendarWidget_RentalStart->setSelectedDate(QDateTime::currentDateTime().date());
    ui->calendarWidget_RentalEnd->setSelectedDate(QDateTime::currentDateTime().date().addDays(7));

    loadAllRentals();
    on_lineEdit_rentalSearchItem_textChanged();

}

MainWindow::~MainWindow()
{
    m_db.close();
    delete ui;
}

// Menü and Tab-Group
// =========================
void MainWindow::on_actionPreferences_triggered()
{
    Settings settingsDialog(this);
    if(settingsDialog.exec())
        qDebug() << "ok";
    else
        qDebug() << "cancel";
}

void MainWindow::on_tabWidget_tabBarClicked(int tabID)
{
    loadAllRentals();
    switch (tabID)
    {
        case 0:     // Rental
            if(!m_camera.isNull()) m_camera->stop();
            on_lineEdit_rentalSearchItem_textChanged();
            break;
        case 1:
            if(!m_camera.isNull()) m_camera->stop();
            break;
        case 2:     // Rental Overview
            if(!m_camera.isNull()) m_camera->stop();
            clearRentalOverview();
            loadRentalOverview(rentalFilterList);
            break;
        case 3:     // Show Inventory
            if(!m_camera.isNull()) m_camera->stop();
            on_pushButton_overviewInventory_Reload_clicked();
            break;
        case 4:     // Add Inventory
            setCamera(QCameraInfo::defaultCamera());
            break;
        default:
            break;
    }
}

MainWindow::Item MainWindow::getItemFromDatabase(QSqlQuery q)
{
    MainWindow::Item i;
    i.ID = -1;

    i.ID = q.value("ID").toInt();
    i.ObjectName = q.value("ObjectName").toString();
    i.Manufacturer = q.value("Manufacturer").toString();
    i.ObjectID = q.value("ObjectID").toString();
    i.Description = q.value("Description").toString();
    i.StorageRoom = q.value("StorageRoom").toString();
    i.MarkedAsRemoved = q.value("MarkedAsRemoved").toInt();

    return i;
}

MainWindow::User MainWindow::getUserFromDatabase(QSqlQuery q)
{
    MainWindow::User u;
    u.ID = -1;

    u.ID = q.value("ID").toInt();
    u.Name = q.value("Name").toString();
    u.Surname = q.value("Surname").toString();
    u.Department = q.value("Department").toString();
    u.Email = q.value("Email").toString();
    u.Year = q.value("Year").toInt();

    return u;
}

MainWindow::Rental MainWindow::getRentalFromDatabase(QSqlQuery q)
{
    MainWindow::Rental r;
    r.ID = -1;

    r.ID = q.value("ID").toInt();
    r.UserID = q.value("UserID").toInt();
    if(q.value("Itemlist").toString() != "") r.Itemlist = q.value("Itemlist").toString().split(",");
    if(q.value("ItemlistReturn").toString() != "") r.ItemlistReturn = q.value("ItemlistReturn").toString().split(",");
    r.DateBegin = QDate::fromString(q.value("DateBegin").toString(), Qt::ISODate);
    r.DateEnd = QDate::fromString(q.value("DateEnd").toString(), Qt::ISODate);
    r.Comment = q.value("Comment").toString();

    return r;
}


// Tab "Rental"
// =========================

int MainWindow::addUser()
{
    QSqlQuery query;

    query.prepare("INSERT INTO Users (Name, Surname, Department, Year, Email) VALUES (:name, :surname, :department, :year, :email)");
    query.bindValue(":name", ui->lineEdit_userName->text().trimmed());
    query.bindValue(":surname", ui->lineEdit_userSurname->text().trimmed());
    query.bindValue(":department", ui->comboBox_userDepartment->currentText());
    query.bindValue(":year", ui->spinBox_userYear->value());
    query.bindValue(":email", ui->lineEdit_userEmail->text());

    if(!query.exec()) {
        QMessageBox::critical(this, "SQL-Error: ", query.lastError().text());
        return -1;
    }
    return query.lastInsertId().toInt();
}

void MainWindow::on_lineEdit_userName_textChanged()
{
    userID = -1;
    QString userName = ui->lineEdit_userName->text();
    nameList.clear();
    nameListDatabaseIDs.clear();
    nameListModel->setStringList(nameList);

    if (userName == "")
    {
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM Users WHERE Name LIKE :user ORDER BY Name");
    query.bindValue(":user", userName+"%");
    if(!query.exec())
    {
        QMessageBox::critical(this, "SQL-Error: ", query.lastError().text());
    }
    else
    {
        while(query.next()) {
            MainWindow::User u = getUserFromDatabase(query);
            nameListDatabaseIDs.insert(nameList.count(), u.ID);
            QString entry = u.Name + " " +u.Surname + " " + u.Department;
            nameList.append(entry);
            nameListModel->setStringList(nameList);
        }
    }
}

void MainWindow::on_listView_users_doubleClicked(QModelIndex index)
{
    qDebug() << "double clicked " << index;
    qDebug() << "database id: " << nameListDatabaseIDs[index.row()];

    QSqlQuery query;
    query.prepare("SELECT * FROM Users WHERE ID = :id");
    query.bindValue(":id", nameListDatabaseIDs[index.row()]);
    if(!query.exec())
    {
        QMessageBox::critical(this, "SQL-Error: ", query.lastError().text());
    }
    else
    {
        query.next();
        ui->lineEdit_userName->setText(query.value("Name").toString());
        ui->lineEdit_userSurname->setText(query.value("Surname").toString());
        ui->lineEdit_userEmail->setText(query.value("Email").toString());
        ui->spinBox_userYear->setValue(query.value("Year").toInt());
        ui->comboBox_userDepartment->setCurrentText(query.value("Department").toString());
        userID = query.value("ID").toInt();
    }

}

void MainWindow::on_lineEdit_rentalSearchItem_textChanged()
{
    clearInventorySearchResults();

    QString searchItem = ui->lineEdit_rentalSearchItem->text();

    QSqlQuery query;
    query.prepare("SELECT * FROM Inventory WHERE (ObjectID LIKE :keyword) OR (ObjectName LIKE :keyword) OR (Manufacturer LIKE :keyword)");
    query.bindValue(":keyword", "%" + searchItem + "%");

    if(!query.exec())
    {
        QMessageBox::critical(this, "SQL-Error: ", query.lastError().text());
    }
    else
    {
        while(query.next()) {
            MainWindow::Item i = getItemFromDatabase(query);
            if (i.MarkedAsRemoved) continue;

            QString filename = dataDirectory + "img/" + i.ObjectID + ".jpg";

            InventoryWidgetSmall* inventoryWidgetSmall = new InventoryWidgetSmall();
            inventoryWidgetSmall->setMainWindow(this);
            inventoryWidgetSmall->setMaximumWidth(65535);
            inventoryWidgetSmall->setItemName(QString(i.Manufacturer + " " + i.ObjectName));
            inventoryWidgetSmall->setItemID(i.ObjectID);
            inventoryWidgetSmall->setItemDescription(i.Description);
            inventoryWidgetSmall->removeRemoveButton();
            inventoryWidgetSmall->removeReturnButton();
            QImage image = loadImageFile(filename);
            if(image.data_ptr() != NULL) {
                inventoryWidgetSmall->setImage(image);
            }
            ui->scrollAreaLayout_SearchItem->addWidget(inventoryWidgetSmall);

            // disable object if it's in the rental list:
            if(itemRentalList.indexOf(i.ObjectID) > -1)
                inventoryWidgetSmall->setDisabled(true);

            // disable object if it's already lent.
            for (MainWindow::Rental r : allRentals)
            {
                if(r.Itemlist.contains(i.ObjectID) && !r.ItemlistReturn.contains(i.ObjectID))
                {
                    inventoryWidgetSmall->setDisabled(true);
                }
            }



            lastSearchItemID = i.ObjectID;
        }

    }
}

void MainWindow::clearInventorySearchResults()
{
    if(ui->scrollAreaLayout_SearchItem->count() <= 0) return;
    QLayoutItem* child;
    while( (child = ui->scrollAreaLayout_SearchItem->takeAt(0)) != 0) {
        child->widget()->setHidden(true);
        child->widget()->deleteLater();
    }
}

void MainWindow::addItemToRental(QString ID)
{
    itemRentalList.append(ID);

    QSqlQuery query;
    query.prepare("SELECT * FROM Inventory WHERE ObjectID = :objectid");
    query.bindValue(":objectid", ID);

    if(!query.exec())
    {
        QMessageBox::critical(this, "SQL-Error: ", query.lastError().text());
    }
    else
    {
        while(query.next()) {
            QString objectName = query.value("ObjectName").toString();
            QString objectID = query.value("ObjectID").toString();
            QString manufacturer = query.value("Manufacturer").toString();
            QString description = query.value("Description").toString();
            QString storageRoom = query.value("StorageRoom").toString();
            QString filename = dataDirectory + "img/" + objectID + ".jpg";

            InventoryWidgetSmall* inventoryWidgetSmall = new InventoryWidgetSmall();
            inventoryWidgetSmall->setMaximumWidth(65535);
            inventoryWidgetSmall->setItemName(QString(manufacturer + " " + objectName));
            inventoryWidgetSmall->setItemID(objectID);
            inventoryWidgetSmall->setItemDescription(description);
            inventoryWidgetSmall->removeAddButton();
            inventoryWidgetSmall->removeReturnButton();

            QImage image = loadImageFile(filename);
            if(image.data_ptr() != NULL) {
                inventoryWidgetSmall->setImage(image);
            }
            inventoryWidgetSmall->setMainWindow(this);
            ui->scrollAreaLayout_RentalItems->addWidget(inventoryWidgetSmall);
        }
    }


}

void MainWindow::removeItemFromRental(QString ID) {
    int index = itemRentalList.lastIndexOf(ID);
    itemRentalList.removeAt(index);
    on_lineEdit_rentalSearchItem_textChanged();
}

void MainWindow::on_lineEdit_rentalSearchItem_returnPressed()
{
    if(ui->scrollAreaLayout_SearchItem->count() == 1){
        addItemToRental(lastSearchItemID);
        ui->lineEdit_rentalSearchItem->setText("");
    }
}

void MainWindow::on_pushButton_rentalSave_clicked()
{
    if(userID < 0)
    {
        userID = addUser();
    }

    itemRentalList.sort();
    QString itemlist = itemRentalList.join(",");

    QSqlQuery query;
    query.prepare("INSERT INTO Rentals (UserID, Itemlist, DateBegin, DateEnd, Comment) VALUES (:userid, :itemlist, :datebegin, :dateend, :comment)");
    query.bindValue(":userid", userID);
    query.bindValue(":itemlist", itemlist);
    query.bindValue(":datebegin", ui->calendarWidget_RentalStart->selectedDate().toString(Qt::ISODate));
    query.bindValue(":dateend", ui->calendarWidget_RentalEnd->selectedDate().toString(Qt::ISODate));
    query.bindValue(":comment", ui->lineEdit_rentalComment->text());

    if(!query.exec()) {
        QMessageBox::critical(this, "SQL-Error: ", query.lastError().text());
    } else {
        ui->statusbar->showMessage("👍 Rental saved to minimum security database.", statusBarTimeout);
    }

    // Clean up GUI
    ui->lineEdit_userName->setText("");
    ui->lineEdit_userSurname->setText("");
    ui->lineEdit_userEmail->setText("");
    ui->comboBox_userDepartment->setCurrentIndex(0);
    ui->spinBox_userYear->setValue(0);
    ui->lineEdit_rentalComment->setText("");

    ui->calendarWidget_RentalStart->setSelectedDate(QDateTime::currentDateTime().date());
    ui->calendarWidget_RentalEnd->setSelectedDate(QDateTime::currentDateTime().date().addDays(7));

    ui->lineEdit_rentalSearchItem->setText("");

    itemRentalList.clear();
    if(ui->scrollAreaLayout_RentalItems->count() > 0)
    {
        QLayoutItem* child;
        while( (child = ui->scrollAreaLayout_RentalItems->takeAt(0)) != 0) {
            child->widget()->setHidden(true);
            child->widget()->deleteLater();
        }
    }


}

void MainWindow::on_calendarWidget_RentalStart_selectionChanged()
{
    if (ui->calendarWidget_RentalStart->selectedDate() < QDateTime::currentDateTime().date())
    {
        ui->statusbar->showMessage("You live in the past!", statusBarTimeout);
        ui->calendarWidget_RentalStart->setSelectedDate(QDateTime::currentDateTime().date());
    }

    if(ui->calendarWidget_RentalEnd->selectedDate() < ui->calendarWidget_RentalStart->selectedDate())
    {
        ui->calendarWidget_RentalEnd->setSelectedDate(QDateTime::currentDateTime().date().addDays(7));
    }
}

void MainWindow::on_calendarWidget_RentalEnd_selectionChanged()
{
    if (ui->calendarWidget_RentalEnd->selectedDate() < ui->calendarWidget_RentalStart->selectedDate())
    {
        ui->statusbar->showMessage("Return date must be after rental date", statusBarTimeout);
        ui->calendarWidget_RentalEnd->setSelectedDate(ui->calendarWidget_RentalStart->selectedDate().addDays(7));
    }
}

// Tab "Return"
// =========================

// Tab "Overview"
// =========================
void MainWindow::loadRentalOverview(const QList<int>& rentalFilterList)
{
    QString searchItem = ui->lineEdit_RentalOverview->text();

    QString querystring = "SELECT * FROM Rentals";
    if(rentalFilterList.count() > 0) {      // if there are search filters, use them; otherwise show all rentals
        querystring += " WHERE ";
        for (int i = 0; i < rentalFilterList.count(); i++)
        {
            querystring += "(ID=";
            querystring += QString::number(rentalFilterList.at(i));
            querystring += ")";
            if (i < rentalFilterList.count() - 1) querystring += "OR";
        }
    }
    querystring += " ORDER BY ID DESC";
    QSqlQuery query;
    query.prepare(querystring);

    if(!query.exec())
    {
        QMessageBox::critical(this, "SQL-Error: ", query.lastError().text());
    }
    else
    {
        while(query.next()) {
            // get rental from database entry
            MainWindow::Rental r = getRentalFromDatabase(query);
            int userID = r.UserID;
            QStringList itemList = r.Itemlist;
            QStringList itemListReturn = r.ItemlistReturn;
            QString dateBegin = r.DateBegin.toString(Qt::ISODate);
            QString dateEnd = r.DateEnd.toString(Qt::ISODate);
            QString comment = r.Comment;


            // get user fromUserID
            QSqlQuery userQuery;
            userQuery.prepare("SELECT * FROM Users WHERE ID = :userid");
            userQuery.bindValue(":userid", userID);

            QString userName = "unknown name";
            QString userSurname = "unknown surname";
            QString userDepartment = "unknown department";
            if(!userQuery.exec())
            {
                QMessageBox::critical(this, "SQL-Error: ", userQuery.lastError().text());
            }
            else
            {
                userQuery.next();
                MainWindow::User u = getUserFromDatabase(userQuery);
                userName = u.Name;
                userSurname = u.Surname;
                userDepartment = u.Department;
            }

            // create Rental Widget
            RentalWidget* rentalWidget = new RentalWidget();
            rentalWidget->setMainWindow(this);
            rentalWidget->setData(userName, userSurname, userDepartment, comment);
            rentalWidget->setStartDate(dateBegin);
            rentalWidget->setEndDate(dateEnd);
            ui->scrollAreaLayout_rentalOverview->addWidget(rentalWidget);


            // get items from ItemList
            for (const QString &itemID : qAsConst(itemList))
            {
                QSqlQuery itemQuery;
                itemQuery.prepare("SELECT * FROM Inventory WHERE ObjectID = :itemid");
                itemQuery.bindValue(":itemid", itemID);
                if(!itemQuery.exec())
                {
                    QMessageBox::critical(this, "SQL-Error: ", itemQuery.lastError().text());
                }
                else
                {
                    itemQuery.next();
                    MainWindow::Item i = getItemFromDatabase(itemQuery);
                    QString filename = dataDirectory + "img/" + i.ObjectID + ".jpg";
                    QImage image = loadImageFile(filename);

                    rentalWidget->addItem(i.ObjectName, i.ObjectID, i.Manufacturer, i.Description, image, itemListReturn.contains(itemID) || i.MarkedAsRemoved);

                }
            }
        }
    }
}

void MainWindow::clearRentalOverview()
{
    if(ui->scrollAreaLayout_rentalOverview->count() <= 0) return;

    QLayoutItem* child;
    while( (child = ui->scrollAreaLayout_rentalOverview->takeAt(0)) != 0) {
        child->widget()->setHidden(true);
        child->widget()->deleteLater();
    }
}

void MainWindow::on_lineEdit_RentalOverview_textChanged()
{
    userID = -1;
    QString searchString = ui->lineEdit_RentalOverview->text();
    clearRentalOverview();
    rentalFilterList.clear();

    if (searchString == "")
    {
        loadRentalOverview(rentalFilterList);
        return;
    }

    QRegExp re("\\d*");  // a digit (\d), zero or more times (*)
    if (re.exactMatch(searchString))
    {
        rentalFilterList = searchByItemID(searchString);
    }
    else
    {
        rentalFilterList = searchByUser(searchString);
    }


    if (rentalFilterList.count() == 0)
        return;

    loadRentalOverview(rentalFilterList);
}

QList<int> MainWindow::searchByUser(QString searchString)
{
    QList<int> rentalList;
    QSqlQuery userQuery;
    userQuery.prepare("SELECT * FROM Users WHERE Name LIKE :user ORDER BY Name");
    userQuery.bindValue(":user", searchString + "%");
    if(!userQuery.exec())
    {
        QMessageBox::critical(this, "SQL-Error: ", userQuery.lastError().text());
    }
    else
    {
        while(userQuery.next()) {
            MainWindow::User u = getUserFromDatabase(userQuery);

            // Search Rentals for UserIDs
            QSqlQuery rentalQuery;
            rentalQuery.prepare("SELECT * FROM Rentals WHERE UserID = :userid");
            rentalQuery.bindValue(":userid", u.ID);
            if(!rentalQuery.exec())
            {
                QMessageBox::critical(this, "SQL-Error: ", rentalQuery.lastError().text());
            }
            else
            {
                while(rentalQuery.next()) {
                    rentalList.append(rentalQuery.value("ID").toInt());
                }
            }
        }
    }
    return rentalList;
}

QList<int> MainWindow::searchByItemID(QString searchString)
{
    QList<int> rentalList;
    QSqlQuery itemQuery;
    itemQuery.prepare("SELECT * FROM Rentals WHERE Itemlist LIKE :itemid");

    itemQuery.bindValue(":itemid", "%"+searchString+"%");
    if(!itemQuery.exec())
    {
        QMessageBox::critical(this, "SQL-Error: ", itemQuery.lastError().text());
    }
    else
    {
        while(itemQuery.next())
        {
            MainWindow::Rental r = getRentalFromDatabase(itemQuery);
            rentalList.append(r.ID);
        }
    }
    return rentalList;
}

void MainWindow::returnItemFromRental(QString objectID)
{
    int rentalID = getRentalIDfromObjectID(objectID);
    if(rentalID < 0) return;

    QSqlQuery query;
    query.prepare("SELECT * FROM Rentals WHERE ID=:rentalid");
    query.bindValue(":rentalid", rentalID);

    QStringList returnItems;
    if(!query.exec())
    {
        QMessageBox::critical(this, "SQL-Error: ", query.lastError().text());
    }
    else
    {
        query.next();
        MainWindow::Rental r = getRentalFromDatabase(query);
        r.ItemlistReturn.append(objectID);
        r.ItemlistReturn.sort();

        query.prepare("UPDATE Rentals SET ItemlistReturn=:itemlistreturn WHERE ID=:rentalid");
        query.bindValue(":rentalid", rentalID);
        query.bindValue(":itemlistreturn", r.ItemlistReturn.join(","));
        if(!query.exec())
        {
            QMessageBox::critical(this, "SQL-Error: ", query.lastError().text());
        }
        else
        {
            ui->statusbar->showMessage("👍 Nice, thanks!", statusBarTimeout);
        }
    }
}

int MainWindow::getRentalIDfromObjectID(QString objectID)
{
    QList<int> rentalIDs;

    for (MainWindow::Rental r : qAsConst(allRentals))
    {
        if(r.Itemlist.contains(objectID) && !r.ItemlistReturn.contains(objectID))
        {
            rentalIDs.append(r.ID);
        }
    }

    if(rentalIDs.count() != 1)
    {
        QMessageBox::critical(this, "Database-Error: ", "multiple entries with ObjectID #" + objectID + " found in Rentals-Database");
        return -1;
    }
    return rentalIDs.at(0);
}

void MainWindow::loadAllRentals()
{
    allRentals.clear();

    QSqlQuery itemQuery;
    itemQuery.prepare("SELECT * FROM Rentals");

    if(!itemQuery.exec())
    {
        QMessageBox::critical(this, "SQL-Error: ", itemQuery.lastError().text());
    }
    else
    {
        while(itemQuery.next())
        {
            MainWindow::Rental r = getRentalFromDatabase(itemQuery);
            allRentals.append(r);
        }
    }

}

// Tab "Show Inventory"
// =========================
void MainWindow::on_pushButton_overviewInventory_Reload_clicked()
{
    clearInventoryOverview();

    QSqlQuery query("SELECT * FROM Inventory");
    uint16_t j = 0;
    while(query.next())
    {
        MainWindow::Item i = getItemFromDatabase(query);

        InventoryWidget* inventorywidget = new InventoryWidget();
        inventorywidget->setItemName(QString(i.Manufacturer + " " + i.ObjectName));
        inventorywidget->setItemID(i.ObjectID);
        inventorywidget->setItemDescription(i.Description);
        QString filename = dataDirectory + "img/" + i.ObjectID + ".jpg";
        QImage image = loadImageFile(filename);
        if(image.data_ptr() != NULL) {
            inventorywidget->setImage(image);
        }
        if(j%2==0) inventorywidget->setBackgroundDark();
        j++;
        if(i.MarkedAsRemoved)
        {
            inventorywidget->setDisabled(true);
        }
        inventorywidget->setMainWindow(this);

        ui->scrollAreaLayout->addWidget(inventorywidget);
    }
}

void MainWindow::on_pushButton_temp_clicked() {
    clearInventoryOverview();
}

void MainWindow::clearInventoryOverview()
{
    if(ui->scrollAreaLayout->count() <= 0) return;

    QLayoutItem* child;
    while( (child = ui->scrollAreaLayout->takeAt(0)) != 0) {
        child->widget()->setHidden(true);
        child->widget()->deleteLater();
    }
}

void MainWindow::deleteItemFromInventory(QString ID)
{
    QSqlQuery query;
    query.prepare("UPDATE Inventory SET MarkedAsRemoved=1 WHERE ObjectID = :objectid");
    query.bindValue(":objectid", ID);

    if(!query.exec())
    {
        QMessageBox::critical(this, "SQL-Error", query.lastError().text());
    }
    on_pushButton_overviewInventory_Reload_clicked();
}


// Tab "Add Inventory"
// =========================
//void MainWindow::on_pushButton_takePicture_clicked()
//{
//    takeImage();
//}

//void MainWindow::on_pushButton_loadPicture_clicked()
//{
//    QString filename = QFileDialog::getOpenFileName(this, "Open file", "/home", "Images(*.jpg *.png *.tif)");
//    loadImageFile(filename);
//}

void MainWindow::on_pushButton_inventorySave_clicked()
{
    if (ui->lineEdit_objectName->text() == ""){
        ui->statusbar->showMessage("👎 Not cool. Please name the object.", statusBarTimeout);
        return;
    }
    else if (ui->lineEdit_objectID->text() == ""){
        ui->statusbar->showMessage("👎 Not cool. No Object ID given.", statusBarTimeout);
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO Inventory (ObjectName, ObjectID, Manufacturer, StorageRoom, Description) VALUES (:objectname, :objectid, :manufacturer, :storageroom, :description)");

    query.bindValue(":objectname", ui->lineEdit_objectName->text() );
    query.bindValue(":objectid", ui->lineEdit_objectID->text() );
    query.bindValue(":manufacturer", ui->lineEdit_objectManufacturer->text() );
    query.bindValue(":description", ui->lineEdit_itemDescription->text());
    query.bindValue(":storageroom", ui->comboBox_storageRoom->currentText() );

    if(query.exec())
    {
        takeImage();
        ui->statusbar->showMessage("👍 Supercool! Inventory is stored 'securely'.", statusBarTimeout);
    }
    else if(query.lastError().nativeErrorCode() == "19")
    {
        QMessageBox::critical(this, "💥 Error", "💥 Error: ObjectID already in use.");
    }
    else
    {
        QMessageBox::critical(this, "💥 SQL-Error: ", query.lastError().text());
    }
}

void MainWindow::on_pushButton_inventoryClear_clicked()
{
    ui->lineEdit_objectName->setText("");
    ui->lineEdit_objectManufacturer->setText("");
    ui->lineEdit_itemDescription->setText("");
    ui->lineEdit_objectID->setText("");
}





// methods for camera:
void MainWindow::setCamera(const QCameraInfo &cameraInfo)
{
    qDebug() << "used camera: " << cameraInfo.description();
    m_camera.reset(new QCamera(cameraInfo));

    m_imageCapture.reset(new QCameraImageCapture(m_camera.data()));


    m_camera->setViewfinder(ui->viewfinder);

    connect(m_imageCapture.data(), &QCameraImageCapture::readyForCaptureChanged, this, &MainWindow::readyForCapture);
    connect(m_imageCapture.data(), &QCameraImageCapture::imageCaptured, this, &MainWindow::processCapturedImage);
    connect(m_imageCapture.data(), &QCameraImageCapture::imageSaved, this, &MainWindow::imageSaved);
    connect(m_imageCapture.data(), QOverload<int, QCameraImageCapture::Error, const QString&>::of(&QCameraImageCapture::error),this, &MainWindow::displayCaptureError);

    m_camera->start();

    m_imageSettings.setResolution(1920,1080);
    m_imageSettings.setQuality(QMultimedia::HighQuality);
    m_imageCapture->setEncodingSettings(m_imageSettings);

}

void MainWindow::readyForCapture(bool ready)
{
    ui->pushButton_inventorySave->setEnabled(ready);
}

void MainWindow::processCapturedImage(int requestId, const QImage& img)
{

}

void MainWindow::imageSaved(int id, const QString &fileName)
{
    //ui->label_InventoryAdd_Info->setText("saved as " + fileName);
    //loadImageFile(fileName);
}

void MainWindow::displayCaptureError(int id, const QCameraImageCapture::Error error, const QString &errorString)
{
    QMessageBox::critical(this, "Error", "capture error" + errorString);
}

void MainWindow::takeImage()
{
    m_imageCapture->capture(dataDirectory + "img/" + ui->lineEdit_objectID->text() + ".jpg");
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData* mimeData = event->mimeData();

    if(mimeData->hasUrls())
    {
        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();
        if(urlList.size() > 0) {
            QString filepath = urlList[0].toLocalFile();

            //ui->label_filepathPicture->setText(filepath);
        }
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}



// others:
QImage MainWindow::loadImageFile(const QString &filename)
{
    QImageReader imageReader(filename);
    const QImage image = imageReader.read();
    return image;
}

//void MainWindow::setImage(const QImage &newImage)
//{
//    itemImage = newImage;
//    ui->label_Image->setPixmap(QPixmap::fromImage(itemImage));
//}



// unsorted:


void MainWindow::on_pushButton_openDialog_clicked()
{
    QMessageBox::information(this, "info", "a dialog!");
}

void MainWindow::createButton()
{
    //QMessageBox::information(this, "info", "dialog from dynamic signal-slot");
    QPushButton* myButton = new QPushButton("test", this);
    myButton->setText("dialog box");
    connect(myButton, SIGNAL(clicked()), this, SLOT(showMessage()));
    //ui->horizontalLayout->addWidget(myButton);
}

void MainWindow::showMessage()
{
    QMessageBox::information(this, "info2", "dialog from dynamic button");
}



