#include "downloaditem.h"

DownloadItem::DownloadItem(const QList<QVariant> &data, DownloadItem *parentItem)
{
    m_parentItem = parentItem;
    m_itemData = data;
    path = new QString("");
    pathView = new QString("");
    itemMode = MixMode;
}

DownloadItem::~DownloadItem()
{
    qDeleteAll(m_childItems);
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
    QString progressData = QString::number(progress);
    if (itemMode == DownloadMode) {
        progressData = "Downloading " + progressData;
    } else if (itemMode == UploadMode) {
        progressData = "Uploading " + progressData;
    }
    m_itemData[ProgressColumn] = progressData;
    m_parentItem->updateProgress();
}

void DownloadItem::setMode(int itemMode)
{
    this->itemMode = itemMode;
}

int DownloadItem::getMode()
{
    return itemMode;
}

void DownloadItem::updateProgress()
{
    if (!m_parentItem) {
        return;
    }
    double totalProgress = 0;
    for (auto item : m_childItems) {
        QStringList data = item->data(ProgressColumn).toString().split(" ");
        int progressIndex = data.size() > 1 ? 1 : 0;
        int progress = data[progressIndex].toInt();
        totalProgress += progress;
    }
    if (m_childItems.count() > 0) {
        int progress = qRound(totalProgress / m_childItems.count());
        setProgress(progress);
    }
}
