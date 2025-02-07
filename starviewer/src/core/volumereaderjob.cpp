#include "volumereaderjob.h"

#include "volumereader.h"
#include "volume.h"
#include "logging.h"

namespace udg {

VolumeReaderJob::VolumeReaderJob(Volume *volume, QObject *parent)
    : Job(parent)
{
    m_volumeToRead = volume;
    m_volumeReadSuccessfully = false;
    m_lastErrorMessageToUser = "";
    m_abortRequested = false;
    m_autoDelete = true;

    connect(this, SIGNAL(done(ThreadWeaver::Job*)), SLOT(autoDelete()));
}

VolumeReaderJob::~VolumeReaderJob()
{
    DEBUG_LOG(QString("Destructor ~VolumeReaderJob pel Volume: %1").arg(m_volumeToRead->getIdentifier().getValue()));
}

void VolumeReaderJob::requestAbort()
{
    // Hem de protegir l'accés a m_volumeReaderToAbort amb un lock ja que si no, es podria donar el cas que es comprovés si és null, donés que no
    // i, jut abans de cridar els seus mètodes, aquest es posés a null, fent petar l'aplicació.
    QMutexLocker locker(&m_volumeReaderToAbortMutex);

    m_abortRequested = true;
    if (!m_volumeReaderToAbort.isNull())
    {
        m_volumeReaderToAbort.data()->requestAbort();
        DEBUG_LOG(QString("requestAbort to Volume: %1 done").arg(m_volumeToRead->getIdentifier().getValue()));
    }
}

bool VolumeReaderJob::success() const
{
    return m_volumeReadSuccessfully && !m_abortRequested;
}

void VolumeReaderJob::setAutoDelete(bool autoDelete)
{
    m_autoDelete = autoDelete;
}

bool VolumeReaderJob::getAutoDelete() const
{
    return m_autoDelete;
}

QString VolumeReaderJob::getLastErrorMessageToUser() const
{
    return m_lastErrorMessageToUser;
}

Volume* VolumeReaderJob::getVolume() const
{
    return m_volumeToRead;
}

void VolumeReaderJob::run()
{
    Q_ASSERT(m_volumeToRead);

    DEBUG_LOG(QString("VolumeReaderJob::run() with Volume: %1").arg(m_volumeToRead->getIdentifier().getValue()));

    VolumeReader *volumeReader = new VolumeReader();

    {
        // El locker només serà vàlid dintre de l'scope. Es fa així en comptes de fer-ho amb un .lock o .unlock per
        // assegurar-nos que si salta una excepció s'alliberarà el lock.
        QMutexLocker locker(&m_volumeReaderToAbortMutex);
        m_volumeReaderToAbort = volumeReader;
    }

    connect(volumeReader, SIGNAL(progress(int)), SLOT(updateProgress(int)));
    m_volumeReadSuccessfully = volumeReader->readWithoutShowingError(m_volumeToRead);
    m_lastErrorMessageToUser = volumeReader->getLastErrorMessageToUser();

    {
        QMutexLocker locker(&m_volumeReaderToAbortMutex);

        m_volumeReaderToAbort.clear();
        delete volumeReader;
    }

    DEBUG_LOG(QString("End VolumeReaderJob::run() with Volume: %1 and result %2").arg(m_volumeToRead->getIdentifier().getValue()).arg(m_volumeReadSuccessfully));
    if (!m_volumeReadSuccessfully)
    {
        DEBUG_LOG(QString("                          Error Volume: %1: %2").arg(m_volumeToRead->getIdentifier().getValue()).arg(m_lastErrorMessageToUser));
    }
}

void VolumeReaderJob::updateProgress(int value)
{
    emit progress(this, value);
}

void VolumeReaderJob::autoDelete()
{
    DEBUG_LOG(QString("VolumeReaderJob::autoDelete() with Volume: %1").arg(m_volumeToRead->getIdentifier().getValue()));
    if (m_autoDelete)
    {
        this->deleteLater();
    }
}

} // End namespace udg
