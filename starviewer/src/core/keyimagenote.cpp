#include "keyimagenote.h"
#include "series.h"
#include "study.h"
#include "image.h"
#include "dicomreferencedimage.h"
#include "logging.h"

namespace udg {

KeyImageNote::KeyImageNote()
{
}

KeyImageNote::~KeyImageNote()
{
}

KeyImageNote::DocumentTitle KeyImageNote::getDocumentTitle() const
{
    return m_documentTitle;
}

void KeyImageNote::setDocumentTitle(KeyImageNote::DocumentTitle documentTitle)
{
    m_documentTitle = documentTitle;
}

KeyImageNote::RejectedForQualityReasons KeyImageNote::getRejectedForQualityReasons() const
{
    return m_rejectedForQualityReasons;
}

void KeyImageNote::setRejectedForQualityReasons(KeyImageNote::RejectedForQualityReasons rejectedForQualityReasons)
{
    m_rejectedForQualityReasons = rejectedForQualityReasons;
}

QString KeyImageNote::getKeyObjectDescription() const
{
    return m_keyObjectDescription;
}

void KeyImageNote::setKeyObjectDescription(const QString &description)
{
    m_keyObjectDescription = description;
}

KeyImageNote::ObserverType KeyImageNote::getObserverContextType() const
{
    return m_observerContextType;
}

void KeyImageNote::setObserverContextType(KeyImageNote::ObserverType contextType)
{
    m_observerContextType = contextType;
}

QList<DICOMReferencedImage*> KeyImageNote::getDICOMReferencedImages() const
{
    return m_DICOMReferencedImages;
}

void KeyImageNote::setDICOMReferencedImages(QList<DICOMReferencedImage*> referencedImageList)
{
    m_DICOMReferencedImages = referencedImageList;
    foreach (DICOMReferencedImage *referencedImage, m_DICOMReferencedImages) 
    {
        referencedImage->setReferenceParentSOPInstanceUID(m_SOPInstanceUID);
    }
}

QString KeyImageNote::getObserverContextName() const
{
    return m_observerContextName;
}

void KeyImageNote::setObserverContextName(const QString &contextName)
{
    m_observerContextName = contextName;
}

void KeyImageNote::setParentSeries(Series *series)
{
    m_parentSeries = series;
    this->setParent(m_parentSeries);
}

Series* KeyImageNote::getParentSeries() const
{
    return m_parentSeries;
}

QString KeyImageNote::getInstanceUID() const
{
    return m_SOPInstanceUID;
}

void KeyImageNote::setInstanceUID(const QString &uid)
{
    m_SOPInstanceUID = uid;
}

bool KeyImageNote::setContentDate(QString date)
{
    return this->setContentDate(QDate::fromString(date.remove("."), "yyyyMMdd"));
}

bool KeyImageNote::setContentDate(QDate date)
{
    bool ok = true;
    if (date.isValid())
    {
        m_contentDate = date;
        ok = true;
    }
    else if (!date.isNull())
    {
        DEBUG_LOG("La data est� en un mal format: " + date.toString( Qt::LocaleDate ) );
        ok = false;
    }
    return ok;
}

bool KeyImageNote::setContentTime(QString time)
{
    time = time.remove(":");

    QStringList split = time.split(".");
    QTime convertedTime = QTime::fromString(split[0], "hhmmss");

    if (split.size() == 2)
    {
        convertedTime = convertedTime.addMSecs( split[1].leftJustified(3,'0',true).toInt() );
    }

    return this->setContentTime(convertedTime);
}

bool KeyImageNote::setContentTime(QTime time)
{
    bool ok = true;
    if (time.isValid())
    {
        m_contentTime = time;
        ok = true;
    }
    else if (!time.isNull())
    {
        DEBUG_LOG( "El time est� en un mal format" );
        ok = false;
    }

    return ok;
}

QDate KeyImageNote::getContentDate() const
{
    return m_contentDate;
}

QTime KeyImageNote::getContentTime() const
{
    return m_contentTime;
}

QList<Image*> KeyImageNote::getReferencedImages()
{
    QList<Image*> referencedImages;

    foreach (DICOMReferencedImage *referencedImage, m_DICOMReferencedImages) 
    {
        Image *image = getImage(referencedImage);
        referencedImages.append(image);
    }

    return referencedImages;
}

Image* KeyImageNote::getImage(DICOMReferencedImage *referencedImage)
{
    Image *image = NULL;
    Study *study = m_parentSeries->getParentStudy();
    int i = 0;
    
    while (!image && i < study->getSeries().size())
    {
        image = getDICOMReferencedImagesFromSeries(study->getSeries().value(i), referencedImage);
        i++;
    }

    if (!image) // Si no la trobem generem una imatge emplenada amb tota la informaci� de la que disposem
    {
        image = new Image();
        image->setSOPInstanceUID(referencedImage->getDICOMReferencedImageSOPInstanceUID());
        image->setFrameNumber(referencedImage->getFrameNumber());
    }

    return image;
}

Image* KeyImageNote::getDICOMReferencedImagesFromSeries(Series *serie, DICOMReferencedImage *referencedImage)
{
    int j = 0;
    Image *image = NULL;

    while (!image && j < serie->getImages().size())
    {
        Image *currentImage = serie->getImages().value(j);
        if (currentImage->getSOPInstanceUID() == referencedImage->getDICOMReferencedImageSOPInstanceUID() && currentImage->getFrameNumber() == referencedImage->getFrameNumber())
        {
            image = currentImage;
        }
        else
        {
            j++;
        }
    }

    return image;
}
QString KeyImageNote::getInstanceNumber() const
{
    return m_instanceNumber;
}

void KeyImageNote::setInstanceNumber(const QString &instanceNumber)
{
    m_instanceNumber = instanceNumber;
}

void KeyImageNote::setRetrievedDate(QDate retrievedDate)
{
    m_retrievedDate = retrievedDate;
}

void KeyImageNote::setRetrievedTime(QTime retrievedTime)
{
    m_retrieveTime = retrievedTime;
}

QDate KeyImageNote::getRetrievedDate() const
{
    return m_retrievedDate;
}

QTime KeyImageNote::getRetrievedTime() const
{
    return m_retrieveTime;
}

QString KeyImageNote::getObserverTypeAsString(KeyImageNote::ObserverType observerType)
{
    switch (observerType)
    {
        case KeyImageNote::Person:
            return tr("Person");
        case KeyImageNote::Device:
            return tr("Device");
        case KeyImageNote::NoneObserverType:
            return tr("None ObserverType");
        default:
            return tr("Bad ObserverType");
  }
}

QString KeyImageNote::getDocumentTitleAsString(KeyImageNote::DocumentTitle documentTitle)
{
    switch (documentTitle)
    {
        case KeyImageNote::OfInterest:
            return tr("Of Interest");
        case KeyImageNote::RejectedForQualityReasonsDocumentTitle:
            return tr("Rejected for Quality Reasons");
        case KeyImageNote::ForReferringProvider:
            return tr("For Referring Provider");
        case KeyImageNote::ForSurgery:
            return tr("For Surgery");
        case KeyImageNote::ForTeaching:
            return tr("For Teaching");
        case KeyImageNote::ForConference:
            return tr("For Conference");
        case KeyImageNote::ForTherapy:
            return tr("For Therapy");
        case KeyImageNote::ForPatient:
            return tr("For Patient");
        case KeyImageNote::ForPeerReview:
            return tr("For Peer Review");
        case KeyImageNote::ForResearch:
            return tr("For Research");
        case KeyImageNote::QualityIssue:
            return tr("Quality Issue");
        case KeyImageNote::NoneDocumentTitle:
            return tr("None Document Title");
        default:
            return tr("Bad DocumentTitle");
    }
}

QString KeyImageNote::getRejectedForQualityReasonsAsString(KeyImageNote::RejectedForQualityReasons rejectedForQualityReasons)
{
    switch (rejectedForQualityReasons)
    {
        case KeyImageNote::ImageArtifacts:
            return tr("Image artifact(s)");
        case KeyImageNote::GridArtifacts:
            return tr("Grid artifact(s)");
        case KeyImageNote::Positioning:
            return tr("Positioning");
        case KeyImageNote::MotionBlur:
            return tr("Motion blur");
        case KeyImageNote::UnderExposed:
            return tr("Under exposed");
        case KeyImageNote::OverExposed:
            return tr("Over exposed");
        case KeyImageNote::NoImage:
            return tr("No image");
        case KeyImageNote::DetectorArtifacts:
            return tr("Detector artifact(s)");
        case KeyImageNote::ArtifactsOtherThanGridOrDetectorArtifact:
            return tr("Artifact(s) other than grid or detector artifact");
        case KeyImageNote::MechanicalFailure:
            return tr("Mechanical failure");
        case KeyImageNote::ElectricalFailure:
            return tr("Electrical failure");
        case KeyImageNote::SoftwareFailure:
            return tr("Software failure");
        case KeyImageNote::InappropiateImageProcessing:
            return tr("Inappropiate image processing");
        case KeyImageNote::OtherFailure:
            return tr("Other failure");
        case KeyImageNote::UnknownFailure:
            return tr("Unknown failure");
        case KeyImageNote::DoubleExposure:
            return tr("Double exposure");
        case KeyImageNote::NoneRejectedForQualityReasons:
            return tr("None rejected for quality reasons");
        default:
            return tr("Bad RejectedForQualityReasons");
    }
}

QStringList KeyImageNote::getAllDocumentTitles()
{
    bool foundEnumerator = false;
    int i = 0;
    QMetaObject metaEnumerator = KeyImageNote::staticMetaObject;
    QStringList enumeratorTypesList;
    while(!foundEnumerator && i < metaEnumerator.enumeratorCount())
    {
        QMetaEnum enumType = metaEnumerator.enumerator(i);
        
        if (enumType.name() == QString("DocumentTitle"))
        {
            for(int j = 0; j < enumType.keyCount(); ++j)
            {
                KeyImageNote::DocumentTitle documentTitleElement = DocumentTitle(enumType.keyToValue(enumType.key(j)));
                
                if (documentTitleElement != KeyImageNote::NoneDocumentTitle)
                {
                    enumeratorTypesList << getDocumentTitleAsString(documentTitleElement);
                }
            }
            foundEnumerator = true;
        }
        i++;

    }
    
    return enumeratorTypesList;
}

QStringList KeyImageNote::getAllRejectedForQualityReasons()
{
    bool foundEnumerator = false;
    int i = 0;
    QMetaObject metaEnumerator = KeyImageNote::staticMetaObject;
    QStringList enumeratorTypesList;
    while(!foundEnumerator && i < metaEnumerator.enumeratorCount())
    {
        QMetaEnum enumType = metaEnumerator.enumerator(i);
        
        if (enumType.name() == QString("RejectedForQualityReasons"))
        {
            for(int j = 0; j < enumType.keyCount(); ++j)
            {
                KeyImageNote::RejectedForQualityReasons rejectedForQualityReasonsElement = RejectedForQualityReasons(enumType.keyToValue(enumType.key(j)));
                
                if (rejectedForQualityReasonsElement != KeyImageNote::NoneRejectedForQualityReasons)
                {
                    enumeratorTypesList << getRejectedForQualityReasonsAsString(rejectedForQualityReasonsElement);
                }
            }
            foundEnumerator = true;
        }
        i++;
    }
    return enumeratorTypesList;
}

bool KeyImageNote::isDocumentTitleModifiedForQualityReasonsOrIssues(KeyImageNote::DocumentTitle documentTitle)
{
    return documentTitle == KeyImageNote::RejectedForQualityReasonsDocumentTitle || documentTitle == KeyImageNote::QualityIssue;
}

KeyImageNote::RejectedForQualityReasons KeyImageNote::getRejectedForQualityReasonsInstanceFromString(const QString &rejectedForQualityReasons)
{

    bool foundEnumerator = false;
    int i = 0;
    int result = 0;
    QMetaObject metaEnumerator = KeyImageNote::staticMetaObject;
    
    while(!foundEnumerator && i < metaEnumerator.enumeratorCount())
    {
        QMetaEnum enumType = metaEnumerator.enumerator(i);
        
        if (enumType.name() == QString("RejectedForQualityReasons"))
        {
            int j = 0;
            while (j < enumType.keyCount() && !foundEnumerator)
            {
                if (getRejectedForQualityReasonsAsString(RejectedForQualityReasons(enumType.keyToValue(enumType.key(j)))) == rejectedForQualityReasons)
                {
                    foundEnumerator = true;
                    result = enumType.keyToValue(enumType.key(j));
                }
                else
                {
                    j++;
                }
            }
        }

        i++;
    }

    return KeyImageNote::RejectedForQualityReasons(result);
}

KeyImageNote::DocumentTitle KeyImageNote::getDocumentTitleInstanceFromString(const QString &documentTitle)
{
    bool foundEnumerator = false;
    int i = 0;
    int result = 0;
    QMetaObject metaEnumerator = KeyImageNote::staticMetaObject;
    
    while(!foundEnumerator && i < metaEnumerator.enumeratorCount())
    {
        QMetaEnum enumType = metaEnumerator.enumerator(i);
        
        if (enumType.name() == QString("DocumentTitle"))
        {
            int j = 0;
            while (j < enumType.keyCount() && !foundEnumerator)
            {
                if (getDocumentTitleAsString(DocumentTitle(enumType.keyToValue(enumType.key(j)))) == documentTitle)
                {
                    foundEnumerator = true;
                    result = enumType.keyToValue(enumType.key(j));
                }
                else
                {
                    j++;
                }
            }
        }

        i++;
    }

    return KeyImageNote::DocumentTitle(result);
}
void KeyImageNote::setPath(QString path)
{
    m_path = path;
}

QString KeyImageNote::getPath() const
{
    return m_path;
}

}