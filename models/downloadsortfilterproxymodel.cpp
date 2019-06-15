#include "downloadsortfilterproxymodel.h"

DownloadSortFilterProxyModel::DownloadSortFilterProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{

}

DownloadSortFilterProxyModel::~DownloadSortFilterProxyModel()
{

}

bool DownloadSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    DownloadItem *itemLeft = static_cast<DownloadItem *>(left.internalPointer());
    DownloadItem *itemRight = static_cast<DownloadItem *>(right.internalPointer());
    if (!itemLeft && !itemRight) {
        return true;
    } else if (!itemLeft) {
        return true;
    } else if (!itemRight) {
        return false;
    }
    QString leftPath = itemLeft->getPath();
    QString rightPath = itemRight->getPath();
    QString leftName = left.data().toString();
    QString rightName = right.data().toString();
    bool isLeftFolder = QFileInfo(QFile(leftPath)).isDir();
    bool isRightFolder = QFileInfo(QFile(rightPath)).isDir();

    if (isLeftFolder && !isRightFolder) {
        return true;
    } else if (isLeftFolder == isRightFolder) {
        return !QString::compare(leftName, rightName, Qt::CaseSensitive);
    } else {
        return false;
    }
}
