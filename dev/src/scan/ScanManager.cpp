/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : AscanModel.h
# Author      : Thierry Lechevallier
# Date        : 26/07/2021
#
# ==========================================================================*/

// -- Internal includes
#include "ScanManager.h"
#include <v2/vector/VectorStd.h>
#include <v2/vector/VectorManager.h>
#include <v2/vector/VectorFuncAScan.h>
#include <v2/scan/Location1d.h>
#include <scan/AScanGraph.h>
#include <scan/BScanGraph.h>
#include "jkPlot/JkGate.h"
#include <v2/exam/ExamManager.h>

// -- Qt Includes
#include <QQmlApplicationEngine>
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDomDocument>
#include <QDir>
#include <QRegularExpression>
#include <QDateTime>
#include <QElapsedTimer>


ScanManager* ScanManager::m_pMyInstance = nullptr;

ScanManager::ScanManager() : XmlSettingsLoggable("ScanManger")
{
    m_pAScanGraphMain = nullptr;
    m_pBScanGraphMain = nullptr;
    m_pGate0 = nullptr;
    m_pGate1 = nullptr;
    m_pGateNoise = nullptr;
    m_pVector = nullptr;
    m_selectedAScanIndex = 0;
    m_lastUpdateAScan = 0;

    m_timUpdateAScan = new QTimer();
    m_timUpdateAScan->setSingleShot(true);
    connect(m_timUpdateAScan, &QTimer::timeout, this, &ScanManager::onUpdateAScanTimed);

    for(int i=0; i<VectorStd::NB_FUNC_ASCAN; i++) {
        m_pAScan[i].init();
    }

}
ScanManager::~ScanManager()
{
    if( m_timUpdateAScan->isActive() ) {
        m_timUpdateAScan->stop();
    }
    delete m_timUpdateAScan;
    m_timUpdateAScan = nullptr;

    if( nullptr != m_pGate0) {
        delete m_pGate0;
        m_pGate0 = nullptr;
    }
    if( nullptr != m_pGate1) {
        delete m_pGate1;
        m_pGate1 = nullptr;
    }
    if( nullptr != m_pGateNoise) {
        delete m_pGateNoise;
        m_pGateNoise = nullptr;
    }

    if( nullptr != m_pAScanGraphMain ) {
        delete m_pAScanGraphMain;
        m_pAScanGraphMain = nullptr;
    }
    if( nullptr != m_pBScanGraphMain ) {
        delete m_pBScanGraphMain;
        m_pBScanGraphMain = nullptr;
    }
}


ScanManager *ScanManager::makeInstance()
{
    if( nullptr != m_pMyInstance ) {
        Logger::logError("ScanManager::makeInstance instance already exists !");
    } else {
        m_pMyInstance = new ScanManager();
    }

    return m_pMyInstance;
}

ScanManager *ScanManager::getInstance()
{
    return m_pMyInstance;
}

void ScanManager::init()
{
    m_pVector = VectorManager::getInstance()->getVectorStd();
    m_pExamManager = ExamManager::getInstance();

    connect(m_pVector->funcAScan0(), &VectorFuncAScan::evNewAScanTreated, this, &ScanManager::onNewAScanTreated);
    connect(m_pVector->funcAScan1(), &VectorFuncAScan::evNewAScanTreated, this, &ScanManager::onNewAScanTreated);
    connect(m_pVector->funcAScan2(), &VectorFuncAScan::evNewAScanTreated, this, &ScanManager::onNewAScanTreated);
    connect(m_pVector->funcAScan3(), &VectorFuncAScan::evNewAScanTreated, this, &ScanManager::onNewAScanTreated);
    connect(m_pVector, &VectorStd::evNbFuncAScanEnabledChanged, this, &ScanManager::onVectorNbAScanEnabledChanged);

    connect(m_pExamManager, &ExamManager::evExamStepChanged, this, &ScanManager::onExamManagerStepChanged);
}

