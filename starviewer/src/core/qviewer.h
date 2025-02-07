#ifndef UDGQVIEWER_H
#define UDGQVIEWER_H

#include "orthogonalplane.h"
#include "anatomicalplane.h"

#include <QWidget>
// Llista de captures de pantalla
#include <QList>
#include <vtkImageData.h>

// Fordward declarations
class QStackedLayout;
class QVTKWidget;
class vtkCamera;
class vtkRenderer;
class vtkRenderWindow;
class vtkRenderWindowInteractor;
class vtkWindowToImageFilter;
class vtkEventQtSlotConnect;

namespace udg {

// Fordward declarations
class Volume;
class Series;
class Image;
class ToolProxy;
class WindowLevelPresetsToolData;
class PatientBrowserMenu;
class QViewerWorkInProgressWidget;
class WindowLevel;

/**
    Classe base per a totes les finestres de visualització
  */
class QViewer : public QWidget {
Q_OBJECT
public:
    QViewer(QWidget *parent = 0);
    ~QViewer();

    /// Tipus de fitxer que pot desar
    enum FileType { PNG, JPEG, TIFF, DICOM, PNM, META, BMP };

    /// Tipus de format de gravació de vídeo suportats
    enum RecordFileFormatType { MPEG2 };

    /// Estat del viewer
    enum ViewerStatus { NoVolumeInput, DownloadingVolume, DownloadingError, LoadingVolume, LoadingError, VisualizingVolume, VisualizingError };

    /// Retorna l'interactor renderer
    virtual vtkRenderWindowInteractor* getInteractor() const;

    /// Retorna el renderer
    virtual vtkRenderer* getRenderer() const;

    /// Returns the main input
    virtual Volume* getMainInput() const;

    /// Returns the total number of inputs of the viewer
    virtual int getNumberOfInputs() const;
    
    /// Returns true if viewer has some input set, false otherwise
    bool hasInput() const;
    
    /// Retorna el vtkRenderWindow
    vtkRenderWindow* getRenderWindow() const;

    /// Retorna la mida en coordenades de pantalla (píxels?) de tot el viewport del viewer
    /// amaga la crida vtkRenderWindow::GetSize()
    QSize getRenderWindowSize() const;

    /// Mètodes per obtenir l'actual i/o l'anterior coordenada del cursor
    /// del ratolí durant l'últim event
    /// Les coordenades retornades estan en sistema de coordenades de Display
    QPoint getEventPosition() const;
    QPoint getLastEventPosition() const;

    /// Ens diu si el viewer és actiu en aquell moment
    /// @return Cert si actiu, fals altrament
    bool isActive() const;

    /// Ens retorna el ToolProxy del viewer
    ToolProxy* getToolProxy() const;

    /// Passa coordenades de display a coordenades de món i viceversa
    void computeDisplayToWorld(double x, double y, double z, double worldPoint[3]);
    void computeWorldToDisplay(double x, double y, double z, double displayPoint[3]);

    /// Ens dóna la coordenada de món de l'últim (o previ a aquest) event capturat
    void getEventWorldCoordinate(double worldCoordinate[3]);
    void getLastEventWorldCoordinate(double worldCoordinate[3]);

    /// Fa una captura de la vista actual i la guarda en una estructura interna
    void grabCurrentView();

    /// Desa la llista de captures en un arxiu de diversos tipus amb el nom de fitxer base \c baseName i en format especificat per \c extension.
    /// Retorna TRUE si hi havia imatges per guardar, FALSE altrament
    bool saveGrabbedViews(const QString &baseName, FileType extension);

    /// Retorna el nombre de vistes capturades que estan desades
    int grabbedViewsCount()
    {
        return m_grabList.size();
    }

    /// Grava en format de vídeo els frames que s'hagin capturat amb grabCurrentView.
    /// Un cop gravat, esborra la llista de frames.
    /// TODO de moment només accepta format MPEG
    bool record(const QString &baseName, RecordFileFormatType format = MPEG2);

    /// Fa zoom sobre l'escena amb el factor donat
    /// @param factor Factor de zoom que volem aplicar a la càmera
    void zoom(double factor);

    /// Absolute zoom to the scene based on the factor value
    void absoluteZoom(double factor);

    /// Desplaça la càmera segons el vector de moviment que li passem
    /// @param motionVector[] Vector de moviment que determina cap on i quant es mou la càmera
    void pan(double motionVector[3]);

