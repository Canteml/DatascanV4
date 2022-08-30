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

#define U32_MEP_TABLE_BEGIN_ROW ((quint32) 33)
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
    cellFormat.setBorderStyle(QXlsx::Format::BorderThin);

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
            doc.write("C9", pExam->getMepBegin().toString("dd.MM.yyyy hh:mm:ss"));
            doc.write("E16", pExam->getMepHeight());
            doc.write("I16", pExam->getMepDiameter());
            QString prodName = ProtMsg::prodName(VectorManager::getInstance()->getVectorStd()->getProdId());
            doc.write("I19", prodName);

            for(quint32 u32Row = 0 ; u32Row < mepSize.rows ; u32Row++) {
                QString rowDistance = QString::number(abs((pExam->getMepHeight() - (u32Row) * pExam->getMepIntervalY()))) + tr("m");
                quint32 rowY = U32_MEP_TABLE_BEGIN_ROW + u32Row + 1;
                doc.write(rowY, U32_MEP_TABLE_BEGIN_COL, rowDistance, headerFormat);
                for(quint32 u32Col = 0 ; u32Col < mepSize.columns ; u32Col++) {
                    quint32 colX = U32_MEP_TABLE_BEGIN_COL + u32Col + 1;
                    if(0 == u32Row) { // Write header for first line
                        doc.write(U32_MEP_TABLE_BEGIN_ROW, colX, u32Col + 1, headerFormat);
                    }
                    doc.write(rowY, colX, pMatrix->getAScanAt(u32Row, u32Col)->getAScanMeasure()->m_thicknessMm1Echo, cellFormat);
                }
            }

            // -- Write file :
            QString fileName = p_fileName.right(p_fileName.length()-8); // remove "file:///" appenened by qml
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
