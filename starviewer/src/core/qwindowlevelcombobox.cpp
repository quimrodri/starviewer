#include "qwindowlevelcombobox.h"
#include "qcustomwindowleveldialog.h"
#include "logging.h"
#include "windowlevelpresetstooldata.h"
#include "qcustomwindowleveleditwidget.h"
#include "coresettings.h"
#include "windowlevel.h"

namespace udg {

QWindowLevelComboBox::QWindowLevelComboBox(QWidget *parent)
 : QComboBox(parent), m_presetsData(0)
{
    m_customWindowLevelDialog = new QCustomWindowLevelDialog();
    m_currentSelectedPreset = "";
    connect(this, SIGNAL(activated(const QString&)), SLOT(setActiveWindowLevel(const QString&)));
    Settings settings;
    this->setMaxVisibleItems(settings.getValue(CoreSettings::MaximumNumberOfVisibleWindowLevelComboItems).toInt());
}

QWindowLevelComboBox::~QWindowLevelComboBox()
{
    delete m_customWindowLevelDialog;
}

void QWindowLevelComboBox::setPresetsData(WindowLevelPresetsToolData *windowLevelData)
{
    if (m_presetsData)
    {
        // Desconectem tot el que teníem connectat aquí
        disconnect(m_presetsData, 0, this, 0);
        disconnect(m_presetsData, 0, m_customWindowLevelDialog, 0);
        disconnect(m_customWindowLevelDialog, 0, m_presetsData, 0);
    }
    m_presetsData = windowLevelData;
    populateFromPresetsData();
    connect(m_presetsData, SIGNAL(presetAdded(WindowLevel)), SLOT(addPreset(WindowLevel)));
    connect(m_presetsData, SIGNAL(presetRemoved(WindowLevel)), SLOT(removePreset(WindowLevel)));
    connect(m_presetsData, SIGNAL(presetSelected(WindowLevel)), SLOT(selectPreset(WindowLevel)));

    // TODO Això es podria substituir fent que el CustomWindowLevelDialog també contingués les dades
    // de window level i directament li fes un setCustomWindowLevel() a WindowLevelPresetsToolData
    connect(m_customWindowLevelDialog, SIGNAL(windowLevel(double, double)), m_presetsData, SLOT(setCustomWindowLevel(double, double)));
}

void QWindowLevelComboBox::clearPresets()
{
    if (m_presetsData)
    {
        // Desconectem tot el que teníem connectat aquí
        disconnect(m_presetsData, 0, this, 0);
        disconnect(m_presetsData, 0, m_customWindowLevelDialog, 0);
        disconnect(m_customWindowLevelDialog, 0, m_presetsData, 0);
    }
    this->clear();
    m_presetsData = 0;
}

void QWindowLevelComboBox::addPreset(const WindowLevel &preset)
{
    int group;
    if (m_presetsData->getGroup(preset, group))
    {
        int index;
        switch (group)
        {
            case WindowLevelPresetsToolData::AutomaticPreset:
                index = m_presetsData->getPresetsFromGroup(WindowLevelPresetsToolData::AutomaticPreset).count() - 1;
                break;

            case WindowLevelPresetsToolData::FileDefined:
                index = m_presetsData->getPresetsFromGroup(WindowLevelPresetsToolData::AutomaticPreset).count() +
                        m_presetsData->getPresetsFromGroup(WindowLevelPresetsToolData::FileDefined).count() - 1;
                break;

            case WindowLevelPresetsToolData::StandardPresets:
                index = m_presetsData->getPresetsFromGroup(WindowLevelPresetsToolData::AutomaticPreset).count() +
                        m_presetsData->getPresetsFromGroup(WindowLevelPresetsToolData::FileDefined).count() +
                        m_presetsData->getPresetsFromGroup(WindowLevelPresetsToolData::StandardPresets).count() - 1;
                break;

            case WindowLevelPresetsToolData::UserDefined:
                index = m_presetsData->getPresetsFromGroup(WindowLevelPresetsToolData::AutomaticPreset).count() +
                        m_presetsData->getPresetsFromGroup(WindowLevelPresetsToolData::FileDefined).count() +
                        m_presetsData->getPresetsFromGroup(WindowLevelPresetsToolData::StandardPresets).count() +
                        m_presetsData->getPresetsFromGroup(WindowLevelPresetsToolData::UserDefined).count() - 1;
                break;

            case WindowLevelPresetsToolData::Other:
                index = m_presetsData->getPresetsFromGroup(WindowLevelPresetsToolData::AutomaticPreset).count() +
                        m_presetsData->getPresetsFromGroup(WindowLevelPresetsToolData::FileDefined).count() +
                        m_presetsData->getPresetsFromGroup(WindowLevelPresetsToolData::StandardPresets).count() +
                        m_presetsData->getPresetsFromGroup(WindowLevelPresetsToolData::UserDefined).count() +
                        m_presetsData->getPresetsFromGroup(WindowLevelPresetsToolData::Other).count() - 1;
                break;

            case WindowLevelPresetsToolData::CustomPreset:
                index = m_presetsData->getPresetsFromGroup(WindowLevelPresetsToolData::AutomaticPreset).count() +
                        m_presetsData->getPresetsFromGroup(WindowLevelPresetsToolData::FileDefined).count() +
                        m_presetsData->getPresetsFromGroup(WindowLevelPresetsToolData::StandardPresets).count() +
                        m_presetsData->getPresetsFromGroup(WindowLevelPresetsToolData::UserDefined).count() +
                        m_presetsData->getPresetsFromGroup(WindowLevelPresetsToolData::Other).count() +
                        m_presetsData->getPresetsFromGroup(WindowLevelPresetsToolData::CustomPreset).count() - 1;
                break;
        }
        this->insertItem(index, preset.getName());
    }
    else
    {
        DEBUG_LOG("El preset " + preset.getName() + " no està present en les dades de window level proporcionades");
    }

    this->selectPreset(m_currentSelectedPreset);
}

void QWindowLevelComboBox::removePreset(const WindowLevel &preset)
{
    removePreset(preset.getName());
}

void QWindowLevelComboBox::removePreset(const QString &preset)
{
    int index = this->findText(preset);
    if (index > -1)
    {
        this->removeItem(index);
    }
}

void QWindowLevelComboBox::selectPreset(const WindowLevel &preset)
{
    selectPreset(preset.getName());
}

void QWindowLevelComboBox::selectPreset(const QString &preset)
{
    int index = this->findText(preset);
    if (index > -1)
    {
        m_currentSelectedPreset = preset;
        this->setCurrentIndex(index);
    }
    else
    {
        this->setCurrentIndex(this->findText(WindowLevelPresetsToolData::getCustomPresetName()));
    }
}

void QWindowLevelComboBox::populateFromPresetsData()
{
    if (!m_presetsData)
    {
        return;
    }

    this->clear();
    this->addItems(m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::AutomaticPreset));
    this->addItems(m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::FileDefined));
    this->addItems(m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::StandardPresets));
    this->addItems(m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::UserDefined));
    this->addItems(m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::Other));
    this->insertSeparator(this->count());
    this->addItems(m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::CustomPreset));
    this->addItem(tr("Edit Custom WW/WL"));
}

void QWindowLevelComboBox::setActiveWindowLevel(const QString &text)
{
    if (text == WindowLevelPresetsToolData::getCustomPresetName())
    {
        // Reestablim el valor que hi havia perquè no quedi seleccionat la fila de l'editor.
        this->selectPreset(m_currentSelectedPreset);
        WindowLevel preset = m_presetsData->getCurrentPreset();
        m_customWindowLevelDialog->setDefaultWindowLevel(preset.getWidth(), preset.getCenter());
        m_customWindowLevelDialog->exec();
    }
    else if (text == tr("Edit Custom WW/WL"))
    {
        // Reestablim el valor que hi havia perquè no quedi seleccionat la fila de l'editor.
        this->selectPreset(m_currentSelectedPreset);
        WindowLevel preset = m_presetsData->getCurrentPreset();

        QCustomWindowLevelEditWidget customWindowLevelEditWidget;
        customWindowLevelEditWidget.setDefaultWindowLevel(preset);
        customWindowLevelEditWidget.exec();
    }
    else
    {
        m_presetsData->selectCurrentPreset(text);
    }
}

};
