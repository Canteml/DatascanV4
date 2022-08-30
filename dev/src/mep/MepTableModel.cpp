#include "MepTableModel.h"

#include <QObject>
#include <v2/exam/ExamManager.h>
#include <v2/scan/AScan.h>
#include <v2/scan/MepMatrix.h>

MepTableModel::MepTableModel(QObject *parent) : QAbstractTableModel{parent}
{
    m_examManager = ExamManager::getInstance();
    connect(m_examManager, &ExamManager::evMepNbRowChanged, this, &MepTableModel::onLayoutChanged);
    connect(m_examManager, &ExamManager::evMepNbColChanged, this, &MepTableModel::onLayoutChanged);
    connect(m_examManager, &ExamManager::evMepZoomChanged, this, &MepTableModel::onLayoutChanged);
    connect(m_examManager, &ExamManager::evMepHeaderUpdated, this, &MepTableModel::onHeaderDataChanged);
    connect(m_examManager, &ExamManager::evMepUpdated, this, &MepTableModel::onDataChanged);
    onLayoutChanged();
}

MepTableModel::~MepTableModel()
{
    m_examManager = nullptr;
}

int MepTableModel::rowCount(const QModelIndex &) const
{
    return m_u32NbRow; //m_examManager->getMepNbRow();
}

int MepTableModel::columnCount(const QModelIndex &) const
{
    return m_u32NbCol; //m_examManager->getMepNbCol();
}

QVariant MepTableModel::data(const QModelIndex &index, int role) const
{
    MepMatrix* pMatrix = m_examManager->getMepMatrix();
    if(nullptr == pMatrix) {
        return QString();
    }
    AScan* pAScan = pMatrix->getAScanAt(index.row(), index.column());
    if(nullptr == pAScan) {
        return QString();
    }
    return QString::number(pAScan->getAScanMeasure()->m_thicknessMmMultiEcho, 'f', 1);
}

QVariant MepTableModel::headerData(int p_i32Section, Qt::Orientation p_eOrientation, int p_i32Role) const
{
    if(nullptr == m_examManager->getMepMatrix()) {
        return QString();
    }
    if(Qt::Vertical == p_eOrientation) {
        qreal begin = abs((m_examManager->getMepHeight() - (p_i32Section) * m_examManager->getMepIntervalY()));
        //qreal end = begin + m_examManager->getMepIntervalY();
        return  QString::number(m_examManager->getMepNbRow() - p_i32Section)
                //+ QString(" | ") + QString::number(begin, 'f', 1) + QString("m")// + QString(" → ") +
                            /*QString::number(end, 'f', 1) + QString("m")*/;
    }
    //return QVariant(p_i32Section + 1);
    QString qsLabel;
    quint32 u32Index = (quint32) p_i32Section;
    quint32 base = 26;
    if(u32Index < base) {
        qsLabel = QString((char) (u32Index + 'A'));
    } else {
        u32Index++;
        while(u32Index > 0) {
            u32Index--;
            qsLabel = QString((char) (u32Index % base + 'A') ) + qsLabel;
            u32Index /= base;
        }
    }
    if(0 == m_examManager->getMepMatrix()->getColumnDirection(p_i32Section)) {
        qsLabel += " ↑";

    } else if(1 == m_examManager->getMepMatrix()->getColumnDirection(p_i32Section)){
        qsLabel += " ↓";
    }
    return QVariant(qsLabel);
}

QHash<int, QByteArray> MepTableModel::roleNames() const
{
    return { {Qt::DisplayRole, "display"} };
}

void MepTableModel::onLayoutChanged()
{
    beginResetModel();
    m_u32NbRow = m_examManager->getMepNbRow();
    m_u32NbCol = m_examManager->getMepNbCol();
    endResetModel();
    // emit dataChanged(index(0,0), index(m_u32NbRow-1, m_u32NbCol-1));
    // emit layoutChanged();
}

void MepTableModel::onDataChanged()
{
    emit dataChanged(index(0,0), index(m_u32NbRow-1, m_u32NbCol-1));
}

void MepTableModel::onHeaderDataChanged()
{
    emit headerDataChanged(Qt::Vertical, 0, m_u32NbRow - 1);
    emit headerDataChanged(Qt::Horizontal, 0, m_u32NbCol - 1);
}
