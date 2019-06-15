#include "downloadmodel.h"

DownloadModel::DownloadModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootData << "Name" << "Progress";
    rootItem = new DownloadItem(rootData);
}

DownloadModel::~DownloadModel()
{
    delete rootItem;
}

QVariant DownloadModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    DownloadItem *item = static_cast<DownloadItem *>(index.internalPointer());
    if (role == Qt::DecorationRole) {
        if (item->parentItem() == rootItem) {
            return QIcon(":/icons/user.png");
        }
        if (item->data(DownloadItem::NameColumn).toString().contains("Transfer")) {
            return QIcon(":/icons/transfer.png");
        }
        QFileIconProvider iconProvider;
        QFile file(item->getPath());
        QFileInfo fileInfo(file);
        return iconProvider.icon(fileInfo);
    } else if (role == Qt::DisplayRole) {
        return item->data(index.column());
    } else {
        return QVariant();
    }
}

Qt::ItemFlags DownloadModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant DownloadModel::headerData(int section, Qt::Orientation orientation,
                                   int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex DownloadModel::index(int row, int column,
                                 const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    DownloadItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<DownloadItem *>(parent.internalPointer());

    DownloadItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex DownloadModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    DownloadItem *childItem = static_cast<DownloadItem *>(index.internalPointer());
    DownloadItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int DownloadModel::rowCount(const QModelIndex &parent) const
{
    DownloadItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<DownloadItem *>(parent.internalPointer());

    return parentItem->childCount();
}

int DownloadModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<DownloadItem *>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

void DownloadModel::appendUser(const QString &user)
{
    beginResetModel();
    QList<QVariant> userData;
    userData << user << 100;
    DownloadItem *item = new DownloadItem(userData, rootItem);
    rootItem->appendChild(item);
    endResetModel();
}

DownloadItem *DownloadModel::appendDownload(const QString &user, const QString &path)
{
    DownloadItem *name = nullptr;
    if (!user.isEmpty()) {
        for (int i = 0; i < rootItem->childCount(); i++) {
            if (rootItem->child(i)->data(DownloadItem::NameColumn) == user) {
                name = rootItem->child(i);
            }
        }
    }

    if (name) {
        return appendDownload(name, path);
    }
    return nullptr;
}

DownloadItem *DownloadModel::appendDownload(DownloadItem *itemParent, const QString &path)
{
    beginResetModel();
    QList<QVariant> download;
    download << path.mid(path.lastIndexOf("/") + 1) << 0;
    DownloadItem *item = new DownloadItem(download, itemParent);
    item->setPath(path);
    itemParent->appendChild(item);
    endResetModel();
    return item;
}

DownloadItem *DownloadModel::appendTransfer(const QString &user)
{
    DownloadItem *name = nullptr;
    if (!user.isEmpty()) {
        for (int i = 0; i < rootItem->childCount(); i++) {
            if (rootItem->child(i)->data(DownloadItem::NameColumn) == user) {
                name = rootItem->child(i);
            }
        }
    }

    if (name) {
        beginResetModel();
        QList<QVariant> download;
        download << "Transfer #" + QString::number(name->childCount() + 1) << 0;
        DownloadItem *item = new DownloadItem(download, name);
        name->appendChild(item);
        endResetModel();
        return item;
    }
    return nullptr;
}

void DownloadModel::setProgress(DownloadItem *item, const int progress)
{ 
    if (item == rootItem) {
        return;
    };
    item->setProgress(progress);
    while (item != rootItem) {
        QModelIndex itemIndex = createIndex(item->row(), DownloadItem::ProgressColumn, item);
        emit dataChanged(itemIndex, itemIndex);
        item = item->parentItem();
    }
}

void DownloadModel::beginAppendRow(DownloadItem *item)
{
    QModelIndex itemIndex;
    if (item != rootItem) {
        itemIndex = createIndex(item->row(), DownloadItem::NameColumn, item);
    }
    int rowIndex = item->childCount();
    beginInsertRows(itemIndex, rowIndex, rowIndex);
}

void DownloadModel::endAppendRow()
{
    endInsertRows();
}
