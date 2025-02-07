#ifndef UDGSYNCACTION_H
#define UDGSYNCACTION_H

#include "syncactionmetadata.h"

#include <QList>

namespace udg {

class QViewer;
class SyncCriterion;

/**
    Abstract class to implement a syncronization or propagation action to be performed on a viewer.
    Each inherited class should implement the run() method where the desired action would be performed.
 */
class SyncAction {
public:
    SyncAction();
    virtual ~SyncAction();

    /// This method runs the corresponding sync action on the given viewer
    /// This method should be implemented by each subclass 
    virtual void run(QViewer *viewer) = 0;

    /// Returns the associated SyncAction meta data
    SyncActionMetaData getMetaData();

    /// Returns a list with the default list of defined SynCriterion for an specific SyncAction
    QList<SyncCriterion*> getDefaultSyncCriteria();

protected:
    /// The corresponding metadata of the derived SyncAction will be set up on this method.
    virtual void setupMetaData() = 0;

    /// Sets up the default list of SyncCriterion
    virtual void setupDefaultSyncCriteria() = 0;

protected:
    /// The corresponding metadata for the SyncAction
    SyncActionMetaData m_metaData;

    /// List with the default defined SyncCriteria for the SyncAction
    QList<SyncCriterion*> m_defaultSyncCriteria;
};

} // End namespace udg

#endif
