#include "ImportToml.h"

#include <initializer_list>
#include <QFile>
#include <QCryptographicHash>
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>
#include <QVariantMap>

ImportToml::ImportToml(const QString &filePath, QObject *parent)
    : QObject(parent)
    , m_filePath(filePath)
{
}

void ImportToml::report(const QString &level, const QString &text)
{
    qWarning().noquote() << "[ImportToml]" << text;

    QVariantMap msg;
    msg.insert("level", level);
    msg.insert("text", text);
    m_messages.append(msg);
}

bool ImportToml::read()
{
    m_messages.clear();

    QFile file(m_filePath);
    if( !file.open(QIODevice::ReadOnly | QIODevice::Text) ) {
        report("error", tr("Impossible d'ouvrir le fichier planner : %1").arg(m_filePath));
        m_firstImport = true;
        m_securityPlannedMissing = true;
        emit finImport(true);
        return false;
    }

    const QString content = QString::fromUtf8(file.readAll());
    file.close();

    if( !parseToml(content) ) {
        report("error", tr("Fichier planner illisible (aucune clé/valeur) : %1").arg(m_filePath));
        m_securityNonReadable = true;
        m_keyIsValid = true;
        emit finImport(true);
        return false;
    }

    // A planner .toml has actually been imported.
    m_fileIsImported = true;
    m_importedFileName = QFileInfo(m_filePath).fileName();
    m_computedKey = computeShaString(content);

    const QFileInfo tomlInfo(m_filePath);
    const QDateTime modifiedAt = tomlInfo.birthTime().isValid()
                                     ? tomlInfo.birthTime()
                                     : tomlInfo.lastModified();
    const QString modifiedAtStr = modifiedAt.toString(Qt::ISODate);

    qDebug() << "[ImportToml] modified at =" << modifiedAtStr;

    // -- Read the file block by block (add one call per block).
    readSchemaBlock();
    readEquipmentBlock();
    readObjectBlock();
    if( m_objectType == "robe" ) {
        readGeomRobeBlock();
        if( m_robeNombreViroles > 0){
            readViroleBlock();
        }
    }else if( m_objectType == "tuyauterie" ) {
        readGeomTuyaBlock();
    }else if( m_objectType == "coque" ) {
        readGeomCoqueBlock();
    }else if( m_objectType == "fond" ) {
        readGeomFondBlock();
    }else if( m_objectType == "toit" ) {
        readGeomToitBlock();
    }
    readAcquisitionBlock();
    if(m_geometrieReferentiel == "libre") {
        readGrilleBlock();
    }

    readLigneBlock();
    readInterruptionBlock();
    readEvaluationBlock();
    readCampagneBlock();
    readVisiteBlock();
    readFaitBlock();
    readReperageBlock();
    readObstacleBlock();
    readZoneExclueBlock();

    const bool securityOk = securityCheck(modifiedAtStr) && !m_securityModifiedFile;

    if( m_messages.isEmpty() ) {
        QVariantMap msg;
        msg.insert("level", "ok");
        msg.insert("text", tr("Fichier importé : %1").arg(m_importedFileName));
        m_messages.append(msg);
    }

    // -- Single emission: fail when the security check did not pass.
    emit finImport(!securityOk);
    return true;
}

// TOML reader
bool ImportToml::parseToml(const QString &content)
{
    m_values.clear();
    m_arrayCounts.clear();
    QString section;

    const QStringList lines = content.split('\n');
    for( QString line : lines ) {
        // strip comments (naive : also cuts a '#' that would be inside a string)
        const int hash = line.indexOf('#');
        if( hash >= 0 ) {
            line = line.left(hash);
        }

        line = line.trimmed();
        if( line.isEmpty() ) {
            continue;
        }

        // [[array of tables]] element : "name" -> section "name.<index>".
        if( line.startsWith("[[") && line.endsWith("]]") ) {
            const QString name = line.mid(2, line.length() - 4).trimmed().toLower();
            const int index = m_arrayCounts.value(name, 0);
            m_arrayCounts.insert(name, index + 1);
            section = name + '.' + QString::number(index);
            continue;
        }

        if( line.startsWith('[') && line.endsWith(']') ) {
            section = line.mid(1, line.length() - 2).trimmed().toLower();
            continue;
        }

        // key = value
        const int eq = line.indexOf('=');
        if( eq < 0 ) {
            continue;
        }

        const QString key = line.left(eq).trimmed();
        QString val = line.mid(eq + 1).trimmed();
        if( key.isEmpty() ) {
            continue;
        }

        // strip surrounding quotes
        if( val.length() >= 2 &&
            ((val.startsWith('"')  && val.endsWith('"')) ||
             (val.startsWith('\'') && val.endsWith('\''))) ) {
            val = val.mid(1, val.length() - 2);
        }

        const QString fullKey = section.isEmpty() ? key : section + '.' + key;
        m_values.insert(fullKey, val);
    }

    return !m_values.isEmpty();
}

// -- [schema] block : version / producteur / date_creation / date_cloture / auteur
void ImportToml::readSchemaBlock()
{
    m_tomlSchemaVersion  = m_values.value("schema.version");
    m_tomlSchemaProducer = m_values.value("schema.producteur");
    m_tomlSchemaAutor    = m_values.value("schema.auteur");
    m_tomlSchemaNom      = m_values.value("schema.nom_projet");
    m_tomlSchemaClient   = m_values.value("schema.client");
    m_tomlSchemaSite     = m_values.value("schema.site");
    m_tomlSchemaCommand  = m_values.value("schema.numero");
    m_tomlSchemaDate     = m_values.value("schema.date_creation");


    qDebug() << "[ImportToml] [schema] version          =" << m_tomlSchemaVersion;
    qDebug() << "[ImportToml] [schema] producteur       =" << m_tomlSchemaProducer;
    qDebug() << "[ImportToml] [schema] auteur           =" << m_tomlSchemaAutor;
    qDebug() << "[ImportToml] [schema] nom_projet       =" << m_tomlSchemaNom;
    qDebug() << "[ImportToml] [schema] client           =" << m_tomlSchemaClient;
    qDebug() << "[ImportToml] [schema] site             =" << m_tomlSchemaSite;
    qDebug() << "[ImportToml] [schema] numero           =" << m_tomlSchemaCommand;
    qDebug() << "[ImportToml] [schema] date_creation    =" << m_tomlSchemaDate;
}

