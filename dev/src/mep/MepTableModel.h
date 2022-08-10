#pragma once

#include <QAbstractTableModel>
#include <QObject>

#include <qqml.h>

class MepTableModel : public QAbstractTableModel
{
    Q_OBJECT
    QML_ELEMENT

public:

    int rowCount(const QModelIndex & = QModelIndex()) const override;

    int columnCount(const QModelIndex & = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QHash<int, QByteArray> roleNames() const override;

};
