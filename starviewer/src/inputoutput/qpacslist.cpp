/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qpacslist.h"

#include <QTreeView>
#include <QList>

#include "pacsdevicemanager.h"
#include "pacsdevice.h"

namespace udg {

QPacsList::QPacsList( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );

    m_PacsTreeView->setColumnHidden(0, true); //la columna PacsId està amagada
    m_PacsTreeView->setColumnHidden(4, true); //la columna PACSAddress està amagada

    refresh();

    // Cada cop que cliquem sobre un item el marcarem com a PACS defecte segons si queda seleccionat o no
    connect( m_PacsTreeView, SIGNAL(itemClicked(QTreeWidgetItem *,int)), SLOT(setDefaultPACS(QTreeWidgetItem *)) );
}

QPacsList::~QPacsList()
{
}

void QPacsList::refresh()
{
    PacsDeviceManager pacsDeviceManager;
    QList<PacsDevice> pacsList;

    m_PacsTreeView->clear();

    pacsList = pacsDeviceManager.getPACSList();

    foreach(PacsDevice pacs, pacsList)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem( m_PacsTreeView );

        item->setText(0, pacs.getID());
        item->setText(1, pacs.getAETitle());
        item->setText(2, pacs.getInstitution());
        item->setText(3, pacs.getDescription());
        item->setText(4, pacs.getAddress());

        item->setSelected(pacs.isDefault());
    }
}

QList<PacsDevice> QPacsList::getSelectedPacs()
{
    PacsDeviceManager pacsDeviceManager;
    QList<PacsDevice> selectedPacsList;
    QList< QTreeWidgetItem * > qPacsList( m_PacsTreeView->selectedItems() );

    QTreeWidgetItem *item;
    for ( int i = 0; i < qPacsList.count(); i++ )
    {
        PacsDevice pacs;
        item = qPacsList.at( i );
        pacs = pacsDeviceManager.getPACSDeviceByID(item->text(0)); //fem el query per cercar la informació del PACS
        selectedPacsList.append( pacs ); //inserim a la llista
    }

    return selectedPacsList;
}

void QPacsList::setDefaultPACS(QTreeWidgetItem *item)
{
    Q_ASSERT(item);

    PacsDeviceManager pacsDeviceManager;

    PacsDevice pacs;
    pacs = pacsDeviceManager.getPACSDeviceByID(item->text(0));
    pacs.setDefault( item->isSelected() );
}

};
