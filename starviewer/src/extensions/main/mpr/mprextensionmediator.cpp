#include "mprextensionmediator.h"

#include "extensioncontext.h"

// QT
#include <QMessageBox>

namespace udg {

MPRExtensionMediator::MPRExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

MPRExtensionMediator::~MPRExtensionMediator()
{
}

DisplayableID MPRExtensionMediator::getExtensionID() const
{
    return DisplayableID("MPRExtension", tr("MPR 2D"));
}

bool MPRExtensionMediator::initializeExtension(QWidget *extension, const ExtensionContext &extensionContext)
{
    QMPRExtension *mprExtension;

    if (!(mprExtension = qobject_cast<QMPRExtension*>(extension)))
    {
        return false;
    }

    Volume *input = extensionContext.getDefaultVolume();
    if (!input)
    {
        QMessageBox::information(0, tr("Starviewer"), tr("The selected item is not an image"));
    }
    else
    {
        mprExtension->setInput(input);
    }

    return true;
}

} // End udg namespace
