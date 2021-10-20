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
    switch (tabID)
    {
        case 0:
            if(!m_camera.isNull()) m_camera->stop();
            break;
        case 1:
            if(!m_camera.isNull()) m_camera->stop();
            break;
        case 2:
            if(!m_camera.isNull()) m_camera->stop();
            break;
        case 3:
            if(!m_camera.isNull()) m_camera->stop();
            on_pushButton_overviewInventory_Reload_clicked();
            break;
        case 4:
            setCamera(QCameraInfo::defaultCamera());
            break;
        default:
            break;
    }

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
            nameListDatabaseIDs.insert(nameList.count(), query.value("ID").toInt());
            QString entry = query.value("Name").toString() + " "
                            + query.value("Surname").toString() + ", "
                            + query.value("Department").toString();
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
//    if (searchItem.length() < 1) {
//        return;
//    }

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
            QString objectName = query.value("ObjectName").toString();
            QString objectID = query.value("ObjectID").toString();
            QString manufacturer = query.value("Manufacturer").toString();
            QString description = query.value("Description").toString();
            QString storageRoom = query.value("StorageRoom").toString();
            QString filename = dataDirectory + "img/" + objectID + ".jpg";

            InventoryWidgetSmall* inventoryWidgetSmall = new InventoryWidgetSmall();
            inventoryWidgetSmall->setItemName(QString(manufacturer + " " + objectName));
            inventoryWidgetSmall->setItemID(objectID);
            inventoryWidgetSmall->setItemDescription(description);
            inventoryWidgetSmall->removeRemoveButton();
            QImage image = loadImageFile(filename);
            if(image.data_ptr() != NULL) {
                inventoryWidgetSmall->setImage(image);
            }

            inventoryWidgetSmall->setMainWindow(this);
            ui->scrollAreaLayout_SearchItem->addWidget(inventoryWidgetSmall);

            if(itemRentalList.indexOf(objectID) > -1)
                inventoryWidgetSmall->setDisabled(true);

            lastSearchItemID = objectID;
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
        //uint16_t i = 0;
        while(query.next()) {
            QString objectName = query.value("ObjectName").toString();
            QString objectID = query.value("ObjectID").toString();
            QString manufacturer = query.value("Manufacturer").toString();
            QString description = query.value("Description").toString();
            QString storageRoom = query.value("StorageRoom").toString();
            QString filename = dataDirectory + "img/" + objectID + ".jpg";

            InventoryWidgetSmall* inventoryWidgetSmall = new InventoryWidgetSmall();
            inventoryWidgetSmall->setItemName(QString(manufacturer + " " + objectName));
            inventoryWidgetSmall->setItemID(objectID);
            inventoryWidgetSmall->setItemDescription(description);
            inventoryWidgetSmall->removeAddButton();
            QImage image = loadImageFile(filename);
            if(image.data_ptr() != NULL) {
                inventoryWidgetSmall->setImage(image);
            }
            //if(i%2==0) inventoryWidgetSmall->setBackgroundDark();
            //i++;
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

    QString itemlist = "";
    int itemCount = itemRentalList.count();
    for(int i = 0; i < itemCount; i++) {
        itemlist += itemRentalList.at(i);
        if(i < itemCount - 1) itemlist += ",";
    }

    QSqlQuery query;
    query.prepare("INSERT INTO Rentals (UserID, Itemlist, DateBegin, DateEnd, Comment) VALUES (:userid, :itemlist, :datebegin, :dateend, :comment)");
    query.bindValue(":userid", userID);
    query.bindValue(":itemlist", itemlist);
    query.bindValue(":datebegin", ui->calendarWidget_RentalStart->selectedDate().toString());
    query.bindValue(":dateend", ui->calendarWidget_RentalEnd->selectedDate().toString());
    query.bindValue(":comment", ui->lineEdit_rentalComment->text());

    if(!query.exec()) {
        QMessageBox::critical(this, "SQL-Error: ", query.lastError().text());
    } else {
        ui->statusbar->showMessage("👍 Rental saved to minimum security database.", statusBarTimeout);
    }
}


// Tab "Reservation"
// =========================

// Tab "Return"
// =========================

// Tab "Overview"
// =========================

// Tab "Show Inventory"
// =========================
void MainWindow::on_pushButton_overviewInventory_Reload_clicked()
{
    clearInventoryOverview();

    QSqlQuery query("SELECT * FROM Inventory");
    uint16_t i = 0;
    while(query.next())
    {
        QString objectName = query.value("ObjectName").toString();
        QString objectID = query.value("ObjectID").toString();
        QString manufacturer = query.value("Manufacturer").toString();
        QString description = query.value("Description").toString();
        QString storageRoom = query.value("StorageRoom").toString();
        QString filename = dataDirectory + "img/" + objectID + ".jpg";

        InventoryWidget* inventorywidget = new InventoryWidget();
        inventorywidget->setItemName(QString(manufacturer + " " + objectName));
        inventorywidget->setItemID(objectID);
        inventorywidget->setItemDescription(description);
        QImage image = loadImageFile(filename);
        if(image.data_ptr() != NULL) {
            inventorywidget->setImage(image);
        }
        if(i%2==0) inventorywidget->setBackgroundDark();
        i++;
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

//    for(int i = ui->scrollAreaLayout->count(); i >= 0 ; --i) {
//       ui->scrollAreaLayout->removeItem(ui->scrollAreaLayout->itemAt(i));
//       //delete ui->scrollAreaLayout->itemAt(i);
//    }

    QLayoutItem* child;
    while( (child = ui->scrollAreaLayout->takeAt(0)) != 0) {
        child->widget()->setHidden(true);
        child->widget()->deleteLater();
        //delete child;
    }

//    ui->scrollAreaLayout->update();
//    ui->scrollAreaWidgetContents->updateGeometry();
//    ui->scrollArea_InventoryItems->updateGeometry();
//    ui->tab_inventoryOverview->updateGeometry();
}

void MainWindow::deleteItemFromInventory(QString ID)
{
    qDebug() << "delete Item: " << ID;
    QSqlQuery query;
    query.prepare("DELETE FROM Inventory WHERE ObjectID = :objectid");
    query.bindValue(":objectid", ID);

    if(!query.exec())
    {
        //qDebug() << query.lastError().text();
        QMessageBox::critical(this, "SQL-Error", query.lastError().text());
    }
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

    takeImage();

    QSqlQuery query;
    query.prepare("INSERT INTO Inventory (ObjectName, ObjectID, Manufacturer, StorageRoom, Description) VALUES (:objectname, :objectid, :manufacturer, :storageroom, :description)");

    query.bindValue(":objectname", ui->lineEdit_objectName->text() );
    query.bindValue(":objectid", ui->lineEdit_objectID->text() );
    query.bindValue(":manufacturer", ui->lineEdit_objectManufacturer->text() );
    query.bindValue(":description", ui->lineEdit_itemDescription->text());
    query.bindValue(":storageroom", ui->comboBox_storageRoom->currentText() );

    if(query.exec())
    {
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



