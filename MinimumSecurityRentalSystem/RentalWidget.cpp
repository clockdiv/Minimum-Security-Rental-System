#include "RentalWidget.h"
#include "ui_RentalWidget.h"
#include "InventoryWidgetSmall.h"

RentalWidget::RentalWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RentalWidget)
{
    ui->setupUi(this);
}

RentalWidget::~RentalWidget()
{
    delete ui;
}

void RentalWidget::setData(const QString& name, const QString& surname, const QString& department, const QString& comment)
{
    ui->label_rentalWidget_Name->setText(name + " " + surname + ", " + department);
    ui->label_rentalWidget_comment->setText(comment);
}

void RentalWidget::addItem(const QString& objectName, const QString& objectID, const QString& manufacturer, const QString& description, const QImage &image, bool isReturned)
{
    InventoryWidgetSmall* inventorywidget = new InventoryWidgetSmall();
    inventorywidget->setItemName(manufacturer + " " + objectName);
    inventorywidget->setItemID(objectID);
    inventorywidget->setItemDescription(description);
    inventorywidget->setMainWindow(mainWindow);

    if(!image.isNull()) {
        inventorywidget->setImage(image);
    }

    inventorywidget->removeAddButton();
    inventorywidget->removeRemoveButton();

    if(isReturned)
    {
        inventorywidget->setReturned();
    }
    ui->verticalLayout_rentals->addWidget(inventorywidget);
}

void RentalWidget::setStartDate(const QString& date)
{
    ui->label_rentalWidget_rentalStart->setText(date);

}
void RentalWidget::setEndDate(const QString& date)
{
    ui->label_rentalWidget_rentalEnd->setText(date);
}

void RentalWidget::setMainWindow(MainWindow* mw)
{
    mainWindow = mw;
}
