#include "referencelinestooldata.h"
#include "imageplane.h"

namespace udg {

ReferenceLinesToolData::ReferenceLinesToolData(QObject *parent)
 : ToolData(parent)
{
}

ReferenceLinesToolData::~ReferenceLinesToolData()
{
    foreach (ImagePlane *plane, m_planesToProject)
    {
        delete plane;
    }
}

QString ReferenceLinesToolData::getFrameOfReferenceUID() const
{
    return m_frameOfReferenceUID;
}

QList<ImagePlane*> ReferenceLinesToolData::getPlanesToProject() const
{
    return m_planesToProject;
}

void ReferenceLinesToolData::setFrameOfReferenceUID(const QString &frameOfReference)
{
    m_frameOfReferenceUID = frameOfReference;
}

void ReferenceLinesToolData::setPlanesToProject(QList<ImagePlane*> planes)
{
    foreach (ImagePlane *plane, m_planesToProject)
    {
        delete plane;
    }
    m_planesToProject.clear();
    m_planesToProject = planes;
    emit changed();
}

void ReferenceLinesToolData::setPlanesToProject(ImagePlane *plane)
{
    foreach (ImagePlane *plane, m_planesToProject)
    {
        delete plane;
    }
    m_planesToProject.clear();
    if (plane)
    {
        m_planesToProject << plane;
    }
    emit changed();
}

}
