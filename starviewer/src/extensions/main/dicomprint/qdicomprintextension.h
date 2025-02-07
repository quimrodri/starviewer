
#ifndef UDGQDICOMPRINTEXTENSION_H
#define UDGQDICOMPRINTEXTENSION_H

#include "ui_qdicomprintextensionbase.h"
#include "dicomprint.h"

#include <QTimer>
#include <QList>
#include <QPair>

namespace udg {

class QDicomPrinterConfigurationWidget;
class Study;
class Image;
class DicomPrintPage;
class DicomPrintJob;
class Volume;
class ToolManager;
class DICOMPrintPresentationStateImage;

class QDicomPrintExtension : public QWidget, private::Ui::QDicomPrintExtensionBase {
Q_OBJECT

public:
    QDicomPrintExtension(QWidget *parent = 0);

public slots:
    /// Li assigna el volum que s'ha d'imprimir
    void setInput(Volume *input);
    /// Actualitzem tots els elements que estan lligats al Volume que estem visualitzant.
    void updateInput();

private slots:
    // Actualitza la informaci� que mostrem de la impressora
    void selectedDicomPrinterChanged(int indexOfSelectedDicomPrinter);
    /// Omple el combobox amb les impressores i mostra com a seleccionada la que esta marca com a impressora per defecte
    void fillSelectedDicomPrinterComboBox();
    /// Activa/desactiva el fram que cont� poder escollir el interval i des de quina imatge fins quina imprimir en funci� del mode de selecci�
    /// d'imatges escollit
    void imageSelectionModeChanged();
    /// Slot que s'executa quan canviem de valor a trav�s del Slider l'interval d'imatges a imprimir
    void m_intervalImagesSliderValueChanged(int value);
    /// Slot que s'executa quan canviem de valor a trav�s del Slider a partir de quina imatge imprimir
    void m_fromImageSliderValueChanged();
    /// Slot que s'executa quan canviem de valor a trav�s del Slider fins quina imatge imprimir
    void m_toImageSliderValueChanged();
    /// Slot que s'exectua quan el LineEdit per especificar el valor del interval d'imatges a imprimir �s editat, per traspassar el seu valor al Slider
    void m_intervalImagesLineEditTextEdited(const QString &text);
    /// Slot que s'exectua quan el LineEdit per especificar a partir de quina imatges s'haa imprimir �s editat, per traspassar el seu valor al Slider
    void m_fromImageLineEditTextEdited(const QString &text);
    /// Slot que s'exectua quan el LineEdit per especificar fins quina imatge s'ha d'imprimir �s editat, per traspassar el seu valor al Slider
    void m_toImageLineEditTextEdited(const QString &text);
    /// Actualitza label indicant el n�mero de p�gines dicom print que s'imprimiran
    void updateNumberOfDicomPrintPagesToPrint();

    /// Envia a imprimir les imatges seleccionades a la impressora seleccionada
    void print();

    /// Amaga el frame que indica que s'han enviat a imprimir correctament les imatges
    void timeoutTimer();

    /// Afegeix les imatges selccionades com a grup d'imatges per imprimir i les mostra al ThumbnailsPreview
    void addSelectedImagesToGroupedDICOMImagesToPrint();

    /// Esborra els grups d'imatges seleccionades per imprimir
    void clearDICOMImagesGroupedToPrint();

    /// Esborra el grups d'imatges per imprimir seleccionades en el control ThumbnailsPreview
    void removeGroupedDICOMImagesToPrintSelectedInThumbnailsPreview();

private:
    /// Crea les connexions
    void createConnections();

    /// Crea inputValidators pels lineEdit de la selecci� d'imatges
    void configureInputValidator();

    /// Inicialitza les tools que volem tenir activades al viewer
    void initializeViewerTools();

    /// Reinicia i configura els controls de selecci� d'imatges en funci� dels nombre d'imatges
    void resetAndUpdateSelectionImagesValue();

    /// Retorna el DicomPrintJob que s'ha d'enviar a imprimir en funci� de la impressora i imatges seleccionades
    DicomPrintJob getDicomPrintJobToPrint();

    /// Retorna una llista de p�gines per imprimir
    QList<DicomPrintPage> getDicomPrintPageListToPrint();

    /// Retorna les imatges s'han d'enviar a imprimir en funci� de lo definit a la selecci� d'imatges
    QList<Image*> getSelectedImagesToAddToPrint() const;

    /// Retorna la impressora seleccionada
    DicomPrinter getSelectedDicomPrinter() const;

    /// Indica amb la selecci� actua el n�mero de p�gines a imprimir, sin� l'ha pogut calcu�lar correctament retorna 0
    int getNumberOfPagesToPrint() const;

    /// Ens retorna un DicomPrintPage amb els par�metres d'impressi� omplerts a partir d'una impressora. No afegeix les imatges ni n�mero de p�gina
    DicomPrintPage fillDicomPrintPagePrintSettings(const DicomPrinter &dicomPrinter) const;

    /// Mostra per pantalla els errors que s'han produ�t alhora d'imprimir
    /// Degut a nom�s podem tenir una p�gina per FilmSession degut a limitacions de dcmtk fa que haguem d'imprimir p�gina per p�gina
    /// per aix� ens podem trobar que la primera p�gina s'imprimeixi b�, i les restants no, per aix� passem el par�metre printedSomePage per indica que nom�s
    /// algunes de les p�gines han fallat
    void showDicomPrintError(DicomPrint::DicomPrintError error, bool printedSomePage);

    /// Comprova si se suporta el format de la s�rie i actualitza la interf�cie segons convingui.
    void updateVolumeSupport();

    /// Activa/desactiva els controls de configuraci� d'impressi� i el bot� d'impressi�
    void setEnabledPrintControls(bool enabled);

    ///Activa/desactiva els controls per afegir imatges a imprimir
    void setEnabledAddImagesToPrintControls(bool enabled);

    /// Ens afegeix anotacions a la p�gina a imprimir amb informaci� de l'estudi, com nom de pacient,
    /// Data i hora estudi, Instituci�, ID Estudi i descripci�,...
    void addAnnotationsToDicomPrintPage(DicomPrintPage *dicomPrintPage, Image *imageToPrint);

    /// Retorna la descripcio pel thumbnail que mostra un rang d'imatges seleccionades per imprimir
    QString getThumbnailPreviewDescriptionOfSelectedGroupedDICOMImagesToPrint() const;

    /// Treu del grups d'imatges per imprimir el grup d'imatges amb l'ID passat per parametre
    void removeGroupedDICOMImagesToPrint(int IDGroup);

    /// Retorna les imatges a imprimir
    QList<QPair<Image*, DICOMPrintPresentationStateImage> > getImagesToPrint() const;

    /// Retorna el PresentationState a aplicar a les imatges seleccionades per imprimir
    DICOMPrintPresentationStateImage getDICOMPrintPresentationStateImageForCurrentSelectedImages() const;

private:
    /// Contenidor d'objectes associats a l'estudi que serveix per facilitar la intercomunicaci�
    struct GroupedDICOMImagesToPrint
    {
        QList<Image*> imagesToPrint;
        DICOMPrintPresentationStateImage dicomPrintPresentationStateImage;
        int ID;
    };

    QList<GroupedDICOMImagesToPrint> m_groupedDICOMImagesToPrintList;
    int m_lastIDGroupedDICOMImagesToPrint;

    /// Gestor de tools pel viewer
    ToolManager *m_toolManager;

    QTimer *m_qTimer;

    QDicomPrinterConfigurationWidget *m_qDicomPrinterConfigurationWidgetProof;
};

} // end namespace udg.

#endif
