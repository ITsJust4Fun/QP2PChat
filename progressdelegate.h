#ifndef PROGRESSDELEGATE_H
#define PROGRESSDELEGATE_H

#include <QStyledItemDelegate>
#include <QApplication>

#include "items/downloaditem.h"

class ProgressDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    ProgressDelegate(QObject *parent = nullptr);
    virtual ~ProgressDelegate() override;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const override;
};

#endif // PROGRESSDELEGATE_H
