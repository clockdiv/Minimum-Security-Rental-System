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

InventoryWidget::~InventoryWidget(){}

void InventoryWidget::on_pushButton_delete_clicked()
{
    QString objectID = ui->label_inventoryItem_ID->text();
    mainWindow->deleteItemFromInventory(objectID);
    this->deleteLater();
}

void InventoryWidget::setItemName(const QString& name)
{
    ui->label_inventoryItem_name->setText(name);
}

void InventoryWidget::setItemID(const QString& id)
{
    ui->label_inventoryItem_ID->setText(id);
}

void InventoryWidget::setItemDescription(const QString &description)
{
    ui->label_inventoryItem_description->setText(description);
}

void InventoryWidget::setImage(const QImage& image)
{
    ui->label_itemImage->setPixmap(QPixmap::fromImage(image));
}

void InventoryWidget::setBackgroundDark()
{
    QString qss = "background-color: rgb(28, 28, 28);";
    this->setStyleSheet(qss);
}

void InventoryWidget::setMainWindow(MainWindow* mw)
{
    mainWindow = mw;
}
