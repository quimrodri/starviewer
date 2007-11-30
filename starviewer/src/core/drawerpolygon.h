/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDRAWERPOLYGON_H
#define UDGDRAWERPOLYGON_H

#include "drawerprimitive.h"

class vtkPolyData;
class vtkPoints;
class vtkCellArray;
class vtkActor2D;
class vtkPolyDataMapper2D;

namespace udg {

/**
Primitiva de polígon per al Drawer

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DrawerPolygon : public DrawerPrimitive
{
Q_OBJECT
public:
    DrawerPolygon(QObject *parent = 0);

    ~DrawerPolygon();

    /**
     * Afegim un vèrtex al polígon.
     * @param point[] Punt que defineix el vèrtex del polígon
     */
    void addVertix( double point[3] );

    vtkProp *getAsVtkProp();

protected slots:
    void updateVtkProp();

private:
    /// Mètode intern per passar de la llista de punts a l'estructura vtk pertinent
    void buildVtkPoints();

    /// Mètode intern per transformar les propietats de la primitiva a propietats de vtk
    void updateVtkActorProperties();

private:
    /// Llista de punts del polígon
    QList< QVector<double> > m_pointsList;

    /// Estructures de vtk, per construir el polígon
    vtkPolyData *m_vtkPolydata;
    vtkPoints *m_vtkPoints;
    vtkCellArray *m_vtkCellArray;
    vtkActor2D *m_vtkActor;
    vtkPolyDataMapper2D *m_vtkMapper;
};

}

#endif
