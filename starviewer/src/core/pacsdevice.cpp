#include "pacsdevice.h"

#include "logging.h"
#include "coresettings.h"
#include <QStringList>

namespace udg {

PacsDevice::PacsDevice()
{
    m_queryRetrieveServicePort = -1;
    m_storeServicePort = -1;
}

void PacsDevice::setAddress(const QString &address)
{
    m_address = address;
}

QString PacsDevice::getAddress() const
{
    return m_address;
}

void PacsDevice::setQueryRetrieveServicePort(int queryRetrieveServicePort)
{
    m_queryRetrieveServicePort = queryRetrieveServicePort;
}

int PacsDevice::getQueryRetrieveServicePort() const
{
    return m_queryRetrieveServicePort;
}

void PacsDevice::setAETitle(const QString &AETitle)
{
    m_AETitle = AETitle;
}

QString PacsDevice::getAETitle() const
{
    return m_AETitle;
}

void PacsDevice::setInstitution(const QString &institution)
{
    m_institution = institution;
}

QString PacsDevice::getInstitution() const
{
    return m_institution;
}

void PacsDevice::setLocation(const QString &location)
{
    m_location = location;
}

QString PacsDevice::getLocation() const
{
    return m_location;
}

void PacsDevice::setDescription(const QString &description)
{
    m_description = description;
}

QString PacsDevice::getDescription() const
{
    return m_description;
}

void PacsDevice::setDefault(bool isDefault)
{
    QStringList pacsList = getDefaultPACSKeyNamesList();
    QString keyName = getKeyName();
    // Afegir
    if (isDefault)
    {
        // Si no est� marcat ja
        if (!pacsList.contains(keyName))
        {
            Settings settings;
            QString value = settings.getValue(CoreSettings::DefaultPACSListToQuery).toString();
            value += keyName + "//";
            settings.setValue(CoreSettings::DefaultPACSListToQuery, value);
        }
    }
    else
    {
        // Eliminar
        Settings settings;
        QString value = settings.getValue(CoreSettings::DefaultPACSListToQuery).toString();
        value.remove(keyName + "//");
        settings.setValue(CoreSettings::DefaultPACSListToQuery, value);
    }
}

bool PacsDevice::isDefault() const
{
    QStringList pacsList = getDefaultPACSKeyNamesList();
    if (pacsList.contains(getKeyName()))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void PacsDevice::setID(const QString &id)
{
    m_id = id;
}

QString PacsDevice::getID() const
{
    return m_id;
}

/// Assigna/Retorna si podem fer consultes/descarregues al PACS
void PacsDevice::setQueryRetrieveServiceEnabled(bool isQueryRetrieveServiceEnabled)
{
    m_isQueryRetrieveServiceEnabled = isQueryRetrieveServiceEnabled;
}

bool PacsDevice::isQueryRetrieveServiceEnabled() const
{
    return m_isQueryRetrieveServiceEnabled;
}

void PacsDevice::setStoreServiceEnabled(bool isStoreServiceEnabled)
{
    m_isStoreServiceEnabled = isStoreServiceEnabled;
}

bool PacsDevice::isStoreServiceEnabled() const
{
    return m_isStoreServiceEnabled;
}

void PacsDevice::setStoreServicePort(int storeServicePort)
{
    m_storeServicePort = storeServicePort;
}

int PacsDevice::getStoreServicePort() const
{
    return m_storeServicePort;
}

bool PacsDevice::isEmpty() const
{
    if (m_AETitle.isEmpty() &&
        m_address.isEmpty() &&
        m_description.isEmpty() &&
        m_institution.isEmpty() &&
        m_location.isEmpty() &&
        m_id.isEmpty())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool PacsDevice::isSamePacsDevice(const PacsDevice &pacsDevice) const
{
    return m_AETitle == pacsDevice.getAETitle()
        && m_address == pacsDevice.getAddress()
        && m_queryRetrieveServicePort == pacsDevice.getQueryRetrieveServicePort();
}

bool PacsDevice::operator ==(const PacsDevice &device)
{
    return m_AETitle == device.m_AETitle
        && m_address == device.m_address
        && m_description == device.m_description
        && m_institution == device.m_institution
        && m_location == device.m_location
        && m_id == device.m_id
        && m_isQueryRetrieveServiceEnabled == device.m_isQueryRetrieveServiceEnabled
        && m_queryRetrieveServicePort == device.m_queryRetrieveServicePort
        && m_isStoreServiceEnabled == device.m_isStoreServiceEnabled
        && m_storeServicePort == device.m_storeServicePort;
}

QString PacsDevice::getKeyName() const
{
    return m_AETitle + m_address + ":" + QString::number(m_queryRetrieveServicePort);
}

QStringList PacsDevice::getDefaultPACSKeyNamesList() const
{
    Settings settings;
    QString value = settings.getValue(CoreSettings::DefaultPACSListToQuery).toString();
    QStringList pacsList = value.split("//", QString::SkipEmptyParts);

    if (pacsList.isEmpty())
    {
        // Migraci� de dades. Si encara no tenim definits els PACS per defecte en el nou format, obtenim els PACS per defecte
        // del format antic, �s a dir, a partir dels elements amb els valors "default" = "S" de la llista de PACS
        // Un cop llegits, els escrivim en el nou format
        Settings::SettingListType list = settings.getList(CoreSettings::PacsListConfigurationSectionName);
        foreach (Settings::SettingsListItemType item, list)
        {
            // El camp "default" t� aquesta clau
            if (item.contains("."))
            {
                if (item.value(".").toString() == "S")
                {
                    // Hem de fer servir els mateixos camps i format que al m�tode PacsDevice::getKeyName()
                    pacsList << item.value("AETitle").toString() + item.value("PacsHostname").toString() + ":" + item.value("PacsPort").toString();
                }
            }
        }
        if (pacsList.isEmpty())
        {
            INFO_LOG("No hi ha PACS per defecte definits en el nou format i tampoc s'han trobat de definits en l'antic format");
        }
        else
        {
            INFO_LOG("No hi ha PACS per defecte definits en el nou format. Els obtenim del format antic i els migrem al nou format. S�n aquests: " +
                     pacsList.join("//") + "//");
            Settings settings;
            settings.setValue(CoreSettings::DefaultPACSListToQuery, pacsList.join("//") + "//");
        }
    }

    return pacsList;
}

}
