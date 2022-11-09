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

bool ProductLimiter::isProductEnabled(ProtMsg::PROD_ID p_prodId)
{
    return m_aEnabledProducts.contains(p_prodId);
}

ProtMsg::PROD_ID ProductLimiter::getProductIdFromIndex(qint32 p_u32Index)
{
    return m_aEnabledProducts[p_u32Index];
}

QStringList ProductLimiter::getEnabledProductsNames()
{
    QList<QString> productNames;
    for(ProtMsg::PROD_ID prodId : m_aEnabledProducts) {
        productNames.append(ProtMsg::prodName(prodId));
    }
    return productNames;
}

void ProductLimiter::setProdIndex(qint32 p_u32Index)
{
    ProtMsg::PROD_ID prodId = getProductIdFromIndex(p_u32Index);
    qint32 intProdId = static_cast<qint32>(prodId);
    VectorStd* currentVector = VectorManager::getInstance()->getVectorStd();
    if(currentVector->getProdId() != intProdId) {
        currentVector->setProdIdDeviceAd(intProdId, 0);
    }
}

qint32 ProductLimiter::getCurrentIndex()
{
    VectorStd* currentVector = VectorManager::getInstance()->getVectorStd();
    ProtMsg::PROD_ID currentId = static_cast<ProtMsg::PROD_ID>(currentVector->getProdId());
    return m_aEnabledProducts.indexOf(currentId);
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
    equipmentFile.open(QIODevice::ReadOnly | QIODevice::Text);
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
    emit enabledProductsChanged(getEnabledProductsNames());
    VectorStd* currentVector = VectorManager::getInstance()->getVectorStd();
    ProtMsg::PROD_ID currentId = static_cast<ProtMsg::PROD_ID>(currentVector->getProdId());
    if(!m_aEnabledProducts.contains(currentId)) {
        currentVector->setProdIdDeviceAd(static_cast<qint32>(m_aEnabledProducts[0]), 0);
    }
}

void ProductLimiter::initDefaultEquipmentFile()
{
    QFile equipmentFile(getEquipmentFilePath());
    equipmentFile.open(QIODevice::WriteOnly | QIODevice::Text);
    equipmentFile.write(DEFAULT_CONTENT);
    equipmentFile.close();
}
