#include "downloaditem.h"

DownloadItem::DownloadItem(const QList<QVariant> &data, DownloadItem *parentItem)
{
    m_parentItem = parentItem;
    m_itemData = data;
}

DownloadItem::~DownloadItem()
{
    qDeleteAll(m_childItems);
}

void DownloadItem::appendChild(DownloadItem *child)
{
     m_childItems.append(child);
     updateProgress();
}

DownloadItem *DownloadItem::child(int row)
{
    return m_childItems.value(row);
}

int DownloadItem::childCount() const
{
    return m_childItems.count();
}

int DownloadItem::columnCount() const
{
    return m_itemData.count();
}

QVariant DownloadItem::data(int column) const
{
    return m_itemData.value(column);
}

int DownloadItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<DownloadItem *>(this));

    return 0;
}

DownloadItem *DownloadItem::parentItem()
{
    return m_parentItem;
}

void DownloadItem::setPath(const QString &path)
{
    this->path = path;
}

QString DownloadItem::getPath() const
{
    return path;
}

void DownloadItem::setProgress(int progress)
{
    m_itemData[ProgressColumn] = progress;
    m_parentItem->updateProgress();
}

void DownloadItem::updateProgress()
{
    double totalProgress = 0;
    for (auto item : m_childItems) {
        totalProgress += item->data(ProgressColumn).toInt();
    }
    if (m_childItems.count() > 0) {
        int progress = qRound(totalProgress / m_childItems.count());
        m_itemData[ProgressColumn] = progress;
    }
}
