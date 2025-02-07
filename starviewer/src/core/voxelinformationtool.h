#ifndef UDGVOXELINFORMATIONTOOL_H
#define UDGVOXELINFORMATIONTOOL_H

#include "tool.h"
#include <QPointer>

namespace udg {

class QViewer;
class Q2DViewer;
class DrawerText;
class Volume;

/**
    Tool per mostrar la informació del voxel (coordenades i valor) en un visualitzador 2D
  */
class VoxelInformationTool : public Tool {
Q_OBJECT
public:
    VoxelInformationTool(QViewer *viewer, QObject *parent = 0);
    ~VoxelInformationTool();

    void handleEvent(unsigned long eventID);

private slots:
    /// Actualitza la informació de voxel que ha de mostrar el caption
    /// d'acord amb la posició on estigui el cursor
    void updateCaption();

    /// Crear l'actor on es mostrarà la informació del voxel
    void createCaption();

private:
    /// Returns the voxel value in a properly formatted string corresponding to the given worldCoordinate on the i-th viewer's input
    QString computeVoxelValueOnInput(double worldCoordinate[3], int i);

    /// Calcula quin és el punt on col·locarem el caption i la justificació del texte corresponent
    /// segons la posició en la que es trobi el punter del mouse.
    void computeCaptionAttachmentPointAndTextAlignment(double attachmentPoint[3], QString &horizontalJustification, QString &verticalJustification);

private:
    /// 2DViewer amb el que operem
    Q2DViewer *m_2DViewer;

    /// El texte per mostrar les annotacions de voxel
    QPointer<DrawerText> m_caption;
};

}

#endif
