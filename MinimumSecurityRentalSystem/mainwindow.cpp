#include <QDebug>
#include <QScrollBar>
#include <QSizePolicy>
#include <QTableView>
#include <QCompleter>
#include <QMessageBox>
#include <QImage>
#include <QImageReader>
#include <QResizeEvent>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rentercompleter.h"
#include "settings.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->dockWidgetRight->setTitleBarWidget(new QWidget());

    loadDatabaseSettings();

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(filename_db);
    if(!m_db.open()) {
        QMessageBox::critical(this, "Database Error", "Database not found. Please select file in Preferences and restart the application.");
    }

    setCalendarDateRange();
    initializeCalendarTable();

    loadInventoryFromDB();
    initializeInventoryTable();

    initializeRentalTable();

    // Init GUI Group "Least Suspicious Renter"
    ui->pushButton_RenterAdd->setText("");
    ui->pushButton_RenterUpdate->setText("");
    ui->label_RenterDatabaseID->setText("");
    ui->pushButton_RenterAdd->setIcon(QIcon(":/icons/renter/icons/renterAdd.png"));
    ui->pushButton_RenterUpdate->setIcon(QIcon(":/icons/renter/icons/renterUpdate.png"));

    // Init GUI Group "Rental"
    rentalStartDate = QDate::currentDate();
    rentalEndDate = QDate::currentDate();
    ui->label_Rental_startDate->setText(rentalStartDate.toString(DATEFORMATREADABLE));
    ui->label_Rental_endDate->setText(rentalEndDate.toString(DATEFORMATREADABLE));

    // Init GUI Group "Item"
    ui->groupBox_item->hide();

    // Init GUI Group "Likely Lost Rental"
    ui->groupBox_rentalBooked->hide();

    // SIGNALS and SLOTS

    // show Item Details when item is clicked in one of the tables
    connect(frozenInventoryTableView, &QTableView::clicked, this, &MainWindow::ItemSelectedInInventory);
    connect(frozenRentalTableView, &QTableView::clicked, this, &MainWindow::ItemSelectedInRental);

    // show Rental Details when a rental-cell is clicked in one of the tables
    connect(ui->inventoryCalendarTableView, &QTableView::clicked, this, &MainWindow::RentalSelectedInInventoryCalendar);
    connect(ui->rentalCalendarTableView, &QTableView::clicked, this, &MainWindow::RentalSelectedInRentalCalendar);


    // move rows when dobule-clicked
    connect(frozenInventoryTableView, &QAbstractItemView::doubleClicked, this, &MainWindow::moveItemToRental);
    connect(frozenRentalTableView, &QAbstractItemView::doubleClicked, this, &MainWindow::moveItemToInventory);

    // Connect tables for synchronous scrolling
    connect(ui->mainCalendarTableView->horizontalScrollBar(), &QAbstractSlider::valueChanged, ui->inventoryCalendarTableView->horizontalScrollBar(), &QAbstractSlider::setValue);
    connect(ui->mainCalendarTableView->horizontalScrollBar(), &QAbstractSlider::valueChanged, ui->rentalCalendarTableView->horizontalScrollBar(), &QAbstractSlider::setValue);

    connect(ui->inventoryCalendarTableView->horizontalScrollBar(), &QAbstractSlider::valueChanged, ui->mainCalendarTableView->horizontalScrollBar(), &QAbstractSlider::setValue);
    connect(ui->inventoryCalendarTableView->horizontalScrollBar(), &QAbstractSlider::valueChanged, ui->rentalCalendarTableView->horizontalScrollBar(), &QAbstractSlider::setValue);

    connect(ui->inventoryCalendarTableView->verticalScrollBar(), &QAbstractSlider::valueChanged, frozenInventoryTableView->verticalScrollBar(), &QAbstractSlider::setValue);
    connect(frozenInventoryTableView->verticalScrollBar(), &QAbstractSlider::valueChanged, ui->inventoryCalendarTableView->verticalScrollBar(), &QAbstractSlider::setValue);

    connect(ui->rentalCalendarTableView->horizontalScrollBar(), &QAbstractSlider::valueChanged, ui->mainCalendarTableView->horizontalScrollBar(), &QAbstractSlider::setValue);
    connect(ui->rentalCalendarTableView->horizontalScrollBar(), &QAbstractSlider::valueChanged, ui->inventoryCalendarTableView->horizontalScrollBar(), &QAbstractSlider::setValue);

    connect(ui->rentalCalendarTableView->verticalScrollBar(), &QAbstractSlider::valueChanged, frozenRentalTableView->verticalScrollBar(), &QAbstractSlider::setValue);
    connect(frozenRentalTableView->verticalScrollBar(), &QAbstractSlider::valueChanged, ui->rentalCalendarTableView->verticalScrollBar(), &QAbstractSlider::setValue);

    // connect tables for synchronous column-resizing --- not used anymore at the moment ---
    // connect(ui->inventoryCalendarTableView->horizontalHeader(), &QHeaderView::sectionResized, ui->mainCalendarTableView->horizontalHeader(), &QHeaderView::resizeSection);
    // connect(ui->inventoryCalendarTableView->horizontalHeader(), &QHeaderView::sectionResized, ui->rentalCalendarTableView->horizontalHeader(), &QHeaderView::resizeSection);

    // days selection for rental start- and end-date
    connect(ui->mainCalendarTableView->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            SLOT(dateSelected(const QItemSelection &, const QItemSelection &)));

    loadUsersfromDB();
}


