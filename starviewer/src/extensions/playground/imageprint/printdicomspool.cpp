#include "printdicomspool.h"

#include "dcmtk/dcmimgle/dcmimage.h" //classe DicomImage
#include "dcmtk/dcmpstat/dvpspr.h" //classe DVPSPrintEventHandler
#include "dcmtk/dcmpstat/dvpssp.h" //classe DVPSStoredPrint
#include "dcmtk/dcmpstat/dvpshlp.h" //classe DVPSHelper

#include <QDir>

#include "dicomprintpage.h"
#include "logging.h"
#include "../inputoutput/pacsdevice.h"

namespace udg{

void PrintDicomSpool::printBasicGrayscale(DicomPrinter dicomPrinter, DicomPrintJob dicomPrintJob, const QString &storedPrintDcmtkFilePath, const QString &spoolDirectoryPath)
{
    DVPSPrintMessageHandler printerConnection;
    OFCondition result;
    bool printerSupportsPresentationLUTSOPClass = false, printerSupportsAnnotationSOPClass = false, transferSyntaxImplicit = false;

    m_dicomPrinter = dicomPrinter;
    m_dicomPrintJob = dicomPrintJob;

    /*Connectem amb la impressora
        1r Par�metre �s de tipus objecte tlayer nom�s s'utilitza si la comunicaci� es fa amb ssl
        2n AETitle del Starviewer 3r AETitle de la impressora Dicom
        4t Hostname de la impressora dicom 5� Port pel qual ens hem de comunicar amb la impressora
        6� Tamany m�xim de la pdu per la comunicaci�, li donem el valor per defecte del els dcmtk
        7� Indiquem si la impressora suporta Presentation LUT SOP Class, com que de moment nosaltres no ho soportem li indiquem fals
        8� Indiquem si la impressora suporta Basic Annotation Box SOP Class, com que de moment nosaltres no ho soportem li indiquem fals
        9� Indica si la comunicaci� s'ha de fer amb transfer syntax Implicit, aix� �s degut a que dispositius vells que suportaven Dicom 
           tot i que indicar en el seu conformance que suportaven transfer syntax explicit, alhora de la veritat tenien problemes, per aix�
           existeix un par�metre per indicar nom�s de comunicar-se amb Implicit, com que de moment no ens hem de comunicar amb dispositius vells
           i tots els moderns suporten Explicit indiquem false
     */
    result = printerConnection.negotiateAssociation(NULL, qPrintable(PacsDevice::getLocalAETitle()), qPrintable(m_dicomPrinter.getAETitle()), 
                                                    qPrintable(m_dicomPrinter.getHostname()), m_dicomPrinter.getPort(), ASC_DEFAULTMAXPDU, 
                                                    printerSupportsPresentationLUTSOPClass, printerSupportsAnnotationSOPClass, transferSyntaxImplicit);

    if (result.bad())
    {
        ERROR_LOG(QString("No s'ha pogut connectar amb la impressora amb AETitle: %1, IP: %2, port: %3, descripci� error:%4 ").arg(m_dicomPrinter.getAETitle(), 
                           m_dicomPrinter.getHostname(), QString().setNum(m_dicomPrinter.getPort()), result.text()));
        return;
    }

    printStoredPrintDcmtkContent(printerConnection, storedPrintDcmtkFilePath, spoolDirectoryPath);

    result = printerConnection.releaseAssociation();
    if (result.bad())
    {
        DEBUG_LOG(QString("spooler: release of connection to printer failed. %1").arg(result.text()));
    }

}

void PrintDicomSpool::printStoredPrintDcmtkContent(DVPSPrintMessageHandler &printerConnection, const QString storedPrintDcmtkFilePath, const QString &spoolDirectoryPath)
{
    OFCondition result;
    DVPSStoredPrint	*storedPrintDcmtk = loadStoredPrintFileDcmtk(storedPrintDcmtkFilePath);
    /*Aquesta variable indica si els Attibutes de PLUT que s�n Illumination (2010,015E) i (2010,0160) s'han d'afegir al BasicFilmSession o al BasicFilmBox
     *Nosaltres indiquem que els afegim al BasicFilmBox perqu� si mirem la documentaci� DICOM, tot i que dcmtk ho suporta aquests dos tags no apareixen com 
     *a atributs possibles d'un BasicFilmBox.
     *De totes maneres si mirem el codi de les dcmtk on passem aquesta variable, aquesta no t� cap afecte, perqu� com no soportem les PresentatioLUTSOP el valor
     *que cont� la variable s'ignora i aquestes dos tags no s'envien*/
    bool addPLUTAttributesInBasicFilmSession = false;

    if (!storedPrintDcmtk) return;

    /*TODO: �s necessari ?, simplement demana a la impressora informaci� de certs par�metres, t� algun sentit ? Preguntar al f�rum,
      Enganxar codi dvpssp.cxx l�nia 1342 ensenyant que no fa res*/
    result = storedPrintDcmtk->printSCUgetPrinterInstance(printerConnection);
    if (EC_Normal != result)
    {
        DEBUG_LOG(QString("spooler: printer communication failed, unable to request printer settings. %1").arg(result.text()));
    }

    //Abans de fer un acci� amb la impressora comprovem si la �ltima ha anat b� amb EC_NORMAL=result, si ha fallat totes les accions restants no s'executaran
    if (EC_Normal == result)
    {
        result = result = storedPrintDcmtk->printSCUcreateBasicFilmSession(printerConnection, getAttributesBasicFilmSession(), addPLUTAttributesInBasicFilmSession);
        if (EC_Normal != result)
        {
            DEBUG_LOG(QString("spooler: printer communication failed, unable to create basic film session. %1").arg(result.text()));
        }
    }

    if (EC_Normal == result)
    {
        result = storedPrintDcmtk->printSCUcreateBasicFilmBox(printerConnection, addPLUTAttributesInBasicFilmSession);
        if (EC_Normal != result)
        {
            DEBUG_LOG(QString("spooler: printer communication failed, unable to create basic film box. %1").arg(result.text()));
        }
    }

    if (EC_Normal == result)
    {
        for (size_t imageNumber = 0; imageNumber < storedPrintDcmtk->getNumberOfImages(); imageNumber++)
        {
            result = createAndSendBasicGrayscaleImageBox(printerConnection, storedPrintDcmtk, imageNumber, spoolDirectoryPath);

            if (result != EC_Normal) break;
        }
    }

    //Donem ordre d'imprimir el BasicFilmBox, no cal donar ordre d'imprimir el FilmSession amb el BasicFilmBox n'hi ha suficent
    if (EC_Normal == result)
    {
        result = storedPrintDcmtk->printSCUprintBasicFilmBox(printerConnection);
        if (EC_Normal != result)
        {
            DEBUG_LOG(QString("spooler: printer communication failed, unable to print. %1").arg(result.text()));
        }
    }

    /*El printSCUDelete el fem sempre encara que algun pas hagi fallat, per indicar a la impressora que ja pot alliberar els recursos guardats per 
      emmagatzemar el FilmSession,FilmBox i ImageBox. Aquest m�tode nom�s resultar� �til quan la impressi� ha anat correcte o si s'ha produ�t un error
      s'ha produ�t despr�s de crear el FilmSession*/

    result = storedPrintDcmtk->printSCUdelete(printerConnection);
    if (EC_Normal != result)
    {
        DEBUG_LOG(QString("spooler: printer communication failed, unable to delete print objects %1.").arg(result.text()));
    }

    delete storedPrintDcmtk;
}

DcmDataset PrintDicomSpool::getAttributesBasicFilmSession()
{
    DcmDataset datasetBasicFilmSession;
    DcmElement *attributeBasicFilmSession = NULL;

    attributeBasicFilmSession = new DcmCodeString(DCM_MediumType);
    attributeBasicFilmSession->putString(qPrintable(m_dicomPrintJob.getMediumType()));
    datasetBasicFilmSession.insert(attributeBasicFilmSession);

    attributeBasicFilmSession = new DcmCodeString(DCM_FilmDestination);
    attributeBasicFilmSession->putString(qPrintable(m_dicomPrintJob.getFilmDestination())); 
    datasetBasicFilmSession.insert(attributeBasicFilmSession);

    //TODO:Comprovar si l'utilitzarem  
    /*attributeBasicFilmSession = new DcmLongString(DCM_FilmSessionLabel);
    attributeBasicFilmSession->putString(printerFilmSessionLabel.c_str()); else result=EC_IllegalCall;
    datasetBasicFilmSession.insert(attributeBasicFilmSession, OFTrue replaceOld);
   */

    attributeBasicFilmSession = new DcmCodeString(DCM_PrintPriority);
    attributeBasicFilmSession->putString(qPrintable(m_dicomPrintJob.getPrintPriority()));
    datasetBasicFilmSession.insert(attributeBasicFilmSession);

    /*TODO:Si no m'equivoco nom�s serveix per quan s'esborra el treball impr�s del servidor d'impressi�, si no l'especifiquem 
     el servidor d'impressi� l'emplena autom�ticament*/
    /*attributeBasicFilmSession = new DcmShortString(DCM_OwnerID);
    attributeBasicFilmSession->putString(printerOwnerID.c_str());
    datasetBasicFilmSession.insert(attributeBasicFilmSession, OFTrue replaceOld);
    */

    attributeBasicFilmSession = new DcmIntegerString(DCM_NumberOfCopies);
    attributeBasicFilmSession->putString(qPrintable(QString().setNum(m_dicomPrintJob.getNumberOfCopies()))); 
    datasetBasicFilmSession.insert(attributeBasicFilmSession);

    return datasetBasicFilmSession;
}

OFCondition PrintDicomSpool::createAndSendBasicGrayscaleImageBox(DVPSPrintMessageHandler& printerConnection, DVPSStoredPrint	*storedPrintDcmtk, size_t imageNumber, const QString &spoolDirectoryPath)
{
    OFCondition result;
    const char *studyUID = NULL, *seriesUID = NULL, *instanceUID = NULL;
    DicomImage *imageToPrint;
    QString imageToPrintPath;
    bool isImageMonochrome1;

    //Busquem el studyUID, seriesUID i instaceUID de la imatge a imprimir
    result = storedPrintDcmtk->getImageReference(imageNumber, studyUID, seriesUID, instanceUID);

    if (result != EC_Normal || !studyUID || !seriesUID || !instanceUID)
    {
        ERROR_LOG(QString("No s'ha trobat la imatge n�mero %1 per imprimir").arg(QString().setNum(imageNumber)));
        return result;
    }

    //TODO:S'hauria de fer a un altre lloc el c�lcul del path de la imatge perqu� tamb� s'utilitza a CreateDicomPrintSpool
    imageToPrintPath = QDir::toNativeSeparators(spoolDirectoryPath + QDir::separator() + instanceUID + ".dcm");
    imageToPrint = new DicomImage(qPrintable(imageToPrintPath));

    if (imageToPrint && EIS_Normal == imageToPrint->getStatus())
    {
        isImageMonochrome1 = imageToPrint->getPhotometricInterpretation() == EP_Interpretation::EPI_Monochrome1;

        //Enviem la imatge
        result = storedPrintDcmtk->printSCUsetBasicImageBox(printerConnection, imageNumber, *imageToPrint, isImageMonochrome1);
        if (EC_Normal != result)
        {
            DEBUG_LOG(QString("spooler: printer communication failed, unable to transmit basic grayscale image box. %1").arg(result.text()));
        }
    } 
    else 
    {
        DEBUG_LOG("spooler: unable to load image file " + imageToPrintPath );
    }
    
    //No s'ha de fer el delete del studyUID, seriesUID i instanceUID perqu� s�n un punter a informaci� del storedPrint
    delete imageToPrint;
    
    return result;
}

DVPSStoredPrint* PrintDicomSpool::loadStoredPrintFileDcmtk(const QString &pathStoredPrintDcmtkFile)
{
    /*El constructor del DVPStoredPrint se li ha de passar com a par�metres
        1r El tag (2010,015E) Illumination de la Basic Film Box
        2n El tag (2010,0160) Reflected Ambient Light de la Basic Film Box
        3r AETitle del Starviewer

        Els dos primer par�metres nom�s s'utilitzen si la impressora suporta el Presentation Lut, ara mateix no ho soportem (no est� implementat) per tant se 
        suposa que aquests valors s'ignoraran. De totes maneres se li ha donat aquests valors per defecte 2000 i 10 respectivament perqu� s�n els que utilitza dcmtk i tamb�
        s'ha consultat el dicom conformance de les impressores agfa i kodak i tamb� utiltizen aquests valors per defecte.
     */
    DVPSStoredPrint *storedPrint = new DVPSStoredPrint(2000, 10, qPrintable(PacsDevice::getLocalAETitle()));
    DcmFileFormat *storedPrintDcmtkFile = NULL;
    DcmDataset *datasetStoredPrintDcmtkFile = NULL;

    OFCondition result = DVPSHelper::loadFileFormat(qPrintable(QDir::toNativeSeparators(pathStoredPrintDcmtkFile)), storedPrintDcmtkFile);
    if (EC_Normal != result)
    {
        ERROR_LOG("No s'ha pogut obrir el fitxer StoredPrint de dcmtk, path:" + pathStoredPrintDcmtkFile);
        return NULL;
    }
    
    datasetStoredPrintDcmtkFile  = storedPrintDcmtkFile->getDataset();

    if (!datasetStoredPrintDcmtkFile)
    {
        ERROR_LOG("No s'ha pogut obtenir el dataset del fitxer StoredPrint de dcmtk");
        return NULL;
    }

    result = storedPrint->read(*datasetStoredPrintDcmtkFile); 

    if (EC_Normal != result)
    {
        ERROR_LOG("No s'ha pogut carregar les dades del StoredPrint per imprimir");
        return NULL;
    }

    delete storedPrintDcmtkFile;

    return storedPrint;
}
}