void ImportToml::readEquipmentBlock()
{
    m_equipmentNom      = m_values.value("equipment.nom");
    m_equipmentNumero   = m_values.value("equipment.numero_serie");
    m_equipmentDate     = m_values.value("equipment.annee");
    m_inspectorNom      = m_values.value("equipment.nom_inspecteur");
    m_inspectorNumero   = m_values.value("equipment.numero_inspecteur");
    m_inspectorDate     = m_values.value("equipment.inspecteur_date");
    m_supervisorNom     = m_values.value("equipment.nom_superviseur");
    m_supervisorNumero  = m_values.value("equipment.numero_superviseur");
    m_supervisorDate    = m_values.value("equipment.control_date");
    m_sondeNumero       = m_values.value("equipment.sonde_numero");
    m_controlleurNom    = m_values.value("equipment.nom_ controlleur");


    qDebug() << "[ImportToml] [equipment] nom                   =" << m_equipmentNom;
    qDebug() << "[ImportToml] [equipment] numero_serie          =" << m_equipmentNumero;
    qDebug() << "[ImportToml] [equipment] annee                 =" << m_equipmentDate;
    qDebug() << "[ImportToml] [equipment] nom_inspecteur        =" << m_inspectorNom;
    qDebug() << "[ImportToml] [equipment] numero_inspecteur     =" << m_inspectorNumero;
    qDebug() << "[ImportToml] [equipment] inspecteur_date       =" << m_inspectorDate;
    qDebug() << "[ImportToml] [equipment] nom_superviseur       =" << m_supervisorNom;
    qDebug() << "[ImportToml] [equipment] numero_superviseur    =" << m_supervisorNumero;
    qDebug() << "[ImportToml] [equipment] control_date          =" << m_supervisorDate;
    qDebug() << "[ImportToml] [equipment] sonde_numero          =" << m_sondeNumero;
    qDebug() << "[ImportToml] [equipment] nom_ controlleur      =" << m_controlleurNom;
}

// -- [object] block : type / numero / site / client
// parameter depends from "type": produit_stocke / densite_produit / hauteur_remplissage_max
void ImportToml::readObjectBlock()
{
    m_objectType    = m_values.value("object.type");

    if(m_objectType == "robe"){
        m_objectProduit = m_values.value("object.produit_stocke");
        m_objectDensity = m_values.value("object.densite_produit");
        m_objectHauter  = m_values.value("object.hauteur_remplissage_max");
    }
    qDebug() << "[ImportToml] [object] type                    =" << m_objectType;
    if(m_objectType == "robe"){
        qDebug() << "[ImportToml] [object] produit_stocke          =" << m_objectProduit;
        qDebug() << "[ImportToml] [object] densite_produit         =" << m_objectDensity;
        qDebug() << "[ImportToml] [object] hauteur_remplissage_max =" << m_objectHauter;
    }
}

// -- [geometrie.robe] block: diametre, nb_viroles
void ImportToml::readGeomRobeBlock()
{
    if( m_objectType != "robe" ) {
        return;
    }

    bool okDiametre = false;
    bool okViroles  = false;
    bool okGeneratrices = false;

    const double diametre       = m_values.value("geometrie.robe.diametre").toDouble(&okDiametre);
    const int    viroles        = m_values.value("geometrie.robe.nb_viroles").toInt(&okViroles);
    const int    generatrices   = m_values.value("geometrie.robe.nb_generatrices").toInt(&okGeneratrices);

    m_robeDiametre          = okDiametre ? diametre : 0.0;
    m_robeNombreViroles     = okViroles  ? viroles  : 0;
    m_geometrieReferentiel  = m_values.value("geometrie.robe.referentiel");
    m_geometrieGeneratrices = okGeneratrices ? generatrices : 0;

    qDebug() << "[ImportToml] [geometrie.robe] diametre    =" << m_robeDiametre
             << (okDiametre ? "" : "(missing/invalid)");
    qDebug() << "[ImportToml] [geometrie.robe] nb_viroles  =" << m_robeNombreViroles
             << (okViroles ? "" : "(missing/invalid)");
    qDebug() << "[ImportToml] [geometrie.robe] referentiel =" << m_geometrieReferentiel;
    qDebug() << "[ImportToml] [geometrie.robe] nb_generatrices =" << m_geometrieGeneratrices
             << (okGeneratrices ? "" : "(missing/invalid)");
}

// -- [[virole]] array (depend from [geometrie.robe].nb_viroles != 0): hauteur / ep_nominale / nuance for each virole.
void ImportToml::readViroleBlock()
{
    m_viroles.clear();

    int nbViroles = m_values.value("geometrie.robe.nb_viroles").toInt();

    const int entryCount = m_arrayCounts.value("virole", 0);

    if(nbViroles != entryCount){
        report("warning", tr("Structure incorrecte : %1 viroles déclarées, %2 blocs [[virole]] trouvés")
                              .arg(nbViroles).arg(entryCount));
        return;
    }

    if( m_robeNombreViroles == 0 ) {
        m_robeNombreViroles = nbViroles;
    }

    for(int e = 0; e < entryCount; ++e){
        const QString prefix = QStringLiteral("virole.%1.").arg(e);
        Virole v;
        int viroleNumero;
        double viroleHauteur;
        double viroleEpNominale;
        QString viroleNuance;

        v.viroleNumero = m_values.value(prefix + "numero").toInt();
        v.viroleHauteur = m_values.value(prefix + "hauteur").toDouble();
        v.viroleEpNominale = m_values.value(prefix + "ep_nominale").toDouble();
        v.viroleNuance = m_values.value(prefix + "nuance");

        m_viroles.append(v);

        qDebug() << "[ImportToml] [[Virole]] " << (e + 1)
                 << "numero = " << v.viroleNumero
                 << "hauteur = " << v.viroleHauteur
                 << "ep_nominale =" << v.viroleEpNominale
                 << "nuance =" << v.viroleNuance;
    }
}

