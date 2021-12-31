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
    void setReturned();
//    void setEnabled(const bool& enabled);

    void setBackgroundDark();
    void setMainWindow(MainWindow* mw);
    void removeAddButton();
    void removeRemoveButton();
    void removeReturnButton();
//    QString getObjectID();


private:
    Ui::InventoryWidgetSmall *ui;
    MainWindow* mainWindow;
//    bool labelsEnabled;

signals:
    void addItemClicked(const QString& objectID);       // Sender

private slots:
    void on_pushButton_InventoryItemSmallAdd_clicked();
    void on_pushButton_InventoryItemSmallRemove_clicked();
    void on_pushButton_InventoryItemSmallReturn_clicked();




};

#endif // INVENTORYWIDGETSMALL_H
