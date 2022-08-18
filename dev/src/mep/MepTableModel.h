#pragma once

#include <QAbstractTableModel>
#include <QObject>

#include <qqml.h>

class ExamManager;

class MepTableModel : public QAbstractTableModel
{
    Q_OBJECT
    QML_ELEMENT

public:

    explicit MepTableModel(QObject *parent = nullptr);
    ~MepTableModel();

    int rowCount(const QModelIndex & = QModelIndex()) const override;

    int columnCount(const QModelIndex & = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int p_i32Section, Qt::Orientation p_eOrientation, int p_i32Role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

public slots:

    void onLayoutChanged();
    void onDataChanged();

protected:

    ExamManager *m_examManager;

};
