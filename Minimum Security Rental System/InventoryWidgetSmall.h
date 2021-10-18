#ifndef INVENTORYWIDGETSMALL_H
#define INVENTORYWIDGETSMALL_H

#include <QWidget>
#include "mainwindow.h"

namespace Ui {
class InventoryWidgetSmall;
}

class InventoryWidgetSmall : public QWidget
{
    Q_OBJECT

public:
    explicit InventoryWidgetSmall(QWidget *parent = nullptr);
    ~InventoryWidgetSmall();
    void setItemName(const QString& name);
    void setItemID(const QString& id);
    void setItemDescription(const QString& description);
    void setImage(const QImage& image);

    void setBackgroundDark();
    void setMainWindow(MainWindow* mw);
    void removeAddButton();
    void removeRemoveButton();
//    QString getObjectID();


private:
    Ui::InventoryWidgetSmall *ui;
    MainWindow* mainWindow;

private slots:
    void on_pushButton_InventoryItemSmallAdd_clicked();
    void on_pushButton_InventoryItemSmallRemove_clicked();

};

#endif // INVENTORYWIDGETSMALL_H
