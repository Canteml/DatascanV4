#pragma once

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QVector>
#include <QVariantList>

class ImportToml : public QObject
{
    Q_OBJECT

public:
    explicit ImportToml(const QString &filePath, QObject *parent = nullptr);

    // -- [[virole]] structure block : numero / hauteur / Epaisseur Nominale / nuance
    struct Virole {
        int viroleNumero;
        double viroleHauteur;
        double viroleEpNominale;
        QString viroleNuance;
    };

    // -- [[ligne]] structure block : code / angle / mode / vecteur / priorite / tolerance / description
    struct Ligne {
        QString ligneCode;
        double ligneAngle;
        QString ligneMode;
        QString ligneVecteur;
        int lignePriorite;
        double ligneTolerance;
        QString ligneDescription;
    };

    // -- One [[interruption]] entry (all fields kept as raw strings, "" when absent).
    struct Interruption {
        int ligne;
        double positionMm;
        double etendueMm;
        QString cause;
        QString description;
    };

    // -- [[campagne]] structure block
    struct Campagne {
        QString campagneData;
        QString campagneMethode;
        double campagneEpMin;
        QString campagneDefauts;
        QString campagneComments;
    };

    // -- [[visite]] strucutre block
    struct Visite {
        QString visiteData;
        QString visiteType;
        QString visiteConclusion;
        QString visiteEcheance;
        QString visiteComments;
    };

    // -- [[Fait_notables]] structure block
    struct Fait {
        QString faitDate;
        QString faitNature;
        QString faitDescription;
        QString faitComments;
    };

    // -- [[trou_homme]] structure block
    struct TrouHomme {
        QString trouHommeRepere;
        int trouHommeVirole;
        double trouHommeAngle;
        double trouHommeCentre;
        bool trouHommeObstacle;
    };

    // -- [[piquage]] structure block
    struct Piquage {
        QString piquageRepere;
        int piquageVirole;
        double piquageAngle;
        double piquageCentre;
        bool piquageObstacle;
    };

    struct Raidisseur {
        QString raidisseurRepere;
        double raidisseurHauteur;
    };

    struct Acces {
        QString accesRepere;
        QString accesType;
        double accesAngleDepart;
        double accesAngleArrive;
    };

    struct ZoneExclue {
        QString zoneExclueRepere;
        double zoneExclueDebut;
        double zoneExclueFin;
        double zoneExclueHautMin;
        double zoneExclueHautMax;
        QString zoneExclueMotif;
    };

    bool read();
    bool securityCheck(QString modifiedAtData);

    // -- Security check results (valid after read())
    bool firstImport()              const { return m_firstImport; }
    // -- true as soon as one planner .toml has been imported (parsed).
    bool fileIsImported()           const { return m_fileIsImported; }
    // -- File name (without path) of the imported planner .toml, "" when none.
    QString importedFileName()      const { return m_importedFileName; }
    // -- true when the import is NOT correct (a security check failed): in that
    // case the original key must also be recorded next to the computed one.
    bool keyIsValid()               const { return m_keyIsValid; }
    // -- SHA256 of the imported planner file, and the key stored in the sidecar
    // ".sha256" file (empty when that file is missing/unreadable).
    QByteArray computedKey()        const { return m_computedKey; }
    QByteArray originalKey()        const { return m_originalKey; }
    bool securityPlannedMissing()   const { return m_securityPlannedMissing; }
    bool securityMissingFile()      const { return m_securityMissingFile; }
    bool securityModifiedFile()     const { return m_securityModifiedFile; }
    bool securityIncompatibleDate() const { return m_securityIncompatibleDate; }
    bool securityNonReadable()      const { return m_securityNonReadable; }

    // -- Import messages shown next to the "Charger une fichier" button (CentralPage0.qml).
    // Each entry is a map { "level": "error" | "warning" | "ok", "text": QString }.
    QVariantList messages()         const { return m_messages; }

    /// Raw access to every parsed entry, keyed "section.key" (e.g. "object.site").
    QString value(const QString &key) const { return m_values.value(key); }
    QMap<QString, QString> values() const { return m_values; }

    // -- [schema] block
    QString tomlSchemaVersion()  const { return m_tomlSchemaVersion; }
    QString tomlSchemaProducer() const { return m_tomlSchemaProducer; }
    QString tomlSchemaAutor()    const { return m_tomlSchemaAutor; }
    QString tomlSchemaNom()      const { return m_tomlSchemaNom; }
    QString tomlSchemaClient()  const { return m_tomlSchemaClient; }
    QString tomlSchemaSite()  const { return m_tomlSchemaSite; }
    QString tomlSchemaCommand()  const { return m_tomlSchemaCommand; }
    QString tomlSchemaDate()  const { return m_tomlSchemaDate; }


