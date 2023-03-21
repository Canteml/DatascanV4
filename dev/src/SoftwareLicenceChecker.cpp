#include "SoftwareLicenceChecker.h"

#include "CoreApp.h"
#include <QFile>
#include <QMessageBox>

#include <QDebug>

#define STR_LICENCE_FILE_NAME "licence_file.lic"

#define U32_LICENCE_APPROACHING_LIMIT (30 * 3)

SoftwareLicenceChecker::SoftwareLicenceChecker(QObject *parent)
    : QObject{parent}
{
    m_strLicence = QString();
}

void SoftwareLicenceChecker::init()
{
    m_strFilePath = CoreApp::getDirDataLicence() + QString("/") + STR_LICENCE_FILE_NAME;
    QFile licenceFile(m_strFilePath);
    if(licenceFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray baContent = licenceFile.readAll();
        m_strLicence = baContent;
        licenceFile.close();
    }
    updateLicenceFile();
}

bool SoftwareLicenceChecker::checkValidity()
{
    SOFTWARE_LICENCE_STATE state = getLicenceState();
    QString strTitle = tr("Licence alert");
    QString strMessage;
    bool bIsValid = true;
    bool bHasWarning = false;
    qint32 i32LicenceRemainingDays = QDate::currentDate().daysTo(m_dLicenceExpiration);
    qint32 i32ApplicationRemainingDays = QDate::currentDate().daysTo(m_dApplicationExpiration);
    switch(state) {
    case LICENCE_NONE:
        strMessage = tr("Aucune licence n'a été enregistrée\nVeuillez contacter ROBOPLANET pour une nouvelle clef");
        bIsValid = false;
        break;
    case LICENCE_OK:
        break;
    case LICENCE_SOON_EXPIRED:
        strMessage = tr("Votre licence va bientôt s'expirer : %1 jours restant\nVeuillez contacter ROBOPLANET pour renouveler votre abonnement")
                .arg(i32LicenceRemainingDays);
        bHasWarning = true;
        break;
    case LICENCE_EXPIRED:
        strMessage = tr("Votre abonnement pour ce programme a expiré le %1. Si vous souhaitez continuer à utiliser ce programme,\nVeuillez contacter Roboplanet pour renouveler votre abonnement")
                .arg(m_dLicenceExpiration.toString("dd/MM/yyyy"));
        bIsValid = false;
        break;
    case APP_SOON_DEPRECATED:
        strMessage = tr("Votre application va bientôt être obsolète : %1 jours restant\nVeuillez contacter ROBOPLANET pour renouveler le programme")
                .arg(i32ApplicationRemainingDays);
        bHasWarning = true;
        break;
    case APP_DEPRECATED:
        strMessage = tr("Votre application est devenue obsolète\nVeuillez contacter ROBOPLANET pour renouveler le programme");
        bIsValid = false;
        break;
    case LICENCE_INVALID:
        strMessage = tr("Votre licence est invalide\nVeuillez contacter ROBOPLANET pour acquérir une nouvelle licence");
        bHasWarning = true;
        break;
    }
    if(!bIsValid) {
        QMessageBox::critical(0, strTitle, strMessage);
    }
    if(bHasWarning) {
        QMessageBox::warning(0, strTitle, strMessage);
    }
    return bIsValid;
}

QString SoftwareLicenceChecker::getTitleMessage() const
{
    qint32 i32RemainingDays = QDate::currentDate().daysTo(m_dLicenceExpiration);
    if(i32RemainingDays > U32_LICENCE_APPROACHING_LIMIT) {
        return "";
    }
    return tr("Licence : %1 jours restants").arg(i32RemainingDays);
}

void SoftwareLicenceChecker::updateLicenceFile()
{
    QFile licenceFile(m_strFilePath);
    if(!licenceFile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(0, "error", licenceFile.errorString());
        return;
    }

    // Traitements
    licenceFile.write(QDateTime::currentDateTime().toString("dd/MM/YYYY - hh:mm:ss").toUtf8());


    licenceFile.close();
}

SoftwareLicenceChecker::SOFTWARE_LICENCE_STATE SoftwareLicenceChecker::getLicenceState()
{
    if(m_strLicence.isEmpty()) {
        m_dLicenceExpiration = QDate::currentDate();
        return LICENCE_NONE;
    }

    // Traitements avec m_strLicence

    m_dLicenceExpiration.setDate(2023, 2, 25);
    m_dApplicationExpiration.setDate(2023, 7, 25);

    // Automatic

    qint32 i32LicenceRemainingDays = QDate::currentDate().daysTo(m_dLicenceExpiration);
    qint32 i32ApplicationRemainingDays = QDate::currentDate().daysTo(m_dApplicationExpiration);

    if(i32LicenceRemainingDays <= 0) {
        return LICENCE_EXPIRED;
    }
    if(i32ApplicationRemainingDays <= 0) {
        return APP_DEPRECATED;
    }
    if(i32LicenceRemainingDays < U32_LICENCE_APPROACHING_LIMIT) {
        return LICENCE_SOON_EXPIRED;
    }
    if(i32ApplicationRemainingDays < U32_LICENCE_APPROACHING_LIMIT) {
        return APP_SOON_DEPRECATED;
    }

    return LICENCE_OK;
}