// -- [geometrie.fond] block: Diametre / nb_rayons
void ImportToml::readGeomFondBlock()
{
    if( m_objectType != "fond" ) {
        return;
    }

    bool okFondDiametre = false;
    bool okFondEpNominale = false;
    bool okFondNbAnnulair = false;
    bool okFondNbCentrale = false;
    bool okGeneratrices = false;

    const double fondDiametre   = m_values.value("geometrie.fond.diametre").toDouble(&okFondDiametre);
    const double fondEpNominale = m_values.value("geometrie.fond.ep_nominale").toDouble(&okFondEpNominale);
    const double fondNbAnnulair = m_values.value("geometrie.fond.nb_toles_annulaires").toDouble(&okFondNbAnnulair);
    const double fondNbCentrale = m_values.value("geometrie.fond.nb_toles_centrales").toDouble(&okFondNbCentrale);
    const int    generatrices   = m_values.value("geometrie.fond.nb_generatrices").toInt(&okGeneratrices);

    m_fondDiametre   = okFondDiametre   ? fondDiametre   : 0.0;
    m_fondEpNominale = okFondEpNominale ? fondEpNominale : 0.0;
    m_fondNbAnnulair = okFondNbAnnulair ? fondNbAnnulair : 0.0;
    m_fondNbCentrale = okFondNbCentrale ? fondNbCentrale : 0.0;
    m_geometrieReferentiel = m_values.value("geometrie.fond.referentiel");
    m_geometrieGeneratrices = okGeneratrices ? generatrices : 0;

    qDebug() << "[ImportToml] [geometrie.fond] diametre =" << m_fondDiametre
             << (okFondDiametre ? "" : "(missing/invalid)");
    qDebug() << "[ImportToml] [geometrie.fond] ep_nominale =" << m_fondEpNominale
             << (okFondEpNominale ? "" : "(missing/invalid)");
    qDebug() << "[ImportToml] [geometrie.fond] nb_toles_annulaires =" << m_fondNbAnnulair
             << (okFondNbAnnulair ? "" : "(missing/invalid)");
    qDebug() << "[ImportToml] [geometrie.fond] nb_toles_centrales =" << m_fondNbCentrale
             << (okFondNbCentrale ? "" : "(missing/invalid)");
    qDebug() << "[ImportToml] [geometrie.fond] referentiel =" << m_geometrieReferentiel;
    qDebug() << "[ImportToml] [geometrie.fond] nb_generatrices =" << m_geometrieGeneratrices
             << (okGeneratrices ? "" : "(missing/invalid)");
}

// -- [geometrie.toit] block: Diametre / nb_rayons
void ImportToml::readGeomToitBlock()
{
    if( m_objectType != "toit" ) {
        return;
    }

    bool okToitDiametre = false;
    bool okToitEpNominale = false;
    bool okGeneratrices = false;

    const double toitDiametre = m_values.value("geometrie.toit.diametre").toDouble(&okToitDiametre);
    const double toitEpNominale = m_values.value("geometrie.toit.ep_nominale").toDouble(&okToitEpNominale);
    const int    generatrices = m_values.value("geometrie.toit.nb_generatrices").toInt(&okGeneratrices);

    m_toitDiametre = okToitDiametre ? toitDiametre : 0.0;
    m_toitEpNominale = okToitEpNominale ? toitEpNominale : 0.0;
    m_toitType = m_values.value("geometrie.toit.type");
    m_geometrieReferentiel = m_values.value("geometrie.toit.referentiel");
    m_geometrieGeneratrices = okGeneratrices ? generatrices : 0;

    qDebug() << "[ImportToml] [geometrie.toit] diametre         =" << m_toitDiametre
             << (okToitDiametre ? "" : "(missing/invalid)");
    qDebug() << "[ImportToml] [geometrie.toit] ep_nominale      =" << m_toitEpNominale
             << (okToitEpNominale ? "" : "(missing/invalid)");
    qDebug() << "[ImportToml] [geometrie.toit] type             =" << m_toitType;
    qDebug() << "[ImportToml] [geometrie.toit] referentiel      =" << m_geometrieReferentiel;
    qDebug() << "[ImportToml] [geometrie.toit] nb_generatrices  =" << m_geometrieGeneratrices
             << (okGeneratrices ? "" : "(missing/invalid)");
}

// -- [geometrie.tuyauterie] block: diametre, nb_viroles
void ImportToml::readGeomTuyaBlock()
{
    if( m_objectType != "tuyauterie" ) {
        return;
    }

    bool okDiametreMm = false;
    bool okDiametreIn = false;
    bool okTuyauterieEpaisseur = false;
    bool okTuyauterieLongueur  = false;

    const double diametreMm = m_values.value("geometrie.tuyauterie.diametre_mm").toDouble(&okDiametreMm);
    const double diametreIn = m_values.value("geometrie.tuyauterie.diametre_interieur").toDouble(&okDiametreIn);
    const double tuyauterieEpaisseur = m_values.value("geometrie.tuyauterie.ep_nominale").toDouble(&okTuyauterieEpaisseur);
    const double tuyauterieLongueur = m_values.value("geometrie.tuyauterie.longueur_troncon").toDouble(&okTuyauterieLongueur);

    m_tuyauterieDiamtereMm = okDiametreMm ? diametreMm : 0.0;
    m_tuyauterieDiamtereIn = okDiametreIn ? diametreIn : 0.0;
    m_tuyauterieEpaisseur  = okTuyauterieEpaisseur ? tuyauterieEpaisseur : 0.0;
    m_tuyauterieLongueur   = okTuyauterieLongueur  ? tuyauterieLongueur  : 0.0;

    qDebug() << "[ImportToml] [geometrie.tuyauterie] diametre_mm        =" << m_tuyauterieDiamtereMm
             << (okDiametreMm ? "" : "(missing/invalid)");
    qDebug() << "[ImportToml] [geometrie.tuyauterie] diametre_interieur =" << m_tuyauterieDiamtereIn
             << (okDiametreIn ? "" : "(missing/invalid)");
    qDebug() << "[ImportToml] [geometrie.tuyauterie] ep_nominale        =" << m_tuyauterieEpaisseur
             << (okTuyauterieEpaisseur ? "" : "(missing/invalid)");
    qDebug() << "[ImportToml] [geometrie.tuyauterie] longueur_troncon   =" << m_tuyauterieLongueur
             << (okTuyauterieLongueur ? "" : "(missing/invalid)");
}


