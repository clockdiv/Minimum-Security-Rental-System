#ifndef RENTALWIDGET_H
#define RENTALWIDGET_H

#include <QWidget>

namespace Ui {
class RentalWidget;
}

class RentalWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RentalWidget(QWidget *parent = nullptr);
    ~RentalWidget();
    void setData(const QString& name, const QString& surname, const QString& department, const QString& comment);
    void setStartDate(const QString& date);
    void setEndDate(const QString& date);
    void addItem(const QString& objectName, const QString& objectID, const QString& manufacturer, const QString& description, const QImage& image);

private:
    Ui::RentalWidget *ui;
};

#endif // RENTALWIDGET_H
