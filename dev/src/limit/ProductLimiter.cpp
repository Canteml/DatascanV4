#include "ProductLimiter.h"

#include "CoreApp.h"
#include <v2/vector/VectorManager.h>
#include <v2/vector/VectorStd.h>

#include <QFile>
#include <QDebug>

#define EQUIPMENT_FILE_NAME "EquipmentFile.txt"
#define DEFAULT_CONTENT "10000000"

ProductLimiter::ProductLimiter(QObject *p_pParent) : QObject(p_pParent)
{

}

QStringList ProductLimiter::getEnabledProductsNames()
{
    QList<QString> productNames;
    for(ProtMsg::PROD_ID prodId : m_aEnabledProducts) {
        productNames.append(ProtMsg::prodName(prodId));
    }
    return productNames;
}

void ProductLimiter::setProdId(qint32 p_s32ProdId)
{
    if(p_s32ProdId <= ProtMsg::PROD_UNDEF || p_s32ProdId >= ProtMsg::PROD_MAX_ID) {
        return;
    }
    VectorStd* currentVector = VectorManager::getInstance()->getVectorStd();
    if(currentVector->getProdId() != p_s32ProdId) {
        currentVector->setProdIdDeviceAd(p_s32ProdId, 0);
    }
}

qint32 ProductLimiter::getCurrentProdId()
{
    VectorStd* currentVector = VectorManager::getInstance()->getVectorStd();
    return static_cast<qint32>(currentVector->getProdId());
}

QString ProductLimiter::getEquipmentFilePath()
{
    return CoreApp::getDirAppBin() + QString("/") + EQUIPMENT_FILE_NAME;
}

void ProductLimiter::loadLimitations()
{
    m_aEnabledProducts.clear();
    QFile equipmentFile(getEquipmentFilePath());
    if(!equipmentFile.exists()) {
        initDefaultEquipmentFile();
    }
    if (!equipmentFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open equipment file for reading";
        return;
    }
    QByteArray content = equipmentFile.readAll();
    equipmentFile.close();
    int prodId = 1;
    for(char currentChar : content) {
        if(prodId >= ProtMsg::PROD_MAX_ID) {
            break;
        }
        if('1' == currentChar) {
            m_aEnabledProducts.append(static_cast<ProtMsg::PROD_ID>(prodId));
        }
        prodId++;
    }
    if(0 == m_aEnabledProducts.size()) {
        initDefaultEquipmentFile();
        m_aEnabledProducts.append(ProtMsg::PROD_UT_POLE);
    }
    VectorStd* currentVector = VectorManager::getInstance()->getVectorStd();
    ProtMsg::PROD_ID currentId = static_cast<ProtMsg::PROD_ID>(currentVector->getProdId());
    if(!m_aEnabledProducts.contains(currentId)) {
        currentVector->setProdIdDeviceAd(static_cast<qint32>(m_aEnabledProducts[0]), 0);
    }
}

void ProductLimiter::initDefaultEquipmentFile()
{
    QFile equipmentFile(getEquipmentFilePath());
    if (!equipmentFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open equipment file for writing";
        return;
    }
    equipmentFile.write(DEFAULT_CONTENT);
    equipmentFile.close();
}
