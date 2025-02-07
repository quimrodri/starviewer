#include "decaycorrectionfactorformulacalculator.h"

#include <QTime>

#include "dicomtagreader.h"
#include "dicomsequenceattribute.h"
#include "dicomvalueattribute.h"
#include "dicomsequenceitem.h"
#include "dicomvaluerepresentationconverter.h"

namespace udg {

DecayCorrectionFactorFormulaCalculator::DecayCorrectionFactorFormulaCalculator()
{
    initializeParameters();
}

DecayCorrectionFactorFormulaCalculator::~DecayCorrectionFactorFormulaCalculator()
{
}

bool DecayCorrectionFactorFormulaCalculator::canCompute()
{
    gatherRequiredParameters();
    return parameterValuesAreValid();
}

double DecayCorrectionFactorFormulaCalculator::compute()
{
    gatherRequiredParameters();

    return DecayCorrectionFactorFormula::compute(m_radionuclideHalfLifeInSeconds, computeTimeLapseInSeconds());
}

void DecayCorrectionFactorFormulaCalculator::initializeParameters()
{
    m_radionuclideHalfLifeInSeconds = 0.0;
    m_timeLapseInSeconds = 0;
}

bool DecayCorrectionFactorFormulaCalculator::parameterValuesAreValid() const
{
    if (m_decayCorrection == "START" || m_decayCorrection == "ADMIN")
    {
        if (m_radionuclideHalfLifeInSeconds >= 0.0 && computeTimeLapseInSeconds() >= 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (m_decayCorrection == "NONE")
    {
        // TODO Is unclear wether is valid to compute the value or not
        return false;
    }
    else
    {
        return false;
    }
}

void DecayCorrectionFactorFormulaCalculator::gatherRequiredParameters()
{
    // Not all the required parameters are present through image. By the moment we can only retrieve them accessing to the whole dataset
    gatherRequiredParameters(m_tagReaderSource);
}

void DecayCorrectionFactorFormulaCalculator::gatherRequiredParameters(DICOMTagReader *tagReader)
{
    if (!tagReader)
    {
        return;
    }

    m_decayCorrection = tagReader->getValueAttributeAsQString(DICOMDecayCorrection);

    QDateTime seriesDateTime;
    seriesDateTime.setDate(QDate::fromString(tagReader->getValueAttributeAsQString(DICOMSeriesDate), "yyyyMMdd"));
    seriesDateTime.setTime(DICOMValueRepresentationConverter::timeToQTime(tagReader->getValueAttributeAsQString(DICOMSeriesTime)));

    QDateTime radiopharmaceuticalStartDateTime;
    DICOMSequenceAttribute *radiopharmaceuticalInfoSequence = tagReader->getSequenceAttribute(DICOMRadiopharmaceuticalInformationSequence);
    if (radiopharmaceuticalInfoSequence)
    {
        if (!radiopharmaceuticalInfoSequence->getItems().isEmpty())
        {
            // TODO It could be more than one item!
            DICOMSequenceItem *item = radiopharmaceuticalInfoSequence->getItems().first();
            DICOMValueAttribute *radioPharmaceuticalStartDateTimeAttribute = item->getValueAttribute(DICOMRadiopharmaceuticalStartDateTime);
            if (radioPharmaceuticalStartDateTimeAttribute)
            {
                QString radioPharmaceuticalStartDateTimeString = radioPharmaceuticalStartDateTimeAttribute->getValueAsQString();
                radiopharmaceuticalStartDateTime.setDate(QDate::fromString(radioPharmaceuticalStartDateTimeString.left(8), "yyyyMMdd"));
                radiopharmaceuticalStartDateTime.setTime(DICOMValueRepresentationConverter::timeToQTime(radioPharmaceuticalStartDateTimeString.remove(0,8)));
            }
            else
            {
                DICOMValueAttribute *radioPharmaceuticalStartTime = item->getValueAttribute(DICOMRadiopharmaceuticalStartTime);
                if (radioPharmaceuticalStartTime)
                {
                    radiopharmaceuticalStartDateTime.setDate(seriesDateTime.date());
                    radiopharmaceuticalStartDateTime.setTime(DICOMValueRepresentationConverter::timeToQTime(radioPharmaceuticalStartTime->getValueAsQString()));
                }
            }
            m_radionuclideHalfLifeInSeconds = item->getValueAttribute(DICOMRadionuclideHalfLife)->getValueAsDouble();
        }
    }

    if (seriesDateTime.isValid() && radiopharmaceuticalStartDateTime.isValid())
    {
        int hoursInMsecs = radiopharmaceuticalStartDateTime.time().msecsTo(seriesDateTime.time());
        int daysInSecs = radiopharmaceuticalStartDateTime.date().daysTo(seriesDateTime.date()) * 86400;
        m_timeLapseInSeconds = daysInSecs + hoursInMsecs * 0.001;
    }
    else
    {
        m_timeLapseInSeconds = -1;
    }
}

double DecayCorrectionFactorFormulaCalculator::computeTimeLapseInSeconds() const
{
    if (m_decayCorrection == "ADMIN")
    {
        return 0.0;
    }
    else if (m_decayCorrection == "START")
    {
        return m_timeLapseInSeconds;
    }
    else
    {
        return -1.0;
    }
}

} // End namespace udg
