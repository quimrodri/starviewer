#ifndef UGDIMAGEORIENTATION_H
#define UGDIMAGEORIENTATION_H

#include <QString>
#include <QVector3D>

namespace udg {

/**
    Aquesta classe encapsula l�atribut DICOM Image Orientation (Patient) (0020,0037) que defineix les direccions dels vectors de la primera fila i columna 
    de la imatge respecte al pacient. Per m�s informaci� consultar PS 3.3, secci� C.7.6.2.1.1.

    En format DICOM aquest atribut consta de 6 valors separats per '\', essent els 3 primers el vector de la fila i els 3 �ltims el vector de la columna.

    Aquesta classe, a m�s a m�s guarda la normal del pla que formen aquests dos vectors.
  */
class ImageOrientation {
public:
    ImageOrientation();
    ~ImageOrientation();
    
    /// Constructor a partir de dos vectors 3D
    ImageOrientation(const QVector3D &rowVector, const QVector3D &columnVector);
    
    /// Assigna la orientaci� proporcionada en el format estipulat pel DICOM: 6 valors num�rics separats per '\' o una cadena buida. 
    /// Si la cadena no est� en el format esperat, es re-inicialitzen els valors dels vectors i es retorna fals, cert altrament.
    bool setDICOMFormattedImageOrientation(const QString &imageOrientation);

    /// Ens retorna la orientaci� en el format estipulat pel DICOM: 6 valors num�rics separats per '\'
    /// En cas que no s'hagi introduit cap valor anteriorment, es retornar� una cadena buida
    QString getDICOMFormattedImageOrientation() const;

    /// Assigna la orientaci� a trav�s dels 2 vectors 3D corresponents a les direccions de la fila i de la columna respectivament
    void setRowAndColumnVectors(const QVector3D &rowVector, const QVector3D &columnVector);
    
    /// Ens retorna els vectors fila, columna i normal respectivament.
    /// En cas que no s'hagi assignat cap orientaci�, els vectors no tindran cap element
    QVector3D getRowVector() const;
    QVector3D getColumnVector() const;
    QVector3D getNormalVector() const;

    /// Operador igualtat
    bool operator==(const ImageOrientation &imageOrientation) const;

private:
    /// Ens retorna la orientaci� dels dos vectors en una cadena en el format estipulat pel DICOM
    QString convertToDICOMFormatString(const QVector3D &rowVector, const QVector3D &columnVector) const;

    /// Inicialitza els vectors a (0,0,0)
    void setVectorValuesToDefault();

private:
    /// Els 3 vectors de la orientaci� de la imatge.
    /// Els vectors fila i columna els obtindrem directament a partir de la cadena assignada amb setDICOMFormattedImageOrientation()
    /// El vector normal de la orientaci� de la imatge es calcular� fent el producte vectorial dels vectors fila i colummna
    QVector3D m_rowVector;
    QVector3D m_columnVector;
    QVector3D m_normalVector;
};

} // End namespace udg

#endif
