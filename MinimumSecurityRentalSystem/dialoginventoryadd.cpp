#include "dialoginventoryadd.h"
#include "ui_dialoginventoryadd.h"

DialogInventoryAdd::DialogInventoryAdd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogInventoryAdd)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    ui->comboBox_storageRoom->addItem("1.74");
    ui->comboBox_storageRoom->addItem("Ladenlokal");

    setCamera(QCameraInfo::defaultCamera());
}

DialogInventoryAdd::~DialogInventoryAdd()
{
    delete ui;
}

void DialogInventoryAdd::on_pushButton_Add_clicked()
{
    if (ui->lineEdit_objectName->text() == ""){
        QMessageBox::information(this, "Nope...", "Please enter a Object Name.");
        return;
    }
    else if (ui->lineEdit_objectBarcode->text() == ""){
        QMessageBox::information(this, "Nope...", "Please scan a Barcode.");
        return;
    }

    InventoryObject io;
    io.manufacturer = ui->lineEdit_objectManufacturer->text();
    io.name = ui->lineEdit_objectName->text();
    io.barcode = ui->lineEdit_objectBarcode->text();
    io.accessoires = ui->lineEdit_objectAccessoires->text();
    io.description = ui->lineEdit_objectDescription->text();
    io.storageRoom = ui->comboBox_storageRoom->currentText();
    emit addInventoryClicked(io);

}

void DialogInventoryAdd::on_pushButton_Clear_clicked()
{
    ui->lineEdit_objectManufacturer->setText("");
    ui->lineEdit_objectName->setText("");
    ui->lineEdit_objectBarcode->setText("");
    ui->lineEdit_objectAccessoires->setText("");
    ui->lineEdit_objectDescription->setText("");
    ui->comboBox_storageRoom->setCurrentIndex(0);
}

void DialogInventoryAdd::on_pushButton_Close_clicked()
{
    if(!m_camera.isNull())
        m_camera->stop();
    this->close();
}

void DialogInventoryAdd::setCamera(const QCameraInfo &cameraInfo)
{
    qDebug() << "setCamera" << cameraInfo.description();
    m_camera.reset(new QCamera(cameraInfo));

    m_imageCapture.reset(new QCameraImageCapture(m_camera.data()));
    m_imageSettings.setResolution(1920,1080);
    m_imageSettings.setQuality(QMultimedia::HighQuality);
    m_imageCapture->setEncodingSettings(m_imageSettings);
//  m_imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);


    m_camera->setViewfinder(ui->viewfinder);

    connect(m_imageCapture.data(), &QCameraImageCapture::readyForCaptureChanged, this, &DialogInventoryAdd::readyForCapture);
//  connect(m_imageCapture.data(), &QCameraImageCapture::imageCaptured, this, &MainWindow::processCapturedImage);
//  connect(m_imageCapture.data(), &QCameraImageCapture::imageSaved, this, &MainWindow::imageSaved);
    connect(m_imageCapture.data(), QOverload<int, QCameraImageCapture::Error, const QString&>::of(&QCameraImageCapture::error),this, &DialogInventoryAdd::displayCaptureError);
//  connect(m_imageCapture.data(), &QCameraImageCapture::imageAvailable, this, &MainWindow::imageAvailable);

    m_camera->start();
}

void DialogInventoryAdd::readyForCapture(bool ready)
{
    ui->pushButton_Add->setEnabled(ready);
}

void DialogInventoryAdd::displayCaptureError(int id, const QCameraImageCapture::Error error, const QString &errorString)
{
    Q_UNUSED(id);
    Q_UNUSED(error);
    QMessageBox::critical(this, "Error", "capture error" + errorString);
}

void DialogInventoryAdd::takeImage()
{
    qDebug() << "takeImage and save as " << *dataDirectory + "/img/" + ui->lineEdit_objectBarcode->text() + ".jpg";
    m_imageCapture->capture(*dataDirectory + "/img/" + ui->lineEdit_objectBarcode->text() + ".jpg");
//  m_imageCapture->capture();

}

