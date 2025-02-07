#include "hangingprotocolmanager.h"

#include "viewerslayout.h"
#include "patient.h"
#include "series.h"
#include "image.h"
#include "volume.h"
#include "q2dviewerwidget.h"
#include "hangingprotocolsrepository.h"
#include "hangingprotocol.h"
#include "hangingprotocollayout.h"
#include "hangingprotocolmask.h"
#include "hangingprotocolimageset.h"
#include "hangingprotocoldisplayset.h"
#include "identifier.h"
#include "logging.h"
#include "volumerepository.h"
#include "applyhangingprotocolqviewercommand.h"
// Necessari per poder anar a buscar prèvies
#include "../inputoutput/relatedstudiesmanager.h"

namespace udg {

HangingProtocolManager::HangingProtocolManager(QObject *parent)
 : QObject(parent)
{
    m_studiesDownloading = new QMultiHash<QString, StructPreviousStudyDownloading*>();
    m_relatedStudiesManager = new RelatedStudiesManager();

    copyHangingProtocolRepository();

    connect(m_relatedStudiesManager, SIGNAL(errorDownloadingStudy(QString)), SLOT(errorDowlonadingPreviousStudies(QString)));
}

HangingProtocolManager::~HangingProtocolManager()
{
    cancelHangingProtocolDownloading();
    delete m_studiesDownloading;
    delete m_relatedStudiesManager;

    foreach (HangingProtocol *hangingProtocol, m_availableHangingProtocols)
    {
        delete hangingProtocol;
    }
    m_availableHangingProtocols.clear();
}

void HangingProtocolManager::copyHangingProtocolRepository()
{
    foreach (HangingProtocol *hangingProtocol, HangingProtocolsRepository::getRepository()->getItems())
    {
        m_availableHangingProtocols << new HangingProtocol(hangingProtocol);
    }
}

QList<HangingProtocol*> HangingProtocolManager::searchHangingProtocols(Patient *patient)
{
    QList<Study*> previousStudies;

    return searchHangingProtocols(patient, previousStudies);
}

QList<HangingProtocol*> HangingProtocolManager::searchHangingProtocols(Patient *patient, const QList<Study*> &previousStudies)
{
    QList<HangingProtocol*> outputHangingProtocolList;

    QList<Series*> allSeries;

    foreach (Study *study, patient->getStudies())
    {
        allSeries += study->getViewableSeries();
    }

    // Buscar el hangingProtocol que s'ajusta millor a l'estudi del pacient
    // Aprofitem per assignar ja les series, per millorar el rendiment
    foreach (HangingProtocol *hangingProtocol, m_availableHangingProtocols)
    {
        if (isModalityCompatible(hangingProtocol, patient) && isInstitutionCompatible(hangingProtocol, patient))
        {
            int numberOfFilledImageSets = setInputToHangingProtocolImageSets(hangingProtocol, allSeries, previousStudies);

            bool isValidHangingProtocol = false;

            if (hangingProtocol->isStrict())
            {
                if (numberOfFilledImageSets == hangingProtocol->getNumberOfImageSets())
                {
                    isValidHangingProtocol = true;
                }
            }
            else
            {
                if (numberOfFilledImageSets > 0)
                {
                    isValidHangingProtocol = true;
                }
            }

            if (isValidHangingProtocol)
            {
                outputHangingProtocolList << hangingProtocol;
            }
        }
    }

    if (outputHangingProtocolList.size() > 0)
    {
        // Noms per mostrar al log
        QString infoLog;
        foreach (HangingProtocol *hangingProtocol, outputHangingProtocolList)
        {
            infoLog += QString("%1, ").arg(hangingProtocol->getName());
        }
        INFO_LOG(QString("Hanging protocols carregats: %1").arg(infoLog));
    }
    else
    {
        INFO_LOG(QString("No s'ha trobat cap hanging protocol"));
    }

    return outputHangingProtocolList;
}

int HangingProtocolManager::setInputToHangingProtocolImageSets(HangingProtocol *hangingProtocol, const QList<Series*> &inputSeries,
                                                               const QList<Study*> &previousStudies)
{
    int numberOfFilledImageSets = 0;
    // Còpia de les sèries perquè es van eliminant de la llista al ser assignades
    QList<Series*> candidateSeries = inputSeries;

    foreach (HangingProtocolImageSet *imageSet, hangingProtocol->getImageSets())
    {
        if (searchSerie(candidateSeries, imageSet, hangingProtocol->getAllDiferent()))
        {
            ++numberOfFilledImageSets;

            if (imageSet->isPreviousStudy())
            {
                imageSet->setDownloaded(true);
            }
        }
        else
        {
            // Si és de tipus prèvi, se li dóna una segona oportunitat buscant a previs
            if (imageSet->isPreviousStudy())
            {
                Study *referenceStudy = 0;
                Study *previousStudy = 0;
                HangingProtocolImageSet *referenceImageSet = hangingProtocol->getImageSet(imageSet->getPreviousImageSetReference());

                if (referenceImageSet->isDownloaded() && referenceImageSet->getSeriesToDisplay())
                {
                    // L'estudi de referència està descarregat
                    referenceStudy = referenceImageSet->getSeriesToDisplay()->getParentStudy();
                }
                else
                {
                    // L'estudi de referència és un previ que encara no s'ha descarregat
                    referenceStudy = referenceImageSet->getPreviousStudyToDisplay();
                }

                if (referenceStudy)
                {
                    previousStudy = searchPreviousStudy(hangingProtocol, referenceStudy, previousStudies);

                    if (previousStudy)
                    {
                        // S'ha trobat pendent de descarrega
                        ++numberOfFilledImageSets;
                        imageSet->setDownloaded(false);
                        imageSet->setPreviousStudyToDisplay(previousStudy);
                    }
                }
            }
        }
    }

    return numberOfFilledImageSets;
}

void HangingProtocolManager::setBestHangingProtocol(Patient *patient, const QList<HangingProtocol*> &hangingProtocolList, ViewersLayout *layout)
{
    HangingProtocol *bestHangingProtocol = NULL;
    foreach (HangingProtocol *hangingProtocol, hangingProtocolList)
    {
        if (hangingProtocol->isBetterThan(bestHangingProtocol))
        {
            bestHangingProtocol = hangingProtocol;
        }
    }

    if (bestHangingProtocol)
    {
        DEBUG_LOG(QString("Hanging protocol que s'aplica: %1").arg(bestHangingProtocol->getName()));
        applyHangingProtocol(bestHangingProtocol, layout, patient);
    }
}

void HangingProtocolManager::applyHangingProtocol(int hangingProtocolNumber, ViewersLayout *layout, Patient *patient)
{
    HangingProtocol *hangingProtocol = 0;
    bool found = false;
    QListIterator<HangingProtocol*> iterator(m_availableHangingProtocols);

    while (!found && iterator.hasNext())
    {
        HangingProtocol *candidate = iterator.next();
        if (candidate->getIdentifier() == hangingProtocolNumber)
        {
            found = true;
            hangingProtocol = candidate;
        }
    }

    if (found)
    {
        applyHangingProtocol(hangingProtocol, layout, patient);
    }
}

void HangingProtocolManager::applyHangingProtocol(HangingProtocol *hangingProtocol, ViewersLayout *layout, Patient *patient)
{
    // Si hi havia algun estudi descarregant, es treu de la llista d'espera
    cancelHangingProtocolDownloading();

    // Abans d'aplicar un nou hanging protocol, fem neteja del layout i eliminem tot el que hi havia anteriorment
    layout->cleanUp();
    foreach (HangingProtocolDisplaySet *displaySet, hangingProtocol->getDisplaySets())
    {
        HangingProtocolImageSet *hangingProtocolImageSet = displaySet->getImageSet();
        Q2DViewerWidget *viewerWidget = layout->addViewer(displaySet->getPosition());

        if (layout->getNumberOfViewers() == 1)
        {
            layout->setSelectedViewer(viewerWidget);
        }
        
        if (hangingProtocolImageSet->isDownloaded() == false)
        {
            viewerWidget->getViewer()->setViewerStatus(QViewer::DownloadingVolume);

            StructPreviousStudyDownloading *structPreviousStudyDownloading = new StructPreviousStudyDownloading;
            structPreviousStudyDownloading->widgetToDisplay = viewerWidget;
            structPreviousStudyDownloading->displaySet = displaySet;

            bool isDownloading = m_studiesDownloading->contains(hangingProtocolImageSet->getPreviousStudyToDisplay()->getInstanceUID());

            m_studiesDownloading->insert(hangingProtocolImageSet->getPreviousStudyToDisplay()->getInstanceUID(), structPreviousStudyDownloading);

            if (!isDownloading && hangingProtocolImageSet->getPreviousStudyToDisplay()->getDICOMSource().getRetrievePACS().count() > 0)
            {
                //En principi sempre hauríem de tenir algun PACS al DICOMSource
                connect(patient, SIGNAL(studyAdded(Study*)), SLOT(previousStudyDownloaded(Study*)));
                m_relatedStudiesManager->retrieveAndLoad(hangingProtocolImageSet->getPreviousStudyToDisplay(),
                    hangingProtocolImageSet->getPreviousStudyToDisplay()->getDICOMSource().getRetrievePACS().at(0));
            }
        }
        else
        {
            setInputToViewer(viewerWidget, hangingProtocolImageSet->getSeriesToDisplay(), displaySet);
        }
    }

    INFO_LOG(QString("Hanging protocol aplicat: %1").arg(hangingProtocol->getName()));
}

bool HangingProtocolManager::isModalityCompatible(HangingProtocol *protocol, Patient *patient)
{
    foreach (const QString &modality, patient->getModalities())
    {
        if (isModalityCompatible(protocol, modality))
        {
            return true;
        }
    }

    return false;
}

bool HangingProtocolManager::isModalityCompatible(HangingProtocol *protocol, const QString &modality)
{
    return protocol->getHangingProtocolMask()->getProtocolList().contains(modality);
}

bool HangingProtocolManager::isInstitutionCompatible(HangingProtocol *protocol, Patient *patient)
{
    foreach(Study *study, patient->getStudies())
    {
        foreach(Series *series, study->getSeries())
        {
            if (isValidInstitution(protocol, series->getInstitutionName()))
            {
                return true;
            }
        }
    }

    return false;
}

Series* HangingProtocolManager::searchSerie(QList<Series*> &listOfSeries, HangingProtocolImageSet *imageSet, bool quitStudy)
{
    // Si la llista és buida, el resultat de la cerca serà nul
    if (listOfSeries.isEmpty())
    {
        return 0;
    }

    Series *selectedSeries = 0;
    Study *referenceStudy = 0;

    if (imageSet->isPreviousStudy())
    {
        // S'ha de tenir en compte que a la imatge a què es refereix pot estar pendent de descarrega (prèvia)
        HangingProtocolImageSet *referenceImageSet = imageSet->getHangingProtocol()->getImageSet(imageSet->getPreviousImageSetReference());

        if (referenceImageSet->isDownloaded())
        {
            // L'estudi de referència està descarregat
            if (referenceImageSet->getSeriesToDisplay() != 0)
            {
                // No te sèrie anterior, per tant no és valid
                referenceStudy = referenceImageSet->getSeriesToDisplay()->getParentStudy();
            }
        }
        else
        {
            // L'estudi de referència és un previ que encara no s'ha descarregat
            referenceStudy = referenceImageSet->getPreviousStudyToDisplay();
        }

        if (!referenceStudy)
        {
            return 0;
        }
    }

    // Pot ser que busquem una imatge en concret, llavors no cal examinar totes les sèries i/o totes les imatges
    // Només pot ser vàlida una imatge
    if (imageSet->getImageNumberInPatientModality() != -1)
    {
        Patient *patient = listOfSeries.at(0)->getParentStudy()->getParentPatient();
        QStringList modalities = imageSet->getHangingProtocol()->getHangingProtocolMask()->getProtocolList();
        Image *image = getImageByIndexInPatientModality(patient, imageSet->getImageNumberInPatientModality(), modalities);

        if (isValidImage(image, imageSet))
        {
            selectedSeries = image->getParentSeries();
            imageSet->setImageToDisplay(selectedSeries->getImages().indexOf(image));
            imageSet->setSeriesToDisplay(selectedSeries);
            return selectedSeries;
        }
        else
        {
            // Segur que no hi ha cap més imatge vàlida
            // Important, no hi posem cap serie!
            imageSet->setSeriesToDisplay(0);
            imageSet->setImageToDisplay(0);
            return 0;
        }
    }

    int currentSeriesIndex = 0;
    int numberOfSeries = listOfSeries.size();

    while (!selectedSeries && currentSeriesIndex < numberOfSeries)
    {
        Series *serie = listOfSeries.value(currentSeriesIndex);
        bool isCandidateSeries = true;
        if (imageSet->isPreviousStudy())
        {
            if (serie->getParentStudy()->getDate() >= referenceStudy->getDate())
            {
                isCandidateSeries = false;
            }
        }

        if (isCandidateSeries && isModalityCompatible(imageSet->getHangingProtocol(), serie->getModality()))
        {
            if (imageSet->getTypeOfItem() != "image")
            {
                if (isValidSerie(serie, imageSet))
                {
                    selectedSeries = serie;
                    imageSet->setSeriesToDisplay(serie);
                }
            }
            else
            {
                int currentImageIndex = 0;
                QList<Image*> listOfImages = serie->getImages();
                int numberOfImages = listOfImages.size();
                while (!selectedSeries && currentImageIndex < numberOfImages)
                {
                    Image *image = listOfImages.value(currentImageIndex);
                    if (isValidImage(image, imageSet))
                    {
                        selectedSeries = serie;
                        imageSet->setImageToDisplay(currentImageIndex);
                        imageSet->setSeriesToDisplay(serie);
                    }
                    ++currentImageIndex;
                }
            }
        }

        if (selectedSeries && quitStudy)
        {
            listOfSeries.removeAt(currentSeriesIndex);
        }

        ++currentSeriesIndex;
    }

    if (!selectedSeries)
    {
        // Important, no hi posem cap serie!
        imageSet->setSeriesToDisplay(0);
        imageSet->setImageToDisplay(0);
    }

    return selectedSeries;
}

bool HangingProtocolManager::isValidSerie(Series *serie, HangingProtocolImageSet *imageSet)
{
    bool valid = true;
    int i = 0;
    QList<HangingProtocolImageSet::Restriction> listOfRestrictions = imageSet->getRestrictions();
    int numberRestrictions = listOfRestrictions.size();
    HangingProtocolImageSet::Restriction restriction;

    // Els presentation states per defecte no es mostren
    valid = (serie->getModality() != "PR");

    while (valid && i < numberRestrictions)
    {
        restriction = listOfRestrictions.value(i);

        if (restriction.selectorAttribute == "BodyPartExamined")
        {
            if (serie->getBodyPartExamined() != restriction.valueRepresentation)
            {
                valid = false;
            }
        }
        else if (restriction.selectorAttribute == "ProtocolName")
        {
            if (! serie->getProtocolName().contains(restriction.valueRepresentation))
            {
                valid = false;
            }
        }
        else if (restriction.selectorAttribute == "ViewPosition")
        {
            if (serie->getViewPosition() != restriction.valueRepresentation)
            {
                valid = false;
            }
        }
        else if (restriction.selectorAttribute == "SeriesDescription")
        {
            bool contains = serie->getDescription().contains(restriction.valueRepresentation, Qt::CaseInsensitive);
            bool match = (restriction.usageFlag == HangingProtocolImageSet::NoMatch);
            valid = contains ^ match;
        }
        else if (restriction.selectorAttribute == "StudyDescription")
        {
            bool contains = serie->getParentStudy()->getDescription().contains(restriction.valueRepresentation, Qt::CaseInsensitive);
            bool match = (restriction.usageFlag == HangingProtocolImageSet::NoMatch);
            valid = contains ^ match;
        }
        else if (restriction.selectorAttribute == "PatientName")
        {
            if (serie->getParentStudy()->getParentPatient()->getFullName() != restriction.valueRepresentation)
            {
                valid = false;
            }
        }
        else if (restriction.selectorAttribute == "SeriesNumber")
        {
            if (serie->getSeriesNumber() != restriction.valueRepresentation)
            {
                valid = false;
            }
        }
        else if (restriction.selectorAttribute == "MinimumNumberOfImages")
        {
            if (serie->getFirstVolume()->getImages().size() < restriction.valueRepresentation.toInt())
            {
                valid = false;
            }
        }
        ++i;
    }

    return valid;
}

bool HangingProtocolManager::isValidImage(Image *image, HangingProtocolImageSet *imageSet)
{
    if (!image)
    {
        DEBUG_LOG("La imatge passada és NUL·LA! Retornem fals.");
        return false;
    }

    bool valid = true;
    int i = 0;
    QList<HangingProtocolImageSet::Restriction> listOfRestrictions = imageSet->getRestrictions();
    int numberRestrictions = listOfRestrictions.size();
    HangingProtocolImageSet::Restriction restriction;

    while (valid && i < numberRestrictions)
    {
        restriction = listOfRestrictions.value(i);
        if (restriction.selectorAttribute == "ViewPosition")
        {
            bool contains = image->getViewPosition().contains(restriction.valueRepresentation, Qt::CaseInsensitive);
            bool match = (restriction.usageFlag == HangingProtocolImageSet::NoMatch);
            valid = contains ^ match;
        }
        else if (restriction.selectorAttribute == "ImageLaterality")
        {
            if (QString(image->getImageLaterality()) != restriction.valueRepresentation.at(0))
            {
                valid = false;
            }
        }
        else if (restriction.selectorAttribute == "Laterality")
        {
            // Atenció! Aquest atribut està definit a nivell de sèries
            if (QString(image->getParentSeries()->getLaterality()) != restriction.valueRepresentation)
            {
                valid = false;
            }
        }
        else if (restriction.selectorAttribute == "PatientOrientation")
        {
            if (!image->getPatientOrientation().getDICOMFormattedPatientOrientation().contains(restriction.valueRepresentation))
            {
                valid = false;
            }
        }
         // TODO Es podria canviar el nom, ja que és massa genèric. Seria més adequat ViewCodeMeaning per exemple
        else if (restriction.selectorAttribute == "CodeMeaning")
        {
            bool match = (restriction.usageFlag == HangingProtocolImageSet::Match);

            if (!(image->getViewCodeMeaning().contains(restriction.valueRepresentation)))
            {
                valid = false;
            }

            if (!match)
            {
                // Just el cas contrari
                valid = !valid;
            }
        }
        else if (restriction.selectorAttribute == "ImageType")
        {
            bool isLocalyzer = image->getImageType().contains(restriction.valueRepresentation, Qt::CaseInsensitive);
            bool match = (restriction.usageFlag == HangingProtocolImageSet::NoMatch);
            valid = isLocalyzer ^ match;
        }
        else if (restriction.selectorAttribute == "MinimumNumberOfImages")
        {
            Series *serie = image->getParentSeries();
            if (serie->getFirstVolume()->getImages().size() < restriction.valueRepresentation.toInt())
            {
                valid = false;
            }
        }
        ++i;
    }

    return valid;
}

bool HangingProtocolManager::isValidInstitution(HangingProtocol *protocol, const QString &institutionName)
{
    if (protocol->getInstitutionsRegularExpression().isEmpty())
    {
        return true;
    }

    return institutionName.contains(protocol->getInstitutionsRegularExpression());
}

Study* HangingProtocolManager::searchPreviousStudy(HangingProtocol *protocol, Study *referenceStudy, const QList<Study*> &previousStudies)
{
    QList<Study*> sortedPreviousStudies = Study::sortStudies(previousStudies, Study::RecentStudiesFirst);

    foreach (Study *study, sortedPreviousStudies)
    {
        // Atenció, tal com està fet ara, es considera previ si és d'almenys un dia abans, si és del mateix dia, no es considera previ
        if (study->getDate() < referenceStudy->getDate())
        {
            foreach (const QString &modality, study->getModalities())
            {
                if (isModalityCompatible(protocol, modality))
                {
                    return study;
                }
            }
        }
    }

    return 0;
}

void HangingProtocolManager::previousStudyDownloaded(Study *study)
{
    if (m_studiesDownloading->isEmpty())
    {
        return;
    }

    // Es busca quins estudis nous hi ha
    int count = m_studiesDownloading->count(study->getInstanceUID());
    for (int i = 0; i < count; ++i)
    {
        // Per cada estudi que esperàvem que es descarregués
        // Agafem l'estructura amb les dades que s'havien guardat per poder aplicar-ho
        StructPreviousStudyDownloading *structPreviousStudyDownloading = m_studiesDownloading->take(study->getInstanceUID());

        // Busquem la millor serie de l'estudi que ho satisfa
        QList<Series*> studySeries = study->getSeries();
        Series *series = searchSerie(studySeries, structPreviousStudyDownloading->displaySet->getImageSet(), false);

        Q2DViewerWidget *viewerWidget = structPreviousStudyDownloading->widgetToDisplay;
        structPreviousStudyDownloading->displaySet->getImageSet()->setDownloaded(true);

        viewerWidget->getViewer()->setViewerStatus(QViewer::NoVolumeInput);

        setInputToViewer(viewerWidget, series, structPreviousStudyDownloading->displaySet);

        delete structPreviousStudyDownloading;
    }
}

void HangingProtocolManager::errorDowlonadingPreviousStudies(const QString &studyUID)
{
    if (m_studiesDownloading->contains(studyUID))
    {
        // Si és un element que estavem esperant
        int count = m_studiesDownloading->count(studyUID);
        for (int i = 0; i < count; ++i)
        {
            // S'agafa i es treu de la llista
            StructPreviousStudyDownloading *element = m_studiesDownloading->take(studyUID);
            element->widgetToDisplay->getViewer()->setViewerStatus(QViewer::DownloadingError);
            delete element;
        }
    }
}

void HangingProtocolManager::cancelHangingProtocolDownloading()
{
    foreach (const QString &key, m_studiesDownloading->keys())
    {
        emit discardedStudy(key);
        // S'agafa i es treu de la llista l'element que s'està esperant
        // i es treu el label de downloading
        StructPreviousStudyDownloading *element = m_studiesDownloading->take(key);
        element->widgetToDisplay->getViewer()->setViewerStatus(QViewer::NoVolumeInput);
        delete element;
    }
}

bool HangingProtocolManager::isPreviousStudyForHangingProtocol(Study *study)
{
    if (!study)
    {
        return false;
    }
    
    return m_studiesDownloading->contains(study->getInstanceUID());
}

void HangingProtocolManager::setInputToViewer(Q2DViewerWidget *viewerWidget, Series *series, HangingProtocolDisplaySet *displaySet)
{
    if (series)
    {
        if (series->isViewable() && series->getFirstVolume())
        {
            Volume *inputVolume = NULL;
            if ((displaySet->getSlice() > -1 && series->getVolumesList().size() > 1) || displaySet->getImageSet()->getTypeOfItem() == "image")
            {
                Image *image;
                // TODO En el cas de fases no funcionaria, perquè l'índex no és correcte
                if (displaySet->getSlice() > -1)
                {
                    image = series->getImageByIndex(displaySet->getSlice());
                }
                else if (displaySet->getImageSet()->getTypeOfItem() == "image")
                {
                    image = series->getImageByIndex(displaySet->getImageSet()->getImageToDisplay());
                }

                Volume *volumeContainsImage = series->getVolumeOfImage(image);

                if (!volumeContainsImage)
                {
                    // No existeix cap imatge al tall corresponent, agafem el volum per defecte
                    inputVolume = series->getFirstVolume();
                }
                else
                {
                    // Tenim nou volum, i per tant, cal calcular el nou número de llesca
                    int slice = volumeContainsImage->getImages().indexOf(image);
                    displaySet->setSliceModifiedForVolumes(slice);

                    inputVolume = volumeContainsImage;
                }
            }
            else
            {
                inputVolume = series->getFirstVolume();
            }

            ApplyHangingProtocolQViewerCommand *command = new ApplyHangingProtocolQViewerCommand(viewerWidget, displaySet);
            viewerWidget->setInputAsynchronously(inputVolume, command);
        }
    }
}

Image* HangingProtocolManager::getImageByIndexInPatientModality(Patient *patient, int index, QStringList hangingProtocolModalities)
{
    QList<Image*> allImagesInStudy;

    // TODO Es podria millorar amb una cerca fins a la imatge que està a l'índex, envers d'un recorregut agafant-les totes
    
    foreach (Study *study, patient->getStudies())
    {
        foreach (Series *series, study->getSeries())
        {
            if (hangingProtocolModalities.contains(series->getModality()))
            {
                allImagesInStudy.append(series->getImages());
            }
        }
    }

    if (index < allImagesInStudy.size())
    {
        return allImagesInStudy.at(index);
    }
    else
    {
        return 0;
    }
}

}
