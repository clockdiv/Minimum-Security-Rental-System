#ifndef DIALOGINVENTORYADD_H
#define DIALOGINVENTORYADD_H

#include <QDialog>
#include <QDebug>

namespace Ui {
class DialogInventoryAdd;
}

class DialogInventoryAdd : public QDialog
{
    Q_OBJECT

public:
    explicit DialogInventoryAdd(QWidget *parent = nullptr);
    ~DialogInventoryAdd();

private:
    Ui::DialogInventoryAdd *ui;

private slots:
    void on_pushButton_Close_clicked();
};

#endif // DIALOGINVENTORYADD_H
