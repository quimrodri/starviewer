#include "tool.h"

namespace udg {

Tool::Tool(QViewer *viewer, QObject *parent)
 : QObject(parent), m_viewer(viewer), m_toolConfiguration(0), m_toolData(0), m_hasSharedData(false), m_hasPersistentData(false)
{
}

Tool::~Tool()
{
}

void Tool::setConfiguration(ToolConfiguration *configuration)
{
    m_toolConfiguration = configuration;
}

ToolConfiguration* Tool::getConfiguration() const
{
    return m_toolConfiguration;
}

void Tool::setToolData(ToolData *data)
{
    m_toolData = data;
}

ToolData* Tool::getToolData() const
{
    return m_toolData;
}

bool Tool::hasSharedData() const
{
    return m_hasSharedData;
}

bool Tool::hasPersistentData() const
{
    return m_hasPersistentData;
}

QString Tool::toolName()
{
    return m_toolName;
}

}
