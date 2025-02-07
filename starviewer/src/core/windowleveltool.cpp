#include "windowleveltool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "windowlevelpresetstooldata.h"
#include "volumehelper.h"

#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>

namespace udg {

WindowLevelTool::WindowLevelTool(QViewer *viewer, QObject *parent)
: Tool(viewer, parent)
{
    m_toolName = "WindowLevelTool";
    // Ens assegurem que desde la creació tenim un viewer vàlid
    Q_ASSERT(m_viewer);

    reset();

    connect(m_viewer, SIGNAL(volumeChanged(Volume*)), SLOT(reset()));
}

WindowLevelTool::~WindowLevelTool()
{
    m_viewer->unsetCursor();
}

void WindowLevelTool::handleEvent(unsigned long eventID)
{
    switch (eventID)
    {
        case vtkCommand::RightButtonPressEvent:
            this->startWindowLevel();
            break;

        case vtkCommand::MouseMoveEvent:
            if (m_state == WindowLevelling)
            {
                this->doWindowLevel();
            }
            break;

        case vtkCommand::RightButtonReleaseEvent:
            this->endWindowLevel();
            break;

        default:
            break;
    }
}

void WindowLevelTool::reset()
{
    m_state = None;
    updateWindowLevellingBehaviour();
}

void WindowLevelTool::startWindowLevel()
{
    m_state = WindowLevelling;
    double wl[2];
    m_viewer->getCurrentWindowLevel(wl);
    m_initialWindow = wl[0];
    m_initialLevel = wl[1];
    m_windowLevelStartPosition = m_viewer->getEventPosition();
    m_viewer->getInteractor()->GetRenderWindow()->SetDesiredUpdateRate(m_viewer->getInteractor()->GetDesiredUpdateRate());
}

void WindowLevelTool::doWindowLevel()
{
    m_viewer->setCursor(QCursor(QPixmap(":/images/windowLevel.png")));
    m_windowLevelCurrentPosition = m_viewer->getEventPosition();

    QSize size = m_viewer->getRenderWindowSize();

    // Compute normalized delta
    double dx = 4.0 * (m_windowLevelCurrentPosition.x() - m_windowLevelStartPosition.x()) / size.width();
    double dy = 4.0 * (m_windowLevelStartPosition.y() - m_windowLevelCurrentPosition.y()) / size.height();

    // Scale by current values
    if (fabs(m_initialWindow) > 0.01)
    {
        dx = dx * m_initialWindow;
    }
    else
    {
        dx = dx * (m_initialWindow < 0 ? -0.01 : 0.01);
    }
    if (fabs(m_initialLevel) > 0.01)
    {
        dy = dy * m_initialLevel;
    }
    else
    {
        dy = dy * (m_initialLevel < 0 ? -0.01 : 0.01);
    }

    // Abs so that direction does not flip
    if (m_initialWindow < 0.0)
    {
        dx = -1 * dx;
    }
    if (m_initialLevel < 0.0)
    {
        dy = -1 * dy;
    }

    // Compute new window level
    double newWindow;
    double newLevel;
    computeWindowLevelValues(dx, dy, newWindow, newLevel);
    m_viewer->getWindowLevelData()->setCustomWindowLevel(newWindow, newLevel);
}

void WindowLevelTool::endWindowLevel()
{
    m_viewer->unsetCursor();
    m_state = None;
    m_viewer->getInteractor()->GetRenderWindow()->SetDesiredUpdateRate(m_viewer->getInteractor()->GetStillUpdateRate());
    // Necessari perquè es torni a renderitzar a alta resolució en el 3D
    m_viewer->render();
}

void WindowLevelTool::updateWindowLevellingBehaviour()
{
    m_windowLevellingBehaviour = Default;
    
    if (!m_viewer)
    {
        return;
    }

    if (!m_viewer->hasInput())
    {
        return;
    }

    if (VolumeHelper::isPrimaryPET(m_viewer->getMainInput()))
    {
        m_windowLevellingBehaviour = FixedMinimum;
    }
}

void WindowLevelTool::computeWindowLevelValues(double deltaX, double deltaY, double &window, double &level)
{
    switch (m_windowLevellingBehaviour)
    {
        case FixedMinimum:
            computeWindowLevelValuesWithFixedMinimumBehaviour(deltaX, window, level);
            break;

        case Default:
        default:
            computeWindowLevelValuesWithDefaultBehaviour(deltaX, deltaY, window, level);
            break;
    }
}

void WindowLevelTool::computeWindowLevelValuesWithFixedMinimumBehaviour(double deltaX, double &window, double &level)
{
    // HACK We use absolute window value to properly handle the windowlevelling when 
    // values have been inverted with the invert tool (window value is negative)
    window = deltaX + fabs(m_initialWindow);
    level = window * 0.5;

    avoidZeroAndNegative(window, level);
    
    // HACK We use this little hack to properly handle the windowlevelling when 
    // values have been inverted with the invert tool (window value is negative)
    if (m_initialWindow < 0)
    {
        window = -window;
    }
}

void WindowLevelTool::computeWindowLevelValuesWithDefaultBehaviour(double deltaX, double deltaY, double &window, double &level)
{
    window = deltaX + m_initialWindow;
    level = m_initialLevel - deltaY;

    avoidZero(window, level);
}

void WindowLevelTool::avoidZero(double &window, double &level)
{
    // Stay away from zero and really
    if (fabs(window) < 0.01)
    {
        window = 0.01 * (window < 0 ? -1 : 1);
    }
    if (fabs(level) < 0.01)
    {
        level = 0.01 * (level < 0 ? -1 : 1);
    }
}

void WindowLevelTool::avoidZeroAndNegative(double &window, double &level)
{
    if (window < 0.01)
    {
        window =  1;
    }
    if (level < 0.01)
    {
        level = 1;
    }
}

}
