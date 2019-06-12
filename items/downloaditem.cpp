#include "downloaditem.h"

DownloadItem::DownloadItem(const QList<QVariant> &data, DownloadItem *parentItem)
{
    m_parentItem = parentItem;
    m_itemData = data;
    ip = new QString("");
    path = new QString("");
    pathView = new QString("");
}

DownloadItem::~DownloadItem()
{
    qDeleteAll(m_childItems);
    delete ip;
    delete path;
    delete pathView;
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
    delete this->path;
    delete this->pathView;
    this->path = new QString(path);
    pathView = new QString(*m_parentItem->pathView + "/" + m_itemData[NameColumn].toString());
}

QString DownloadItem::getPath() const
{
    return *path;
}

QString DownloadItem::getPathView()
{
    return *pathView;
}

void DownloadItem::setProgress(int progress)
{
    m_itemData[ProgressColumn] = progress;
    m_parentItem->updateProgress();
}

void DownloadItem::setIp(const QString &ip)
{
    delete this->ip;
    this->ip = new QString(ip);
}

QString DownloadItem::getIp()
{
    return *ip;
}

void DownloadItem::updateProgress()
{
    if (!m_parentItem) {
        return;
    }
    double totalProgress = 0;
    for (auto item : m_childItems) {
        totalProgress += item->data(ProgressColumn).toInt();
    }
    if (m_childItems.count() > 0) {
        int progress = qRound(totalProgress / m_childItems.count());
        setProgress(progress);
    }
}
