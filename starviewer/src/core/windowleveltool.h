#ifndef UDGWINDOWLEVELTOOL_H
#define UDGWINDOWLEVELTOOL_H

#include <QPoint>

#include "tool.h"

namespace udg {

class QViewer;

class WindowLevelTool : public Tool {
Q_OBJECT
public:
    /// Estats de la tool
    enum { None, WindowLevelling };

    WindowLevelTool(QViewer *viewer, QObject *parent = 0);
    ~WindowLevelTool();

    void handleEvent(unsigned long eventID);

private slots:
    /// Resets the tool to a valid initial state.
    void reset();

    /// Comença el window level
    void startWindowLevel();

    /// Calcula el nou window level
    void doWindowLevel();

    /// Atura l'estat de window level
    void endWindowLevel();

    /// Updates the way window level is being modified
    void updateWindowLevellingBehaviour();

private:
    /// Computes the corresponding window level values upon the movement factor specified by delta on each axis
    void computeWindowLevelValues(double deltaX, double deltaY, double &window, double &level);
    
    /// Computes the the new window level values having always fixed the minimum value. The window is the only altered value by delta
    /// The value of level will be always computed from the value of window
    void computeWindowLevelValuesWithFixedMinimumBehaviour(double deltaX, double &window, double &level);
    
    /// Computes the new window level from each delta. deltaX sets the window, and deltaY the level.
    void computeWindowLevelValuesWithDefaultBehaviour(double deltaX, double deltaY, double &window, double &level);

    /// Avoids zero values for window and level
    void avoidZero(double &window, double &level);
    
    /// Avoids zero and negative values for window and level
    void avoidZeroAndNegative(double &window, double &level);

private:
    /// Valors per controlar el mapeig del window level
    double m_initialWindow, m_initialLevel, m_currentWindow, m_currentLevel;
    QPoint m_windowLevelStartPosition;
    QPoint m_windowLevelCurrentPosition;

    /// Estats d'execució de la tool
    int m_state;

    /// Available behaviours to modify the window level
    enum WindowLevellingBehaviourType { Default, FixedMinimum };
    WindowLevellingBehaviourType m_windowLevellingBehaviour;
};

}

#endif
