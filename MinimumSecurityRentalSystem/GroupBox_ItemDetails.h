#ifndef GROUPBOX_ITEMDETAILS_H
#define GROUPBOX_ITEMDETAILS_H

#include <QWidget>
#include "mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class GroupBox_ItemDetails; }
QT_END_NAMESPACE


class GroupBox_ItemDetails : public QWidget
{
    Q_OBJECT
public:
    GroupBox_ItemDetails(QWidget *parent = nullptr);
    ~GroupBox_ItemDetails();

private:
    Ui::GroupBox_ItemDetails *ui;
    //MainWindow *mainWindow;
};

#endif // GROUPBOX_ITEMDETAILS_H