    // -- [equipment] block
    QString tomlEquipmentNom() const { return m_equipmentNom; }
    QString tomlEquipmentNumero() const { return m_equipmentNumero; }
    QString tomlEquipmentDate() const { return m_equipmentDate; }
    QString inspectorNom() const { return m_inspectorNom; }
    QString inspectorNumero() const { return m_inspectorNumero; }
    QString inspectorDate() const { return m_inspectorDate; }
    QString supervisorNom() const { return m_supervisorNom; }
    QString supervisorNumero() const { return m_supervisorNumero; }
    QString supervisorDate() const { return m_supervisorDate; }
    QString sondeNumero() const { return m_sondeNumero; }
    QString controlleurNom() const { return m_controlleurNom; }

    // -- [object] block
    QString objectType()    const { return m_objectType; }
    QString objectProduit() const { return m_objectProduit; }
    QString objectDensity() const { return m_objectDensity; }
    QString objectHauter()  const { return m_objectHauter; }

    // -- Referentiel et nbGeneratrices (valide pour robe, toit et fond)
    QString geometrieReferentiel()  const { return m_geometrieReferentiel; }
    int geometrieGeneratrices()     const { return m_geometrieGeneratrices; }

    // -- [geometrie.robe] block
    double robeDiametre()   const { return m_robeDiametre; }
    int robeNombreViroles() const { return m_robeNombreViroles; }

    // -- [[virole]] array, one entry per virole (bounded by [geometrie.robe].nb_viroles).
    QVector<Virole> viroles() const { return m_viroles; }
    int viroleCount()         const { return m_viroles.size(); }

    // -- [[ligne]] array
    QVector<Ligne> lignes() const { return m_lignes; }
    int ligneCount()        const { return m_lignes.size(); }

    // -- [[interruption]] array (optional): one entry per interruption, empty when
    // the planner declares none.
    QVector<Interruption> interruptions() const { return m_interruptions; }
    int interruptionCount()               const { return m_interruptions.size(); }

    // -- [geometrie.tuyauterie] block
    double tuyauterieDiametreMm()   const { return m_tuyauterieDiamtereMm; }
    double tuyauterieDiametreIn()   const { return m_tuyauterieDiamtereIn; }
    double tuyauterieEpaisseur()    const { return m_tuyauterieEpaisseur; }
    double tuyauterieLongueur()     const { return m_tuyauterieLongueur; }

    // -- [geometrie.coque] block
    QString coquePositionX()   const { return m_coquePositionX; }
    QString coqueOrientation() const { return m_coqueOrientation; }
    double coqueLongueur()     const { return m_coqueLongueur; }
    QString coqueDescription() const { return m_coqueDescription; }

    // -- [geometrie.fond] block
    double fondDiametre()   const { return m_fondDiametre; }
    double fondEpNominale() const { return m_fondEpNominale; }
    double fondNbAnnulair() const { return m_fondNbAnnulair; }
    double fondNbCentrale() const { return m_fondNbCentrale; }

    // -- [geometrie.toit] block
    double toitDiametre()   const { return m_toitDiametre; }
    double toitEpNominale() const { return m_toitEpNominale; }
    QString toitType()      const { return m_toitType; }

    // -- [Acquisition] block
    double acquisitionZoneMortePied() const { return m_acquisitionZoneMortePied; }
    double acquisitionZoneMorteTete() const { return m_acquisitionZoneMorteTete; }

    // -- [[interruption]] : see interruptions() / interruptionCount() above.

    // -- [Grille] block (bounded with [Acquisition].mode = "point_par_point")
    int grilleNbLignes() const { return m_grilleNbLignes; }
    int grilleNbColonnes() const { return m_grilleNbColonnes; }
    double grillePasLignes() const { return m_grillePasLignes; }
    double grillePasColonnes() const { return m_grillePasColonnes; }
    QString grilleOrigine() const { return m_grilleOrigine; }
    double grilleNbMeasureMin() const { return m_grilleNbMeasureMin; }