// -- [geometrie.coque] block: position_x / orientation / Longueur
void ImportToml::readGeomCoqueBlock()
{
    if( m_objectType != "coque" ) {
        return;
    }

    bool okCoqueLongueur = false;
    m_coquePositionX = m_values.value("geometrie.coque.position_x");
    m_coqueOrientation  = m_values.value("geometrie.coque.orientation");

    const double coqueLongueur = m_values.value("geometrie.coque.longueur").toDouble(&okCoqueLongueur);
    m_coqueLongueur  = okCoqueLongueur ? coqueLongueur : 0.0;

    m_coqueDescription = m_values.value("geometrie.coque.description");

    qDebug() << "[ImportToml] [geometrie.coque] position_x  =" << m_coquePositionX;
    qDebug() << "[ImportToml] [geometrie.coque] orientation =" << m_coqueOrientation;
    qDebug() << "[ImportToml] [geometrie.coque] longueur    =" << m_coqueLongueur
             << (okCoqueLongueur ? "" : "(missing/invalid)");
    qDebug() << "[ImportToml] [geometrie.coque] description =" << m_coqueDescription;
}

void ImportToml::readLigneBlock()
{
    m_lignes.clear();

    const int entryCount = m_arrayCounts.value("ligne", 0);
    if( entryCount <= 0 ) {
        qDebug() << "[ImportToml] [[lignes]] none";
        return;
    }

    for( int e = 0; e < entryCount; ++e ) {
        const QString prefix = QStringLiteral("ligne.%1.").arg(e);

        Ligne l;
        QString ligneCode;
        double ligneAngle;
        QString ligneMode;
        QString ligneVecteur;
        int lignePriorite;
        double ligneTolerance;
        QString ligneDescription;

        l.ligneCode            = m_values.value(prefix + "code");
        l.ligneAngle           = m_values.value(prefix + "angle").toDouble();
        l.ligneMode            = m_values.value(prefix + "mode");
        l.ligneVecteur         = m_values.value(prefix + "vecteur");
        l.lignePriorite        = m_values.value(prefix + "priorite").toInt();
        l.ligneTolerance       = m_values.value(prefix + "tolerance").toDouble();
        l.ligneDescription     = m_values.value(prefix + "description");

        m_lignes.append(l);

        qDebug() << "[ImportToml] [[ligne]]" << (e + 1)
                 << "code = " << l.ligneCode
                 << "angle = " << l.ligneAngle
                 << "mode = " << l.ligneMode
                 << "vecteur = " << l.ligneVecteur
                 << "priorite = " << l.lignePriorite
                 << "tolerance = " << l.ligneTolerance
                 << "description = " << l.ligneDescription;
    }
}

// -- [Acquisition] block: mode / nb_lignes / mode_repart / zone_morte_pied / zone_securite_tete
void ImportToml::readAcquisitionBlock()
{
    bool okAcquisitionZoneMortePied = false;
    bool okAcquisitionZoneMorteTete = false;

    const double acquisitionZoneMortePied = m_values.value("acquisition.zone_morte_pied").toDouble(&okAcquisitionZoneMortePied);
    const double acquisitionZoneMorteTete = m_values.value("acquisition.zone_securite_tete").toDouble(&okAcquisitionZoneMorteTete);

    m_acquisitionZoneMorteTete = okAcquisitionZoneMorteTete ? acquisitionZoneMorteTete : 0.0;
    m_acquisitionZoneMortePied  = okAcquisitionZoneMortePied ? acquisitionZoneMortePied : 0.0;

    qDebug() << "[ImportToml] [Acquisition] zone_morte_pied     =" << m_acquisitionZoneMortePied
             << (okAcquisitionZoneMortePied ? "" : "(missing/invalid)");
    qDebug() << "[ImportToml] [Acquisition] zone_securite_tete  =" << m_acquisitionZoneMorteTete
             << (okAcquisitionZoneMorteTete ? "" : "(missing/invalid)");
}

// -- [Grille] block: optional (depend on reference) nb_lignes / nb_colonnes / pas_lignes
void ImportToml::readGrilleBlock()
{
    bool okGrilleNbColonnes   = false;
    bool okGrillePasLignes    = false;
    bool okGrillePasColonnes  = false;
    bool okGrilleNbMeasureMin = false;

    const int grilleNbColonnes = m_values.value("grille.nb_colonnes").toInt(&okGrilleNbColonnes);
    const double grillePasLignes = m_values.value("grille.pas_lignes").toDouble(&okGrillePasLignes);
    const double grillePasColonnes = m_values.value("grille.pas_colonnes").toDouble(&okGrillePasColonnes);
    const double grilleNbMeasureMin = m_values.value("grille.nb_measures_minimum").toDouble(&okGrilleNbMeasureMin);

    m_grilleNbColonnes = okGrilleNbColonnes ? grilleNbColonnes : 0;
    m_grillePasLignes = okGrillePasLignes ? grillePasLignes : 0.0;
    m_grillePasColonnes = okGrillePasColonnes ? grillePasColonnes : 0.0;
    m_grilleOrigine = m_values.value("grille.origine");
    m_grilleNbMeasureMin = okGrilleNbMeasureMin ? grilleNbMeasureMin : 0.0;

    qDebug() << "[ImportToml] [Grille] nb_lignes           =" << m_geometrieGeneratrices;
    qDebug() << "[ImportToml] [Grille] nb_colonnes         =" << m_grilleNbColonnes
             << (okGrilleNbColonnes ? "" : "(missing/invalid)");
    qDebug() << "[ImportToml] [Grille] pas_lignes          =" << m_grillePasLignes
             << (okGrillePasLignes ? "" : "(missing/invalid)");
    qDebug() << "[ImportToml] [Grille] pas_colonnes        =" << m_grillePasColonnes
             << (okGrillePasColonnes ? "" : "(missing/invalid)");
    qDebug() << "[ImportToml] [Grille] origine             =" << m_grilleOrigine;
    qDebug() << "[ImportToml] [Grille] nb_measures_minimum =" << m_grilleNbMeasureMin
             << (okGrilleNbMeasureMin ? "" : "(missing/invalid)");
}

