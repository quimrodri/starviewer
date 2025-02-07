#ifndef UDGMHDFILECLASSIFIERSTEP_H
#define UDGMHDFILECLASSIFIERSTEP_H

#include "patientfillerstep.h"

namespace udg {

/**
    Classificador d'arxius mhd. Crea tota l'estructura de pacient i en un estudi coloca tantes series com volums tinguem
  */
class MHDFileClassifierStep : public PatientFillerStep {
public:
    MHDFileClassifierStep();

    ~MHDFileClassifierStep();

    bool fillIndividually();

    void postProcessing(){}

    QString name()
    {
        return "MHDFileClassifierStep";
    }
};

}

#endif