void ScanManager::initQmlObject(QObject * p_pQmlTopLevel)
{
    m_pAScanGraphMain = p_pQmlTopLevel->findChild<AScanGraph*>("AScanGraphMain");    
    if( nullptr == m_pAScanGraphMain ) {
        fLogError("Fail to found m_pAScanGraphMain");
    } else {
        m_pAScanGraphMain->setParent(this);
        m_pGate0 = new JkGate(m_pAScanGraphMain->getPlot(), 0);
        m_pGate1 = new JkGate(m_pAScanGraphMain->getPlot(), 1);
        m_pGateNoise = new JkGate(m_pAScanGraphMain->getPlot(), 2);
        setPrivateSelectedAScanIndex(0);
        m_pAScanGraphMain->initAfterItemAdded();
    }

    m_pBScanGraphMain = p_pQmlTopLevel->findChild<BScanGraph*>("BScanGraphMain");
    if( nullptr == m_pBScanGraphMain ) {
        fLogError("Fail to found m_pBScanGraphMain");
    } else {
        connect(m_pBScanGraphMain, &BScanGraph::evViewXModeChanged, this, &ScanManager::onPrivateBScanViewXModeChanged);
        connect(m_pBScanGraphMain, &BScanGraph::evViewYModeChanged, this, &ScanManager::onPrivateBScanViewYModeChanged);
        connect(m_pBScanGraphMain, &BScanGraph::evAxeXScaleMmChanged, this, &ScanManager::onPrivateBScanAxeXScaleMChanged);
        connect(m_pBScanGraphMain, &BScanGraph::evAxeYMinMmChanged, this, &ScanManager::onPrivateBScanAxeYMinMmChanged);
        connect(m_pBScanGraphMain, &BScanGraph::evAxeYMaxMmChanged, this, &ScanManager::onPrivateBScanAxeYMaxMmChanged);
        connect(m_pBScanGraphMain, &BScanGraph::evAxeXTypeChanged, this, &ScanManager::onPrivateBScanAxeXTypeChanged);

        m_pBScanGraphMain->setParent(this);
        m_pBScanGraphMain->initAfterItemAdded();
    }
}

void ScanManager::stop()
{    
    if( nullptr != m_pAScanGraphMain ) {
        m_pAScanGraphMain->stop();
    }
    if( nullptr != m_pBScanGraphMain ) {
        m_pBScanGraphMain->stop();
        disconnect(m_pBScanGraphMain, &BScanGraph::evViewXModeChanged, this, &ScanManager::onPrivateBScanViewXModeChanged);
        disconnect(m_pBScanGraphMain, &BScanGraph::evViewYModeChanged, this, &ScanManager::onPrivateBScanViewYModeChanged);
        disconnect(m_pBScanGraphMain, &BScanGraph::evAxeXScaleMmChanged, this, &ScanManager::onPrivateBScanAxeXScaleMChanged);
        disconnect(m_pBScanGraphMain, &BScanGraph::evAxeYMinMmChanged, this, &ScanManager::onPrivateBScanAxeYMinMmChanged);
        disconnect(m_pBScanGraphMain, &BScanGraph::evAxeYMaxMmChanged, this, &ScanManager::onPrivateBScanAxeYMaxMmChanged);
        disconnect(m_pBScanGraphMain, &BScanGraph::evAxeXTypeChanged, this, &ScanManager::onPrivateBScanAxeXTypeChanged);
    }
}

quint32 ScanManager::getSelectedAScanIndex() const
{
    return m_selectedAScanIndex;
}

void ScanManager::setSelectedAScanIndex(quint32 p_index)
{
    if( p_index != m_selectedAScanIndex ) {
        setPrivateSelectedAScanIndex(p_index);
    }
}

VectorFuncAScan* ScanManager::getSelectedFuncAScan() const
{
    return m_pVector->getFuncAScan(m_selectedAScanIndex);
}

AScan *ScanManager::getCurrentAScan()
{
    return &(m_pAScan[m_selectedAScanIndex]);
}

void ScanManager::ascanZoomIn()
{
    if( nullptr != m_pAScanGraphMain ) {
        m_pAScanGraphMain->zoomIn();
    }
}

void ScanManager::ascanZoomOut()
{
    if( nullptr != m_pAScanGraphMain ) {
        m_pAScanGraphMain->zoomOut();
    }
}

qreal ScanManager::getAScanThicknessMmMultiEcho()
{
    return getCurrentAScan()->getAScanMeasure()->m_thicknessMmMultiEcho;
}

qreal ScanManager::getAScanThicknessMm1Echo()
{
    return getCurrentAScan()->getAScanMeasure()->m_thicknessMm1Echo;
}