MainWindow::~MainWindow()
{
    saveWindowSettings();
    m_db.close();
    delete usersModel;
    delete renterNameCompleter;
    delete renterSurnameCompleter;
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updateTableGeometry();
}

void MainWindow::fillRenterDialog(QMap<int, QVariant> items)
{
//    qDebug() << "fill Renter Dialog" << items.count();
    if(items.count() <= 0 ) return;

    // get the string from the completion-suggestion
    QString searchString = items.value(0).toString();
//    qDebug() << searchString;

    // find that string in the original model and get the corresponding row
    int modelRow = usersModel->findItems(searchString, Qt::MatchExactly, 0).at(0)->row();
//    qDebug() << "Row:" << modelRow;

    // fill the lineEdits
//    qDebug() << "User ID in Database:" << usersModel->item(modelRow, 1)->text();
    ui->label_RenterDatabaseID->setText("ID: #" + usersModel->item(modelRow, 1)->text());
    ui->lineEdit_userName->setText(usersModel->item(modelRow, 2)->text());
    ui->lineEdit_userSurname->setText(usersModel->item(modelRow, 3)->text());
    ui->lineEdit_userDepartment->setText(usersModel->item(modelRow, 4)->text());
    ui->lineEdit_userYear->setText(usersModel->item(modelRow, 5)->text());
    ui->lineEdit_userEmail->setText(usersModel->item(modelRow, 6)->text());
}


// Table Methods
// ==============
void MainWindow::setCalendarDateRange()
{
    tableStartDate =  QDate::currentDate().addDays(-30);//QDate::fromString("01-12-2021", "dd-MM-yyyy");
    tableEndDate = QDate::currentDate().addDays(100); // QDate::fromString("31-03-2022", "dd-MM-yyyy");
}

