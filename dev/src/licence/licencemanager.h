#pragma once

#include <QObject>
#include <log/Loggable.h>

#define TIME_LEFT_THRESHOLD_WARINING 60
#define DISTANCE_LEFT_THRESHOLD_WARNING 100

class LicenceManager : public QObject, public Loggable
{
    Q_OBJECT

    Q_PROPERTY(bool licenceValidity READ getLicenceValidity WRITE setLicenceValidity NOTIFY evLicenceValidityChanged)
    Q_PROPERTY(QString getEquipmentKey READ getEquipmentKey WRITE setEquipmentKey NOTIFY evEquipmentKeyChanged)

public:

    ~LicenceManager();

    static LicenceManager * makeInstance();
    static LicenceManager * getInstance();

    void init();

    bool getLicenceValidity();
    QString getEquipmentKey();

public slots:

    //void onLicenceMessageReceived();

    void setLicenceValidity(bool p_bIsValid);
    void setEquipmentKey(QString p_qsEquipmentKey);

    void setLicenceInvalid();
    void setLicenceValid();

signals:

    void evLicenceValidityChanged(bool bIsValid);
    void evEquipmentKeyChanged(QString qsEquipmentKey);

private:

    LicenceManager();

    static LicenceManager* m_pMyInstance;
    bool m_bLicenceIsValid;

    QString m_qsEquipmentKey;

    quint32 m_u32RemainingTime;
    quint32 m_u32RemainingDistance;

};