static const QString ASCAN_USER_PARAMS_FILE = "/ascan_user_params.xml";

void ScanManager::ascanSaveUserParams()
{
    if( nullptr == m_pVector ) return;
    VectorFuncAScan* pAscan = getSelectedFuncAScan();
    if( nullptr == pAscan ) return;
    pAscan->writeProjectToFile(QCoreApplication::applicationDirPath() + ASCAN_USER_PARAMS_FILE);
}

void ScanManager::ascanLoadUserParams()
{
    if( nullptr == m_pVector ) return;
    VectorFuncAScan* pAscan = getSelectedFuncAScan();
    if( nullptr == pAscan ) return;
    const QString fileName = QCoreApplication::applicationDirPath() + ASCAN_USER_PARAMS_FILE;
    if( !QFile::exists(fileName) ) return;  // nothing saved yet
    pAscan->readProjectFromFile(fileName);
    emit evSelectedFuncAScanChanged(pAscan);
}

void ScanManager::ascanResetDefaultParams()
{
    if( nullptr == m_pVector ) return;
    VectorFuncAScan* pAscan = getSelectedFuncAScan();
    if( nullptr == pAscan ) return;
    pAscan->setParamDGain(7);
    pAscan->setParamDVoltage(100);
    pAscan->setParamDUtElement(2);
    pAscan->setParamDSampleFreq(0);
    pAscan->setParamDPulseWidth(18);
    pAscan->setParamLCelerity(5940);
    pAscan->setParamLProbeDelayMm(14.4);
    pAscan->setParamLLowPassFilter(25);
    pAscan->setParamDFilterMode(2);
    pAscan->setParamDWaveType(0);
    pAscan->setParamDGate0Option(0);
    pAscan->setParamDGate1Option(0);
    pAscan->setParamLGate0Start(4);
    pAscan->setParamLGate0Width(37);
    pAscan->setParamLGate0Height(11);
    pAscan->setParamLGate1Start(6);
    pAscan->setParamLGate1Width(35);
    pAscan->setParamLGate1Height(10);

    emit evSelectedFuncAScanChanged(pAscan);
}

static const QString ASCAN_PRESETS_DIR = "/ascan_presets";

static QString ascanPresetFilePath(const QString& p_name)
{
    QString name = p_name.trimmed();
    name.replace(QRegularExpression("[\\\\/:*?\"<>|]"),"_");
    if( name.isEmpty() ) return QString();
    return QCoreApplication::applicationDirPath() + ASCAN_PRESETS_DIR + "/" + name + ".xml";
}

bool ScanManager::ascanSavePreset(const QString& p_name)
{
    if(nullptr == m_pVector) return false;
    VectorFuncAScan* pAScan = getSelectedFuncAScan();
    if(nullptr == pAScan) return false;

    const QString fileName = ascanPresetFilePath(p_name);
    if(fileName.isEmpty() ) return false;

    if( !QDir().mkpath(QCoreApplication::applicationDirPath() + ASCAN_PRESETS_DIR) ) return false;

    if( !pAScan->writeProjectToFile(fileName) ) return false;
    emit evAScanPresetChanged();
    return true;
}

bool ScanManager::ascanPresetExists(const QString& p_name) const
{
    const QString fileName = ascanPresetFilePath(p_name);
    return !fileName.isEmpty() && QFile::exists(fileName);
}

QStringList ScanManager::ascanListPresets() const
{
    QDir dir(QCoreApplication::applicationDirPath() + ASCAN_PRESETS_DIR);
    if( !dir.exists() ) return QStringList();

    QStringList names;

    const QFileInfoList files = dir.entryInfoList(QStringList() << "*.xml", QDir::Files, QDir::Time);
    for( const QFileInfo& fi : files ) {
        names << fi.completeBaseName();
    }
    return names;
}

bool ScanManager::ascanLoadPreset(const QString& p_name)
{
    if( nullptr == m_pVector ) return false;
    VectorFuncAScan* pAScan = getSelectedFuncAScan();
    if( nullptr == pAScan) return false;

    const QString fileName = ascanPresetFilePath(p_name);
    if( fileName.isEmpty() || !QFile::exists(fileName) ) return false;
    if( !pAScan->readProjectFromFile(fileName)) return false;

    emit evSelectedFuncAScanChanged(pAScan);
    ascanSaveUserParams();
    return true;
}

