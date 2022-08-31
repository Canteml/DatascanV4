#include "MepExporter.h"

#include <coreUi/CoreUiQml.h>
#include <v2/vector/VectorManager.h>
#include <v2/exam/ExamManager.h>
#include <v2/prot/ProtMsg.h>
#include <v2/vector/VectorStd.h>
#include <v2/scan/MepMatrix.h>
#include <v2/scan/AScan.h>

// -- Qt includes
#include <QFileInfo>

// -- QXlsx includes
#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"
#include "xlsxcell.h"

#include "mep/MepTableModel.h"

#define U32_MEP_TABLE_BEGIN_ROW ((quint32) 48)
#define U32_MEP_TABLE_BEGIN_COL ((quint32) 1)

MepExporter::MepExporter() : Loggable("MepExporter")
{

}


QString MepExporter::startExport(const QString &p_fileName)
{
    // fill excel template with data of exam
    QString sRet;
    // Find template function to the language
    QString fileTemplate = QString("%1/templates/mep_fr.xlsx").arg(CoreUiQml::getDirAppRes());

    ExamManager* pExam = ExamManager::getInstance();
    MepMatrix* pMatrix = pExam->getMepMatrix();
    MatrixSize mepSize = pMatrix->getSize();

    QXlsx::Format headerFormat;
    QXlsx::Format cellFormat;
    headerFormat.setPatternBackgroundColor(QColor("#BBBBBB"));
    headerFormat.setBorderStyle(QXlsx::Format::BorderThin);
    headerFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    cellFormat.setBorderStyle(QXlsx::Format::BorderThin);
    cellFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);

    if( !QFileInfo::exists(fileTemplate) ) {
        fLogError("Template file not found : " + fileTemplate);
        sRet = tr("Base de données corrompue.");
    } else {
        QXlsx::Document doc(fileTemplate);
        if( !doc.load() ) {
            fLogError("Fail to open template : " + fileTemplate);
            sRet = tr("Erreur à l'ouverture de la base de données.");
        } else {

            // -- General Info
            doc.write("C14", pExam->getSiteName());
            doc.write("C15", pExam->getSectorName());
            doc.write("C16", pExam->getStructName());
            doc.write("C16", pExam->getStructName());
            doc.write("C28", pExam->getMepBegin().toString("dd.MM.yyyy hh:mm:ss"));
            doc.write("C20", QString::number(pExam->getMepHeight()) + "m");
            doc.write("C18", QString::number(pExam->getMepDiameter()) + "m");
            doc.write("C17", pExam->isStructureActive() ? "En service" : "Hors service");
            doc.write("C29", pExam->getExamComment());

            if(pExam->getMepVirolesEnabled()) {
                doc.write("C21", QString::number(pExam->getMepNbRow() / pExam->getMeasuresPerVirole()));
                doc.write("C46", QString::number(pExam->getMeasuresPerVirole()) + tr(" points par virole"));
            } else {
                doc.write("C46", QString::number(pExam->getMepNbRow()) + tr(" points par génératrice"));
            }

            QString prodName = ProtMsg::prodName(VectorManager::getInstance()->getVectorStd()->getProdId());
            doc.write("I32", prodName);
            doc.write("C45", pExam->getExamOrigin());

            // -- Table
            quint32 virolePadding = 0;
            if(pExam->getMepVirolesEnabled()) {
                virolePadding = 1;
                doc.write(U32_MEP_TABLE_BEGIN_ROW, U32_MEP_TABLE_BEGIN_COL, "Virole", headerFormat);
                qint32 i32BottomCellY = mepSize.rows + U32_MEP_TABLE_BEGIN_ROW;
                quint32 viroleIndex = 1;
                while(i32BottomCellY > U32_MEP_TABLE_BEGIN_ROW + 1) {
                    qint32 i32TopCellY = i32BottomCellY - pExam->getMeasuresPerVirole() + 1;
                    QXlsx::CellReference refTop(i32TopCellY, U32_MEP_TABLE_BEGIN_COL);
                    QXlsx::CellReference refBottom(i32BottomCellY, U32_MEP_TABLE_BEGIN_COL);
                    QXlsx::CellRange mergeRange(refTop, refBottom);
                    doc.mergeCells(mergeRange, headerFormat);
                    doc.write(refTop, tr("Virole ") + QString::number(viroleIndex), headerFormat);
                    i32BottomCellY = i32TopCellY - 1;
                    viroleIndex++;
                }
            }

            doc.write(U32_MEP_TABLE_BEGIN_ROW, U32_MEP_TABLE_BEGIN_COL + virolePadding, "Indice", headerFormat);
            //doc.write(U32_MEP_TABLE_BEGIN_ROW, U32_MEP_TABLE_BEGIN_COL + virolePadding + 1, "Distance", headerFormat);

            for(quint32 u32Row = 0 ; u32Row < mepSize.rows ; u32Row++) {
                //QString rowDistance = QString::number(abs((pExam->getMepHeight() - (u32Row) * pExam->getMepIntervalY())),'f',2) + tr("m");
                quint32 rowY = U32_MEP_TABLE_BEGIN_ROW + u32Row + 1;
                doc.write(rowY, U32_MEP_TABLE_BEGIN_COL + virolePadding, pExam->getMepNbRow() - u32Row, headerFormat);
                //doc.write(rowY, U32_MEP_TABLE_BEGIN_COL + virolePadding + 1, rowDistance, headerFormat);
                for(quint32 u32Col = 0 ; u32Col < mepSize.columns ; u32Col++) {

                    quint32 colX = U32_MEP_TABLE_BEGIN_COL + u32Col + virolePadding + 1;
                    if(0 == u32Row) { // Write header for first line
                        doc.write(U32_MEP_TABLE_BEGIN_ROW, colX, MepTableModel::getColumnLabel(u32Col), headerFormat);
                    }
                    AScan* cellAScan = pMatrix->getAScanAt(u32Row, u32Col);
                    if(nullptr == cellAScan) {
                        doc.write(rowY, colX, "-", cellFormat);
                    } else {
                        doc.write(rowY, colX, QString::number(cellAScan->getAScanMeasure()->m_thicknessMmMultiEcho,'f',1), cellFormat);
                    }

                }
            }

            doc.write(U32_MEP_TABLE_BEGIN_ROW + mepSize.rows + 2, 1, pExam->getCellsAnnotations());

            // -- Write file :
            QString fileName = p_fileName.right(p_fileName.length()-8); // remove "file:///" append by qml
            if( ".xlsx" != fileName.right(5) ) {
                fileName.append(".xlsx");
            }
            if( ! doc.saveAs(fileName) ) {
                fLogError("Fail to save file : " + fileName);
                sRet = tr("Impossible d'écrire le fichier.");
            }
        }
    }

    return sRet;
}
