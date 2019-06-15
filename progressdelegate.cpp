#include "progressdelegate.h"

ProgressDelegate::ProgressDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{

}

ProgressDelegate::~ProgressDelegate()
{

}

void ProgressDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    if (index.column() == 1)
    {
        QStringList data = index.data().toString().split(" ");
        int progressIndex = data.size() > 1 ? 1 : 0;
        int value = data[progressIndex].toInt();
        QStyleOptionProgressBar progressBarOption;

        progressBarOption.state = QStyle::State_Enabled;
        progressBarOption.direction = QApplication::layoutDirection();
        progressBarOption.rect = option.rect;
        progressBarOption.fontMetrics = QApplication::fontMetrics();
        progressBarOption.minimum = 0;
        progressBarOption.maximum = 100;
        progressBarOption.textAlignment = Qt::AlignCenter;
        progressBarOption.textVisible = true;
        progressBarOption.text = index.data().toString() + "%";
        progressBarOption.progress = value;

        QApplication::style()->drawControl(QStyle::CE_ProgressBar,
        &progressBarOption, painter);
    }
    else
        QStyledItemDelegate::paint(painter, option, index);
}
