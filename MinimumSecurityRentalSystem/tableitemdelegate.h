#ifndef TABLEITEMDELEGATE_H
#define TABLEITEMDELEGATE_H

//#include <QItemDelegate>
#include <QStyledItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QSpinBox>
#include <QPainter>

class TableItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    explicit TableItemDelegate(QObject *parent = 0);
//    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
//    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
//    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
//    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
//    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

signals:

public slots:


};

#endif // TABLEITEMDELEGATE_H
