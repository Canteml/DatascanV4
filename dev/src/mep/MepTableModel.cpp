#include "MepTableModel.h"

int MepTableModel::rowCount(const QModelIndex &) const
{
    return 100;
}

int MepTableModel::columnCount(const QModelIndex &) const
{
    return 100;
}

QVariant MepTableModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
        case Qt::DisplayRole:
            return QString("%1:%2").arg(index.column()).arg(index.row());
        default:
            break;
    }

    return QVariant();
}

QHash<int, QByteArray> MepTableModel::roleNames() const
{
    return { {Qt::DisplayRole, "display"} };
}