static const QString BSCAN_USER_PARAMS_FILE = "/bscan_user_params.xml";

void ScanManager::setEncoderController(EncoderController* p_pEncoder)
{
    m_pEncoderController = p_pEncoder;
}

bool ScanManager::bscanWriteParamsToFile(const QString& p_fileName)
{
    if( nullptr == m_pBScanGraphMain || nullptr == m_pExamManager ) return false;

    QDomDocument doc;
    QDomNode xmlNode = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(xmlNode);
    QDomElement root = doc.createElement("root");
    doc.appendChild(root);

    XmlHelper::xmlWriteUniqueParameter(&doc, &root, "BScanAxeXScaleM",    QVariant(getBScanAxeXScaleM()));
    XmlHelper::xmlWriteUniqueParameter(&doc, &root, "BScanAxeYMinMm",     QVariant(getBScanAxeYMinMm()));
    XmlHelper::xmlWriteUniqueParameter(&doc, &root, "BScanAxeYMaxMm",     QVariant(getBScanAxeYMaxMm()));
    XmlHelper::xmlWriteUniqueParameter(&doc, &root, "ZoneAutoHoleSizeMm", QVariant(m_pExamManager->getZoneAutoHoleSizeMm()));

    if( nullptr != m_pEncoderController ) {
        XmlHelper::xmlWriteUniqueParameter(&doc, &root, "EncoderZeroMm",         QVariant(m_pEncoderController->zeroMm()));
        XmlHelper::xmlWriteUniqueParameter(&doc, &root, "EncoderCalibRealMm",     QVariant(m_pEncoderController->calibrationRealMm()));
        XmlHelper::xmlWriteUniqueParameter(&doc, &root, "EncoderCalibEstimatedMm",QVariant(m_pEncoderController->calibrationEstimatedMm()));
    }

    QFile file(p_fileName);
    if( !file.open(QIODevice::WriteOnly | QIODevice::Text) ) return false;
    QTextStream stream(&file);
    stream << doc.toString();
    file.close();
    return true;
}

bool ScanManager::bscanReadParamsFromFile(const QString& p_fileName)
{
    if( nullptr == m_pBScanGraphMain || nullptr == m_pExamManager ) return false;

    QFile file(p_fileName);
    if( !file.open(QIODevice::ReadOnly) ) return false;

    QDomDocument doc;
    if( !doc.setContent(&file) ) { file.close(); return false; }
    file.close();

    QDomElement root = doc.documentElement();
    if( root.isNull() || root.nodeName() != "root" ) return false;

    qreal xScale     = getBScanAxeXScaleM();
    qreal yMin       = getBScanAxeYMinMm();
    qreal yMax       = getBScanAxeYMaxMm();
    qreal zoneHoleMm = m_pExamManager->getZoneAutoHoleSizeMm();

    XmlHelper::xmlReadUniqueParameter<qreal>(&root, xScale,     "BScanAxeXScaleM");
    XmlHelper::xmlReadUniqueParameter<qreal>(&root, yMin,       "BScanAxeYMinMm");
    XmlHelper::xmlReadUniqueParameter<qreal>(&root, yMax,       "BScanAxeYMaxMm");
    XmlHelper::xmlReadUniqueParameter<qreal>(&root, zoneHoleMm, "ZoneAutoHoleSizeMm");

    setBScanAxeXScaleM(xScale);
    setBScanAxeYMinMm(0);
    setBScanAxeYMaxMm(yMax);
    setBScanAxeYMinMm(yMin);
    m_pExamManager->setZoneAutoHoleSizeMm(zoneHoleMm);

    if( nullptr != m_pEncoderController ) {
        qreal encZero      = m_pEncoderController->zeroMm();
        qreal encRealMm    = m_pEncoderController->calibrationRealMm();
        qreal encEstimMm   = m_pEncoderController->calibrationEstimatedMm();
        XmlHelper::xmlReadUniqueParameter<qreal>(&root, encZero,    "EncoderZeroMm");
        XmlHelper::xmlReadUniqueParameter<qreal>(&root, encRealMm,  "EncoderCalibRealMm");
        XmlHelper::xmlReadUniqueParameter<qreal>(&root, encEstimMm, "EncoderCalibEstimatedMm");
        m_pEncoderController->setCalibration(encZero, encRealMm, encEstimMm);
    }
    return true;
}