void MainWindow::initializeCalendarTable()
{
    // Create the model for the table
    QStringList calenderItemList = {"Year","Month","Day"};
    calendarModel = new QStandardItemModel(calenderItemList.count(), tableStartDate.daysTo(tableEndDate));

    calendarModel->setVerticalHeaderLabels(calenderItemList);

    // setup calendar model
    QStandardItem* item;
    QList<int> monthSpans, yearSpans;
    QDate previousDate;
    int columnWithTodaysDate= -1;

    int i =0;
    for (QDate d = tableStartDate; d < tableEndDate; d = d.addDays(1), i++)
    {

        item = new QStandardItem();
        item->setText(d.toString("dd"));
        item->setData(d, Qt::UserRole+1);
        item->setTextAlignment(Qt::AlignCenter);
        if (d == QDate::currentDate()) {        // today?
            columnWithTodaysDate = i;
            item->setBackground(Qt::green);
        }
        if (d.dayOfWeek() == 6) item->setBackground(Qt::darkGray); // saturday?
        if (d.dayOfWeek() == 7) item->setBackground(Qt::gray);     // sunday?

        calendarModel->setItem(2,i, item);

        if(previousDate.month() != d.month())
        {
            item = new QStandardItem();
            item->setText(d.toString("MMMM"));
            item->setTextAlignment(Qt::AlignCenter);
            calendarModel->setItem(1,i, item);
            monthSpans.append(i);
        }

        if(previousDate.year() != d.year())
        {
            item = new QStandardItem();
            item->setText(d.toString("yyyy"));
            item->setTextAlignment(Qt::AlignCenter);
            calendarModel->setItem(0,i, item);
            yearSpans.append(i);
        }

        previousDate = d;
    }

    // connect model to calendar tableview
    ui->mainCalendarTableView->setModel(calendarModel);

    // set Year Spans
    for(i = 0; i < yearSpans.count()-1; i++) {
        ui->mainCalendarTableView->setSpan(0, yearSpans.at(i), 1, yearSpans.at(i + 1) - yearSpans.at(i));
    }
    ui->mainCalendarTableView->setSpan(0, yearSpans.last(), 1, calendarModel->columnCount());

    // set Month Spans
    for(i = 0; i < monthSpans.count()-1; i++) {
        ui->mainCalendarTableView->setSpan(1, monthSpans.at(i), 1, monthSpans.at(i + 1) - monthSpans.at(i));
    }
    ui->mainCalendarTableView->setSpan(1, monthSpans.last(), 1, calendarModel->columnCount());

    // customize calendar tableview
    ui->mainCalendarTableView->horizontalHeader()->hide();
    ui->mainCalendarTableView->verticalHeader()->hide();
    ui->mainCalendarTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->mainCalendarTableView->setFocusPolicy(Qt::NoFocus);
    //ui->mainCalendarTableView->setSelectionMode(QAbstractItemView::NoSelection);
    //ui->mainCalendarTableView->verticalHeader()->setFixedWidth(ui->itemsTableView->verticalHeader()->width());

    qDebug() << "Column with today's date:" << columnWithTodaysDate;

    // Set Delegate
//    tableItemDelegate = new TableItemDelegate(this);
//    ui->mainCalendarTableView->setItemDelegate(tableItemDelegate);
}

void MainWindow::updateTableGeometry()
{
    if(!frozenInventoryTableView) return;

    qDebug() << "updateTableGeometry";
    ui->inventoryCalendarTableView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->rentalCalendarTableView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->mainCalendarTableView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    ui->inventoryCalendarTableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->rentalCalendarTableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    frozenInventoryTableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    frozenRentalTableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    // 0) InventoryCalendarView
    ui->inventoryCalendarTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents); // 1) set header coluumns width to content....
    ui->inventoryCalendarTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    for(int i = 2; i < inventoryModel->columnCount(); i++) {
        ui->inventoryCalendarTableView->setColumnWidth(i, 30);
    }
    ui->inventoryCalendarTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed); // 2) ...keep header coluumns width
    ui->inventoryCalendarTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);

    // 1) MainCalendarView
    for(int i = 0; i < calendarModel->columnCount(); i++) {
        ui->mainCalendarTableView->setColumnWidth(i, ui->inventoryCalendarTableView->columnWidth(i+2));   // calendar columns width
    }
    ui->mainCalendarTableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);   // set row height
    ui->mainCalendarTableView->setMaximumHeight(ui->mainCalendarTableView->rowHeight(0) * calendarModel->rowCount()+ ui->mainCalendarTableView->frameWidth() * 2);


    // 2) RentalCalendarView
    ui->rentalCalendarTableView->setColumnWidth(0, ui->inventoryCalendarTableView->columnWidth(0));     // header columns width
    ui->rentalCalendarTableView->setColumnWidth(1, ui->inventoryCalendarTableView->columnWidth(1));
    for(int i = 0; i < rentalModel->columnCount(); i++) {
        ui->rentalCalendarTableView->setColumnWidth(i, ui->inventoryCalendarTableView->columnWidth(i));    // calendar columns width                                         //
    }


    // 3) inventory fixed table
    frozenInventoryTableView->setColumnWidth(0, ui->inventoryCalendarTableView->columnWidth(0));
    frozenInventoryTableView->setColumnWidth(1, ui->inventoryCalendarTableView->columnWidth(1));

    frozenInventoryTableView->setGeometry(0,0,
                                    frozenInventoryTableView->columnWidth(0) + frozenInventoryTableView->columnWidth(1) + frozenInventoryTableView->frameWidth() * 2,
                                    ui->inventoryCalendarTableView->height() );

    // 4) rental fixed table
    frozenRentalTableView->setColumnWidth(0, ui->rentalCalendarTableView->columnWidth(0));
    frozenRentalTableView->setColumnWidth(1, ui->rentalCalendarTableView->columnWidth(1));
    frozenRentalTableView->setGeometry(0,0,
                                    frozenRentalTableView->columnWidth(0) + frozenRentalTableView->columnWidth(1) + frozenRentalTableView->frameWidth() * 2,
                                    ui->rentalCalendarTableView->height() );

    // 5) Resize Logo so that calendar columns fit
    ui->AppLogo->setMinimumWidth(ui->inventoryCalendarTableView->columnWidth(0) + ui->inventoryCalendarTableView->columnWidth(1));

}

