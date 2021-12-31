#ifndef RENTERCOMPLETER_H
#define RENTERCOMPLETER_H

#include <QCompleter>

class RenterCompleter : public QCompleter
{
    Q_OBJECT

public:
    explicit RenterCompleter(QObject *parent = nullptr);

    static const int CompleteRole;
    int completeColumn;
    void setCompleteColumn(int column);
    QString pathFromIndex(const QModelIndex &index) const;
};

#endif // RENTERCOMPLETER_H
