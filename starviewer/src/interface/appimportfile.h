#ifndef UDGAPPIMPORTFILE_H
#define UDGAPPIMPORTFILE_H

#include <QObject>
#include <QStringList>

namespace udg {

/**
    Mini-aplicació encarregada de permetre carregar un model des del sistema de fitxers al repositori de volums
  */
class AppImportFile : public QObject {
Q_OBJECT
public:
    AppImportFile(QObject *parent = 0);
    ~AppImportFile();

    /// Obre el diàleg per poder obrir arxius
    void open();

    /// Obre el diàleg per poder obrir arxius d'un directori DICOM
    /// Per defecte explora els continguts de forma recursiva, cercant en els subdirectoris
    void openDirectory(bool recursively = true);

signals:
    /// Senyal que s'emet quan s'han escollit un o més arxius que seran processats externament
    void selectedFiles(QStringList);

private:
    /// Ens retorna la llista d'arxius DICOM agrupables en series d'un directori
    /// @param directory Nom del directori on es troben els arxius
    /// @return La llista de noms de fitxers ordenada
    QStringList generateFilenames(const QString &dirPath);

    /// Donat un path arrel, escaneja tots els subdirectoris recursivament i ens els retorna en una llista incloent el path arrel
    /// Tots els paths retornats, són absoluts
    void scanDirectories(const QString &rootPath, QStringList &dirsList);

    /// Llegeix escriu configuracions
    void readSettings();
    void writeSettings();

private:
    /// Directori de treball per fitxers ordinaris
    QString m_workingDirectory;

    /// Directori de treball per directoris dicom
    QString m_workingDicomDirectory;

    /// Última Extenció seleccionada per l'usuari
    QString m_lastExtension;
};

};  // end namespace udg

#endif
