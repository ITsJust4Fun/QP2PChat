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