void ScanManager::bscanSaveUserParams()
{
    bscanWriteParamsToFile(QCoreApplication::applicationDirPath() + BSCAN_USER_PARAMS_FILE);
}

void ScanManager::bscanLoadUserParams()
{
    bscanReadParamsFromFile(QCoreApplication::applicationDirPath() + BSCAN_USER_PARAMS_FILE);
}

static const QString BSCAN_PRESET_DIR = "/bscan_presets";

static QString bscanPresetFilePath(const QString& p_name)
{
    QString name = p_name.trimmed();
    name.replace(QRegularExpression("[\\\\/:*?\"<>|]"),"_");
    if(name.isEmpty()) return QString();
    return QCoreApplication::applicationDirPath() + BSCAN_PRESET_DIR + "/" + name +".xml";
}

bool ScanManager::bscanSavePreset(const QString& p_name)
{
    const QString fileName = bscanPresetFilePath(p_name);
    if(fileName.isEmpty()) return false;

    if(!QDir().mkpath(QCoreApplication::applicationDirPath() + BSCAN_PRESET_DIR)) return false;

    if(!bscanWriteParamsToFile(fileName)) return false;
    emit evBScanPresetChanged();
    return true;
}

bool ScanManager::bscanPresetExists(const QString& p_name) const
{
    const QString fileName = bscanPresetFilePath(p_name);
    return !fileName.isEmpty() && QFile::exists(fileName);
}

QStringList ScanManager::bscanListPresets() const
{
    QDir dir(QCoreApplication::applicationDirPath() + BSCAN_PRESET_DIR);
    if(!dir.exists()) return QStringList();

    QStringList names;

    const QFileInfoList files = dir.entryInfoList(QStringList() << "*.xml", QDir::Files, QDir::Time);
    for(const QFileInfo& fi : files){
        names << fi.completeBaseName();
    }
    return names;
}

bool ScanManager::bscanLoadPreset(const QString& p_name)
{
    const QString fileName = bscanPresetFilePath(p_name);
    if(fileName.isEmpty() || !QFile::exists(fileName)) return false;
    if(!bscanReadParamsFromFile(fileName)) return false;

    bscanSaveUserParams();
    return true;
}

void ScanManager::bscanResetDefaultParams()
{
    setBScanAxeXScaleM(2);
    setBScanAxeYMaxMm(10);
    setBScanAxeYMinMm(0);
    if( nullptr != m_pExamManager ) {
        m_pExamManager->setZoneAutoHoleSizeMm(30);
    }
    if( nullptr != m_pEncoderController ) {
        m_pEncoderController->resetCalibration();
    }
}

// -- BSCAN

qint32 ScanManager::getBScanViewXMode(void) const
{
    if( nullptr != m_pBScanGraphMain ) {
        return m_pBScanGraphMain->getViewXMode();
    }
    return BScanGraph::VIEW_X_AUTO;
}

void ScanManager::setBScanViewXMode(quint32 p_value)
{
    if( nullptr != m_pBScanGraphMain ) {
        m_pBScanGraphMain->setViewXMode(p_value);
    }
}

qint32 ScanManager::getBScanViewYMode(void) const
{
    if( nullptr != m_pBScanGraphMain ) {
        return m_pBScanGraphMain->getViewYMode();
    }
    return BScanGraph::VIEW_Y_AUTO;
}

void ScanManager::setBScanViewYMode(quint32 p_value)
{
    if( nullptr != m_pBScanGraphMain ) {
        m_pBScanGraphMain->setViewYMode(p_value);
    }
}

qreal ScanManager::getBScanAxeXScaleM(void) const
{
    if( nullptr != m_pBScanGraphMain ) {
        return m_pBScanGraphMain->getAxeXScaleMm();
    }
    return BScanGraph::SCALE_X_M_MIN;
}

void ScanManager::setBScanAxeXScaleM(qreal p_value)
{
    if( nullptr != m_pBScanGraphMain ) {
        return m_pBScanGraphMain->setAxeXScaleMm(p_value);
    }
}

qreal ScanManager::getBScanAxeYMinMm(void) const
{
    if( nullptr != m_pBScanGraphMain ) {
        return m_pBScanGraphMain->getAxeYMinMm();
    }
    return 0;
}