    /// Fem un zoom del requadre definit pels paràmetres topCorner i bottomCorner en coordenades de món
    /// perquè s'ajusti a la mida de la finestra.
    /// La determinació sobre quina coordenada es considera "superior" o "inferior" és relativa segons el punt de vista,
    /// el realment important és que aquestes coordenades siguin dos vèrtexs oposats dins del requadre que es vol definir
    /// @param topCorner Coordenada superior
    /// @param bottomCorner Coordenada inferior oposada a la superior
    /// @param marginRate valor en percentatge del marge que es vol deixar
    /// @return True if scale could be performed, false otherwise
    bool scaleToFit3D(double topCorner[3], double bottomCorner[3], double marginRate = 0.0);

    /// Ens retorna l'objecte que conté tota la informació referent al window level
    /// que es pot aplicar sobre aquest visor
    /// @return L'objecte WindowLevelPresetsToolData
    WindowLevelPresetsToolData* getWindowLevelData() const;

    /// Li assignem el window level data externament
    /// @param windowLevelData
    void setWindowLevelData(WindowLevelPresetsToolData *windowLevelData);

    /// Habilita/deshabilita que els renderings es facin efectius
    /// Útil en els casos en que necessitem fer diverses operacions de
    /// visualització però no volem que aquestes es facin efectives fins que no ho indiquem
    void enableRendering(bool enable);

    /// Ens retorna el menú de pacient amb el que s'escull l'input
    PatientBrowserMenu* getPatientBrowserMenu() const;

    /// Permet especificar si al seleccionar un input del menú de pacient aquest es carregarà automàticament
    /// o, pel contrari, no es farà res. Per defecte està activat i es carregarà l'input al visor.
    void setAutomaticallyLoadPatientBrowserMenuSelectedInput(bool load);

    /// Retorna l'status del viewer. Útil per saber si el visor està visualitzant dades
    /// o està carregant...
    ViewerStatus getViewerStatus() const;
    ViewerStatus getPreviousViewerStatus() const;

    /// Canvia l'status del viewer
    void setViewerStatus(ViewerStatus status);

    /// Returns a unique string identifier of all inputs
    virtual QString getInputIdentifier() const;

    /// Returns the current Zoom Factor
    double getCurrentZoomFactor();

    /// Returns the current focal point of the active camera
    bool getCurrentFocalPoint(double focalPoint[3]);

public slots:
    /// Indiquem les dades d'entrada
    virtual void setInput(Volume *volume) = 0;

    /// Gestiona els events que rep de la finestra
    void eventHandler(vtkObject *object, unsigned long vtkEvent, void *clientData, void *callData, vtkCommand *command);

    /// Força l'execució de la visualització
    void render();

    /// Assignem si aquest visualitzador és actiu, és a dir, amb el que s'està interactuant
    /// @param active
    void setActive(bool active);

    /// Elimina totes les captures de pantalla
    void clearGrabbedViews();

    /// Obté el window level actual de la imatge
    virtual void getCurrentWindowLevel(double wl[2]) = 0;

    /// Resets the view to a determined orthogonal plane
    virtual void resetView(const OrthogonalPlane &view);
    
    /// Resets the view to the specified anatomical plane
    void resetView(AnatomicalPlane::AnatomicalPlaneType anatomicalPlane);
    virtual void resetViewToAxial();
    virtual void resetViewToSagital();
    virtual void resetViewToCoronal();
    
    /// Resets the view to the original acquisition plane of the input volume
    void resetViewToAcquisitionPlane();

    /// Activa o desactiva el menú de contexte
    void enableContextMenu();
    void disableContextMenu();

    /// Ajusta el window/level
    virtual void setWindowLevel(double window, double level) = 0;
    /// Ajusta el window/level a partir del preset. La implementació per defecte acaba cridant setWindowLevel sempre.
    virtual void setWindowLevelPreset(const WindowLevel &preset);

    /// Fits the current rendered item into the viewport size
    void fitRenderingIntoViewport();

signals:
    /// Informem de l'event rebut. \TODO ara enviem el codi en vtkCommand, però podria (o hauria de) canviar per un mapeig nostre
    void eventReceived(unsigned long eventID);

    /// Signal que s'emet quan s'escull una altra serie per l'input
    void volumeChanged(Volume *volume);

    /// S'emet quan els paràmetres de la càmera han canviat
    void cameraChanged();

    /// Informa que el visualitzador ha rebut un event que es considera com que aquest s'ha seleccionat
    void selected(void);

    /// Informa que s'ha canviat el zoom
    void zoomFactorChanged(double factor);

    /// Informa que s'ha mogut la imatge
    void panChanged(double *translation);

    /// Indica que l'estat del visor ha canviat
    void viewerStatusChanged();

    /// Signal emitted when the anatomical view has changed
    void anatomicalViewChanged(AnatomicalPlane::AnatomicalPlaneType anatomicalPlane);

protected:
    /// Gets the bounds of the rendered item
    virtual void getCurrentRenderedItemBounds(double bounds[6]) = 0;

