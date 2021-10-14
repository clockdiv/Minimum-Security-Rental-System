#include <QDebug>
#include "InventoryWidget.h"

#include "ui_InventoryWidget.h"
#include "mainwindow.h"

InventoryWidget::InventoryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InventoryWidget)
{
    ui->setupUi(this);
}


InventoryWidget::~InventoryWidget()
{

}

void InventoryWidget::setItemName(const QString& name)
{
    ui->label_inventoryItem_name->setText(name);
}

void InventoryWidget::setItemID(const QString& id)
{
    ui->label_inventoryItem_ID->setText(id);
}

void InventoryWidget::setItemManufacturer(const QString& manufacturer)
{
    ui->label_inventoryItem_manufacturer->setText(manufacturer);
}

void InventoryWidget::on_pushButton_delete_clicked()
{
    QString objectID = ui->label_inventoryItem_ID->text();
    mainWindow->deleteItemFromInventory(objectID);

}