void ScanManager::setBScanAxeYMinMm(qreal p_value)
{
    if( nullptr != m_pBScanGraphMain ) {
        return m_pBScanGraphMain->setAxeYMinMm(p_value);
    }
}

qreal ScanManager::getBScanAxeYMaxMm(void) const
{
    if( nullptr != m_pBScanGraphMain ) {
        return m_pBScanGraphMain->getAxeYMaxMm();
    }
    return 0;
}

void ScanManager::setBScanAxeYMaxMm(qreal p_value)
{
    if( nullptr != m_pBScanGraphMain ) {
        return m_pBScanGraphMain->setAxeYMaxMm(p_value);
    }
}

qint32 ScanManager::getBScanAxeXType(void) const
{
    if( nullptr != m_pBScanGraphMain ) {
        return m_pBScanGraphMain->getAxeXType();
    }
    return BScanGraph::AXE_X_DISTANCE;
}

void ScanManager::setBScanAxeXType(quint32 p_value)
{
    if( nullptr != m_pBScanGraphMain ) {
        m_pBScanGraphMain->setAxeXType(p_value);
    }
}

void ScanManager::onNewAScanTreated(AScan * p_pAscan)
{
    {
        static QElapsedTimer s_window;
        static qint64 s_maxAgeMs = 0;
        static int    s_count    = 0;
        const qint64 ageMs = QDateTime::currentMSecsSinceEpoch() - p_pAscan->getAScanRawSample()->m_epocTimeMs;
        s_maxAgeMs = qMax(s_maxAgeMs, ageMs);
        ++s_count;
        if( !s_window.isValid() ) {
            s_window.start();
        } else if( s_window.elapsed() >= 1000 ) {
            fLogInfo(QString("[AScanLatency] %1 A-scans/s, last age=%2 ms, max age=%3 ms")
                     .arg(s_count).arg(ageMs).arg(s_maxAgeMs));
            s_maxAgeMs = 0;
            s_count    = 0;
            s_window.restart();
        }
    }

    if( p_pAscan->m_source == m_selectedAScanIndex && nullptr != m_pAScanGraphMain ) {
        m_pAScan[p_pAscan->m_source].copy(*p_pAscan);
        m_pAScanGraphMain->updateAScan(&m_pAScan[p_pAscan->m_source]);
        emitUpdateAScanTimed();
    }

    if(0 == p_pAscan->m_source && nullptr != m_pBScanGraphMain
        && ExamManager::EXAM_PAUSED != m_pExamManager->getExamStep()
        && ExamManager::EXAM_EXPORTING != m_pExamManager->getExamStep() ){
        m_pBScanGraphMain->pushTimeMeasure(p_pAscan->getAScanRawSample()->m_epocTimeMs,
                                           p_pAscan->getAScanMeasure()->m_thicknessMmMultiEcho,
                                           p_pAscan->getAScanMeasure()->m_thicknessMm1Echo);
    }
    m_pExamManager->pushAScan(p_pAscan);
}

void ScanManager::onVectorNbAScanEnabledChanged(qint32)
{
//    // clear ASCAN
//    for(int i=0; i<VectorStd::NB_FUNC_ASCAN; i++) {
//        delete m_pAScan[i];
//        m_pAScan[i] = nullptr;
//    }
    setSelectedAScanIndex(0);
}

void ScanManager::onPrivateBScanViewXModeChanged(qint32 p_value)
{
    emit evBScanViewXModeChanged(p_value);
}

void ScanManager::onPrivateBScanViewYModeChanged(qint32 p_value)
{
    emit evBScanViewYModeChanged(p_value);
}

void ScanManager::onPrivateBScanAxeXScaleMChanged(qreal p_value)
{
    emit evBScanAxeXScaleMChanged(p_value);
}

void ScanManager::onPrivateBScanAxeYMinMmChanged(qreal p_value)
{
    emit evBScanAxeYMinMmChanged(p_value);
}

void ScanManager::onPrivateBScanAxeYMaxMmChanged(qreal p_value)
{
    emit evBScanAxeYMaxMmChanged(p_value);
}

void ScanManager::onPrivateBScanAxeXTypeChanged(qint32 p_value)
{
    emit evBScanAxeXTypeChanged(p_value);
}