// -- [[interruption]] array (optional): pour chaque array: ligne / position_mm / etendue_mm / cause / description
void ImportToml::readInterruptionBlock()
{
    m_interruptions.clear();

    const int entryCount = m_arrayCounts.value("interruption", 0);
    if( entryCount <= 0 ) {
        qDebug() << "[ImportToml] [[interruption]] none";
        return;
    }

    for( int e = 0; e < entryCount; ++e ) {
        const QString prefix = QStringLiteral("interruption.%1.").arg(e);

        Interruption it;
        int ligne;
        double positionMm;
        double etendueMm;
        QString cause;
        QString description;

        it.ligne       = m_values.value(prefix + "ligne").toInt();
        it.positionMm  = m_values.value(prefix + "position_mm").toDouble();
        it.etendueMm   = m_values.value(prefix + "etendue_mm").toDouble();
        it.cause       = m_values.value(prefix + "cause");
        it.description = m_values.value(prefix + "description");
        m_interruptions.append(it);

        qDebug() << "[ImportToml] [[interruption]]" << (e + 1)
                 << "ligne =" << it.ligne
                 << "position_mm =" << it.positionMm
                 << "etendue_mm =" << it.etendueMm
                 << "cause =" << it.cause
                 << "description =" << it.description;
    }
}

// -- [evaluation] block: evaluation critere
// (if == selon_code): code_applicable / contrainte_admissible / coefficient_joint
// (if == pourcentage_nominal):  reference_perte / seuil_vert_orange_pct / seuil_orange_rouge_pct
// procedure_reference / procedure_indice
void ImportToml::readEvaluationBlock()
{
    bool okEvaluationContraint   = false;
    bool okEvaluationCoefficient = false;
    bool okevaluationEpNominale  = false;
    bool okEvaluationPerte       = false;
    bool okEvaluationSeuilVto    = false;
    bool okEvaluationSeuilOtr    = false;

    m_evaluationCritere = m_values.value("evaluation.critere");

    if(m_evaluationCritere == "selon_code")
    {
        m_evaluationCode = m_values.value("evaluation.code_applicable");
        m_evaluationCodeIndex = m_values.value("evaluation.code_indice");

        const double evaluationContraint   = m_values.value("evaluation.contrainte_admissible").toDouble(&okEvaluationContraint);
        const double evaluationCoefficient = m_values.value("evaluation.coefficient_joint").toDouble(&okEvaluationCoefficient);

        m_evaluationContraint   = okEvaluationContraint   ? evaluationContraint   : 0.0;
        m_evaluationCoefficient = okEvaluationCoefficient ? evaluationCoefficient : 0.0;
    } else if(m_evaluationCritere == "pourcentage_nominal")
    {
        const double evaluationEpNominale = m_values.value("evaluation.ep_nominale").toDouble(&okevaluationEpNominale);
        const double evaluationSeuilVto = m_values.value("evaluation.seuil_vert_orange_pct").toDouble(&okEvaluationSeuilVto);
        const double evaluationSeuilOtr = m_values.value("evaluation.seuil_orange_rouge_pct").toDouble(&okEvaluationSeuilOtr);

        m_evaluationEpNominale = okevaluationEpNominale ? evaluationEpNominale : 0.0;
        m_evaluationSeuilVto = okEvaluationSeuilVto ? evaluationSeuilVto : 10.0;
        m_evaluationSeuilOtr = okEvaluationSeuilOtr ? evaluationSeuilOtr : 50.0;
    }

    qDebug() << "[ImportToml] [evaluation] critere                  =" << m_evaluationCritere;

    if(m_evaluationCritere == "selon_code")
    {
        qDebug() << "[ImportToml] [evaluation] code_applicable          =" << m_evaluationCode;
        qDebug() << "[ImportToml] [evaluation] code_indice              =" << m_evaluationCodeIndex;
        qDebug() << "[ImportToml] [evaluation] contrainte_admissible    =" << m_evaluationContraint
                 << (okEvaluationContraint ? "" : "(missing/invalid)");
        qDebug() << "[ImportToml] [evaluation] coefficient_joint        =" << m_evaluationCoefficient
                 << (okEvaluationCoefficient ? "" : "(missing/invalid)");
    } else if(m_evaluationCritere == "pourcentage_nominal")
    {
        qDebug() << "[ImportToml] [evaluation] ep_nominale              =" << m_evaluationEpNominale
                 << (okevaluationEpNominale ? "" : "(missing/invalid)");
        qDebug() << "[ImportToml] [evaluation] seuil_vert_orange_pct    =" << m_evaluationSeuilVto
                 << (okEvaluationSeuilVto ? "" : "(missing/invalid)");
        qDebug() << "[ImportToml] [evaluation] seuil_orange_rouge_pct   =" << m_evaluationSeuilOtr
                 << (okEvaluationSeuilOtr ? "" : "(missing/invalid)");
    }
}

// -- [[campagne]] block (optional): data / methode / ep_min_releve / defauts
void ImportToml::readCampagneBlock()
{
    m_campagnes.clear();

    const int entryCount = m_arrayCounts.value("campagne", 0);
    if( entryCount <= 0 ) {
        qDebug() << "[ImportToml] [[campagne]] none";
        return;
    }

    for( int e = 0; e < entryCount; ++e ) {
        const QString prefix = QStringLiteral("campagne.%1.").arg(e);

        bool okEpMin = false;
        Campagne cp;
        QString campagneData;
        QString campagneMethode;
        double campagneEpMin;
        QString campagneDefauts;
        QString campagneComments;
        cp.campagneData       = m_values.value(prefix + "data");
        cp.campagneMethode  = m_values.value(prefix + "methode");
        cp.campagneEpMin   = m_values.value(prefix + "ep_min_releve").toDouble(&okEpMin);
        cp.campagneDefauts     = m_values.value(prefix + "defauts");
        cp.campagneComments    = m_values.value(prefix + "commentaires");

        m_campagnes.append(cp);

        qDebug() << "[ImportToml] [[campagne]]" << (e + 1)
                 << "data = " << cp.campagneData
                 << "methode = " << cp.campagneMethode
                 << "ep_min_releve = " << cp.campagneEpMin
                 << "defauts = " << cp.campagneDefauts
                 << "commentaires = " << cp.campagneComments;
    }
}