    // -- [evaluation] block
    QString evaluationCritere()    const { return m_evaluationCritere; }
    double evaluationEpNominale()  const { return m_evaluationEpNominale; }
    QString evaluationCode()       const { return m_evaluationCode; }
    QString evaluationCodeIndex()  const { return m_evaluationCodeIndex; }
    double evaluationContraint()   const { return m_evaluationContraint; }
    double evaluationCoefficient() const { return m_evaluationCoefficient; }
    double evaluationPerte()       const { return m_evaluationPerte; }
    double evaluationSeuilVto()    const { return m_evaluationSeuilVto; }
    double evaluationSeuilOtr()    const { return m_evaluationSeuilOtr; }

    // -- [[campagne]] block
    QVector<Campagne> campagnes() const { return m_campagnes; }
    int campagneCount()           const { return m_campagnes.size(); }

    // -- [[visites]] block
    QVector<Visite> visites() const { return m_visites; }
    int visiteCount()         const { return m_visites.size(); }

    // -- [[fait_notable]] block
    QVector<Fait> faits() const { return m_faits; }
    int faitCount()       const { return m_faits.size(); }

    // -- [Reperage] block
    QString reperageMode()              const { return m_reperageMode; }
    QString reperageOriginType()        const { return m_reperageOriginType; }
    QString reperageReference()         const { return m_reperageReference; }
    QString reperageDescription()       const { return m_reperageDescription; }
    QString reperageSensRotation()      const { return m_reperageSensRotation; }
    QString reperageSensAcquisition()   const { return m_reperageSens; }
    QString reperageObservation()       const { return m_reperageObservation; }
    double reperageAngle()              const { return m_reperageAngle; }
    QString reperageDegresUnit()        const { return m_reperageDegresUnit; }
    double repereHauteur()              const { return m_repereHauteur; }


    // -- [obstacle] block
    QString obstacleMode()  const { return m_obstacleMode; }
    QStringList obstacleLibres() const { return m_obstacleLibres; }   ///< libre_obstacle_1, _2 ... sorted by number

    // -- [[trou_homme]] block
    QVector<TrouHomme> trouHommes() const { return m_trouHommes; }
    int trouHommeCount()            const { return m_trouHommes.size(); }

    // -- [[piquage]] block
    QVector<Piquage> piquages() const { return m_piquages; }
    int piquageCount()            const { return m_piquages.size(); }

    // -- [[radisseur]] block
    QVector<Raidisseur> raidisseurs() const { return m_raidisseurs; }
    int raidisseurCount()             const { return m_raidisseurs.size(); }

    // -- [[acces]] block
    QVector<Acces> accesses() const { return m_accesses; }
    int accesCount()          const { return m_accesses.size(); }

    // -- [[zone_exclue]] block
    QVector<ZoneExclue> zoneExclues() const { return m_zoneExclues; }
    int zoneExclueCount()             const { return m_zoneExclues.size(); }

public slots:
    QByteArray computeShaString(QString data);

signals:
    void finImport(bool fail);

private:
    // Minimal TOML reader : [section] headers + `key = value` lines.
    bool parseToml(const QString &content);

    // Log a problem on the console and keep it for the UI (level: "error" / "warning").
    void report(const QString &level, const QString &text);

    // Fill the typed members from the generic map, one block at a time.
    void readSchemaBlock();
    void readEquipmentBlock();
    void readObjectBlock();
    void readGeomRobeBlock();
    void readViroleBlock();
    void readGeomTuyaBlock();
    void readGeomCoqueBlock();
    void readGeomFondBlock();
    void readGeomToitBlock();
    void readAcquisitionBlock();
    void readLigneBlock();
    void readGrilleBlock();
    void readInterruptionBlock();
    void readEvaluationBlock();
    void readCampagneBlock();
    void readVisiteBlock();
    void readFaitBlock();
    void readReperageBlock();
    void readReperageLibre();
    void readObstacleBlock();
    void readObstacleLibre();
    void readTrouHommeBlock();
    void readPiquageBlock();
    void readRaidisseurBlock();
    void readAccesBlock();
    void readZoneExclueBlock();

    QString m_filePath;
    QMap<QString, QString> m_values;
    // Number of elements seen for each [[array-of-tables]] name during parsing.
    QMap<QString, int> m_arrayCounts;

    // -- [schema] block
    QString m_tomlSchemaVersion;
    QString m_tomlSchemaProducer;
    QString m_tomlSchemaAutor;
    QString m_tomlSchemaNom;
    QString m_tomlSchemaClient;
    QString m_tomlSchemaSite;
    QString m_tomlSchemaCommand;
    QString m_tomlSchemaDate;

