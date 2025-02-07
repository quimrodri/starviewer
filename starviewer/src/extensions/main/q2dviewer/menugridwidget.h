#ifndef UDGMENUGRIDWIDGET_H
#define UDGMENUGRIDWIDGET_H

#include <QWidget>

// FWD declarations
class QWidget;
class QGridLayout;
class QGroupBox;
class QLabel;

namespace udg {

// FWD declarations
class ItemMenu;
class HangingProtocol;

/**
    Classe que representa el menu desplegable per seleccionar el grid, amb opcions de grids predefinides.
  */
class MenuGridWidget : public QWidget {
Q_OBJECT
public:
    MenuGridWidget(QWidget *parent = 0);
    ~MenuGridWidget();

    /// Elimina tot el contingut del widget
    void dropContent();

    /// Afegeix hanging protocols al menú
    void addHangingItems(const QList<HangingProtocol*> &items);

    /// Posa una element que informa que s'estan carregant
    void addSearchingItem();

    /// Informa de si ha de posar un element que informi que s'està carregant o no
    void setSearchingItem(bool state);

public slots:
    /// Posa els hanging protocols que ha de representar el menú
    void setHangingItems(const QList<HangingProtocol*> &listOfCandidates);
    
    /// Mètode que cada vegada que es seleccioni un dels items emet el grid resultat
    void emitSelected(ItemMenu *selected);

signals:
    /// Emet que s'ha escollit un grid
    void selectedGrid(int);

protected:
    /// Mètode que crea una icona segons un hanging protocol
    ItemMenu* createIcon(const HangingProtocol *hangingProtocol);

    /// Crea el widget que conté una animació i un label que es mostrarà quan estem buscant estudis previs
    /// per trobar hanging protocols potencials a aplicar sobre l'estudi
    void createSearchingWidget();

private:
    /// Inicialitza el widget i el deixa apunt per afegir-hi hanging protocols.
    void initializeWidget();

protected:
    /// Nombre de columnes a mostrar
    static const int MaximumNumberOfColumns;

    /// Grid per mostrar els elements predefinits
    QGridLayout *m_gridLayout;

    /// Llista dels items
    QList<ItemMenu*> m_itemList;

    /// Fila a on col·locar el següent element a la zona de hangings
    int m_nextHangingProtocolRow;

    /// Columna a on col·locar el següent element a la zona de hangings
    int m_nextHangingProtocolColumn;

    /// Distribució regular a la zona dels hanging protocols
    QGridLayout *m_gridLayoutHanging;

    /// Indicador de si cal posar l'element de carregant o no
    bool m_putLoadingItem;

    /// Columna a on s'ha col·locat l'element de carregant
    int m_loadingColumn;

    /// Fila a on s'ha col·locat l'element de carregant
    int m_loadingRow;

    /// Widget que informa que s'esta carregant (buscant)
    QWidget *m_searchingWidget;

    /// Etiqueta que posarem com a títol del widget
    QLabel *m_caption;

    /// Etiqueta per mostrar quan no hi ha cap hanging protocol disponible per aplicar
    QLabel *m_noHangingProtocolsAvailableLabel;
};

}

#endif
