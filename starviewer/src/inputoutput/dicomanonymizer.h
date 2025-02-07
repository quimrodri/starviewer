#ifndef UDGDICOMANONYMIZER_H
#define UDGDICOMANONYMIZER_H

#include <QHash>

#include "gdcmanonymizerstarviewer.h"

class QString;

namespace udg {

/**
    Aquesta classe permet anonimitzar fitxers DICOM seguint les normes descrites pel Basic Application Level Confidentiality Profile de DICOM
    que podem trobar al annex E del PS 3.15, mantenint la consist�ncia en tags com Frame Of Reference o Image Of Reference, Study Instance UID,
    Series Instance UID, ... despr�s de ser anonimitzats. Per defecte tamb� treu els tags privats de les imatges ja que aquests poden contenir
    informaci� sensible del pacient, ens aconsellen que els treiem a http://groups.google.com/group/comp.protocols.dicom/browse_thread/thread/fb89f7f5d120db44

    Ens permet anonimitzar fitxers sols o tots els fitxers dins i subdirectoris del directori especificat.
  */
class DICOMAnonymizer {

public:
    DICOMAnonymizer();
    ~DICOMAnonymizer();

    /// Ens anonimitza els fitxers d'un Directori
    bool anonymyzeDICOMFilesDirectory(const QString &directoryPath);

    /// Ens anonimitza un fitxer DICOM
    /// Atenci�!!! si utilitzem aquesta opci� per anonimitzar diversos fitxers d'un mateix estudi, aquests fitxers s'han d'anonimitzar utilitzant la mateixa
    /// inst�ncia del DICOMAnonymizer per mantenir la consit�ncia de Tags com Study Instance UID, Series Instance UID, Frame Of Reference, Image Reference ...
    /// Si no es respecta aquest requisit passar� que imatges d'un mateix estudi despr�s de ser anonimitzades tindran Study Instance UID diferents.
    bool anonymizeDICOMFile(const QString &inputPathFile, const QString &outputPathFile);

    /// Ens indica quin nom de pacient han de tenir els estudis anonimitzats. El nom no pot tenir m�s de 64 car�cters seguint la normativa DICOM per a tags de
    /// tipus PN (Person Name) si es passa un nom de m�s de 64 car�cters es trunca.
    void setPatientNameAnonymized(const QString &patientNameAnonymized);
    QString getPatientNameAnonymized() const;

    /// En comptes d'eliminar el valor del StudyID tal com indica el Basic Profile el substitueix per un valor arbitrari
    /// Aquesta opci� est� pensada pel DICOMDIR en que �s obligatori que els estudis tinguin PatientID, tots els estudis a anonimitzar que tinguin en com�
    /// mateix el Patient ID abans de ser anonimitzats, despr�s de ser-ho tindran un nou Patient ID en com�, d'aquesta manera es podr� veure que aquells estudis
    /// s�n del mateix pacient.
    void setReplacePatientIDInsteadOfRemove(bool replace);
    bool getReplacePatientIDInsteadOfRemove();

    /// En comptes d'eliminar el valor del StudyID tal com indica el Basic Profile el substitueix per un valor arbitrari
    /// Aquesta opci� est� pensada pel DICOMDIR en que �s obligatori que els estudis tinguin StudyID
    void setReplaceStudyIDInsteadOfRemove(bool replace);
    bool getReplaceStudyIDInsteadOfRemove();

    /// Indica si s'han de treure els tags privats de les imatges. Per defecte es treuen si no s'indica el contrari, ja que els tags privats poden
    /// contenir informaci� sensible del pacient que segons el Basic Application Level Confidentiality Profile de DICOM s'ha d'anonimitzar, com podria
    /// ser el nom del pacient, edat, ....
    void setRemovePrivateTags(bool removePritaveTags);
    bool getRemovePrivateTags();

private:
    /// Inicialitza les variables de gdcm necess�ries per anonimitzar
    void initializeGDCM();

    /// Retorna el valor de PatientID anonimitzat a partir del PatientID original del fitxer. Aquest m�tode �s consistent de manera que si li passem
    /// una o m�s vegades el mateix PatientID sempre retornar� el mateix valor com a PatientID anonimitzat.
    QString getAnonimyzedPatientID(const QString &originalPatientID);

    /// Retorna el valor de StudyID anonimitzat a partir del Study Instance UID original del fitxer. Aquest m�tode �s consistent de manera que si li passem
    /// una o m�s vegades el mateix study Instance UID sempre retornar� el mateix valor com de Study ID anonimitzat.
    QString getAnonymizedStudyID(const QString &originalStudyInstanceUID);

    /// Retorna el valor d'un Tag en un string, si no troba el tag retorna un string buit
    QString readTagValue(gdcm::File *gdcmFile, gdcm::Tag) const;

private:
    QString m_patientNameAnonymized;
    bool m_replacePatientIDInsteadOfRemove;
    bool m_replaceStudyIDInsteadOfRemove;
    bool m_removePritaveTags;

    QHash<QString, QString> m_hashOriginalPatientIDToAnonimyzedPatientID;
    QHash<QString, QString> m_hashOriginalStudyInstanceUIDToAnonimyzedStudyID;

    gdcm::gdcmAnonymizerStarviewer *m_gdcmAnonymizer;
};

};

#endif
