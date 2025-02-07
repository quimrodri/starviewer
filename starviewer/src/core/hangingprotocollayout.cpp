#include "hangingprotocollayout.h"

namespace udg {

HangingProtocolLayout::HangingProtocolLayout(QObject *parent)
: QObject(parent)
{
}

HangingProtocolLayout::~HangingProtocolLayout()
{
}

void HangingProtocolLayout::setNumberOfScreens(int numberOfScreens)
{
    m_numberOfScreens = numberOfScreens;
}

void HangingProtocolLayout::setVerticalPixelsList(const QList<int> &verticalPixelsList)
{
    m_verticalPixelsList = verticalPixelsList;
}

void HangingProtocolLayout::setHorizontalPixelsList(const QList<int> &horizontalPixelsList)
{
    m_horizontalPixelsList = horizontalPixelsList;
}

void HangingProtocolLayout::setDisplayEnvironmentSpatialPositionList(const QStringList &displayEnvironmentSpatialPosition)
{
    m_displayEnvironmentSpatialPosition = displayEnvironmentSpatialPosition;
}

int HangingProtocolLayout::getNumberOfScreens() const
{
    return m_numberOfScreens;
}

QList<int> HangingProtocolLayout::getVerticalPixelsList() const
{
    return m_verticalPixelsList;
}

QList<int> HangingProtocolLayout::getHorizontalPixelsList() const
{
    return m_horizontalPixelsList;
}

QStringList HangingProtocolLayout::getDisplayEnvironmentSpatialPositionList() const
{
    return m_displayEnvironmentSpatialPosition;
}

}
