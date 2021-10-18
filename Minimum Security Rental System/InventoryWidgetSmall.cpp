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
}

void InventoryWidgetSmall::on_pushButton_InventoryItemSmallRemove_clicked()
{
    this->deleteLater();

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
    ui->label_itemSmallImage->setPixmap(QPixmap::fromImage(image));
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

//QString InventoryWidgetSmall::getObjectID()
//{
//    return this->ui->label_inventoryItemSmall_ID->text();
//}
