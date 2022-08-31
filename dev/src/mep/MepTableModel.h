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

    static QString getColumnLabel(const quint32 p_i32Section);

public slots:

    void onLayoutChanged();
    void onDataChanged();
    void onHeaderDataChanged();

protected:

    ExamManager *m_examManager;

    quint32 m_u32NbCol;
    quint32 m_u32NbRow;

};

class MepVirolesHeaderModel : public MepTableModel
{
    Q_OBJECT
    QML_ELEMENT

public:

    explicit MepVirolesHeaderModel(QObject *parent = nullptr);
    ~MepVirolesHeaderModel();

    QVariant headerData(int p_i32Section, Qt::Orientation p_eOrientation, int p_i32Role = Qt::DisplayRole) const override;

public slots:

    void onHeaderDataChanged();
};
