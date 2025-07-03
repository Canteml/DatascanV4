#include "MepMatrix.h"

#include <QVector>
#include <v2/scan/AScan.h>

MepMatrix::MepMatrix(QObject *parent) : QObject{parent}, Loggable("MepMatrix")
{
    m_data = QVector<QVector<AScan*>>();
    m_data.append(QVector<AScan*>());
    m_data[0].append(nullptr);
    m_columnsDirections = QVector<quint8>();
    m_columnsDirections.append(2);
}

MepMatrix::~MepMatrix()
{
    for(int row = 0 ; row < m_data.size() ; row++) {
        QVector<AScan*> column = m_data[row];
        for(int c = 0 ; c < column.size() ; c++) {
            delete column[c];
        }
    }
}

void MepMatrix::resetSize(MatrixSize p_sSize)
{
    if(p_sSize.rows * p_sSize.columns == 0) {
        fLogError("Error : MEP matrix size can't have a null coordinate");
    }
    m_data = QVector<QVector<AScan*>>();
    m_data.append(QVector<AScan*>());
    m_data[0].append(nullptr);
    m_columnsDirections = QVector<quint8>();
    m_columnsDirections.append(2);
    while(getSize().rows < p_sSize.rows) {
        addRow();
    }
    while(getSize().columns < p_sSize.columns) {
        addColumn();
    }
}

MatrixSize MepMatrix::getSize()
{
    return MatrixSize { (quint32) m_data.size(), (quint32) m_data[0].size() };
}

void MepMatrix::addRow(quint32 p_u32Index)
{
    MatrixSize sSize = getSize();
    if(p_u32Index > sSize.rows) {
        p_u32Index = sSize.rows;
    }
    QVector<AScan*> vTemp;
    for(quint32 column = 0 ; column < sSize.columns ; column++) {
        vTemp.append(nullptr);
    }
    m_data.insert(p_u32Index, vTemp);
}

void MepMatrix::addColumn(quint32 p_u32Index)
{
    MatrixSize sSize = getSize();
    if(p_u32Index > sSize.columns) {
        p_u32Index = sSize.columns;
    }
    for(quint32 row = 0 ; row < sSize.rows ; row++) {
        m_data[row].insert(p_u32Index, nullptr);
    }
    m_columnsDirections.insert(p_u32Index, 2);
}

void MepMatrix::removeRow(quint32 p_u32Index)
{
    MatrixSize sSize = getSize();
    if(p_u32Index >= sSize.rows) {
        p_u32Index = sSize.rows - 1;
    }
    for(AScan* pAScan : m_data[p_u32Index]) {
        delete pAScan;
    }
    m_data.remove(p_u32Index);
}

void MepMatrix::removeColumn(quint32 p_u32Index)
{
    MatrixSize sSize = getSize();
    if(p_u32Index >= sSize.columns) {
        p_u32Index = sSize.columns - 1;
    }
    for(quint32 row = 0 ; row < sSize.rows ; row++) {
        delete m_data[row][p_u32Index];
        m_data[row].remove(p_u32Index);
    }
    m_columnsDirections.remove(p_u32Index);
}

void MepMatrix::appendRow()
{
    addRow(getSize().rows);
}

void MepMatrix::appendColumn()
{
    addColumn(getSize().columns);
}

AScan* MepMatrix::getAScanAt(quint32 p_u32Row, quint32 p_u32Column)
{
    MatrixSize sSize = getSize();
    if(sSize.rows <= p_u32Row || sSize.columns <= p_u32Column) {
        fLogError(tr("Index out of range : (%1;%2) in [%3;%4]").arg(p_u32Row).arg(p_u32Column).arg(sSize.rows).arg(sSize.columns));
        return nullptr;
    }
    return m_data[p_u32Row][p_u32Column];
}

void MepMatrix::pushAScan(AScan* p_pAScan, quint32 p_u32Row, quint32 p_u32Column)
{
    MatrixSize sSize = getSize();
    if(sSize.rows <= p_u32Row || sSize.columns <= p_u32Column) {
        fLogError(tr("Index out of range : (%1;%2) in [%3;%4]").arg(p_u32Row).arg(p_u32Column).arg(sSize.rows).arg(sSize.columns));
        return;
    }
    AScan* pCurrentAScan = m_data[p_u32Row][p_u32Column];
    delete pCurrentAScan;
    m_data[p_u32Row][p_u32Column] = p_pAScan;
    emit evDataUpdated();
}

quint8 MepMatrix::getColumnDirection(quint32 p_u32Col)
{
    return m_columnsDirections[p_u32Col];
}

void MepMatrix::setColumnDirection(quint32 p_u32Col, quint8 p_u8Direction)
{
    m_columnsDirections[p_u32Col] = p_u8Direction;
}
