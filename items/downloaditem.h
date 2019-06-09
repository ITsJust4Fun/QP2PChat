#ifndef DOWNLOADITEM_H
#define DOWNLOADITEM_H

#include <QList>

class DownloadItem
{
public:
    explicit DownloadItem(const QList<QVariant> &data, DownloadItem *parentItem = nullptr);
    virtual ~DownloadItem();

    void appendChild(DownloadItem *child);

    DownloadItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    DownloadItem *parentItem();

private:
    QList<DownloadItem *> m_childItems;
    QList<QVariant> m_itemData;
    DownloadItem *m_parentItem;
};

#endif // DOWNLOADITEM_H