    // -- [equipment] block
    QString m_equipmentNom;
    QString m_equipmentNumero;
    QString m_equipmentDate;
    QString m_inspectorNom;
    QString m_inspectorNumero;
    QString m_inspectorDate;
    QString m_supervisorNom;
    QString m_supervisorNumero;
    QString m_supervisorDate;

    // -- [object] block
    QString m_objectType;
    QString m_objectProduit;
    QString m_objectDensity;
    QString m_objectHauter;

    // -- Referentiel (valide pour robe, toit et fond)
    QString m_geometrieReferentiel;
    int m_geometrieGeneratrices;

    // -- [geometrie.robe] block
    double m_robeDiametre = 0.0;
    int m_robeNombreViroles = 0;

    // -- [[virole]] array
    QVector<Virole> m_viroles;

    // -- [geometrie.fond] block
    double m_fondDiametre;
    double m_fondEpNominale;
    double m_fondNbAnnulair;
    double m_fondNbCentrale;

    // -- [geometrie.toit] block
    double m_toitDiametre;
    double m_toitEpNominale;
    QString m_toitType;

    // -- [geometrie.tuyauterie] block
    double m_tuyauterieDiamtereMm;
    double m_tuyauterieDiamtereIn;
    double m_tuyauterieEpaisseur;
    double m_tuyauterieLongueur;

    // -- [geometrie.coque] block
    QString m_coquePositionX;
    QString m_coqueOrientation;
    double m_coqueLongueur;
    QString m_coqueDescription;

    // -- [acquisition] block
    double m_acquisitionZoneMortePied;
    double m_acquisitionZoneMorteTete;

    // -- [Ligne] block
    QVector<Ligne> m_lignes;

    // -- [[interruption]] array (empty when the planner declares none)
    QVector<Interruption> m_interruptions;

    // -- [Grille] block (bounded with [Acquisition].mode = "point_par_point")
    int m_grilleNbLignes = 0;
    int m_grilleNbColonnes = 0;
    double m_grillePasLignes = 0.0;
    double m_grillePasColonnes = 0.0;
    QString m_grilleOrigine;
    double m_grilleNbMeasureMin = 0.0;

    // -- [evaluation] block
    QString m_evaluationCritere;
    double m_evaluationEpNominale = 0.0;
    QString m_evaluationCode;
    QString m_evaluationCodeIndex;
    double m_evaluationContraint    = 0.0;
    double m_evaluationCoefficient  = 0.0;
    double m_evaluationPerte        = 0.0;
    double m_evaluationSeuilVto     = 0.0;
    double m_evaluationSeuilOtr     = 0.0;

    // -- [[campagne]] block
    QVector<Campagne> m_campagnes;

    // -- [[visites]] block
    QVector<Visite> m_visites;

    // -- [[fait_notables]] block
    QVector<Fait> m_faits;

    // -- [Reperage] block
    QString m_reperageMode;
    QString m_reperageOriginType;
    QString m_reperageReference;
    QString m_reperageDescription;
    QString m_reperageSensRotation;
    QString m_reperageSens;
    QString m_reperageObservation;
    double m_reperageAngle;
    QString m_reperageDegresUnit;
    double m_repereHauteur;
    QStringList m_reperageLibres;
    QString m_sondeNumero;
    QString m_controlleurNom;

    // -- [obstacle] block
    QString m_obstacleMode;
    QStringList m_obstacleLibres;   ///< mode "libre" : values of libre_obstacle_N, sorted by N

    // -- [[trou_homme]] block
    QVector<TrouHomme> m_trouHommes;

    // -- [[piquge]] block
    QVector<Piquage> m_piquages;

    // -- [[radisseur]] block
    QVector<Raidisseur> m_raidisseurs;

    // -- [[acces]] block
    QVector<Acces> m_accesses;

    // -- [[zone_exclue]] block
    QVector<ZoneExclue> m_zoneExclues;

    // -- Security check block
    QString m_importedFileName;
    bool m_fileIsImported           = false;
    bool m_keyIsValid               = false;
    QByteArray m_computedKey;
    QByteArray m_originalKey;
    bool m_firstImport              = false;
    bool m_securityPlannedMissing   = false;
    bool m_securityMissingFile      = false;
    bool m_securityModifiedFile     = false;
    bool m_securityIncompatibleDate = false;
    bool m_securityNonReadable      = false;

    // -- Messages for the UI (see messages())
    QVariantList m_messages;
};
