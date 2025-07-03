#pragma once

#include <log/Loggable.h>
#include <QObject>
#include <QVector>
#include <QSize>

typedef struct MatrixSize {
    quint32 rows;
    quint32 columns;
} MatrixSize;

/*typedef struct MatrixIndex {
    quint16 row;
    quint16 column;
} MatrixIndex;*/

class AScan;

class MepMatrix : public QObject, public Loggable
{
    Q_OBJECT

public:

    explicit MepMatrix(QObject *parent = nullptr);
    virtual ~MepMatrix();

    void resetSize(MatrixSize p_sSize);
    MatrixSize getSize();

    void addRow(quint32 p_u32Index = 0);
    void addColumn(quint32 p_u32Index = 0);
    void removeRow(quint32 p_u32Index = 0);
    void removeColumn(quint32 p_u32Index = 0);

    void appendRow();
    void appendColumn();

    AScan* getAScanAt(quint32 p_u32Row, quint32 p_u32Column);

    void pushAScan(AScan* p_pAScan, quint32 p_u32Row, quint32 p_u32Column);

    quint8 getColumnDirection(quint32 p_u32Col);
    void setColumnDirection(quint32 p_u32Col, quint8 p_u8Direction);

signals:

    void evDataUpdated();

protected:

    QVector<QVector<AScan*>> m_data;
    QVector<quint8> m_columnsDirections;

};

