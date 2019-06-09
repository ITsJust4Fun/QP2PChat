#ifndef DOWNLOADMODEL_H
#define DOWNLOADMODEL_H

#include <QAbstractItemModel>

#include "items/downloaditem.h"

class DownloadModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit DownloadModel(QObject *parent = nullptr);
    virtual ~DownloadModel() override;
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    void appendUser(QString &user);
    void appendDownload(QString &user, QString &path);

private:
    DownloadItem *rootItem;
};

#endif // DOWNLOADMODEL_H
