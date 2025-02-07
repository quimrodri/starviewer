#ifndef PHASESIGNALTOSYNCACTIONMAPPER_H
#define PHASESIGNALTOSYNCACTIONMAPPER_H

#include "signaltosyncactionmapper.h"

namespace udg {

/**
    Implementation of SignalToSyncActionMapper to map phase changes to PhaseSyncAction.
 */
class PhaseSignalToSyncActionMapper : public SignalToSyncActionMapper {

    Q_OBJECT

public:
    PhaseSignalToSyncActionMapper(QObject *parent = 0);
    virtual ~PhaseSignalToSyncActionMapper();

    /// Maps current phase of the viewer
    virtual void mapProperty();

protected:
    /// Maps the phaseChanged(int) signal from the viewer to the actionMapped(SyncAction*) signal.
    virtual void mapSignal();

    /// Unmaps the phaseChanged(int) signal from the viewer to the actionMapped(SyncAction*) signal.
    virtual void unmapSignal();

private slots:
    /// Updates the mapped sync action with the given phase index and the viewer's main volume and emits the actionMapped(SyncAction*) signal.
    void mapToSyncAction(int phase);

};

}

#endif
