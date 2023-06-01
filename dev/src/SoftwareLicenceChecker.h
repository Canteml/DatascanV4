#pragma once

#include <QObject>
#include <QDate>

class SoftwareLicenceChecker : public QObject
{
    Q_OBJECT

public:

    enum SOFTWARE_LICENCE_STATE {
        LICENCE_NONE = 0,
        LICENCE_OK,
        LICENCE_SOON_EXPIRED,
        LICENCE_EXPIRED,
        APP_SOON_DEPRECATED,
        APP_DEPRECATED,
        PC_NOT_COMPATIBLE,
        LICENCE_INVALID
    };

    enum SOFTWARE_APP {
        APP_DATASCAN_SELECTED=1,
    };


    explicit SoftwareLicenceChecker(QObject *parent = nullptr);

    bool bInit();

    bool checkValidity();

    QString getTitleMessage() const;

private:

    void updateLicenceFile();


    SOFTWARE_LICENCE_STATE getLicenceState();

    QString m_strFilePath;
    QString m_strLicence;
    QByteArray m_baLicence;
    uint8_t m_au8Lic_soft_Crypted[88];
    uint8_t m_u8CodeError;
    SOFTWARE_LICENCE_STATE m_LicenceState;

    QDate m_dLicenceExpiration;
    QDate m_dApplicationExpiration;
    QDate m_dApplicationlastLaunch;


signals:

};
