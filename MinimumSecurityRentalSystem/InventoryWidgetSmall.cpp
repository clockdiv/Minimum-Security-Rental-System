#include "InventoryWidgetSmall.h"
#include "ui_InventoryWidgetSmall.h"

InventoryWidgetSmall::InventoryWidgetSmall(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InventoryWidgetSmall)
{
    ui->setupUi(this);
}

InventoryWidgetSmall::~InventoryWidgetSmall()
{
    delete ui;
}

void InventoryWidgetSmall::on_pushButton_InventoryItemSmallAdd_clicked()
{
    QString objectID = ui->label_inventoryItemSmall_ID->text();
    mainWindow->addItemToRental(objectID);
    this->setDisabled(true);
}

void InventoryWidgetSmall::on_pushButton_InventoryItemSmallRemove_clicked()
{
    QString objectID = ui->label_inventoryItemSmall_ID->text();
    mainWindow->removeItemFromRental(objectID);
    this->deleteLater();
}

void InventoryWidgetSmall::on_pushButton_InventoryItemSmallReturn_clicked()
{
    QString objectID = ui->label_inventoryItemSmall_ID->text();
    mainWindow->returnItemFromRental(objectID);
    setReturned();
}

void InventoryWidgetSmall::setReturned()
{
    ui->label_itemSmallImage->setDisabled(true);
    ui->label_inventoryItemSmall_ID->setDisabled(true);
    ui->label_inventoryItemSmall_description->setDisabled(true);
    ui->label_inventoryItemSmall_name->setDisabled(true);
    ui->pushButton_InventoryItemSmallReturn->setDisabled(true);
}

void InventoryWidgetSmall::setItemName(const QString& name)
{
    ui->label_inventoryItemSmall_name->setText(name);
}

void InventoryWidgetSmall::setItemID(const QString& id)
{
    ui->label_inventoryItemSmall_ID->setText(id);
}

void InventoryWidgetSmall::setItemDescription(const QString &description)
{
    ui->label_inventoryItemSmall_description->setText(description);
}

void InventoryWidgetSmall::setImage(const QImage& image)
{
    ui->label_itemSmallImage->setPixmap(QPixmap::fromImage(image).scaledToHeight(ui->label_itemSmallImage->height()));
}

void InventoryWidgetSmall::setBackgroundDark()
{
    QString qss = "background-color: rgb(28, 28, 28);";
    this->setStyleSheet(qss);
}

void InventoryWidgetSmall::setMainWindow(MainWindow* mw)
{
    mainWindow = mw;
}

void InventoryWidgetSmall::removeAddButton()
{
    ui->pushButton_InventoryItemSmallAdd->deleteLater();
}

void InventoryWidgetSmall::removeRemoveButton()
{
    ui->pushButton_InventoryItemSmallRemove->deleteLater();
}

void InventoryWidgetSmall::removeReturnButton()
{
    ui->pushButton_InventoryItemSmallReturn->deleteLater();
}

//QString InventoryWidgetSmall::getObjectID()
//{
//    return this->ui->label_inventoryItemSmall_ID->text();
//}