void ScanManager::onExamManagerStepChanged(qint32 p_value)
{
    if( p_value == ExamManager::EXAM_EXPORTING ) {
        if(m_pBScanGraphMain->getAxeXType() != BScanGraph::AXE_X_DISTANCE){
            m_pBScanGraphMain->setAxeXType(BScanGraph::AXE_X_DISTANCE);
        }
        if( m_pBScanGraphMain->getViewXMode() != BScanGraph::VIEW_X_FULL ) {
            m_pBScanGraphMain->setViewXMode(BScanGraph::VIEW_X_FULL);
        }
        if(  m_pBScanGraphMain->getViewYMode() != BScanGraph::VIEW_Y_FULL ) {
            m_pBScanGraphMain->setViewYMode(BScanGraph::VIEW_Y_FULL);
        }
    } else if( p_value == ExamManager::EXAM_RECORDING || p_value == ExamManager::EXAM_STOPPED  || p_value == ExamManager::EXAM_PAUSED) {
        // Go to Auto if full
        if( m_pBScanGraphMain->getViewXMode() == BScanGraph::VIEW_X_FULL ) {
             m_pBScanGraphMain->setViewXMode(BScanGraph::VIEW_X_AUTO);
        }
        if( m_pBScanGraphMain->getViewYMode() == BScanGraph::VIEW_Y_FULL ) {
            m_pBScanGraphMain->setViewYMode(BScanGraph::VIEW_Y_USER);
        }

        // When go to STOPPED (at the end of one acquisition), clear BSCAN (even if the vector is disconnected)
        if( p_value == ExamManager::EXAM_STOPPED ) {
            m_pBScanGraphMain->clearBScan();
        }
    }
}

void ScanManager::onUpdateAScanTimed()
{
    emitUpdateAScanTimed(true);
}

bool ScanManager::customLoadSettings(QDomElement * p_pParent)
{
    bool bRet = true;
    if( nullptr != m_pAScanGraphMain ) {
        bRet &= m_pAScanGraphMain->loadSettings(p_pParent);
    }
    if( nullptr != m_pBScanGraphMain ) {
        bRet &= m_pBScanGraphMain->loadSettings(p_pParent);
    }
    return bRet;
}

bool ScanManager::customLoadDefaultSettings()
{
    bool bRet = true;

    return bRet;
}

bool ScanManager::customApplySettings()
{
    bool bRet = true;

    if (m_pGate0) m_pGate0->setAScan(getSelectedFuncAScan());
    if (m_pGate1) m_pGate1->setAScan(getSelectedFuncAScan());
    if (m_pGateNoise) m_pGateNoise->setAScan(getSelectedFuncAScan());
    pushAutoGainParamsToGraph();

    return bRet;
}

bool ScanManager::customSaveSettings(QDomDocument * /*p_pDoc*/, QDomElement * p_pParent)
{
    bool bRet = true;
    if( nullptr != m_pAScanGraphMain ) {
        bRet &= m_pAScanGraphMain->saveSettings(p_pParent);
    }
    if( nullptr != m_pBScanGraphMain ) {
        bRet &= m_pBScanGraphMain->saveSettings(p_pParent);
    }
    return bRet;
}


