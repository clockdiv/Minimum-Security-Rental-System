#include "GroupBox_ItemDetails.h"
#include "ui_GroupBox_ItemDetails.h"
#include "mainwindow.h"

GroupBox_ItemDetails::GroupBox_ItemDetails(QWidget *parent):
    QWidget(parent),
    ui (new Ui::GroupBox_ItemDetails)
{
    ui->setupUi(this);
}

GroupBox_ItemDetails::~GroupBox_ItemDetails(){}

