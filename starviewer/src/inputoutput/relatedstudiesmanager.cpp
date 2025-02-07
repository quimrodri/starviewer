#include "relatedstudiesmanager.h"

#include "study.h"
#include "dicommask.h"
#include "patient.h"
#include "pacsmanager.h"
#include "queryscreen.h"
#include "singleton.h"
#include "pacsdevicemanager.h"
#include "logging.h"
#include "querypacsjob.h"
#include "inputoutputsettings.h"

namespace udg {

RelatedStudiesManager::RelatedStudiesManager()
{
    m_pacsManager = new PacsManager();
    m_studyInstanceUIDOfStudyToFindRelated = "invalid";

    Settings settings;
    m_searchRelatedStudiesByName = settings.getValue(InputOutputSettings::SearchRelatedStudiesByName).toBool();
}

RelatedStudiesManager::~RelatedStudiesManager()
{
    cancelCurrentQuery();
    deleteQueryResults();
}

void RelatedStudiesManager::queryMergedStudies(Patient *patient)
{
    INFO_LOG("Es buscaran els estudis del pacient " + patient->getFullName() + " amb ID " + patient->getID());

    this->makeAsynchronousStudiesQuery(patient);
}

void RelatedStudiesManager::queryMergedPreviousStudies(Study *study)
{
    INFO_LOG("Es buscaran els estudis previs del pacient " + study->getParentPatient()->getFullName() + " amb ID " + study->getParentPatient()->getID() +
    " de l'estudi " + study->getInstanceUID() + " fet a la data " + study->getDate().toString());

    m_studyInstanceUIDOfStudyToFindRelated = study->getInstanceUID();

    this->makeAsynchronousStudiesQuery(study->getParentPatient(), study->getDate());
}

void RelatedStudiesManager::makeAsynchronousStudiesQuery(Patient *patient, QDate untilDate)
{
    initializeQuery();

    QList<PacsDevice> pacsDeviceListToQuery = PacsDeviceManager().getPACSList(PacsDeviceManager::PacsWithQueryRetrieveServiceEnabled, true);
    pacsDeviceListToQuery = PacsDeviceManager::removeDuplicateSamePACS(pacsDeviceListToQuery + getPACSRetrievedStudiesOfPatient(patient));

    if (pacsDeviceListToQuery.count() == 0)
    {
        // Sinó hi ha cap PACS pel qual cercar per defecte fem l'emit del queryFinished
        queryFinished();
        return;
    }

    QList<DicomMask> queryDicomMasksList;

    if (!patient->getID().isEmpty())
    {
        DicomMask maskQueryByID = getBasicDicomMask();
        maskQueryByID.setPatientID(patient->getID());
        queryDicomMasksList << maskQueryByID;
    }

    if (m_searchRelatedStudiesByName && !patient->getFullName().isEmpty())
    {
        DicomMask maskQueryByName = getBasicDicomMask();
        maskQueryByName.setPatientName(patient->getFullName());
        queryDicomMasksList << maskQueryByName;
    }

    if (queryDicomMasksList.count() == 0)
    {
        // Sinó hi ha cap cconsulta a fer donem la cerca per finalitzada
        queryFinished();
    }
    else
    {
        // Si ens diuen que volen els study's fins una data, hem de marcar aquesta data en els dicomMasks
        if (untilDate.isValid())
        {
            foreach (DicomMask dicomMask, queryDicomMasksList)
            {
                dicomMask.setStudyDate(QDate(), untilDate);
            }
        }

        foreach (const PacsDevice &pacsDevice, pacsDeviceListToQuery)
        {
            foreach (DicomMask queryDicomMask, queryDicomMasksList)
            {
                enqueueQueryPACSJobToPACSManagerAndConnectSignals(new QueryPacsJob(pacsDevice, queryDicomMask, QueryPacsJob::study));
            }
        }
    }
}

void RelatedStudiesManager::initializeQuery()
{
    cancelCurrentQuery();

    /// Fem neteja de consultes anteriors
    deleteQueryResults();
    m_pacsDeviceIDErrorEmited.clear();
}

void RelatedStudiesManager::enqueueQueryPACSJobToPACSManagerAndConnectSignals(QueryPacsJob *queryPACSJob)
{
    connect(queryPACSJob, SIGNAL(PACSJobFinished(PACSJob*)), SLOT(queryPACSJobFinished(PACSJob*)));
    connect(queryPACSJob, SIGNAL(PACSJobCancelled(PACSJob*)), SLOT(queryPACSJobCancelled(PACSJob*)));

    m_pacsManager->enqueuePACSJob(queryPACSJob);
    m_queryPACSJobPendingExecuteOrExecuting.insert(queryPACSJob->getPACSJobID(), queryPACSJob);
}

void RelatedStudiesManager::cancelCurrentQuery()
{
    foreach (QueryPacsJob *queryPACSJob, m_queryPACSJobPendingExecuteOrExecuting)
    {
        m_pacsManager->requestCancelPACSJob(queryPACSJob);
        m_queryPACSJobPendingExecuteOrExecuting.remove(queryPACSJob->getPACSJobID());
    }

    m_studyInstanceUIDOfStudyToFindRelated = "invalid";
}

bool RelatedStudiesManager::isExecutingQueries()
{
    return !m_queryPACSJobPendingExecuteOrExecuting.isEmpty();
}

void RelatedStudiesManager::queryPACSJobCancelled(PACSJob *pacsJob)
{
    // Aquest slot també serveix per si alguna altre classe ens cancel·la un PACSJob nostre per a que ens n'assabentem
    QueryPacsJob *queryPACSJob = qobject_cast<QueryPacsJob*>(pacsJob);

    if (queryPACSJob == NULL)
    {
        ERROR_LOG("El PACSJob que s'ha cancel·lat no es un QueryPACSJob");
    }
    else
    {
        m_queryPACSJobPendingExecuteOrExecuting.remove(queryPACSJob->getPACSJobID());

        // Fem un deleteLater per si algú més ha capturat el signal de PACSJobFinished per aquest aquest job no es trobi l'objecte destruït
        queryPACSJob->deleteLater();

        if (m_queryPACSJobPendingExecuteOrExecuting.isEmpty())
        {
            queryFinished();
        }
    }
}

void RelatedStudiesManager::queryPACSJobFinished(PACSJob *pacsJob)
{
    QueryPacsJob *queryPACSJob = qobject_cast<QueryPacsJob*>(pacsJob);

    if (queryPACSJob == NULL)
    {
        ERROR_LOG("El PACSJob que ha finalitzat no es un QueryPACSJob");
    }
    else
    {
        if (queryPACSJob->getStatus() == PACSRequestStatus::QueryOk)
        {
            mergeFoundStudiesInQuery(queryPACSJob);
        }
        else if (queryPACSJob->getStatus() != PACSRequestStatus::QueryCancelled)
        {
            errorQueringPACS(queryPACSJob);
        }

        m_queryPACSJobPendingExecuteOrExecuting.remove(queryPACSJob->getPACSJobID());

        // Fem un deleteLater per si algú més ha capturat el signal de PACSJobFinished per aquest aquest job no es trobi l'objecte destruït
        queryPACSJob->deleteLater();

        if (m_queryPACSJobPendingExecuteOrExecuting.isEmpty())
        {
            queryFinished();
        }
    }
}

void RelatedStudiesManager::mergeFoundStudiesInQuery(QueryPacsJob *queryPACSJob)
{
    if (queryPACSJob->getQueryLevel() != QueryPacsJob::study)
    {
        /// Si la consulta no era d'estudis no ens interessa, només cerquem estudis
        return;
    }

    foreach (Patient *patient, queryPACSJob->getPatientStudyList())
    {
        foreach (Study *study, patient->getStudies())
        {
            if (!isStudyInMergedStudyList(study) && !isMainStudy(study))
            {
                // Si l'estudi no està a llista ja d'estudis afegits i no és el mateix estudi pel qua ens han demanat el
                // previ l'afegim
                m_mergedStudyList.append(study);
            }
        }
    }
}

void RelatedStudiesManager::errorQueringPACS(QueryPacsJob *queryPACSJob)
{
    if (queryPACSJob->getStatus() != PACSRequestStatus::QueryOk && queryPACSJob->getStatus() != PACSRequestStatus::QueryCancelled)
    {
        // Com que fem dos cerques al mateix pacs si una falla, l'altra segurament també fallarà per evitar enviar
        // dos signals d'error si les dos fallen, ja que per des de fora ha de ser transparent el número de consultes
        // que es fa al PACS, i han de rebre un sol error comprovem si tenim l'ID del PACS a la llista de signals
        // d'errors en PACS emesos
        if (!m_pacsDeviceIDErrorEmited.contains(queryPACSJob->getPacsDevice().getID()))
        {
            m_pacsDeviceIDErrorEmited.append(queryPACSJob->getPacsDevice().getID());
            emit errorQueryingStudies(queryPACSJob->getPacsDevice());
        }
    }
}

void RelatedStudiesManager::queryFinished()
{
    // Quan totes les query han acabat és quant fem l'emit amb els estudis previs trobats. No podem emetre els resultats que anem rebent,
    // perquè hem de fer un merge del resultats rebuts, per no tenir duplicats (Estudis del matiex pacient que estiguin a més d'un PACS)
    emit queryStudiesFinished(m_mergedStudyList);
}

bool RelatedStudiesManager::isStudyInMergedStudyList(Study *study)
{
    bool studyFoundInMergedList = false;

    foreach (Study *studyMerged, m_mergedStudyList)
    {
        if (study->getInstanceUID() == studyMerged->getInstanceUID())
        {
            studyFoundInMergedList = true;
            break;
        }
    }

    return studyFoundInMergedList;
}

bool RelatedStudiesManager::isMainStudy(Study *study)
{
    return study->getInstanceUID() == m_studyInstanceUIDOfStudyToFindRelated;
}

DicomMask RelatedStudiesManager::getBasicDicomMask()
{
    DicomMask dicomMask;

    /// Definim els camps que la consulta ha de retornar
    dicomMask.setPatientName("");
    dicomMask.setPatientID("");
    dicomMask.setStudyID("");
    dicomMask.setStudyDescription("");
    dicomMask.setStudyModality("");
    dicomMask.setStudyDate(QDate(), QDate());
    dicomMask.setStudyTime(QTime(), QTime());
    dicomMask.setStudyInstanceUID("");

    return dicomMask;
}

void RelatedStudiesManager::retrieve(Study *study, const PacsDevice &pacsDevice)
{
    retrieveAndApplyAction(study, pacsDevice, None);
}

void RelatedStudiesManager::retrieveAndLoad(Study *study, const PacsDevice &pacsDevice)
{
    retrieveAndApplyAction(study, pacsDevice, Load);
}

void RelatedStudiesManager::retrieveAndView(Study *study, const PacsDevice &pacsDevice)
{
    retrieveAndApplyAction(study, pacsDevice, View);
}

void RelatedStudiesManager::retrieveAndApplyAction(Study *study, const PacsDevice &pacsDevice, ActionsAfterRetrieve action)
{
    QInputOutputPacsWidget::ActionsAfterRetrieve queryScreenAction = QInputOutputPacsWidget::None;
    switch (action)
    {
        case None:
            queryScreenAction = QInputOutputPacsWidget::None;
            break;

        case View:
            queryScreenAction = QInputOutputPacsWidget::View;
            break;

        case Load:
            queryScreenAction = QInputOutputPacsWidget::Load;
            break;
    }
    
    QueryScreen *queryScreen = SingletonPointer<QueryScreen>::instance();
    queryScreen->retrieveStudy(queryScreenAction, pacsDevice, study);
    connect(queryScreen, SIGNAL(studyRetrieveFailed(QString)), SIGNAL(errorDownloadingStudy(QString)));
}

void RelatedStudiesManager::deleteQueryResults()
{
    QList<Patient*> patientsStudy;

    foreach (Study* study, m_mergedStudyList)
    {
        patientsStudy.append(study->getParentPatient());
    }

    qDeleteAll(m_mergedStudyList);
    qDeleteAll(patientsStudy);

    m_mergedStudyList.clear();
}

QList<PacsDevice> RelatedStudiesManager::getPACSRetrievedStudiesOfPatient(Patient *patient)
{
    QList<PacsDevice> pacsDeviceRetrievedStudies;

    foreach (Study *studyPatient, patient->getStudies())
    {
        pacsDeviceRetrievedStudies = PacsDeviceManager::removeDuplicateSamePACS(pacsDeviceRetrievedStudies + studyPatient->getDICOMSource().getRetrievePACS());
    }

    return pacsDeviceRetrievedStudies;
}

}
