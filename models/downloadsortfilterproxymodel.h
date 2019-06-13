#ifndef DOWNLOADSORTFILTERPROXYMODEL_H
#define DOWNLOADSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QFile>
#include <QFileInfo>

#include "items/downloaditem.h"

class DownloadSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit DownloadSortFilterProxyModel(QObject *parent = nullptr);
    virtual ~DownloadSortFilterProxyModel() override;

    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

#endif // DOWNLOADSORTFILTERPROXYMODEL_H