void MainWindow::initializeInventoryTable()
{
    // init inventory calendar table
    ui->inventoryCalendarTableView->setModel(inventoryModel);

    ui->inventoryCalendarTableView->horizontalHeader()->hide();
    ui->inventoryCalendarTableView->verticalHeader()->hide();
    //ui->inventoryCalendarTableView->setSelectionMode(QAbstractItemView::NoSelection);
    ui->inventoryCalendarTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->inventoryCalendarTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->inventoryCalendarTableView->show();

    // init inventory fixed table
    frozenInventoryTableView = new QTableView(ui->inventoryCalendarTableView);
    frozenInventoryTableView->setModel(inventoryModel);

    frozenInventoryTableView->horizontalHeader()->hide();
    frozenInventoryTableView->verticalHeader()->hide();
    frozenInventoryTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);  // it's hidden underneath the other scrollbar, still fixes a gui bug
    frozenInventoryTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    frozenInventoryTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    frozenInventoryTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    frozenInventoryTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);


    for(int i = 2; i < inventoryModel->columnCount(); i++)    {
        frozenInventoryTableView->hideColumn(i);
    }
    frozenInventoryTableView->show();
}

void MainWindow::initializeRentalTable()
{
    rentalModel = new QStandardItemModel(0, calendarModel->columnCount());
    ui->rentalCalendarTableView->setModel(rentalModel);

    ui->rentalCalendarTableView->horizontalHeader()->hide();
    ui->rentalCalendarTableView->verticalHeader()->hide();
    ui->rentalCalendarTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->rentalCalendarTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->rentalCalendarTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->rentalCalendarTableView->show();



    // init fixed table for rental
    frozenRentalTableView = new QTableView(ui->rentalCalendarTableView);
    frozenRentalTableView->setModel(rentalModel);

    frozenRentalTableView->horizontalHeader()->hide();
    frozenRentalTableView->verticalHeader()->hide();
    frozenRentalTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    frozenRentalTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    frozenRentalTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    frozenRentalTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    frozenRentalTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->rentalCalendarTableView->viewport()->stackUnder(frozenRentalTableView);
    for(int i = 2; i < rentalModel->columnCount(); i++)
    {
        frozenRentalTableView->hideColumn(i);
    }
    frozenRentalTableView->show();
}


void MainWindow::ItemSelectedInInventory(const QModelIndex &index)
{
    frozenRentalTableView->clearSelection();
    ui->rentalCalendarTableView->clearSelection();
    ui->inventoryCalendarTableView->clearSelection();

    QStandardItem *item = inventoryModel->itemFromIndex(index);
    showItemPreview(item);
}

void MainWindow::ItemSelectedInRental(const QModelIndex &index)
{
    frozenInventoryTableView->clearSelection();
    ui->rentalCalendarTableView->clearSelection();
    ui->inventoryCalendarTableView->clearSelection();

    QStandardItem *item = rentalModel->itemFromIndex(index);
    showItemPreview(item);
}

void MainWindow::showItemPreview(QStandardItem* item)
{
    ui->label_Item_Manufacturer->setText(item->data(Qt::UserRole+1).value<Item>().Manufacturer);
    ui->label_Item_Name->setText(item->data(Qt::UserRole+1).value<Item>().Name);
    ui->label_Item_Description->setText(item->data(Qt::UserRole+1).value<Item>().Description);
    ui->label_Item_Barcode->setText(item->data(Qt::UserRole+1).value<Item>().Barcode);
    ui->label_Item_Accessoires->setText(item->data(Qt::UserRole+1).value<Item>().Accessoires);

    QString imageFilename = dataDirectory + "img/" + item->data(Qt::UserRole+1).value<Item>().Barcode + ".jpg";
    QImage itemImage = loadImage(imageFilename);
    int w = ui->label_Item_Image->width();
    int h = ui->label_Item_Image->height();
    ui->label_Item_Image->setPixmap(QPixmap::fromImage(itemImage).scaled(w, h, Qt::KeepAspectRatio));

    ui->groupBox_rentalBooked->hide();
    ui->groupBox_item->show();
}

