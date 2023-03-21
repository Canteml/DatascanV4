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
        LICENCE_INVALID
    };

    explicit SoftwareLicenceChecker(QObject *parent = nullptr);

    void init();

    bool checkValidity();

    QString getTitleMessage() const;

private:

    void updateLicenceFile();

    SOFTWARE_LICENCE_STATE getLicenceState();

    QString m_strFilePath;
    QString m_strLicence;

    QDate m_dLicenceExpiration;
    QDate m_dApplicationExpiration;

signals:

};
