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

    if (role != Qt::DisplayRole)
        return QVariant();

    DownloadItem *item = static_cast<DownloadItem *>(index.internalPointer());

    return item->data(index.column());
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
    QList<QVariant> userData;
    userData << user << 100;
    rootItem->appendChild(new DownloadItem(userData, rootItem));
}

void DownloadModel::appendDownload(const QString &user, const QString &path)
{
    DownloadItem *name = nullptr;
    for (int i = 0; i < rootItem->childCount(); i++) {
        if (rootItem->child(i)->data(DownloadItem::NameColumn) == user) {
            name = rootItem->child(i);
        }
    }

    if (name) {
        QList<QVariant> download;
        download << path.mid(path.lastIndexOf("/") + 1) << 0;
        DownloadItem *item = new DownloadItem(download, name);
        item->setPath(path);
        name->appendChild(item);
    }
}
