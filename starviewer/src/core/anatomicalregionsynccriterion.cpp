#include "anatomicalregionsynccriterion.h"

#include "q2dviewer.h"
#include "imageplane.h"
#include "mathtools.h"
#include "logging.h"
#include "volume.h"

namespace udg {

AnatomicalRegionSyncCriterion::AnatomicalRegionSyncCriterion()
{
}

AnatomicalRegionSyncCriterion::~AnatomicalRegionSyncCriterion()
{
}

bool AnatomicalRegionSyncCriterion::criterionIsMet(QViewer *viewer1, QViewer *viewer2)
{
    return criterionIsMet(Q2DViewer::castFromQViewer(viewer1), Q2DViewer::castFromQViewer(viewer2));
}

bool AnatomicalRegionSyncCriterion::criterionIsMet(Q2DViewer *viewer1, Q2DViewer *viewer2)
{
    if (!viewer1 || !viewer2)
    {
        return false;
    }
    else
    {
        ImagePlane *imagePlane1 = viewer1->getCurrentImagePlane();
        ImagePlane *imagePlane2 = viewer2->getCurrentImagePlane();

        if (!imagePlane1 || !imagePlane2)
        {
            return false;
        }

        QList<QVector<double> > planeBounds1 = imagePlane1->getCentralBounds();
        QList<QVector<double> > planeBounds2 = imagePlane2->getCentralBounds();

        int yIndex1 = getYIndex(viewer1->getMainInput()->getAcquisitionPlane(), viewer1->getCurrentAnatomicalPlane());
        int yIndex2 = getYIndex(viewer2->getMainInput()->getAcquisitionPlane(), viewer2->getCurrentAnatomicalPlane());

        if (yIndex1 == -1 || yIndex2 == -1)
        {
            return false;
        }

        double min1 = planeBounds1.at(3)[yIndex1];
        double min2 = planeBounds2.at(3)[yIndex2];
        double max1 = planeBounds1.at(0)[yIndex1];
        double max2 = planeBounds2.at(0)[yIndex2];

        if (min1 > max1)
        {
            double value = min1;
            min1 = max1;
            max1 = value;
        }

        if (min2 > max2)
        {
            double value = min2;
            min2 = max2;
            max2 = value;
        }

        // If a volume is inside the other returns true
        double tolerance = 5 * imagePlane1->getSpacing().y();
        if ((min1 - tolerance <= min2 && max1 + tolerance >= max2) || (min2 - tolerance <= min1 && max2 + tolerance >= max1))
        {
            return true;
        }
        else
        {
            double range = qMax(max1, max2) - qMin(min1, min2);
            double minPercentageOfOverlap = 0.7;

            // Returns true if the overlap is equal or greater than 70% of the total region
            if (min1 > min2 && min1 < max2 && max2 < max1)
            {
                return ((max2 - min1) / range) >= minPercentageOfOverlap;
            }
            else if (min2 > min1 && min2 < max1 && max1 < max2)
            {
                return ((max1 - min2)) / range >= minPercentageOfOverlap;
            }
            else
            {
                return false;
            }
        }
    }
}

int AnatomicalRegionSyncCriterion::getYIndex(AnatomicalPlane::AnatomicalPlaneType originalPlane, AnatomicalPlane::AnatomicalPlaneType currentPlane)
{
    if (originalPlane == AnatomicalPlane::Axial)
    {
        if (currentPlane == AnatomicalPlane::Axial)
        {
            return 1;
        }
        else if (currentPlane == AnatomicalPlane::Sagittal)
        {
            return 2;
        }
        else if (currentPlane == AnatomicalPlane::Coronal)
        {
            return 2;
        }
    }
    else if (originalPlane == AnatomicalPlane::Sagittal)
    {
        if (currentPlane == AnatomicalPlane::Axial)
        {
            return 0;
        }
        else if (currentPlane == AnatomicalPlane::Sagittal)
        {
            return 2;
        }
        else if (currentPlane == AnatomicalPlane::Coronal)
        {
            return 0;
        }
    }
    else if (originalPlane == AnatomicalPlane::Coronal)
    {
        if (currentPlane == AnatomicalPlane::Axial)
        {
            return 1;
        }
        else if (currentPlane == AnatomicalPlane::Sagittal)
        {
            return 1;
        }
        else if (currentPlane == AnatomicalPlane::Coronal)
        {
            return 2;
        }
    }

    return -1;
}

} // namespace udg