// -- [[visite]] block : data / type / conclusion / echeance
void ImportToml::readVisiteBlock()
{
    m_visites.clear();

    const int entryCount = m_arrayCounts.value("visite", 0);
    if( entryCount <= 0 ) {
        qDebug() << "[ImportToml] [[visite]] none";
        return;
    }

    for( int e = 0; e < entryCount; ++e ) {
        const QString prefix = QStringLiteral("visite.%1.").arg(e);

        Visite vi;
        QString visiteData;
        QString visiteType;
        QString visiteConclusion;
        QString visiteEcheance;
        QString visiteComments;

        vi.visiteData       = m_values.value(prefix + "data");
        vi.visiteType  = m_values.value(prefix + "type");
        vi.visiteConclusion   = m_values.value(prefix + "conclusion");
        vi.visiteEcheance     = m_values.value(prefix + "echeance");
        vi.visiteComments     = m_values.value(prefix + "commentaires");

        m_visites.append(vi);

        qDebug() << "[ImportToml] [[visite]]" << (e + 1)
                 << "data = " << vi.visiteData
                 << "type = " << vi.visiteType
                 << "conclusion = " << vi.visiteConclusion
                 << "echeance = " << vi.visiteEcheance
                 << "commentaires =" << vi.visiteComments;
    }
}


// -- [[Fait_notables]] structure block
void ImportToml::readFaitBlock()
{
    m_faits.clear();

    const int entryCount = m_arrayCounts.value("fait", 0);
    if( entryCount <= 0 ) {
        qDebug() << "[ImportToml] [[fait]] none";
        return;
    }

    for( int e = 0; e < entryCount; ++e ) {
        const QString prefix = QStringLiteral("fait.%1.").arg(e);

        Fait f;
        QString faitDate;
        QString faitNature;
        QString faitDescription;
        QString faitComments;

        f.faitDate       = m_values.value(prefix + "date");
        f.faitNature  = m_values.value(prefix + "nature");
        f.faitDescription   = m_values.value(prefix + "description");
        f.faitComments      = m_values.value(prefix + "commentaires");

        m_faits.append(f);

        qDebug() << "[ImportToml] [[fait]]" << (e + 1)
                 << "date = " << f.faitDate
                 << "nature = " << f.faitNature
                 << "description = " << f.faitDescription
                 << "commentaires = " << f.faitComments;
    }
}

// -- [Reperage] block : type / description / sens_rotation / sens_acquisition / unite_affichage / sens_observation / repere_observation
void ImportToml::readReperageBlock()
{
    m_reperageMode = m_values.value("reperage.mode");

    qDebug() << "[ImportToml] [Repere] mode                 =" << m_reperageMode;

    if(m_reperageMode == "baque_stockage") {
        m_reperageOriginType        = m_values.value("reperage.origin_type");
        m_reperageDescription       = m_values.value("reperage.origine_description");
        if(m_reperageOriginType == "autre_repere"){
            m_reperageReference     = m_values.value("reperage.origine_reference");
        }
        m_reperageSensRotation      = m_values.value("reperage.sens_rotation");
        m_reperageSens              = m_values.value("reperage.sens_acquisition");
        m_reperageObservation       = m_values.value("reperage.repere_observation");
        m_reperageAngle             = m_values.value("reperage.repere_angle").toDouble();
        m_reperageDegresUnit        = m_values.value("reperage.unite_affichage");
        m_repereHauteur             = m_values.value("reperage.repere_hauteur").toDouble();
    }else if(m_reperageMode == "libre"){
        readReperageLibre();
    }

    if(m_reperageMode == "baque_stockage") {
        qDebug() << "[ImportToml] [Repere] origin_type          =" << m_reperageOriginType;
        qDebug() << "[ImportToml] [Repere] origine_description  =" << m_reperageDescription;
        if(m_reperageOriginType == "autre_repere"){
            qDebug() << "[ImportToml] [Repere] origine_reference    =" << m_reperageReference;
        }
        qDebug() << "[ImportToml] [Repere] sens_rotation        =" << m_reperageSensRotation;
        qDebug() << "[ImportToml] [Repere] sens_acquisition     =" << m_reperageSens;
        qDebug() << "[ImportToml] [Repere] repere_observation   =" << m_reperageObservation;
        qDebug() << "[ImportToml] [Repere] repere_angle         =" << m_reperageAngle;
        qDebug() << "[ImportToml] [Repere] unite_affichage      =" << m_reperageDegresUnit;
        qDebug() << "[ImportToml] [Repere] repere_hauteur       =" << m_repereHauteur;

    }else if(m_reperageMode == "libre"){

    }
}

void ImportToml::readReperageLibre()
{
    const QString prefix = QStringLiteral("reperage.libre_data_");

    QMap<int, QString> byNumber;
    for( auto it = m_values.constBegin(); it != m_values.constEnd(); ++it) {
        if( !it.key().startsWith(prefix)) {
            continue;
        }
        bool ok = false;
        const int number = it.key().mid(prefix.length()).toInt(&ok);
        if(!ok) {
            report("warning", tr("Clé reperage invalide :&1").arg(it.key()));
            continue;
        }
        byNumber.insert(number, it.value());
    }

    m_reperageLibres = byNumber.values();

    for( auto it = byNumber.constBegin(); it != byNumber.constEnd(); ++it) {
        qDebug() << "[ImportToml] [reperage] libre_data_" << it.key() << "=" << it.value();
    }
    if(m_reperageLibres.isEmpty() ){
        qDebug() << "[ImportToml] [reperage] libre : none";
    }
}