void ScanManager::setPrivateSelectedAScanIndex(quint32 p_index)
{
    m_selectedAScanIndex = p_index;
    emit evSelectedAScanIndexChanged(m_selectedAScanIndex);
    emit evSelectedFuncAScanChanged(getSelectedFuncAScan());
    m_pAScanGraphMain->updateAScan(&m_pAScan[p_index]);
    m_pGate0->setAScan(getSelectedFuncAScan());
    m_pGate1->setAScan(getSelectedFuncAScan());
    m_pGateNoise->setAScan(getSelectedFuncAScan());

    if (m_pConnectedAutoGainAScan) {
        disconnect(m_pConnectedAutoGainAScan, &VectorFuncAScan::paramDAutoGainChanged,          this, &ScanManager::onAutoGainParamsChanged);
        disconnect(m_pConnectedAutoGainAScan, &VectorFuncAScan::paramDAutoGainThresholdChanged,  this, &ScanManager::onAutoGainParamsChanged);
        disconnect(m_pConnectedAutoGainAScan, &VectorFuncAScan::paramLGateNoiseStartChanged,     this, &ScanManager::onAutoGainParamsChanged);
        disconnect(m_pConnectedAutoGainAScan, &VectorFuncAScan::paramLGateNoiseWidthChanged,     this, &ScanManager::onAutoGainParamsChanged);
        disconnect(m_pConnectedAutoGainAScan, &VectorFuncAScan::paramLGate0StartChanged,         this, &ScanManager::onMeasureGateChanged);
        disconnect(m_pConnectedAutoGainAScan, &VectorFuncAScan::paramLGate0WidthChanged,         this, &ScanManager::onMeasureGateChanged);
    }
    m_pConnectedAutoGainAScan = getSelectedFuncAScan();
    if (m_pConnectedAutoGainAScan) {
        connect(m_pConnectedAutoGainAScan, &VectorFuncAScan::paramDAutoGainChanged,          this, &ScanManager::onAutoGainParamsChanged, Qt::QueuedConnection);
        connect(m_pConnectedAutoGainAScan, &VectorFuncAScan::paramDAutoGainThresholdChanged,  this, &ScanManager::onAutoGainParamsChanged, Qt::QueuedConnection);
        connect(m_pConnectedAutoGainAScan, &VectorFuncAScan::paramLGateNoiseStartChanged,     this, &ScanManager::onAutoGainParamsChanged, Qt::QueuedConnection);
        connect(m_pConnectedAutoGainAScan, &VectorFuncAScan::paramLGateNoiseWidthChanged,     this, &ScanManager::onAutoGainParamsChanged, Qt::QueuedConnection);
        connect(m_pConnectedAutoGainAScan, &VectorFuncAScan::paramLGate0StartChanged,         this, &ScanManager::onMeasureGateChanged, Qt::QueuedConnection);
        connect(m_pConnectedAutoGainAScan, &VectorFuncAScan::paramLGate0WidthChanged,         this, &ScanManager::onMeasureGateChanged, Qt::QueuedConnection);
        pushAutoGainParamsToGraph();
        pushMeasureGateToGraph();
    }

    emitUpdateAScanTimed(true);
}

void ScanManager::onAutoGainParamsChanged()
{
    pushAutoGainParamsToGraph();
}

void ScanManager::onMeasureGateChanged()
{
    pushMeasureGateToGraph();
}

void ScanManager::pushAutoGainParamsToGraph()
{
    if (nullptr == m_pAScanGraphMain || nullptr == m_pConnectedAutoGainAScan) return;

    bool autoGainActive = m_pConnectedAutoGainAScan->paramDAutoGain();

    m_pGateNoise->setIsVisible(autoGainActive);
    qreal threshold = static_cast<qreal>(m_pConnectedAutoGainAScan->paramDAutoGainThreshold());
    m_pGateNoise->setNoiseGateThreshold(threshold);
    m_pGateNoise->setAScan(m_pConnectedAutoGainAScan);

    m_pAScanGraphMain->setAutoGainParams(
        autoGainActive,
        m_pConnectedAutoGainAScan->paramDAutoGainThreshold(),
        m_pConnectedAutoGainAScan->paramLGateNoiseStart(),
        m_pConnectedAutoGainAScan->paramLGateNoiseWidth()
    );
}

void ScanManager::pushMeasureGateToGraph()
{
    if (nullptr == m_pAScanGraphMain || nullptr == m_pConnectedAutoGainAScan) return;
    m_pAScanGraphMain->setMeasureGate(
        m_pConnectedAutoGainAScan->getParamLGate0Start(),
        m_pConnectedAutoGainAScan->getParamLGate0Width()
        );
}

void ScanManager::emitUpdateAScanTimed(bool p_force)
{
    qint64 curTime = QDateTime::currentMSecsSinceEpoch();

    if( (curTime - m_lastUpdateAScan) >= ASCAN_MEASURE_REFRESH_DELAY ) {
        p_force = true;
    }

    if( p_force ) {
        m_lastUpdateAScan = QDateTime::currentMSecsSinceEpoch();
        emit evUpdateAScan();
    } else {
        if( !m_timUpdateAScan->isActive() ) {
            m_timUpdateAScan->start(ASCAN_MEASURE_REFRESH_DELAY - (curTime - m_lastUpdateAScan));
        }
    }
}