void MainWindow::RentalSelectedInInventoryCalendar(const QModelIndex &index)
{
    frozenInventoryTableView->clearSelection();
    frozenRentalTableView->clearSelection();
    ui->rentalCalendarTableView->clearSelection();

    QStandardItem *item = inventoryModel->itemFromIndex(index);
    showRentalPreview(item);
}

void MainWindow::RentalSelectedInRentalCalendar(const QModelIndex &index)
{
    frozenInventoryTableView->clearSelection();
    frozenRentalTableView->clearSelection();
    ui->inventoryCalendarTableView->clearSelection();

    QStandardItem *item = rentalModel->itemFromIndex(index);
    showRentalPreview(item);
}

void MainWindow::showRentalPreview(QStandardItem* item)
{
    if (item->data(Qt::UserRole+1) == QVariant::Invalid) {
        ui->groupBox_item->hide();
        ui->groupBox_rentalBooked->hide();
        return;
    }

    ui->label_Rental_UserID->setText(QString::number(item->data(Qt::UserRole+1).value<Rental>().UserID));
    ui->label_Rental_Comment->setText(item->data(Qt::UserRole+1).value<Rental>().Comment);
    ui->label_Rental_Project->setText(item->data(Qt::UserRole+1).value<Rental>().Project);
    ui->label_Rental_Room->setText(item->data(Qt::UserRole+1).value<Rental>().Room);
    ui->label_Rental_AdditionalItems->setText(item->data(Qt::UserRole+1).value<Rental>().AdditionalItems);

    ui->groupBox_item->hide();
    ui->groupBox_rentalBooked->show();
}


void MainWindow::dateSelected(const QItemSelection &selected, const QItemSelection &deselected)
{
    rentalStartDate = ui->mainCalendarTableView->selectionModel()->selectedIndexes().first().data(Qt::UserRole+1).toDate();
    rentalEndDate = ui->mainCalendarTableView->selectionModel()->selectedIndexes().last().data(Qt::UserRole+1).toDate();

    ui->label_Rental_startDate->setText(rentalStartDate.toString(DATEFORMATREADABLE));
    ui->label_Rental_endDate->setText(rentalEndDate.toString(DATEFORMATREADABLE));
}

void MainWindow::moveItemToRental(const QModelIndex & index)
{
    moveRow(inventoryModel, index.row(), rentalModel);
}

void MainWindow::moveItemToInventory(const QModelIndex & index)
{
    moveRow(rentalModel, index.row(), inventoryModel);
}

void MainWindow::renterNameCompleterActivated(const QModelIndex &index)
{
    // get selected item(s) from the current list of completion-suggestions
    // this should logically be only 1, but somehow always 2 items are returned(?)
    QMap<int, QVariant> completerSelection = renterNameCompleter->completionModel()->itemData(index);
    fillRenterDialog(completerSelection);
}

void MainWindow::renterSurnameCompleterActivated(const QModelIndex &index)
{
    QMap<int, QVariant> completerSelection = renterSurnameCompleter->completionModel()->itemData(index);
    fillRenterDialog(completerSelection);
}