    virtual void contextMenuEvent(QContextMenuEvent *menuEvent);

    void contextMenuRelease();

    /// Actualitza les dades contingudes a m_windowLevelData
    void updateWindowLevelData();

    /// Fixem la orientació de la càmera del renderer principal
    /// Si el paràmetre donat no és un dels valors enumerats vàlids, no farà res
    /// @param orientation Orientació
    void setCameraOrientation(const OrthogonalPlane &orientation);

    /// Configura la orientació de la càmera donada
    void setCameraViewPlane(const OrthogonalPlane &viewPlane);

    /// Adjusts camera scale factor. Returns false if no scale could be performed, true otherwise.
    /// To be used by zoom(), scaleToFit3D() methods and the so
    bool adjustCameraScaleFactor(double factor);
    
    /// Sets the default rendered item orientation for the given anatomical plane
    virtual void setDefaultOrientation(AnatomicalPlane::AnatomicalPlaneType anatomicalPlane);
    
    /// Ens retorna la càmera activa pel renderer principal, si n'hi ha, NUL altrament.
    vtkCamera* getActiveCamera();

    /// Ens dóna la coordenada de món de l'últim (o previ a aquest) event capturat
    /// @param worldCoordinate Variable on es retornarà la coordenada
    /// @param current Si true, ens dóna la coordenada de l'event més recent,
    /// si fals, ens dóna la coordenada anterior a l'event més recent
    void getRecentEventWorldCoordinate(double worldCoordinate[3], bool current);

    virtual void setupInteraction();

    /// Returns the current view plane.
    virtual OrthogonalPlane getCurrentViewPlane() const;

    /// Sets the current view plane.
    virtual void setCurrentViewPlane(const OrthogonalPlane &viewPlane);

    /// Handles errors produced by lack of memory space for visualization.
    void handleNotEnoughMemoryForVisualizationError();

private slots:
    /// Slot que s'utilitza quan s'ha seleccionat una sèrie amb el PatientBrowserMenu
    /// Mètode que especifica un input seguit d'una crida al mètode render()
    /// TODO: Convertit en virtual per tal de poder ser reimplementat per Q2DViewer per càrrega asíncrona
    virtual void setInputAndRender(Volume *volume);

private:
    /// Actualitza quin és el widget actual que es mostra per pantalla a partir de l'estat del viewer
    void setCurrentWidgetByViewerStatus(ViewerStatus status);

    /// Inicialitza el widget QWorkInProgress a partir de l'status
    void initializeWorkInProgressByViewerStatus(ViewerStatus status);

    /// Creates and configures the render window with the desired features.
    void setupRenderWindow();

protected:
    /// El volum a visualitzar
    Volume *m_mainVolume;

    /// El widget per poder mostrar una finestra vtk amb qt
    QVTKWidget *m_vtkWidget;

    /// La llista de captures de pantalla
    QList<vtkImageData*> m_grabList;

    /// Filtre per connectar el que es visualitza pel renderer en un pipeline, epr guardar les imatges en un arxiu, per exemple
    vtkWindowToImageFilter *m_windowToImageFilter;

    /// Connector d'events vtk i slots qt
    vtkEventQtSlotConnect *m_vtkQtConnections;

    /// Renderer per defecte
    vtkRenderer *m_renderer;

    /// Indica si el menú de contexte està actiu o no. Per defecte sempre està actiu
    bool m_contextMenuActive;

    /// ToolProxy
    ToolProxy *m_toolProxy;

    /// Ens servirà per controlar si entre event o event s'ha mogut el mouse
    bool m_mouseHasMoved;

    /// Dades de valors predeterminats de window level i dels valors actuals que s'apliquen
    WindowLevelPresetsToolData *m_windowLevelData;

    /// Indica si hem de fer l'acció de renderitzar o no
    bool m_isRenderingEnabled;

    /// Menú de pacient a través del qual podem escollir l'input del viewer
    PatientBrowserMenu *m_patientBrowserMenu;

    /// Widget que es mostra quan s'està realitzant algun treball asíncron
    QViewerWorkInProgressWidget *m_workInProgressWidget;

    /// The default margin for fit into viewport. Should be between 0..1.
    double m_defaultFitIntoViewportMarginRate;

private:
    /// Current view plane: plane that is perpendicular to the camera pointing direction.
    OrthogonalPlane m_currentViewPlane;

    /// Indica si el viewer és actiu o no
    bool m_isActive;

    /// Estats actual i previ del visor actual
    ViewerStatus m_viewerStatus;
    ViewerStatus m_previousViewerStatus;

    /// Layout que ens permet crear widgets diferents per els estats diferents del visor.
    QStackedLayout *m_stackedLayout;
};

};  // End namespace udg

#endif
