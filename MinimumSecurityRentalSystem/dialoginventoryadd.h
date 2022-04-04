#ifndef DIALOGINVENTORYADD_H
#define DIALOGINVENTORYADD_H

#include <QDialog>
#include <QDebug>
#include <QMessageBox>
#include <QCamera>
#include <QCameraInfo>
#include <QCameraImageCapture>


namespace Ui {
    class DialogInventoryAdd;
}

struct InventoryObject {
    QString manufacturer;
    QString name;
    QString barcode;
    QString accessoires;
    QString description;
    QString storageRoom;
};

class DialogInventoryAdd : public QDialog
{
    Q_OBJECT

public:
    explicit DialogInventoryAdd(QWidget *parent = nullptr);
    ~DialogInventoryAdd();
    void takeImage();
    QString* dataDirectory;


private:
    Ui::DialogInventoryAdd *ui;

    QScopedPointer<QCamera> m_camera;
    QScopedPointer<QCameraImageCapture> m_imageCapture;
    QImageEncoderSettings m_imageSettings;

    void setCamera(const QCameraInfo &cameraInfo);
    void readyForCapture(bool ready);
    void displayCaptureError(int id, const QCameraImageCapture::Error error, const QString &errorString);

signals:
    void addInventoryClicked(const InventoryObject &object);

private slots:
    void on_pushButton_Add_clicked();
    void on_pushButton_Clear_clicked();
    void on_pushButton_Close_clicked();
};

#endif // DIALOGINVENTORYADD_H
