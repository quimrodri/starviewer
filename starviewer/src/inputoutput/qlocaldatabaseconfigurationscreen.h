#ifndef UDGQLOCALDATABASECONFIGURATIONSCREEN_H
#define UDGQLOCALDATABASECONFIGURATIONSCREEN_H

#include "ui_qlocaldatabaseconfigurationscreenbase.h"

namespace udg {

/**
    Widget en el que es configuren els paràmetres de la Base de Dades local
  */
class QLocalDatabaseConfigurationScreen : public QWidget, private Ui::QLocalDatabaseConfigurationScreenBase {
Q_OBJECT

public:
    /// Constructor de la classe
    QLocalDatabaseConfigurationScreen(QWidget *parent = 0);

    /// Destructor de classe
    ~QLocalDatabaseConfigurationScreen();

private slots:
    /// Mostra un QDialog per especificar on es troba la base de dades de la caché
    void examinateDataBaseRoot();

    /// Mostra un QDialog per especificar on s'han de guardar les imatges descarregades
    void examinateCacheImagePath();

    /// Esborra tota la caché
    void deleteStudies();

    /// Neteja la cache. Esborra tots els estudis de la caché i reinstal·la la base de dades
    void clearCache();

    /// Compacta la base de dades de la cache
    void compactCache();

    /// Update settings slots
    void checkAndUpdateDatabasePathSetting(const QString &text);
    void checkAndUpdateCachePathSetting(const QString &text);
    void updateMinimumSpaceRequiredSetting(const QString &text);
    void updateDeleteOldStudiesSetting(bool enable);
    void updateMaximumDaysNotViewedSetting(const QString &text);
    void updateDeleteStudiesWhenNoFreeSpaceSetting(bool enable);
    void updateDiskSpaceToFreeSetting(const QString &text);

private:
    /// Crea els connects dels signals i slots
    void createConnections();

    /// Crea els input validators necessaris pels diferents camps d'edició.
    void configureInputValidator();

    /// Carrega les dades de configuració de la cache
    void loadCacheDefaults();

    /// Validation of settings values methods
    bool validateDatabaseApplicationPath(const QString &text);
    bool validateDICOMFilesCachePath(const QString &text);

    /// Afegeix la '/' al final del path del directori si l'usuari no l'ha escrit
    void cacheImagePathEditingFinish();
private:
};

};// end namespace udg

#endif
