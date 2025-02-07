#ifndef UDGPHOTOMETRICINTERPRETATION_H
#define UDGPHOTOMETRICINTERPRETATION_H

#include <QString>
#include <QMap>

namespace udg {

/**
    Class to represent the DICOM Photometric Interpretation attribute.
    Defined on PS 3.3 C.7.6.3.1.2 Photometric Interpretation (2011 Standard)
*/
class PhotometricInterpretation {
public:
    PhotometricInterpretation();
    
    /// Enumerated photometric interpretation values
    enum PhotometricType { Monochrome1, Monochrome2, RGB, Palette_Color, YBR_Full, YBR_Full_422, YBR_Partial_422, YBR_Partial_420, YBR_ICT, YBR_RCT, None };
    
    PhotometricInterpretation(PhotometricType value);
    PhotometricInterpretation(const QString &value);
    ~PhotometricInterpretation();

    /// Sets the value
    void setValue(PhotometricType value);
    void setValue(const QString &value);
    
    /// Returns the photometric value
    PhotometricType getValue() const;
    
    /// Returns true if the current photometric interpretation value corresponds to a color one (not monochrome)
    bool isColor() const;

    /// Gets the value as a DICOM formatted string
    QString getAsQString() const;

    /// Equality operators. Led compare the same object or the object with the enum value
    bool operator==(const PhotometricInterpretation &value) const;
    bool operator==(const PhotometricType &value) const;
    bool operator!=(const PhotometricInterpretation &value) const;
    bool operator!=(const PhotometricType &value) const;

private:
    /// Initializes the data of the class
    void init();

    /// Gets the enumerated value from a string
    PhotometricType getFromString(const QString &value) const;

private:
    /// The photometric interpretation value
    PhotometricType m_value;
    
    /// Maps the enumerated values with the corresponding string
    QMap<PhotometricType, QString> m_typeStringMap;
};

} // End namespace udg
#endif
