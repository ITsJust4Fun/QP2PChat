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
        int value = index.data().toInt();
        QStyleOptionProgressBar progressBarOption;
        progressBarOption.state = QStyle::State_Enabled;
        progressBarOption.direction = QApplication::layoutDirection();
        progressBarOption.rect = option.rect;
        progressBarOption.fontMetrics = QApplication::fontMetrics();
        progressBarOption.minimum = 0;
        progressBarOption.maximum = 100;
        progressBarOption.textAlignment = Qt::AlignCenter;
        progressBarOption.textVisible = true;
        progressBarOption.text = QString::number(value) + "%";
        progressBarOption.progress = value;

        DownloadItem *item = static_cast<DownloadItem *>(index.internalPointer());

        if (item->getMode() == DownloadItem::UploadMode) {
            QPalette palette = progressBarOption.palette;
            QColor color(0, 200, 200);
            palette.setColor(QPalette::Highlight, color);
            progressBarOption.palette = palette;
        } else if (item->getMode() == DownloadItem::MixMode) {
            QPalette palette = progressBarOption.palette;
            QColor color(255, 0, 0);
            palette.setColor(QPalette::Highlight, color);
            progressBarOption.palette = palette;
        }

        QApplication::style()->drawControl(QStyle::CE_ProgressBar,
        &progressBarOption, painter);
    }
    else
        QStyledItemDelegate::paint(painter, option, index);
}
