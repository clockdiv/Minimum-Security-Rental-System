#include "dialoginventoryadd.h"
#include "ui_dialoginventoryadd.h"

DialogInventoryAdd::DialogInventoryAdd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogInventoryAdd)
{
    ui->setupUi(this);
}

DialogInventoryAdd::~DialogInventoryAdd()
{
    delete ui;
}

void DialogInventoryAdd::on_pushButton_Close_clicked()
{
    qDebug() << "close dialog";
    this->close();
}
