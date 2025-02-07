#include "extensioncontext.h"

#include "patient.h"
#include "volume.h"
#include "logging.h"

namespace udg {

ExtensionContext::ExtensionContext()
{
}

ExtensionContext::~ExtensionContext()
{
}

Patient* ExtensionContext::getPatient() const
{
    return m_patient;
}

void ExtensionContext::setPatient(Patient *patient)
{
    m_patient = patient;
}

Volume *ExtensionContext::getDefaultVolume() const
{
    Volume *defaultVolume = NULL;
    Series *defaultSeries = NULL;
    bool searchForDefaultSeries = false;
    QList<Series*> selectedSeries = m_patient->getSelectedSeries();

    if (selectedSeries.isEmpty())
    {
        searchForDefaultSeries = true;
    }
    else
    {
        // TODO de moment només agafem la primera de les possibles seleccionades
        defaultSeries = selectedSeries.at(0);
        // Necessitem que les sèries siguin visualitzables
        if (!defaultSeries->isViewable())
        {
            searchForDefaultSeries = true;
        }
        else
        {
            defaultVolume = defaultSeries->getFirstVolume();
        }
    }
    // En comptes de searchForDefaultSeries podríem fer servir
    // defaultVolume, però amb la var. booleana el codi és més llegible
    if (searchForDefaultSeries)
    {
        bool ok = false;
        foreach (Study *study, m_patient->getStudies())
        {
            QList<Series*> viewableSeries = study->getViewableSeries();
            if (!viewableSeries.isEmpty())
            {
                ok = true;
                defaultVolume = viewableSeries.at(0)->getFirstVolume();
                break;
            }
        }
        if (!ok)
        {
            DEBUG_LOG("No hi ha cap serie de l'actual pacient que sigui visualitzable. Retornem volum NUL.");
            ERROR_LOG("No hi ha cap serie de l'actual pacient que sigui visualitzable. Retornem volum NUL.");
        }
    }

    return defaultVolume;
}

}