// -- [obstacle] block : mode / libre_obstacle_N (mode "libre")
void ImportToml::readObstacleBlock()
{
    m_obstacleMode = m_values.value("obstacle.mode");
    m_obstacleLibres.clear();

    qDebug() << "[ImportToml] [obstacle] mode = " << m_obstacleMode;

    if( m_obstacleMode == "baque_stockage") {
        readTrouHommeBlock();
        readPiquageBlock();
        readRaidisseurBlock();
        readAccesBlock();
    } else if( m_obstacleMode == "libre" ) {
        readObstacleLibre();
    }
}

// -- [obstacle] mode "libre" : libre_obstacle_1, libre_obstacle_2 ... (unknown count)
void ImportToml::readObstacleLibre()
{
    const QString prefix = QStringLiteral("obstacle.libre_obstacle_");

    // m_values is sorted as text ("_10" before "_2") : sort by number, gaps are allowed
    QMap<int, QString> byNumber;
    for( auto it = m_values.constBegin(); it != m_values.constEnd(); ++it ) {
        if( !it.key().startsWith(prefix) ) {
            continue;
        }
        bool ok = false;
        const int number = it.key().mid(prefix.length()).toInt(&ok);
        if( !ok ) {
            report("warning", tr("Clé obstacle invalide : %1").arg(it.key()));
            continue;
        }
        byNumber.insert(number, it.value());
    }

    m_obstacleLibres = byNumber.values();

    for( auto it = byNumber.constBegin(); it != byNumber.constEnd(); ++it ) {
        qDebug() << "[ImportToml] [obstacle] libre_obstacle_" << it.key() << "=" << it.value();
    }
    if( m_obstacleLibres.isEmpty() ) {
        qDebug() << "[ImportToml] [obstacle] libre : none";
    }
}

// -- [[trou_homme]] block : repere / virole / angle_deg / diametre / obstacle
void ImportToml::readTrouHommeBlock()
{
    m_trouHommes.clear();

    const int entryCount = m_arrayCounts.value("trou_homme", 0);
    if( entryCount <= 0 ) {
        qDebug() << "[ImportToml] [[trou_homme]] none";
        return;
    }

    for( int e = 0; e < entryCount; ++e ) {
        const QString prefix = QStringLiteral("trou_homme.%1.").arg(e);

        TrouHomme th;
        QString trouHommeRepere;
        int trouHommeVirole;
        double trouHommeAngle;
        double trouHommeCentre;
        bool trouHommeObstacle;

        th.trouHommeRepere      = m_values.value(prefix + "repere");
        th.trouHommeVirole  = m_values.value(prefix + "virole").toInt();
        th.trouHommeAngle   = m_values.value(prefix + "angle_deg").toDouble();
        th.trouHommeCentre     = m_values.value(prefix + "centre").toDouble();
        if(m_values.value(prefix + "Obstacle") == "true"){
            th.trouHommeObstacle = true;
        }else if(m_values.value(prefix + "Obstacle") == "false"){
            th.trouHommeObstacle = false;
        }

        m_trouHommes.append(th);

        qDebug() << "[ImportToml] [[trou_homme]]" << (e + 1)
                 << "repere =" << th.trouHommeRepere
                 << "virole =" << th.trouHommeVirole
                 << "angle_deg =" << th.trouHommeAngle
                 << "centre =" << th.trouHommeCentre
                 << "Obstacle =" << th.trouHommeObstacle;
    }
}

// -- [[piquage]] block : repere / virole / angle / centre / obstacle
void ImportToml::readPiquageBlock()
{
    m_piquages.clear();

    const int entryCount = m_arrayCounts.value("piquage", 0);
    if( entryCount <= 0 ) {
        qDebug() << "[ImportToml] [[piquage]] none";
        return;
    }

    for( int e = 0; e < entryCount; ++e ) {
        const QString prefix = QStringLiteral("piquage.%1.").arg(e);

        Piquage pi;
        QString piquageRepere;
        int piquageVirole;
        double piquageAngle;
        double piquageCentre;
        bool piquageObstacle;

        pi.piquageRepere      = m_values.value(prefix + "repere");
        pi.piquageVirole  = m_values.value(prefix + "virole").toInt();
        pi.piquageAngle   = m_values.value(prefix + "angle_deg").toDouble();
        pi.piquageCentre     = m_values.value(prefix + "centre").toDouble();
        if(m_values.value(prefix + "Obstacle") == "true"){
            pi.piquageObstacle = true;
        }else if(m_values.value(prefix + "Obstacle") == "false"){
            pi.piquageObstacle = false;
        }

        m_piquages.append(pi);

        qDebug() << "[ImportToml] [[piquage]]" << (e + 1)
                 << "repere =" << pi.piquageRepere
                 << "virole =" << pi.piquageVirole
                 << "angle_deg =" << pi.piquageAngle
                 << "centre =" << pi.piquageCentre
                 << "Obstacle =" << pi.piquageObstacle;
    }
}

// -- [[raidisseur]] block : repere / hauteuer
void ImportToml::readRaidisseurBlock()
{
    m_raidisseurs.clear();

    const int entryCount = m_arrayCounts.value("raidisseur", 0);
    if( entryCount <= 0 ) {
        qDebug() << "[ImportToml] [[raidisseur]] none";
        return;
    }

    for( int e = 0; e < entryCount; ++e ) {
        const QString prefix = QStringLiteral("raidisseur.%1.").arg(e);

        Raidisseur raid;
        QString raidisseurRepere;
        double raidisseurHauteur;

        raid.raidisseurRepere      = m_values.value(prefix + "repere");
        raid.raidisseurHauteur   = m_values.value(prefix + "hauteur").toDouble();

        m_raidisseurs.append(raid);

        qDebug() << "[ImportToml] [[raidisseur]]" << (e + 1)
                 << "repere  =" << raid.raidisseurRepere
                 << "hauteur =" << raid.raidisseurHauteur;
    }
}