void MainWindow::loadInventoryFromDB()
{
    inventoryModel = new QStandardItemModel(0, calendarModel->columnCount());

    QSqlQuery query("SELECT * FROM Inventory");
    int row = 0;
    if(!query.exec()){
        QMessageBox::critical(this, "SQL-Error", query.lastError().text());
    }
    while(query.next())
    {
        Item item;// = new MainWindow::Item;
        item.ID = query.value("ID").toInt();
        item.Name = query.value("Name").toString();
        item.Manufacturer = query.value("Manufacturer").toString();
        item.Barcode = query.value("Barcode").toString();
        item.Description = query.value("Description").toString();
        item.StorageRoom = query.value("StorageRoom").toString();
        item.Accessoires = query.value("Accessoires").toString();
        item.DateRemoved = query.value("DateRemoved").toString();
        item.Timestamp = query.value("Timestamp").toDate();

        // create the items in the table
        QStandardItem* barcodeItem = new QStandardItem(item.Barcode);
        QVariant itemVariant;
        itemVariant.setValue(item);
        barcodeItem->setData(itemVariant, Qt::UserRole+1);
        inventoryModel->setItem(row, 0, barcodeItem);

        QStandardItem* nameItem = new QStandardItem(item.Manufacturer + " " + item.Name);
        nameItem->setData(itemVariant, Qt::UserRole+1);
        inventoryModel->setItem(row, 1, nameItem);

//        if(item->MarkedAsRemoved)
//        {
//           // disable row?
//        }

        QList<Rental> itemRentals = getRentalsForItem(item.ID);
        for(const Rental &r : qAsConst(itemRentals))
        {
            int columnStart = tableStartDate.daysTo(r.DateBegin);
            int daysCount = r.DateBegin.daysTo(r.DateEnd);
            for(int i = columnStart; i <= columnStart + daysCount; i++)
            {
                QStandardItem* rentalItem = new QStandardItem;
                QVariant rentalVariant;
                rentalVariant.setValue(r);
                rentalItem->setData(rentalVariant, Qt::UserRole+1);
                rentalItem->setBackground(Qt::darkGray);
                rentalItem->setToolTip("UserID: " + QString::number(r.UserID)+ ", Project: " + r.Project);
                inventoryModel->setItem(row, i+2, rentalItem);
            }
        }
        row++;
    }

    qDebug() << "Inventory loaded:" << inventoryModel->rowCount() << "items found";
}

QList<Rental> MainWindow::getRentalsForItem(int itemid)
{
    QList<Rental> results;

    QSqlQuery query;
    // get all rentals where the item is listed:
    query.prepare("SELECT * FROM RentedItems WHERE ItemID=:itemid");
    query.bindValue(":itemid", itemid);
    if(!query.exec()){
        QMessageBox::critical(this, "SQL-Error", query.lastError().text());
    }

    while(query.next()) {
        // iterate over all rentals for this item:
        int rentalID = query.value("RentalID").toInt();

        QSqlQuery rentalsQuery;
        rentalsQuery.prepare("SELECT * FROM Rentals WHERE ID=:id");
        rentalsQuery.bindValue(":id", rentalID);
        if(!rentalsQuery.exec()){
            QMessageBox::critical(this, "SQL-Error", query.lastError().text());
        }
        rentalsQuery.first();   // there should be only one result as rentalID is unique...

        Rental rental;
        rental.ID = rentalsQuery.value("ID").toInt();
        rental.UserID = rentalsQuery.value("UserID").toInt();
        rental.DateBegin = rentalsQuery.value("DateBegin").toDate();
        rental.DateEnd = rentalsQuery.value("DateEnd").toDate();
        rental.Room = rentalsQuery.value("Room").toString();
        rental.Project = rentalsQuery.value("Project").toString();
        rental.AdditionalItems = rentalsQuery.value("AdditionalItems").toString();
        rental.Comment = rentalsQuery.value("Comment").toString();
        rental.Timestamp = rentalsQuery.value("Timestamp").toDate();

        results.append(rental);
    }
    return results;
}

void MainWindow::moveRow(QStandardItemModel* source, int rowIndex, QStandardItemModel* destination)
{
    for(int i = 0; i < source->columnCount(); i++) {
        if(source->item(rowIndex, i))       // copy only cells with content
        {
            QStandardItem* destItem = source->item(rowIndex,i)->clone();
            if(i==0)
                destination->setItem(destination->rowCount(), i, destItem); // create new row
            else
                destination->setItem(destination->rowCount()-1, i, destItem);  // add to existing row
        }
    }

    // sort rental table if needed
    if(destination == inventoryModel)
        inventoryModel->sort(0, Qt::AscendingOrder);

   // remove row from source
    source->removeRow(rowIndex);
}

