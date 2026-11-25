#pragma once

#include <v2/prot/ProtMsg.h>
#include <QObject>

class ProductLimiter : public QObject
{

    Q_OBJECT

public:

    ProductLimiter(QObject* p_pParent = nullptr);

    QStringList getEnabledProductsNames();

    Q_INVOKABLE void setProdId(qint32 p_s32ProdId);
    Q_INVOKABLE qint32 getCurrentProdId();

    QString getEquipmentFilePath();

public slots:

    void loadLimitations();
    void initDefaultEquipmentFile();

private:

    QList<ProtMsg::PROD_ID> m_aEnabledProducts;

};
