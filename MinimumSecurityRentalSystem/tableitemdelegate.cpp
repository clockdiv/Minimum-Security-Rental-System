#include "tableitemdelegate.h"

TableItemDelegate::TableItemDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

//QWidget *TableItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
//{
//    QSpinBox *editor = new QSpinBox(parent);
//    editor->setMinimum(0);
//    editor->setMaximum(100);
//    return editor;

//}

void TableItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
//    if(index.data().userType() == QMetaType::Int){
    if(index.column() == 10) {
        //painter->fillRect(option.rect, Qt::red);
        int x = option.rect.topLeft().x();
        int y = option.rect.topLeft().y();
        painter->drawLine(x, y, x+100, y+100);
    }
    QStyledItemDelegate::paint(painter, option, index);
}

//void TableItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
//{
////    int value = index.model()->data(index, Qt::EditRole).toInt();
////    QSpinBox *spinbox = static_cast<QSpinBox*>(editor);
////    spinbox->setValue(value);
//}

//void TableItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
//{
////    QSpinBox *spinbox = static_cast<QSpinBox*>(editor);
////    spinbox->interpretText();
////    int value = spinbox->value();
////    model->setData(index, value, Qt::EditRole);
//}

//QSize TableItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
//{

//}

//void TableItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
//{
////    editor->setGeometry(option.rect);
//}
