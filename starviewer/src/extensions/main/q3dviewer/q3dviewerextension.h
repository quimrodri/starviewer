#ifndef UDGQ3DVIEWEREXTENSION_H
#define UDGQ3DVIEWEREXTENSION_H

#include "ui_q3dviewerextensionbase.h"

#include <QDir>
#include <QMap>

#include "transferfunction.h"

class QAction;
class QModelIndex;
class QStandardItemModel;

namespace udg {

class Q3DViewer;
class Volume;
class ToolManager;

/**
 * ...
*/
class Q3DViewerExtension : public QWidget, private ::Ui::Q3DViewerExtensionBase {
Q_OBJECT
public:
    Q3DViewerExtension(QWidget *parent = 0);
    virtual ~Q3DViewerExtension();

    /// Li donem l'input a processar
    void setInput(Volume *input);

public slots:
    /// Aplica una funció de transferència predefinida a la visualització.
    void applyPresetClut(const QString &clutName);

    /// Aplica la funció de transferència passada a la visualització.
    void applyClut(const TransferFunction &clut, bool preset = false);

    /// Comença a calcular les obscurances, i si ja s'estan calculant ho cancel·la.
    void computeOrCancelObscurance();
    void endComputeObscurance();

    void setScalarRange(double min, double max);

    void updateUiForRenderingMethod(int index);

    /// Actualitza la vista d'acord amb tots els paràmetres actuals de la interfície.
    void updateView(bool fast = true);

signals:
    void newTransferFunction();

private:
    /// Posa a punt les tools que es poden fer servir en l'extensió
    void initializeTools();

    /// Obté la llista de funcions de transferència predefinides.
    void loadClutPresets();

    /// Carrega els estils de rendering predefinits.
    void loadRenderingStyles();

    /// Estableix les connexions de signals i slots
    void createConnections();

    void enableObscuranceRendering(bool on);

    /// Fa que es cridi updateView() quan canvia qualsevol element de la interfície.
    void enableAutoUpdate();
    /// Fa que no es cridi updateView() quan canvia qualsevol element de la interfície.
    void disableAutoUpdate();

private slots:
    void render();
    void autoCancelObscurance();
    void loadClut();
    void saveClut();
    void switchEditor();
    void applyEditorClut();
    void toggleClutEditor();
    void hideClutEditor();
    /// Canvia l'estat del botó del custom style depenent de si s'està mostrant o no l'editor en aquell moment.
    void setCustomStyleButtonStateBySplitter();
    void changeViewerTransferFunction();
    void applyRenderingStyle(const QModelIndex &index);
    void showScreenshotsExporterDialog();

private:
    /// El volum d'input
    Volume *m_input;

    /// Gestor de tools
    ToolManager *m_toolManager;

    /// Directori de funcions de transferència predefinides.
    QDir m_clutsDir;
    /// Mapa entre noms de funcions de transferència i el nom de fitxer corresponent.
    QMap<QString, QString> m_clutNameToFileName;

    /// Última clut aplicada.
    TransferFunction m_currentClut;

    /// Ens indica si en aquests moments s'estan calculant les obscurances.
    bool m_computingObscurance;

    /// Serà cert abans d'entrar el primer input.
    bool m_firstInput;

    /// Model que guarda els estils de rendering.
    QStandardItemModel *m_renderingStyleModel;

    /// Temporitzador que serveix per fer renderings amb qualitat després d'un rendering ràpid.
    QTimer *m_timer;
};

} // end namespace udg

#endif
