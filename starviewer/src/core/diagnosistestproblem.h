#ifndef UDGDIAGNOSISTESTPROBLEM_H
#define UDGDIAGNOSISTESTPROBLEM_H

#include <QString>

namespace udg {

class DiagnosisTestProblem {
public:
    /// Representa l'estat del test, Invalid significa o bé que el test no s'ha fet encara,
    /// o bé que l'estat del test no s'ha assignat correctament
    enum DiagnosisTestProblemState { Ok, Warning, Error, Invalid };

    DiagnosisTestProblem(DiagnosisTestProblemState state = Invalid, QString description = "", QString solution = "");
    ~DiagnosisTestProblem();

    /// Retorna l'estat del test
    DiagnosisTestProblemState getState() const;
    /// Assigna l'estat del test
    void setState(DiagnosisTestProblemState state);
    /// Retorna la descripció del test. Aquest camp tindrà valor si el test no ha donat Ok.
    QString getDescription() const;
    /// Assigna la descripció del resultat del test
    void setDescription(const QString &description);
    /// Retorna la possible solució al problema del test. Només tindrà valor si el resultat no és Ok.
    QString getSolution() const;
    /// Assigna la possible solució o solucions a l'error del test
    void setSolution(const QString &solution);

private:
    /// Guarda l'estat del resultat del test
    DiagnosisTestProblemState m_state;
    /// La descripció del problema en el cas que l'estat no sigui OK
    QString m_description;
    /// Una o diverses possibles solucions en cas de que hi hagi un problema
    QString m_solution;
};

}

#endif
