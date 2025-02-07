#include "thickslabsignaltosyncactionmapper.h"

#include "q2dviewer.h"
#include "thickslabsyncaction.h"

namespace udg {

ThickSlabSignalToSyncActionMapper::ThickSlabSignalToSyncActionMapper(QObject *parent)
 : SignalToSyncActionMapper(parent)
{
    m_mappedSyncAction = new ThickSlabSyncAction();
}

ThickSlabSignalToSyncActionMapper::~ThickSlabSignalToSyncActionMapper()
{
}

void ThickSlabSignalToSyncActionMapper::mapProperty()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);

    if (viewer2D)
    {
        mapThicknessToSyncAction(viewer2D->getSlabThickness());
        mapProjectionModeToSyncAction(viewer2D->getSlabProjectionMode());
    }
}

void ThickSlabSignalToSyncActionMapper::mapSignal()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);

    if (viewer2D)
    {
        connect(viewer2D, SIGNAL(slabProjectionModeChanged(int)), SLOT(mapProjectionModeToSyncAction(int)));
        connect(viewer2D, SIGNAL(slabThicknessChanged(int)), SLOT(mapThicknessToSyncAction(int)));
    }
}

void ThickSlabSignalToSyncActionMapper::unmapSignal()
{
    Q2DViewer *viewer2D = Q2DViewer::castFromQViewer(m_viewer);

    if (viewer2D)
    {
        disconnect(viewer2D, SIGNAL(slabProjectionModeChanged(int)), this, SLOT(mapProjectionModeToSyncAction(int)));
        disconnect(viewer2D, SIGNAL(slabThicknessChanged(int)), this, SLOT(mapThicknessToSyncAction(int)));
    }
}

void ThickSlabSignalToSyncActionMapper::mapProjectionModeToSyncAction(int slabProjectionMode)
{
    ThickSlabSyncAction *action = static_cast<ThickSlabSyncAction*>(m_mappedSyncAction);
    action->setVolume(m_viewer->getMainInput());
    action->setSlabProjectionMode(slabProjectionMode);

    emit actionMapped(m_mappedSyncAction);
}

void ThickSlabSignalToSyncActionMapper::mapThicknessToSyncAction(int slabThickness)
{
    ThickSlabSyncAction *action = static_cast<ThickSlabSyncAction*>(m_mappedSyncAction);
    action->setVolume(m_viewer->getMainInput());
    action->setSlabThickness(slabThickness);

    emit actionMapped(m_mappedSyncAction);
}

}