// User Data Methods
// =================
void MainWindow::loadUsersfromDB()
{
    usersModel = new QStandardItemModel;
    QSqlQuery query("SELECT * FROM Users");

    while(query.next())
    {
        QList<QStandardItem*> user;
        QStandardItem *item = new QStandardItem;
        item->setText(query.value("Name").toString() + " " +
                      query.value("Surname").toString() + ", " +
                      query.value("Department").toString());
        item->setData(query.value("Name").toString(), RenterCompleter::CompleteRole);
        user.append(item);
        user.append(new QStandardItem(query.value("ID").toString()));
        user.append(new QStandardItem(query.value("Name").toString()));
        user.append(new QStandardItem(query.value("Surname").toString()));
        user.append(new QStandardItem(query.value("Department").toString()));
        user.append(new QStandardItem(query.value("Year").toString()));
        user.append(new QStandardItem(query.value("Email").toString()));
        usersModel->appendRow(user);
    }

    renterNameCompleter = new RenterCompleter(this);
    renterNameCompleter->setModel(usersModel);
    renterNameCompleter->setCompletionColumn(0);
    renterNameCompleter->setCompleteColumn(2);
    renterNameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->lineEdit_userName->setCompleter(renterNameCompleter);

    renterSurnameCompleter = new RenterCompleter(this);
    renterSurnameCompleter->setModel(usersModel);
    renterSurnameCompleter->setCompletionColumn(0);
    renterSurnameCompleter->setCompleteColumn(3);

    renterSurnameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->lineEdit_userSurname->setCompleter(renterSurnameCompleter);

    connect(renterNameCompleter, QOverload<const QModelIndex&>::of(&QCompleter::activated), this, &MainWindow::renterNameCompleterActivated);
    connect(renterSurnameCompleter, QOverload<const QModelIndex&>::of(&QCompleter::activated), this, &MainWindow::renterSurnameCompleterActivated);
}

int MainWindow::addUserToDB()
{
    if(ui->lineEdit_userName->text() == "") { QMessageBox::critical(this, "User Error", "Please add renter's name"); return -1; }
    if(ui->lineEdit_userSurname->text() == "") { QMessageBox::critical(this, "User Error", "Please add renter's surname"); return -1; }
    if(ui->lineEdit_userDepartment->text() == "") { QMessageBox::critical(this, "User Error", "Please add renter's departement"); return -1; }
    if(ui->lineEdit_userEmail->text() == "") { QMessageBox::critical(this, "User Error", "Please add renter's E-Mail"); return -1; }

    QSqlQuery query;

    query.prepare("INSERT INTO Users (Name, Surname, Department, Year, Email, Timestamp) VALUES (:name, :surname, :department, :year, :email, :timestamp)");
    query.bindValue(":name", ui->lineEdit_userName->text().trimmed());
    query.bindValue(":surname", ui->lineEdit_userSurname->text().trimmed());
    query.bindValue(":department", ui->lineEdit_userDepartment->text().trimmed());
    query.bindValue(":year", ui->lineEdit_userYear->text().trimmed());
    query.bindValue(":email", ui->lineEdit_userEmail->text().trimmed());
    query.bindValue(":timestamp", QDate::currentDate().toString(DATEFORMAT));
    if(!query.exec()) {
        QMessageBox::critical(this, "SQL-Error: ", query.lastError().text());
        return -1;
    }
    return query.lastInsertId().toInt();
}

int MainWindow::updateUserInDB()
{
    QSqlQuery query;

    query.prepare("UPDATE Users SET Name=:name, Surname=:surname, Department=:department, Year=:year, Email=:email  WHERE ID=:id");
    query.bindValue(":id", ui->label_RenterDatabaseID->text().split("#").back().toInt());
    query.bindValue(":name", ui->lineEdit_userName->text().trimmed());
    query.bindValue(":surname", ui->lineEdit_userSurname->text().trimmed());
    query.bindValue(":department", ui->lineEdit_userDepartment->text().trimmed());
    query.bindValue(":year", ui->lineEdit_userYear->text().toInt());
    query.bindValue(":email", ui->lineEdit_userEmail->text());

    if(!query.exec()) {
        QMessageBox::critical(this, "SQL-Error: ", query.lastError().text());
        return -1;
    }
    return 0;

}

QImage MainWindow::loadImage(QString filename)
{
    QImageReader imageReader(filename);
    const QImage image = imageReader.read();
    return image;
}



// Menü and Buttons
// ================
void MainWindow::on_actionSettings_triggered()
{
    Settings settingsDialog(this);
    settingsDialog.setMainWindow(this);
    settingsDialog.setUiDatabaseFilename(filename_db);

    QSettings setting(ORGANISATION, APPNAME);
    setting.beginGroup("MailServer");
        QString emailAddress = setting.value("EmailAddress").toString();
        QString password = setting.value("Password").toString();
        QString server = setting.value("Server").toString();
        int port = setting.value("Port").toInt();
    setting.endGroup();
    settingsDialog.setMailserver(emailAddress, password, server, port);


    if(settingsDialog.exec())
    {
        loadDatabaseSettings();
    }
    else
        qDebug() << "cancel";
}

