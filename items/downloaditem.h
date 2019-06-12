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
    void updateProgress();
    void setProgress(int progress);
    void setPath(const QString &path);
    QString getPath() const;
    QString getPathView();
    void setIp(const QString &ip);
    QString getIp();


    enum columns
    {
        NameColumn,
        ProgressColumn
    };

private:
    QList<DownloadItem *> m_childItems;
    QList<QVariant> m_itemData;
    DownloadItem *m_parentItem;
    QString *path;
    QString *pathView;
    QString *ip;
};

#endif // DOWNLOADITEM_H
