#include <QMessageBox>
#include "qdicomaddprinterwidget.h"
#include "starviewerapplication.h"
#include "dicomprinter.h"
#include "dicomprintermanager.h"

namespace udg {

// Public Methods
QDicomAddPrinterWidget::QDicomAddPrinterWidget()
{
    setupUi(this);

    Qt::WindowFlags flags;
    flags = Qt::WindowMinimizeButtonHint;
    setWindowFlags(flags);

    createConnections();
    configureInputValidator();
}

void QDicomAddPrinterWidget::clearInputs()
{
    m_printerAetitleLineEdit->setText("");
    m_printerHostnameLineEdit->setText("");
    m_printerPortLineEdit->setText("");
    m_printerDescriptionLineEdit->setText("");
    m_printerDefaultPrinterCheckBox->setChecked(false);

    m_addButton->setEnabled(false);
}

// Private Methods
void QDicomAddPrinterWidget::createConnections()
{
    connect(m_addButton, SIGNAL(clicked()), SLOT(addPrinter()));
    connect(m_cancelButton, SIGNAL(clicked()), SLOT(close()));
    connect(m_printerAetitleLineEdit, SIGNAL(textChanged(const QString&)), SLOT(enableAddButton()));
    connect(m_printerHostnameLineEdit, SIGNAL(textChanged(const QString&)), SLOT(enableAddButton()));
    connect(m_printerPortLineEdit, SIGNAL(textChanged(const QString&)), SLOT(enableAddButton()));
}

void QDicomAddPrinterWidget::configureInputValidator()
{
    m_printerPortLineEdit->setValidator(new QIntValidator(0, 65535, m_printerPortLineEdit));
}

// Private Slots
void QDicomAddPrinterWidget::enableAddButton()
{
    if (m_printerAetitleLineEdit->text().length() == 0 || m_printerPortLineEdit->text().length() == 0 ||
        m_printerHostnameLineEdit->text().length() == 0)
    {
        m_addButton->setEnabled(false);
    }
    else
    {
        m_addButton->setEnabled(true);
    }
}

void QDicomAddPrinterWidget::addPrinter()
{
    DicomPrinterManager dicomPrinterManager;
    DicomPrinter dicomPrinter = dicomPrinterManager.getDefaultAvailableParametersValuesDICOMPrinters();

    getPrinterSettingsFromControls(dicomPrinter);

    if (!dicomPrinterManager.addPrinter(dicomPrinter))
    {
        QString messageError = tr("Unable to add the printer %1 because a printer with the same AE Title already exists.")
                             .arg(dicomPrinter.getAETitle());
        QMessageBox::critical(this, ApplicationNameString, messageError);
    }
    else
    {
        emit newPrinterAddedSignal(dicomPrinter.getID());
        this->close();
    }
}

void QDicomAddPrinterWidget::getPrinterSettingsFromControls(DicomPrinter &printer)
{
    printer.setAETitle(m_printerAetitleLineEdit->text());
    printer.setHostname(m_printerHostnameLineEdit->text());
    printer.setPort(m_printerPortLineEdit->text().toInt());
    printer.setDescription(m_printerDescriptionLineEdit->text());
    printer.setIsDefault(m_printerDefaultPrinterCheckBox->isChecked());

    // Indiquem un valor de FilmLayout per la impressora perqu� �s un camp "Mandatory" segons DICOM, escollim el primer valor
    // dels disponibles. A part d'aquest camp dels par�metres configurables d'impressi� no n'hi cap m�s d'obligatori
    printer.setDefaultFilmLayout(printer.getAvailableFilmLayoutValues().at(0));
}
}
