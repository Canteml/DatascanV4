#include "SoftwareLicenceChecker.h"

#include "CoreApp.h"
#include <QFile>
#include <QMessageBox>

//#include <QDebug>

#include <iostream>
using namespace std;
#include <QSysInfo>
#include <QStorageInfo>
#include <QSettings>
#include <QProcess>

#include <QRandomGenerator>

#define STR_LICENCE_FILE_NAME "licence_file.lic"

#define U32_LICENCE_APPROACHING_LIMIT (30 * 3)

SoftwareLicenceChecker::SoftwareLicenceChecker(QObject *parent)
    : QObject{parent}
{
    m_strLicence = QString();
}

bool SoftwareLicenceChecker::bInit()
{
m_strFilePath = CoreApp::getDirDataLicence() + QString("/") + STR_LICENCE_FILE_NAME;
QFile licenceFile(m_strFilePath);

    /*
    if(licenceFile.open(QIODevice::ReadOnly | QIODevice::Text)) { //if there is a file licence
        QByteArray baContent = licenceFile.readAll();
        m_strLicence = baContent;
        m_baLicence  = baContent;
        licenceFile.close();

        m_LicenceState = this->getLicenceState();

        return this->checkValidity();

    }

    m_LicenceState= LICENCE_NONE;
    */
    m_LicenceState = LICENCE_OK;
    return this->checkValidity();
}

