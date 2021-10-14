#include <QMessageBox>
#include <QtSql>
#include <QCameraInfo>
#include <QMimeData>
#include <QFileDialog>
#include <QImage>
#include <QImageReader>

//#include <QCameraViewfinder>
//#include <QMediaMetaData>

#include <QStyleFactory>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "InventoryWidget.h"
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
    setCamera(QCameraInfo::defaultCamera());
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
    m_imageSettings.setResolution(1920,1080);
    m_imageSettings.setQuality(QMultimedia::HighQuality);
    m_imageCapture->setEncodingSettings(m_imageSettings);


    connect(ui->pushButton_openDialog_2, SIGNAL(clicked()), this, SLOT(createButton()));
    //connect(ui->pushButton_inventorySave, SIGNAL(clicked()), this, SLOT(saveInventoryToDB()));

    ui->comboBox_storageRoom->addItem("1.74");
    ui->comboBox_storageRoom->addItem("Ladenlokal");
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

void MainWindow::on_tabWidget_tabBarClicked(int i)
{
    qDebug() << i;
    if(i == 5)
    {
        // turn camera on
        setCamera(QCameraInfo::defaultCamera());
    }
    else {
        m_camera->stop();
    }
}

// Tab "Rental"
// =========================

// Tab "Reservation"
// =========================

// Tab "Return"
// =========================

// Tab "Overview"
// =========================

// Tab "Show Inventory"
// =========================



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
        ui->label_InventoryAdd_Info->setText("👎 Not cool. Please name the object.");
        return;
    }
    else if (ui->lineEdit_objectID->text() == ""){
        ui->label_InventoryAdd_Info->setText("👎 Not cool. No Object ID given.");
        return;
    }

    takeImage();

    QSqlQuery query;
    query.prepare("INSERT INTO Inventory (ObjectName, ObjectID, Manufacturer, StorageRoom) VALUES (:objectname, :objectid, :manufacturer, :storageroom)");

    query.bindValue(":objectname", ui->lineEdit_objectName->text() );
    query.bindValue(":objectid", ui->lineEdit_objectID->text() );
    query.bindValue(":manufacturer", ui->lineEdit_objectManufacturer->text() );
    query.bindValue(":storageroom", ui->comboBox_storageRoom->currentText() );

    if(query.exec())
    {
        //QMessageBox::information(this, "Inventar", "Inventar sicher gespeichert.");
        ui->label_InventoryAdd_Info->setText("👍 Supercool! Inventory is stored 'securely'.");
    }
    else if(query.lastError().nativeErrorCode() == "19")
    {
        ui->label_InventoryAdd_Info->setText("💥 Error: ObjectID already in use.");
        //QMessageBox::critical(this, "Inventar", "Fehler: ObjectID bereits vergeben.");
    }
    else
    {
        ui->label_InventoryAdd_Info->setText("💥 Error: " + query.lastError().text());
        //QMessageBox::critical(this, "Inventar", query.lastError().text());
    }
}

void MainWindow::on_pushButton_inventoryClear_clicked()
{
    ui->lineEdit_objectName->setText("");
    ui->lineEdit_objectManufacturer->setText("");
    ui->lineEdit_objectID->setText("");
    ui->label_InventoryAdd_Info->setText("");
}

void MainWindow::on_lineEdit_objectName_textChanged()
{
    qDebug()<< "change";
    ui->label_InventoryAdd_Info->setText("");
}

void MainWindow::on_lineEdit_objectManufacturer_textChanged()
{
    qDebug()<< "change";
    ui->label_InventoryAdd_Info->setText("");
}

void MainWindow::on_lineEdit_objectID_textChanged()
{
    qDebug()<< "change";
    ui->label_InventoryAdd_Info->setText("");
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
    ui->label_InventoryAdd_Info->setText("capture error" + errorString);

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
//void MainWindow::loadImageFile(const QString &filename)
//{
//    QImageReader imageReader(filename);
//    const QImage image = imageReader.read();
//    QFileInfo qi(filename);
//    imageFilename = qi.fileName();
//    setImage(image);
//}

//void MainWindow::setImage(const QImage &newImage)
//{
//    itemImage = newImage;
//    ui->label_Image->setPixmap(QPixmap::fromImage(itemImage));
//}



// unsorted:

void MainWindow::on_pushButton_overviewInventory_Reload_clicked()
{
    clearInventoryOverview();

    QSqlQuery query("SELECT * FROM Inventory");

    while(query.next())
    {
        QString objectName = query.value("ObjectName").toString();
        QString objectID = query.value("ObjectID").toString();
        QString manufacturer = query.value("Manufacturer").toString();
        QString storageRoom = query.value("StorageRoom").toString();
        qDebug() << objectName << objectID << manufacturer << storageRoom;

        InventoryWidget* inventorywidget = new InventoryWidget();
        inventorywidget->setItemName(objectName);
        inventorywidget->setItemID(objectID);
        inventorywidget->setItemManufacturer(manufacturer);
        inventorywidget->setMainWindow(this);

        ui->scrollAreaLayout->addWidget(inventorywidget);
    }

}

void MainWindow::on_pushButton_openDialog_clicked()
{
    QMessageBox::information(this, "info", "beer is empty");
}

void MainWindow::createButton()
{
    //QMessageBox::information(this, "info", "dialog from dynamic signal-slot");
    QPushButton* myButton = new QPushButton("test", this);
    myButton->setText("dialog box");
    connect(myButton, SIGNAL(clicked()), this, SLOT(showMessage()));
    ui->horizontalLayout->addWidget(myButton);
}

void MainWindow::showMessage()
{
    QMessageBox::information(this, "info2", "dialog from dynamic button");
}

void MainWindow::clearInventoryOverview()
{
    qDebug() << ui->scrollAreaLayout->count();
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

void MainWindow::on_pushButton_temp_clicked() {
    clearInventoryOverview();
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
        QMessageBox::critical(this, "Error", query.lastError().text());
    }
}