void ImportToml::readAccesBlock()
{
    m_accesses.clear();

    const int entryCount = m_arrayCounts.value("acces", 0);
    if( entryCount <= 0) {
        qDebug() << "[Import] [[acces]] none";
        return;
    }

    for(int e = 0; e < entryCount; ++e) {
        const QString prefix = QStringLiteral("acces.%1.").arg(e);

        Acces ac;
        QString accesRepere;
        QString accesType;
        double accesAngleDepart;
        double accesAngleArrive;

        ac.accesRepere      = m_values.value(prefix + "repere");
        ac.accesType        = m_values.value(prefix + "type");
        ac.accesAngleDepart = m_values.value(prefix + "angle_depart_deg").toDouble();
        ac.accesAngleArrive = m_values.value(prefix + "angle_arrivee_deg").toDouble();

        m_accesses.append(ac);

        qDebug() << "[ImportToml] [[acces]]" << (e + 1)
                 << "repere = " << ac.accesRepere
                 << "type = " << ac.accesType
                 << "angle_depart_deg = " << ac.accesAngleDepart
                 << "angle_arrivee_deg = " << ac.accesAngleArrive;
    }
}

void ImportToml::readZoneExclueBlock()
{
    m_zoneExclues.clear();

    const int entryCount = m_arrayCounts.value("zone_exclue", 0);
    if( entryCount <= 0) {
        qDebug() << "[Import] [[zone_exclue]] none";
        return;
    }

    for(int e = 0; e < entryCount; ++e) {
        const QString prefix = QStringLiteral("zone_exclue.%1.").arg(e);

        ZoneExclue ze;
        QString zoneExclueRepere;
        double zoneExclueDebut;
        double zoneExclueFin;
        double zoneExclueHautMin;
        double zoneExclueHautMax;
        QString zoneExclueMotif;


        ze.zoneExclueRepere = m_values.value(prefix + "repere");
        ze.zoneExclueDebut = m_values.value(prefix + "angle_debut").toDouble();
        ze.zoneExclueFin = m_values.value(prefix + "angle_fin").toDouble();
        ze.zoneExclueHautMin = m_values.value(prefix + "hauteur_min").toDouble();
        ze.zoneExclueHautMax = m_values.value(prefix + "hauteur_max").toDouble();
        ze.zoneExclueMotif = m_values.value(prefix + "motif");

        m_zoneExclues.append(ze);

        qDebug() << "[ImporToml] [[zone_exclue]]" << (e + 1)
                 << "repere = " << ze.zoneExclueRepere
                 << "angle_debut = " << ze.zoneExclueDebut
                 << "angle_fin = " << ze.zoneExclueFin
                 << "hauteur_min = " << ze.zoneExclueHautMin
                 << "hauteur_max = " << ze.zoneExclueHautMax
                 << "motif = " << ze.zoneExclueMotif;

    }
}

// Secuirity check
bool ImportToml::securityCheck(QString modifiedAtData)
{
    // -- Security check block
    m_firstImport = true;
    m_securityPlannedMissing = false;
    m_securityMissingFile = false;
    m_securityModifiedFile = false;
    m_securityIncompatibleDate = false;
    m_securityNonReadable = false;
    // -- Import is assumed correct until one of the checks below fails.
    m_keyIsValid = false;
    m_originalKey.clear();

    const QString shaFilePath = m_filePath + ".sha256";

    const QFileInfo tomlInfo(m_filePath);
    const QFileInfo shaInfo(shaFilePath);

    if( !tomlInfo.exists() ) {
        report("error", tr("Fichier planner introuvable : %1").arg(m_filePath));
        m_securityPlannedMissing = true;
        m_keyIsValid = true;
        return false;
    }
    if( !shaInfo.exists() ) {
        report("warning", tr("Fichier de sécurité introuvable : %1").arg(shaFilePath));
        m_securityMissingFile = true;
        m_keyIsValid = true;
        return false;
    }

    QFile shaFile(shaFilePath);
    if( !shaFile.open(QIODevice::ReadOnly | QIODevice::Text) ) {
        report("warning", tr("Impossible d'ouvrir le fichier de sécurité : %1").arg(shaFilePath));
        m_keyIsValid = true;
        return false;
    }
    const QString shaContent = QString::fromUtf8(shaFile.readAll()).trimmed();
    shaFile.close();

    // Key stored by the planner alongside the .toml.
    m_originalKey = shaContent.toUtf8();

    const QDateTime tomlCreated = tomlInfo.birthTime().isValid()
                                      ? tomlInfo.birthTime()
                                      : tomlInfo.lastModified();
    const QDateTime shaCreated  = shaInfo.birthTime().isValid()
                                     ? shaInfo.birthTime()
                                     : shaInfo.lastModified();

    const QString tomlCreatedStr = tomlCreated.toString(Qt::ISODate);
    const QString shaCreatedStr  = shaCreated.toString(Qt::ISODate);

    const qint64 kToleranceSeconds = 2;
    if( qAbs(tomlCreated.secsTo(shaCreated)) > kToleranceSeconds ) {
        report("warning", tr("Contrôle de sécurité échoué : la date du .sha256 ne correspond pas au .toml"));
        m_securityIncompatibleDate = true;
        m_keyIsValid = true;
        return false;
    } else {
        qDebug() << "[ImportToml] The import date is valid";
    }

    QFile tomlFile(m_filePath);
    if( !tomlFile.open(QIODevice::ReadOnly | QIODevice::Text) ) {
        report("error", tr("Impossible d'ouvrir le fichier planner : %1").arg(m_filePath));
        m_keyIsValid = true;
        return false;
    }
    QString payload = QString::fromUtf8(tomlFile.readAll());
    tomlFile.close();
    qDebug() << "[ImportTOml] sha from data :" << shaContent;

    m_computedKey = computeShaString(payload);   // same content as read(), recomputed defensively

    if(m_computedKey == m_originalKey){
        qDebug() << "[ImportTOml] strings are compatible";
        m_securityModifiedFile = false;
    }else{
        report("warning", tr("Fichier planner modifié : la clé SHA256 ne correspond pas au .sha256"));
        m_securityModifiedFile = true;
        m_keyIsValid = true;
    }

    return true;
}

// SHA256 computation
QByteArray ImportToml::computeShaString(QString data)
{
    qDebug() << "[ImportToml] sha data input =" << data;
    QByteArray hash = QCryptographicHash::hash(data.toUtf8(), QCryptographicHash::Sha256).toHex();
    qDebug() << "[ImportToml] sha computed =" << hash;
    return hash;
}