bool SoftwareLicenceChecker::checkValidity()
{
QString strTitle = tr("Licence Alert");
QString strMessage;
bool bIsValid = true;
bool bHasWarning = false;
qint32 i32LicenceRemainingDays = QDate::currentDate().daysTo(m_dLicenceExpiration);
qint32 i32ApplicationRemainingDays = QDate::currentDate().daysTo(m_dApplicationExpiration);


    switch(m_LicenceState) {
        case LICENCE_NONE:
            strMessage = tr("Aucune licence n'a été enregistrée\nVeuillez contacter votre fournisseur pour une nouvelle clef\ne-mail: contact@roboplanet.fr");
            bIsValid = false;
            break;
        case LICENCE_OK:
            break;
        case LICENCE_SOON_EXPIRED:
            strMessage = tr("Votre licence va bientôt s'expirer : %1 jours restant\nVeuillez contacter votre fournisseur pour renouveler votre abonnement\ne-mail: contact@roboplanet.fr")
                    .arg(i32LicenceRemainingDays);
            bHasWarning = true;
            break;
        case LICENCE_EXPIRED:
            strMessage = tr("Votre abonnement pour ce programme a expiré le %1. Si vous souhaitez continuer à utiliser ce programme,\nVeuillez contacter votre fournisseur pour renouveler votre abonnement")
                    .arg(m_dLicenceExpiration.toString("dd/MM/yyyy"));
            bIsValid = false;
            break;
        case APP_SOON_DEPRECATED:
            strMessage = tr("Votre application va bientôt être obsolète : %1 jours restant\nVeuillez contacter votre fournisseur pour renouveler le programme\ne-mail: contact@roboplanet.fr")
                    .arg(i32ApplicationRemainingDays);
            bHasWarning = true;
            break;
        case APP_DEPRECATED:
            strMessage = tr("Votre application est devenue obsolète\nVeuillez contacter votre fournisseur pour renouveler le programme\ne-mail: contact@roboplanet.fr");
            bIsValid = false;
            break;
        case LICENCE_INVALID:
            strMessage = tr("Votre licence est invalide: \nCode erreur : 0x0%1\nVeuillez contacter votre fournisseur pour acquérir une nouvelle licence\ne-mail: contact@roboplanet.fr")
                    .arg( (qint32) (m_u8CodeError));
            bIsValid = false;
            break;

        case PC_NOT_COMPATIBLE:
            strMessage = tr("Un problème de compatibilité à été détecté\nVeuillez contacter votre fournisseur pour une nouvelle clef\ne-mail: contact@roboplanet.fr");
            bIsValid = false;
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
    uint8_t u8Idx,u8IdxModulo,u8TmpMSB,u8TmpLSB;
    QString qstrTemp;
    QFile licenceFile(m_strFilePath);
    if(!licenceFile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(0, "error", licenceFile.errorString());
        return;
    }

    // Traitements
    //licenceFile.write(QDateTime::currentDateTime().toString("dd/MM/YYYY - hh:mm:ss").toUtf8());
    //licenceFile.write("a");
    //licenceFile.write("b");
    //licenceFile.write("C");

    u8IdxModulo=0;
    for(u8Idx=0; u8Idx< 88;u8Idx++)
    {
        u8TmpMSB = (uint8_t) ((m_au8Lic_soft_Crypted[u8Idx]>>4)&0x0F);
        u8TmpLSB = (uint8_t) ((m_au8Lic_soft_Crypted[u8Idx]>>0)&0x0F);



        if( 0x0A<= u8TmpMSB)
        {
            if(0x0A == u8TmpMSB)  licenceFile.write("A");
            if(0x0B == u8TmpMSB)  licenceFile.write("B");
            if(0x0C == u8TmpMSB)  licenceFile.write("C");
            if(0x0D == u8TmpMSB)  licenceFile.write("D");
            if(0x0E == u8TmpMSB)  licenceFile.write("E");
            if(0x0F == u8TmpMSB)  licenceFile.write("F");
        }
        else
        {
            //std::cout << (char)(u8TmpMSB+'0');
            qstrTemp = QChar((u8TmpMSB+'0'));
            licenceFile.write(qstrTemp.toLocal8Bit() );
        }
        if( 0x0A<= u8TmpLSB)
        {
            if(0x0A == u8TmpLSB)  licenceFile.write("A");
            if(0x0B == u8TmpLSB)  licenceFile.write("B");
            if(0x0C == u8TmpLSB)  licenceFile.write("C");
            if(0x0D == u8TmpLSB)  licenceFile.write("D");
            if(0x0E == u8TmpLSB)  licenceFile.write("E");
            if(0x0F == u8TmpLSB)  licenceFile.write("F");
        }
        else
        {
            //std::cout << (char)(u8TmpLSB+'0');
            qstrTemp = QChar((u8TmpLSB+'0'));
            licenceFile.write(qstrTemp.toLocal8Bit() );
        }

        if(2<=u8IdxModulo)
        {
            licenceFile.write("-");
            u8IdxModulo=0;
        }
        else
        {
            u8IdxModulo++;
        }
    }

    licenceFile.close();
}

SoftwareLicenceChecker::SOFTWARE_LICENCE_STATE SoftwareLicenceChecker::getLicenceState()
{
uint8_t u8Idx, u8Idx2;
uint8_t u8LicenceFile_length;
uint8_t au8Licence_rawFileData[205];
uint8_t au8Licence_uncryptedAscii[176];
uint8_t au8Licence_uncryptedBinary[176];
uint8_t au8Licence_decrypting[88];
uint8_t au8Licence_decrypted[88];
uint8_t u8Tmp, u8TmpMSB, u8TmpLSB;
QString strTmp;
uint64_t u64CRC_compute, u64CRC_ref;
uint8_t u8AppSelected;
int16_t s16Extract_EndDateApp_dd,s16Extract_EndDateApp_mm,s16Extract_EndDateApp_aaaa;
int16_t s16Extract_DateLastRunApp_dd,s16Extract_DateLastRunApp_mm,s16Extract_DateLastRunApp_aaaa;
int16_t s16Extract_EndDateLic_dd,s16Extract_EndDateLic_mm,s16Extract_EndDateLic_aaaa;
int16_t s16Extract_FirstCreationDateLic_dd,s16Extract_FirstCreationDateLic_mm,s16Extract_FirstCreationDateLic_aaaa;

QProcess process_system;
QString system_output;
QStringList qastr_listeSID;
QByteArray  qba_AdminSID_notClean;
uint8_t au8AdminSID_notClean[80];
uint8_t au8AdminSID_AsciiCompressed[40];
uint8_t u8IdxStart, u8Idx_AsciiCompressed , u8Idx_BinaryFormat, u8IdxLic, u8IdxCrypt,u8IdxModulo;
uint8_t u8AdminSID_AsciiCompressed_length=0;
uint8_t u8DummyChar = 'A';
uint8_t u8DataBinaryBuild;
QString strTemp;
bool    bIsOk;


uint8_t au8AdminSID_BinaryFormat[20];
uint8_t au8DummyData[7];
uint8_t au8Lic_EndDate[8];
uint8_t u8AppType;
uint8_t au8App_LastConnection[8];
uint8_t au8App_EndDate[8];
uint8_t au8AdminSID_BinaryFormat_Copy[12];
uint8_t au8App_CreationDateFirstLicence[8];
uint64_t u64CRC=0;
uint8_t au8MaskAlea[8];
uint8_t au8Lic_soft_notCrypted[88];


int16_t s16Get_DateCurrent_dd,s16Get_DateCurrent_mm,s16Get_DateCurrent_aaaa;



    //Check if there is a licence file
    if(m_strLicence.isEmpty()) {
        m_dLicenceExpiration = QDate::currentDate();
        return LICENCE_NONE;
    }



    // Traitements avec m_strLicence ------------------------------------------------------------
    u8LicenceFile_length= m_strLicence.length();
    if(205< u8LicenceFile_length)
    {
        m_u8CodeError = 0x01;
        return LICENCE_INVALID;
    }

    //qDebug("Licence length %d", u8LicenceFile_length);

    //Copy of the raw data
    for(u8Idx=0; u8Idx<m_strLicence.length();u8Idx++ )
    {
        au8Licence_rawFileData[u8Idx]=(uint8_t)m_baLicence[u8Idx];
    }
    //qDebug() << "[LICENCE] >> read current file :" << QByteArray(m_baLicence,205) ;

    //Delete separator
    u8Idx2 =0;
    for(u8Idx=0; u8Idx<m_strLicence.length();u8Idx++ )
    {
        if('-'!= au8Licence_rawFileData[u8Idx])
        {
            if(176> u8Idx2)
            {
                au8Licence_uncryptedAscii[u8Idx2] = au8Licence_rawFileData[u8Idx];
                u8Idx2++;
            }
        }
    }

    //ASCII to binary
    for(u8Idx=0; u8Idx<176;u8Idx++ )
    {
        if( 'A'<= au8Licence_uncryptedAscii[u8Idx] && 'F'>=au8Licence_uncryptedAscii[u8Idx])
        {
            au8Licence_uncryptedBinary[u8Idx]= au8Licence_uncryptedAscii[u8Idx] -'A' +10;
        }
        else if( 'a'<= au8Licence_uncryptedAscii[u8Idx] && 'f'>=au8Licence_uncryptedAscii[u8Idx])
        {
            au8Licence_uncryptedBinary[u8Idx]= au8Licence_uncryptedAscii[u8Idx] -'a' +10;
        }
        else if( '0'<= au8Licence_uncryptedAscii[u8Idx] && '9'>=au8Licence_uncryptedAscii[u8Idx])
        {
            au8Licence_uncryptedBinary[u8Idx]= au8Licence_uncryptedAscii[u8Idx] -'0';
        }
        else
        {
            m_u8CodeError = 0x02;
            return LICENCE_INVALID;
        }
        //qDebug("[%d] : %x", u8Idx, au8Licence_uncryptedBinary[u8Idx]);
    }

    //Build Short format
    u8Idx2=0;
    for(u8Idx=0; u8Idx<88;u8Idx++ )
    {
        u8TmpMSB= au8Licence_uncryptedBinary[u8Idx2];
        u8TmpLSB= au8Licence_uncryptedBinary[u8Idx2+1];
        u8Tmp = ((u8TmpMSB<<4)&0xF0)| (u8TmpLSB&0x0F);
        au8Licence_decrypting[u8Idx] = u8Tmp;
        u8Idx2=u8Idx2+2;

        //qDebug("[%d] : %x", u8Idx, au8Licence_decrypting[u8Idx]);
    }

    //Extract Mask Alea
    au8MaskAlea[0]= au8Licence_decrypting[80];
    au8MaskAlea[1]= au8Licence_decrypting[81];
    au8MaskAlea[2]= au8Licence_decrypting[82];
    au8MaskAlea[3]= au8Licence_decrypting[83];
    au8MaskAlea[4]= au8Licence_decrypting[84];
    au8MaskAlea[5]= au8Licence_decrypting[85];
    au8MaskAlea[6]= au8Licence_decrypting[86];
    au8MaskAlea[7]= au8Licence_decrypting[87];

    //Decrypting with Mask Alea and the CRC mask
    u8Idx2=0;
    for(u8Idx=0; u8Idx< 80 ;u8Idx++)
    {
        au8Licence_decrypted[u8Idx]= au8Licence_decrypting[u8Idx]^au8MaskAlea[u8Idx2];
        u8Idx2++;

        if(8<=u8Idx2)
        {
            u8Idx2=0;
        }
        //qDebug("[%d] : %x", u8Idx, au8Licence_decrypted[u8Idx]);
    }
    for(u8Idx=80; u8Idx< 88 ;u8Idx++)
    {
        au8Licence_decrypted[u8Idx]= au8Licence_decrypting[u8Idx];
        //qDebug("[%d] : %x", u8Idx, au8Licence_decrypted[u8Idx]);
    }

    //Check CRC64_ref
    u64CRC_ref=0;
    for(u8Idx = 0; u8Idx <8; u8Idx++) //CRC64
    {
        u64CRC_ref= u64CRC_ref |  (((uint64_t)( au8Licence_decrypted[72+u8Idx]))<<((u8Idx)*8));
    }
    //Calculate u64CRC_compute
    u64CRC_compute =0;
    for(u8Idx = 0; u8Idx <20; u8Idx++)
    {
        u64CRC_compute = u64CRC_compute + (uint64_t)(au8Licence_decrypted[u8Idx]);
    }

    for(u8Idx = 20; u8Idx <27; u8Idx++)
    {
        u64CRC_compute = u64CRC_compute + (uint64_t)(au8Licence_decrypted[u8Idx]);
    }

    for(u8Idx = 27; u8Idx <35; u8Idx++)
    {
        u64CRC_compute = u64CRC_compute + (uint64_t)(au8Licence_decrypted[u8Idx]);
    }

    u64CRC_compute = u64CRC_compute <<(uint64_t)(au8Licence_decrypted[35]);

    for(u8Idx = 36; u8Idx <44; u8Idx++)
    {
        u64CRC_compute = u64CRC_compute<< (uint64_t)(au8Licence_decrypted[u8Idx]);
    }

    for(u8Idx = 44; u8Idx <52; u8Idx++)
    {
        u64CRC_compute = u64CRC_compute << (uint64_t)(au8Licence_decrypted[u8Idx]);
    }

    for(u8Idx = 52; u8Idx <72; u8Idx++)
    {
        u64CRC_compute = u64CRC_compute + (uint64_t)(au8Licence_decrypted[u8Idx]);
    }
    if(u64CRC_ref != u64CRC_compute)
    {
        m_u8CodeError = 0x03;
        return LICENCE_INVALID;
    }


    strTmp= QString("%1").arg(u64CRC_ref , 2, 10);
    //qDebug("CRC64_ref: %s",  strTmp.toLocal8Bit().constData());
    strTmp= QString("%1").arg(u64CRC_compute , 2, 10);
    //qDebug("CRC64_compute: %s",  strTmp.toLocal8Bit().constData());

    //Check the application id
    u8AppSelected = au8Licence_decrypted[35];
    if(APP_DATASCAN_SELECTED!= u8AppSelected)
    {
        m_u8CodeError = 0x04;
        return LICENCE_INVALID;
    }

    //Check the SID of the computer
    //GET SID ADMIN IN BINARY FORMAT
    if(QSysInfo::kernelType() == "winnt")
    {
        QString SID = "wmic useraccount get name,sid";
        process_system.start(SID);
        process_system.waitForFinished();
        system_output = process_system.readAllStandardOutput();
        cout << "SID : "  << system_output.toLocal8Bit().constData() << endl;


        //Extract the line Administrator sid line and convert the data into qbyte array ------------------------------------------------------------------------
        qastr_listeSID = system_output.split("\r");
        //cout << qastr_listeSID[2].toLocal8Bit().constData() ;
        qba_AdminSID_notClean=qastr_listeSID[2].toLocal8Bit();

        //Copy the result of the process system SID on a au8Array and find the start idx of the SID number -----------------------------------------------------
        u8IdxStart=0;
        for(u8Idx=0; u8Idx<qba_AdminSID_notClean.length();u8Idx++ )
        {
            au8AdminSID_notClean[u8Idx] = (uint8_t) qba_AdminSID_notClean[u8Idx];
            //qDebug("Idx %d :%d : %c ", (int)u8Idx, (int)au8AdminSID_notClean[u8Idx], (char) au8AdminSID_notClean[u8Idx]);
            if('S' == au8AdminSID_notClean[u8Idx] )
            {
                u8IdxStart = u8Idx;
                //qDebug("Idx Start %d ", u8Idx);
            }
        }

        //Extract the SID number on the ascii Table
        //qDebug("Idx Start %d ", u8IdxStart);
        u8Idx_AsciiCompressed=0;
        for(u8Idx=u8IdxStart; u8Idx<qba_AdminSID_notClean.length();u8Idx++ )
        {
            if('-' != au8AdminSID_notClean[u8Idx] && ' ' != au8AdminSID_notClean[u8Idx])
            {
                if('S'== au8AdminSID_notClean[u8Idx])
                {
                    au8AdminSID_AsciiCompressed[u8Idx_AsciiCompressed]='C';
                }
                else
                {
                    au8AdminSID_AsciiCompressed[u8Idx_AsciiCompressed]= au8AdminSID_notClean[u8Idx];
                }


                // qDebug("Idx %d :%d : %c "   , (int)u8Idx_AsciiCompressed
                //                            , (int)au8AdminSID_AsciiCompressed[u8Idx_AsciiCompressed]
                //                            , (char) au8AdminSID_AsciiCompressed[u8Idx_AsciiCompressed]);

                u8Idx_AsciiCompressed++;

            }
        }
        u8AdminSID_AsciiCompressed_length = u8Idx_AsciiCompressed;

        //Build the compressed AdminSID in ASCII Format with 40length the dummy data at the end or between "A" and "F"
        for(u8Idx=u8AdminSID_AsciiCompressed_length ; u8Idx<40;u8Idx++ )
        {
            au8AdminSID_AsciiCompressed[u8Idx]= u8DummyChar;
            u8DummyChar=u8DummyChar+1;
            if('F'<u8DummyChar)
            {
                u8DummyChar = 'A';
            }
        }

        //Print the  au8AdminSID_AsciiCompressed value      
        //cout << "ADMIN SID ASCII COMPRESSED : \t" ;

        //for(u8Idx=0 ; u8Idx<40;u8Idx++ )
        //{
        //     cout << (char) au8AdminSID_AsciiCompressed[u8Idx];
        //}


        //Build ADMIN SID IN BINARY FORMAT
        //cout <<endl << "ADMIN SID IN BINARY FORMAT : \t" ;
        u8Idx_AsciiCompressed=0;
        u8Idx_BinaryFormat =0;

        for(u8Idx = 0; u8Idx <40; u8Idx=u8Idx+2)
        {
            //Extract MSB
            if( 'A' <= au8AdminSID_AsciiCompressed[u8Idx] && 'F' >= au8AdminSID_AsciiCompressed[u8Idx])
            {
                u8TmpMSB = au8AdminSID_AsciiCompressed[u8Idx]-'A' + 0x0A;
            }
            else
            {
                u8TmpMSB = au8AdminSID_AsciiCompressed[u8Idx]-'0';
            }
            //Extract LSB
            if( 'A' <= au8AdminSID_AsciiCompressed[u8Idx+1] && 'F' >= au8AdminSID_AsciiCompressed[u8Idx+1])
            {
                u8TmpLSB = au8AdminSID_AsciiCompressed[u8Idx+1]-'A' + 0x0A;
            }
            else
            {
                u8TmpLSB = au8AdminSID_AsciiCompressed[u8Idx+1]-'0';
            }

            u8DataBinaryBuild = u8TmpMSB<<4 | u8TmpLSB;
            au8AdminSID_BinaryFormat[u8Idx_BinaryFormat] =  u8DataBinaryBuild;

            //strTemp= QString("%1.").arg(u8DataBinaryBuild , 2, 16);
            //cout <<  strTemp.toLocal8Bit().constData() ;


            u8Idx_BinaryFormat++;
        }

        for(u8Idx=0;  u8Idx<20; u8Idx++) //Check the SID authentification
        {
            if(au8AdminSID_BinaryFormat[u8Idx] != au8Licence_decrypted[u8Idx])
            {
                m_u8CodeError = 0x05;
                return LICENCE_INVALID;
            }
        }

    }
    else
    {
        return PC_NOT_COMPATIBLE;
    }
    //Extract the date
    s16Extract_EndDateLic_dd    = (uint16_t)((au8Licence_decrypted[27])*10 + au8Licence_decrypted[28]);
    s16Extract_EndDateLic_mm    = (uint16_t)((au8Licence_decrypted[29])*10 + au8Licence_decrypted[30]);
    s16Extract_EndDateLic_aaaa  = (uint16_t)((au8Licence_decrypted[31])*1000)
                                + (uint16_t)((au8Licence_decrypted[32])*100)
                                + (uint16_t)((au8Licence_decrypted[33])*10)
                                + (uint16_t)((au8Licence_decrypted[34]));

    s16Extract_EndDateApp_dd    = (uint16_t)((au8Licence_decrypted[44])*10 + au8Licence_decrypted[45]);
    s16Extract_EndDateApp_mm    = (uint16_t)((au8Licence_decrypted[46])*10 + au8Licence_decrypted[47]);
    s16Extract_EndDateApp_aaaa  = (uint16_t)((au8Licence_decrypted[48])*1000)
                                + (uint16_t)((au8Licence_decrypted[49])*100)
                                + (uint16_t)((au8Licence_decrypted[50])*10)
                                + (uint16_t)((au8Licence_decrypted[51]));

    s16Extract_DateLastRunApp_dd    = (uint16_t)((au8Licence_decrypted[36])*10 + au8Licence_decrypted[37]);
    s16Extract_DateLastRunApp_mm    = (uint16_t)((au8Licence_decrypted[38])*10 + au8Licence_decrypted[39]);
    s16Extract_DateLastRunApp_aaaa  = (uint16_t)((au8Licence_decrypted[40])*1000)
                                    + (uint16_t)((au8Licence_decrypted[41])*100)
                                    + (uint16_t)((au8Licence_decrypted[42])*10)
                                    + (uint16_t)((au8Licence_decrypted[43]));

    s16Extract_FirstCreationDateLic_dd    = (uint16_t)((au8Licence_decrypted[64])*10 + au8Licence_decrypted[65]);
    s16Extract_FirstCreationDateLic_mm    = (uint16_t)((au8Licence_decrypted[66])*10 + au8Licence_decrypted[67]);
    s16Extract_FirstCreationDateLic_aaaa  = (uint16_t)((au8Licence_decrypted[68])*1000)
                                            + (uint16_t)((au8Licence_decrypted[69])*100)
                                            + (uint16_t)((au8Licence_decrypted[70])*10)
                                            + (uint16_t)((au8Licence_decrypted[71]));




    if((0> s16Extract_EndDateLic_dd                 || 31< s16Extract_EndDateLic_dd) ||
       (0> s16Extract_EndDateLic_mm                 || 12< s16Extract_EndDateLic_mm) ||
       (2023> s16Extract_EndDateLic_aaaa            || 2100< s16Extract_EndDateLic_aaaa)||
       (0> s16Extract_EndDateApp_dd                 || 31< s16Extract_EndDateApp_dd) ||
       (0> s16Extract_EndDateApp_mm                 || 12< s16Extract_EndDateApp_mm) ||
       (2023> s16Extract_EndDateApp_aaaa            || 2100< s16Extract_EndDateApp_aaaa)||
       (0> s16Extract_DateLastRunApp_dd             || 31< s16Extract_DateLastRunApp_dd) ||
       (0> s16Extract_DateLastRunApp_mm             || 12< s16Extract_DateLastRunApp_mm) ||
       (2023> s16Extract_DateLastRunApp_aaaa        || 2100< s16Extract_DateLastRunApp_aaaa)||
       (0> s16Extract_FirstCreationDateLic_dd       || 31< s16Extract_FirstCreationDateLic_dd) ||
       (0> s16Extract_FirstCreationDateLic_mm       || 12< s16Extract_FirstCreationDateLic_mm) ||
       (2023> s16Extract_FirstCreationDateLic_aaaa  || 2100< s16Extract_FirstCreationDateLic_aaaa))
    {
        m_u8CodeError = 0x06;
        return LICENCE_INVALID;
    }

    //Get Current Date
    s16Get_DateCurrent_dd   = (int16_t) QDate::currentDate().day();
    s16Get_DateCurrent_mm   = (int16_t) QDate::currentDate().month();
    s16Get_DateCurrent_aaaa = (int16_t) QDate::currentDate().year();

    qDebug("[LICENCE] >> Read Lic end date          :  %d:%d:%d",s16Extract_EndDateLic_dd,s16Extract_EndDateLic_mm, s16Extract_EndDateLic_aaaa);
    qDebug("[LICENCE] >> Read App end date          :  %d:%d:%d",s16Extract_EndDateApp_dd,s16Extract_EndDateApp_mm, s16Extract_EndDateApp_aaaa);
    qDebug("[LICENCE] >> Read Last run date         :  %d:%d:%d",s16Extract_DateLastRunApp_dd,s16Extract_DateLastRunApp_mm, s16Extract_DateLastRunApp_aaaa);
    qDebug("[LICENCE] >> Read current date          :  %d:%d:%d",s16Get_DateCurrent_dd,s16Get_DateCurrent_mm, s16Get_DateCurrent_aaaa);
    qDebug("[LICENCE] >> Read First Creation date   :  %d:%d:%d",s16Extract_FirstCreationDateLic_dd,s16Extract_FirstCreationDateLic_mm, s16Extract_FirstCreationDateLic_aaaa);


    //Expiration date ------------------------------------------------------------

    //s16Extract_EndDateApp_aaaa  = 2023;
    //s16Extract_EndDateApp_mm    = 05;
    //s16Extract_EndDateApp_dd    = 01;

    //s16Extract_EndDateLic_aaaa  = 2024;
    //s16Extract_EndDateLic_mm    = 05;
    //s16Extract_EndDateLic_dd    = 01;

    //s16Extract_DateLastRunApp_aaaa  = 2023;
    //s16Extract_DateLastRunApp_mm    = 03;
    //s16Extract_DateLastRunApp_dd    = 23;


    m_dLicenceExpiration.setDate(s16Extract_EndDateLic_aaaa, s16Extract_EndDateLic_mm, s16Extract_EndDateLic_dd);
    m_dApplicationExpiration.setDate(s16Extract_EndDateApp_aaaa, s16Extract_EndDateApp_mm, s16Extract_EndDateApp_dd);
    m_dApplicationlastLaunch.setDate(s16Extract_DateLastRunApp_aaaa, s16Extract_DateLastRunApp_mm, s16Extract_DateLastRunApp_dd);

    // Automatic -----------------------------------------------------------------

    qint32 i32LicenceRemainingDays = QDate::currentDate().daysTo(m_dLicenceExpiration);
    qint32 i32ApplicationRemainingDays = QDate::currentDate().daysTo(m_dApplicationExpiration);
    qint32 i32IntervalLastConnexionDays = QDate::currentDate().daysTo(m_dApplicationlastLaunch);

    //qDebug("Number of day elapsed since the last run :  %d",i32IntervalLastConnexionDays)
    //Check the coherance with the last date and current date connexion
    if(i32IntervalLastConnexionDays > 0) {
        m_u8CodeError = 0x07;
        return LICENCE_INVALID;
    }

    // Rebuild the licence file --------------------------------------------------

    //GENERATE DUMMY DATA
    //cout <<endl << "GENERATE DUMMY DATA : \t" ;
    for(u8Idx = 0; u8Idx <7; u8Idx++)
    {
        au8DummyData[u8Idx]= (uint8_t)( QRandomGenerator::global()->generate());

        strTemp= QString("%1.").arg(au8DummyData[u8Idx] , 2, 16);
        //cout <<  strTemp.toLocal8Bit().constData() ;
    }

    //DATE FIN LICENCE
    //cout <<endl << "DATE END LICENCE : \t\t" ;
    au8Lic_EndDate[0] = au8Licence_decrypted[27];
    au8Lic_EndDate[1] = au8Licence_decrypted[28];
    au8Lic_EndDate[2] = au8Licence_decrypted[29];
    au8Lic_EndDate[3] = au8Licence_decrypted[30];
    au8Lic_EndDate[4] = au8Licence_decrypted[31];
    au8Lic_EndDate[5] = au8Licence_decrypted[32];
    au8Lic_EndDate[6] = au8Licence_decrypted[33];
    au8Lic_EndDate[7] = au8Licence_decrypted[34];

    //std::cout   << (char)(au8Lic_EndDate[0]+'0')
    //            << (char)(au8Lic_EndDate[1]+'0') << ':'
    //            << (char)(au8Lic_EndDate[2]+'0')
    //            << (char)(au8Lic_EndDate[3]+'0') << ':'
    //            << (char)(au8Lic_EndDate[4]+'0')
    //            << (char)(au8Lic_EndDate[5]+'0')
    //            << (char)(au8Lic_EndDate[6]+'0')
    //            << (char)(au8Lic_EndDate[7]+'0') ;

    //APPLICATION TYPE
    //cout <<endl << "TYPE OF APPLICATION : \t" ;
    u8AppType = APP_DATASCAN_SELECTED;
    //std::cout   << (char)(u8AppType+'0') ;

    //DATE LAST CONNECTION
    //cout <<endl << "DATE LAST CONNECTION : \t" ;
    if(i32IntervalLastConnexionDays > 0)
    {
        au8App_LastConnection[0] = (uint8_t) (s16Extract_DateLastRunApp_dd/10);
        au8App_LastConnection[1] = (uint8_t) (s16Extract_DateLastRunApp_dd%10);
        au8App_LastConnection[2] = (uint8_t) (s16Extract_DateLastRunApp_mm/10);
        au8App_LastConnection[3] = (uint8_t) (s16Extract_DateLastRunApp_mm%10);
        au8App_LastConnection[4] = (uint8_t) (s16Extract_DateLastRunApp_aaaa/1000);
        au8App_LastConnection[5] = (uint8_t) ((s16Extract_DateLastRunApp_aaaa/100)%10);
        au8App_LastConnection[6] = (uint8_t) ((s16Extract_DateLastRunApp_aaaa/10)%10);
        au8App_LastConnection[7] = (uint8_t) ((s16Extract_DateLastRunApp_aaaa)%10);
    }
    else
    {
        au8App_LastConnection[0] = (uint8_t) (s16Get_DateCurrent_dd/10);
        au8App_LastConnection[1] = (uint8_t) (s16Get_DateCurrent_dd%10);
        au8App_LastConnection[2] = (uint8_t) (s16Get_DateCurrent_mm/10);
        au8App_LastConnection[3] = (uint8_t) (s16Get_DateCurrent_mm%10);
        au8App_LastConnection[4] = (uint8_t) (s16Get_DateCurrent_aaaa/1000);
        au8App_LastConnection[5] = (uint8_t) ((s16Get_DateCurrent_aaaa/100)%10);
        au8App_LastConnection[6] = (uint8_t) ((s16Get_DateCurrent_aaaa/10)%10);
        au8App_LastConnection[7] = (uint8_t) ((s16Get_DateCurrent_aaaa)%10);
    }

    //std::cout   << (char)(au8App_LastConnection[0]+'0')
    //            << (char)(au8App_LastConnection[1]+'0') << ':'
    //            << (char)(au8App_LastConnection[2]+'0')
    //            << (char)(au8App_LastConnection[3]+'0') << ':'
    //            << (char)(au8App_LastConnection[4]+'0')
    //            << (char)(au8App_LastConnection[5]+'0')
    //            << (char)(au8App_LastConnection[6]+'0')
    //            << (char)(au8App_LastConnection[7]+'0') ;

    //DATE FIN APPLICATION
    //cout <<endl << "DATE END APPLICATION : \t" ;
    //17:05:2024
    au8App_EndDate[0] = au8Licence_decrypted[44];
    au8App_EndDate[1] = au8Licence_decrypted[45];
    au8App_EndDate[2] = au8Licence_decrypted[46];
    au8App_EndDate[3] = au8Licence_decrypted[47];
    au8App_EndDate[4] = au8Licence_decrypted[48];
    au8App_EndDate[5] = au8Licence_decrypted[49];
    au8App_EndDate[6] = au8Licence_decrypted[50];
    au8App_EndDate[7] = au8Licence_decrypted[51];

    //std::cout   << (char)(au8App_EndDate[0]+'0')
    //            << (char)(au8App_EndDate[1]+'0') << ':'
    //            << (char)(au8App_EndDate[2]+'0')
    //            << (char)(au8App_EndDate[3]+'0') << ':'
    //            << (char)(au8App_EndDate[4]+'0')
    //            << (char)(au8App_EndDate[5]+'0')
    //            << (char)(au8App_EndDate[6]+'0')
    //            << (char)(au8App_EndDate[7]+'0') ;

    //ADMIN SID COPY
    //cout <<endl << "ADMIN SID COPY : \t\t" ;
    for(u8Idx = 0; u8Idx <12; u8Idx++)
    {
        au8AdminSID_BinaryFormat_Copy[u8Idx]= au8AdminSID_BinaryFormat[u8Idx];

        strTemp= QString("%1.").arg(au8AdminSID_BinaryFormat_Copy[u8Idx] , 2, 16);
        //std::cout <<  strTemp.toLocal8Bit().constData() ;
    }


    //DATE FIRST LICENCE CREATION
    //cout <<endl << "DATE END APPLICATION : \t" ;
    //17:05:2024
    au8App_CreationDateFirstLicence[0] = au8Licence_decrypted[64];
    au8App_CreationDateFirstLicence[1] = au8Licence_decrypted[65];
    au8App_CreationDateFirstLicence[2] = au8Licence_decrypted[66];
    au8App_CreationDateFirstLicence[3] = au8Licence_decrypted[67];
    au8App_CreationDateFirstLicence[4] = au8Licence_decrypted[68];
    au8App_CreationDateFirstLicence[5] = au8Licence_decrypted[69];
    au8App_CreationDateFirstLicence[6] = au8Licence_decrypted[70];
    au8App_CreationDateFirstLicence[7] = au8Licence_decrypted[71];

    //std::cout   << (char)(au8App_CreationDateFirstLicence[0]+'0')
    //            << (char)(au8App_CreationDateFirstLicence[1]+'0') << ':'
    //            << (char)(au8App_CreationDateFirstLicence[2]+'0')
    //            << (char)(au8App_CreationDateFirstLicence[3]+'0') << ':'
    //            << (char)(au8App_CreationDateFirstLicence[4]+'0')
    //            << (char)(au8App_CreationDateFirstLicence[5]+'0')
    //            << (char)(au8App_CreationDateFirstLicence[6]+'0')
    //            << (char)(au8App_CreationDateFirstLicence[7]+'0') ;

    //CRC64   -------------------------------------------------------------------------------------
    //cout <<endl << "CRC64 : \t\t\t" ;
    for(u8Idx = 0; u8Idx <20; u8Idx++)
    {
        u64CRC = u64CRC + (uint64_t)(au8AdminSID_BinaryFormat[u8Idx]);
    }
    for(u8Idx = 0; u8Idx <7; u8Idx++)
    {
        u64CRC = u64CRC + (uint64_t)(au8DummyData[u8Idx]);
    }
    for(u8Idx = 0; u8Idx <8; u8Idx++)
    {
        u64CRC = u64CRC + (uint64_t)(au8Lic_EndDate[u8Idx]);
    }
    u64CRC = u64CRC <<(uint64_t)(u8AppType);

    for(u8Idx = 0; u8Idx <8; u8Idx++)
    {
        u64CRC = u64CRC<< (uint64_t)(au8App_LastConnection[u8Idx]);
    }

    for(u8Idx = 0; u8Idx <8; u8Idx++)
    {
        u64CRC = u64CRC << (uint64_t)(au8App_EndDate[u8Idx]);
    }

    for(u8Idx = 0; u8Idx <12; u8Idx++)
    {
        u64CRC = u64CRC + (uint64_t)(au8AdminSID_BinaryFormat_Copy[u8Idx]);
    }

    for(u8Idx = 0; u8Idx <8; u8Idx++)
    {
        u64CRC = u64CRC + (uint64_t)(au8App_CreationDateFirstLicence[u8Idx]);
    }


    strTemp= QString("%1").arg(u64CRC , 2, 10);
    //cout <<  strTemp.toLocal8Bit().constData() ;

    //MASK ALEA
    //cout <<endl << "GENERATE MASK ALEA : \t" ;
    for(u8Idx = 0; u8Idx <8; u8Idx++)
    {
        au8MaskAlea[u8Idx]= (uint8_t)( QRandomGenerator::global()->generate());

        strTemp= QString("%1.").arg(au8MaskAlea[u8Idx] , 2, 16);
        //cout <<  strTemp.toLocal8Bit().constData() ;
    }
    //Build Licence Buffer not crypted ---------------------------------------------------------------
    //cout <<endl << "GENERATE LICENCE APP KEY : \t" ;
    u8IdxLic=0;

    for(u8Idx = 0; u8Idx <20; u8Idx++) //ADMIN SID IN BINARY FORMA
    {
        au8Lic_soft_notCrypted[u8IdxLic] = au8AdminSID_BinaryFormat[u8Idx];
        u8IdxLic++;
    }

    for(u8Idx = 0; u8Idx <7; u8Idx++) //DUMMY DATA
    {
        au8Lic_soft_notCrypted[u8IdxLic] = au8DummyData[u8Idx];
        u8IdxLic++;
    }

    for(u8Idx = 0; u8Idx <8; u8Idx++) //DATE END LICENCE
    {
        au8Lic_soft_notCrypted[u8IdxLic] = au8Lic_EndDate[u8Idx];
        u8IdxLic++;
    }

    au8Lic_soft_notCrypted[u8IdxLic] =u8AppType ; //TYPE OF APPLICATION
    u8IdxLic++;

    for(u8Idx = 0; u8Idx <8; u8Idx++) //DATE LAST CONNECTION
    {
        au8Lic_soft_notCrypted[u8IdxLic] = au8App_LastConnection[u8Idx];
        u8IdxLic++;
    }

    for(u8Idx = 0; u8Idx <8; u8Idx++) //DATE END APPLICATION
    {
        au8Lic_soft_notCrypted[u8IdxLic] = au8App_EndDate[u8Idx];
        u8IdxLic++;
    }

    for(u8Idx = 0; u8Idx <12; u8Idx++) //ADMIN SID COPY
    {
        au8Lic_soft_notCrypted[u8IdxLic]= au8AdminSID_BinaryFormat_Copy[u8Idx];
        u8IdxLic++;
    }

    for(u8Idx = 0; u8Idx <8; u8Idx++) //DATE FIRST CREATION LIC
    {
        au8Lic_soft_notCrypted[u8IdxLic]= au8App_CreationDateFirstLicence[u8Idx];
        u8IdxLic++;
    }

    for(u8Idx = 0; u8Idx <8; u8Idx++) //CRC64
    {
        au8Lic_soft_notCrypted[u8IdxLic] = (uint8_t)((u64CRC>>(u8Idx*8))&0x000000FF);
        u8IdxLic++;
    }

    for(u8Idx = 0; u8Idx <8; u8Idx++) //MASK ALEA
    {
        au8Lic_soft_notCrypted[u8IdxLic] = au8MaskAlea[u8Idx];
        u8IdxLic++;
    }

    strTemp= QString("%1").arg(u8IdxLic , 2, 10);

    //cout <<endl;
    for(u8Idx=0; u8Idx< 88;u8Idx++)
    {
        strTemp= QString("%1.").arg(au8Lic_soft_notCrypted[u8Idx] , 1, 16);
        //std::cout <<  strTemp.toLocal8Bit().constData() ;
    }

    //Build Licence Buffer crypted with the SID
    //cout <<endl << "GENERATE LICENCE APP KEY CRYPTED : \t" <<endl ;
    u8IdxCrypt =0;
    for(u8Idx=0; u8Idx< 80 ;u8Idx++)
    {
        m_au8Lic_soft_Crypted[u8Idx]= au8Lic_soft_notCrypted[u8Idx]^au8MaskAlea[u8IdxCrypt];
        u8IdxCrypt++;

        if(8<=u8IdxCrypt)
        {
            u8IdxCrypt=0;
        }
    }

    for(u8Idx=80; u8Idx< 88 ;u8Idx++)
    {
        m_au8Lic_soft_Crypted[u8Idx]= au8Lic_soft_notCrypted[u8Idx];
    }


    //PRINT Licence Buffer crypted with the SID and seperator
    u8IdxModulo=0;
    for(u8Idx=0; u8Idx< 88;u8Idx++)
    {
        u8TmpMSB = (uint8_t) ((m_au8Lic_soft_Crypted[u8Idx]>>4)&0x0F);
        u8TmpLSB = (uint8_t) ((m_au8Lic_soft_Crypted[u8Idx]>>0)&0x0F);



        if( 0x0A<= u8TmpMSB)
        {
            if(0x0A == u8TmpMSB)  std::cout << 'A';
            if(0x0B == u8TmpMSB)  std::cout << 'B';
            if(0x0C == u8TmpMSB)  std::cout << 'C';
            if(0x0D == u8TmpMSB)  std::cout << 'D';
            if(0x0E == u8TmpMSB)  std::cout << 'E';
            if(0x0F == u8TmpMSB)  std::cout << 'F';
        }
        else
        {
            //std::cout << (char)(u8TmpMSB+'0');
        }
        if( 0x0A<= u8TmpLSB)
        {
            if(0x0A == u8TmpLSB)  std::cout << 'A';
            if(0x0B == u8TmpLSB)  std::cout << 'B';
            if(0x0C == u8TmpLSB)  std::cout << 'C';
            if(0x0D == u8TmpLSB)  std::cout << 'D';
            if(0x0E == u8TmpLSB)  std::cout << 'E';
            if(0x0F == u8TmpLSB)  std::cout << 'F';
        }
        else
        {
            //std::cout << (char)(u8TmpLSB+'0');
        }

        if(2<=u8IdxModulo)
        {
            //std::cout << '-';
            u8IdxModulo=0;
        }
        else
        {
            u8IdxModulo++;
        }
    }




    // Automatic -----------------------------------------------------------------
    this->updateLicenceFile();


    if(i32ApplicationRemainingDays <= 0) {
        return APP_DEPRECATED;
    }
    if(i32LicenceRemainingDays <= 0) {
        return LICENCE_EXPIRED;
    }
    if(i32LicenceRemainingDays < U32_LICENCE_APPROACHING_LIMIT) {
        return LICENCE_SOON_EXPIRED;
    }
    if(i32ApplicationRemainingDays < U32_LICENCE_APPROACHING_LIMIT) {
        return APP_SOON_DEPRECATED;
    }


    return LICENCE_OK;
}


