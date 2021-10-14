#ifndef INVENTORYWIDGET_H
#define INVENTORYWIDGET_H

#include <QWidget>
#include "mainwindow.h"


QT_BEGIN_NAMESPACE
namespace Ui { class InventoryWidget; }
QT_END_NAMESPACE

class InventoryWidget : public QWidget
{
    Q_OBJECT
public:
    InventoryWidget(QWidget *parent = nullptr);
    ~InventoryWidget();

    void setItemName(const QString& name);
    void setItemID(const QString& id);
    void setItemManufacturer(const QString& manufacturer);
    void setMainWindow(MainWindow* mw){mainWindow = mw;}


private:
    Ui::InventoryWidget *ui;

    MainWindow* mainWindow;

private slots:
    void on_pushButton_delete_clicked();


signals:

};

#endif // INVENTORYWIDGET_H
