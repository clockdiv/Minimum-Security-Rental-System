#include <QDebug>
#include "rentercompleter.h"


const int RenterCompleter::CompleteRole = Qt::UserRole + 1;

RenterCompleter::RenterCompleter(QObject *parent) : QCompleter(parent)
{
    completeColumn = 0;
}

void RenterCompleter::setCompleteColumn(int column)
{
    completeColumn = column;
}


QString RenterCompleter::pathFromIndex(const QModelIndex &index) const
{
    int row = index.row();
    QModelIndex itemIndex =  model()->index(row, completeColumn);
    QMap<int, QVariant> data = model()->itemData(itemIndex);

    QString code = data.value(0).toString();
    return code;
}
