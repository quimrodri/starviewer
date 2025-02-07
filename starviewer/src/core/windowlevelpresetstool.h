#ifndef UDGWINDOWLEVELPRESETSTOOL_H
#define UDGWINDOWLEVELPRESETSTOOL_H

#include "tool.h"

#include <QStringList>
#include <QMap>

namespace udg {

class Q2DViewer;
class WindowLevelPresetsToolData;

/**
    Tool per canviar window levels predefinits
  */
class WindowLevelPresetsTool : public Tool {
Q_OBJECT
public:
    WindowLevelPresetsTool(QViewer *viewer, QObject *parent = 0);
    ~WindowLevelPresetsTool();

    void handleEvent(unsigned long eventID);

    /// Ens retorna el nombre de presets que té carregats
    int getNumberOfPresets() const;

    void setToolData(ToolData *toolData);

private:
    /// Aplica el window level predeterminat per la tecla donada
    /// @param key Caràcter rebut
    void applyPreset(char key);

private slots:
    /// Actualitza la llista de window/level definits per defecte
    void updateWindowLevelData();

private:
    /// Dades de window level amb tots els presets
    WindowLevelPresetsToolData *m_myToolData;

    /// Llista de presets predefinits
    QStringList m_standardPresets;

    /// Llista de presets per defecte del volum actual
    QStringList m_defaultPresets;

    /// Índex de l'últim preset per defecte escollit
    int m_defaultPresetsIndex;

    /// Aquest mapa ens donarà la relació entre el caràcter premut i l'índex corresponent dels presets
    QMap <char, int> m_characterIndexMap;
};

}

#endif
