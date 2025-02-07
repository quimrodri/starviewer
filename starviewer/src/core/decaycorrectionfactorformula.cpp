#include "decaycorrectionfactorformula.h"

#include <QtCore/qmath.h>

namespace udg {

DecayCorrectionFactorFormula::DecayCorrectionFactorFormula()
{
}

DecayCorrectionFactorFormula::~DecayCorrectionFactorFormula()
{
}

double DecayCorrectionFactorFormula::compute(double radionuclideHalfLifeInSeconds, double timeLapseInSeconds)
{
    return qPow(2.0, -timeLapseInSeconds / radionuclideHalfLifeInSeconds);
}

} // End namespace udg
