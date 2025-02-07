#include "dicomvalueattribute.h"

#include "dicomtag.h"

#include <QVariant>
#include <QDate>

namespace udg {

DICOMValueAttribute::DICOMValueAttribute()
 : DICOMAttribute()
{
    setValueRepresentation(DICOMValueAttribute::Unknown);
}

DICOMValueAttribute::~DICOMValueAttribute()
{

}

bool DICOMValueAttribute::isValueAttribute()
{
    return true;
}

bool DICOMValueAttribute::isSequenceAttribute()
{
    return false;
}

QString DICOMValueAttribute::toString(bool verbose)
{
    QString result;
    
    if (verbose)
    {
        result = getTag()->getName() + " ";
    }
    result += QString("%1: %2").arg(getTag()->getKeyAsQString()).arg(getValueAsQString());
    
    return result;
}

void DICOMValueAttribute::setValue(int value)
{
    m_value = value;
    setValueRepresentation(DICOMValueAttribute::Int);
}

void DICOMValueAttribute::setValue(unsigned int value)
{
    m_value = value;
    setValueRepresentation(DICOMValueAttribute::Uint);
}

void DICOMValueAttribute::setValue(double value)
{
    m_value = value;
    setValueRepresentation(DICOMValueAttribute::Double);
}

void DICOMValueAttribute::setValue(const QString &value)
{
    m_value = value;
    setValueRepresentation(DICOMValueAttribute::String);
}

void DICOMValueAttribute::setValue(const QByteArray &value)
{
    m_value = value;
    setValueRepresentation(DICOMValueAttribute::ByteArray);
}

void DICOMValueAttribute::setValue(const QDate &value)
{
    m_value = value.toString("yyyyMMdd");
    setValueRepresentation(DICOMValueAttribute::Date);
}

void DICOMValueAttribute::setValue(const QTime &value)
{
    m_value = value.toString("HHmmss.zzz");
    setValueRepresentation(DICOMValueAttribute::Time);
}

int DICOMValueAttribute::getValueAsInt()
{
    return m_value.toInt();
}

unsigned int DICOMValueAttribute::getValueAsUnsignedInt()
{
    return m_value.toUInt();
}

float DICOMValueAttribute::getValueAsFloat()
{
    return static_cast<float>(m_value.toDouble());
}

double DICOMValueAttribute::getValueAsDouble()
{
    return m_value.toDouble();
}

QString DICOMValueAttribute::getValueAsQString()
{
    return m_value.toString();
}

QByteArray DICOMValueAttribute::getValueAsByteArray()
{
    return m_value.toByteArray();
}

QDate DICOMValueAttribute::getValueAsDate()
{
    return m_value.toDate();
}

QTime DICOMValueAttribute::getValueAsTime()
{
    return m_value.toTime();
}

DICOMValueAttribute::ValueRepresentation DICOMValueAttribute::getValueRepresentation()
{
    return m_valueRepresentation;
}

void DICOMValueAttribute::setValueRepresentation(ValueRepresentation value)
{
    m_valueRepresentation = value;
}

}
