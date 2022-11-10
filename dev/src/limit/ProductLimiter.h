#pragma once

#include <v2/prot/ProtMsg.h>
#include <QObject>

class ProductLimiter : public QObject
{

    Q_OBJECT

    Q_PROPERTY(QStringList enabledProducts READ getEnabledProductsNames NOTIFY enabledProductsChanged)
    Q_PROPERTY(bool hasPipeEnabled READ isPipeEnabled NOTIFY pipeEnabledChanged)

public:

    ProductLimiter(QObject* p_pParent = nullptr);

    bool isProductEnabled(ProtMsg::PROD_ID p_prodId);

    ProtMsg::PROD_ID getProductIdFromIndex(qint32 p_u32Index);
    Q_INVOKABLE QStringList getEnabledProductsNames();

    Q_INVOKABLE void setProdIndex(qint32 p_u32Index);
    Q_INVOKABLE qint32 getCurrentIndex();

    bool isPipeEnabled();

    QString getEquipmentFilePath();

public slots:

    void loadLimitations();
    void initDefaultEquipmentFile();

signals:

    void enabledProductsChanged(QStringList);
    void pipeEnabledChanged(bool);

private:

    QList<ProtMsg::PROD_ID> m_aEnabledProducts;

};