void MainWindow::on_pushButton_RenterAdd_clicked()
{
    int result = addUserToDB();
    if(result > -1) {
        ui->label_RenterDatabaseID->setText("ID: #" + QString::number(result));
        QMessageBox::information(this, "Info", "User added");
        delete usersModel;
        delete renterNameCompleter;
        delete renterSurnameCompleter;
        loadUsersfromDB();
    }
}

void MainWindow::on_pushButton_RenterUpdate_clicked()
{
    int result = updateUserInDB();
    if(result > -1) {
        QMessageBox::information(this, "Info", "User updated");
        delete usersModel;
        delete renterNameCompleter;
        delete renterSurnameCompleter;
        loadUsersfromDB();
    }
}

void MainWindow::on_pushButton_RenterClear_clicked()
{
    ui->lineEdit_userDepartment->clear();
    ui->lineEdit_userEmail->clear();
    ui->lineEdit_userName->clear();
    ui->lineEdit_userSurname->clear();
    ui->lineEdit_userYear->clear();
    ui->label_RenterDatabaseID->clear();
}

void MainWindow::on_pushButton_ConfirmRental_clicked()
{
    // Check User Information
    int userID = ui->label_RenterDatabaseID->text().split("#").back().toInt();
    if(userID <= 0) {
        userID = addUserToDB();
        if(userID < 0) {
            QMessageBox::critical(this, "Rental Error", "No rental added because of user error.");
            return;
        }
    }

    // Check Start- and End-Date
    if (!rentalStartDate.isValid() || !rentalEndDate.isValid()) {
        QMessageBox::critical(this, "Rental Error", "No valid start- or enddate selected");
        return;
    }

    // Add rental to Database-Table "Rentals"
    QSqlQuery query;
    query.prepare("INSERT INTO Rentals (UserID, DateBegin, DateEnd, Room, Project, AdditionalItems, Comment, Timestamp) VALUES (:userid, :datebegin, :dateend, :room, :project, :additionalitems, :comment, :timestamp)");
    query.bindValue(":userid", userID);
    query.bindValue(":datebegin", rentalStartDate.toString(DATEFORMAT));
    query.bindValue(":dateend", rentalEndDate.toString(DATEFORMAT));
    query.bindValue(":room", ui->lineEdit_Rental_Room->text());
    query.bindValue(":project", ui->lineEdit_Rental_Project->text());
    query.bindValue(":additionalitems", ui->lineEdit_Rental_AdditionalItems->text());
    query.bindValue(":comment", ui->lineEdit_Rental_Comment->text());
    query.bindValue(":timestamp", QDate::currentDate().toString(DATEFORMAT));

    if(!query.exec()) {
        QMessageBox::critical(this, "SQL-Error: ", query.lastError().text());
        return;
    } else {
        QMessageBox::information(this, "Rental Info", "Rental added");
    }

    // Get rental-ID and add rented Items to Database-Table "RentedItems"
    int rentalID = query.lastInsertId().toInt();
    for(int i = 0; i < rentalModel->rowCount(); i++) {
        int itemID = rentalModel->item(i, 0)->data(Qt::UserRole+1).value<Item>().ID;
        query.clear();
        query.prepare("INSERT INTO RentedItems (RentalID, ItemID) VALUES (:rentalid, :itemid)");
        query.bindValue(":rentalid", rentalID);
        query.bindValue(":itemid", itemID);
        if(!query.exec()){
            QMessageBox::critical(this, "SQL-Error: ", query.lastError().text());
            return;
        }
    }
}

// load and save Settings:
// =========================

void MainWindow::saveWindowSettings()
{
    QSettings setting(ORGANISATION, APPNAME);

    setting.beginGroup("MainWindow");
        setting.setValue("geometry", saveGeometry());
        setting.setValue("windowState", saveState());
    setting.endGroup();
}

void MainWindow::loadWindowSettings()
{
    QSettings setting(ORGANISATION, APPNAME);

    setting.beginGroup("MainWindow");
        restoreGeometry(setting.value("geometry").toByteArray());
        restoreState(setting.value("windowState").toByteArray());
    setting.endGroup();
}

void MainWindow::loadDatabaseSettings()
{
    QSettings setting(ORGANISATION, APPNAME);

    setting.beginGroup("Database");
        filename_db = setting.value("Filename").toString();
        dataDirectory = QFileInfo(filename_db).absolutePath() + "/";
    setting.endGroup();
}
