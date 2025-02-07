#include "imagepipeline.h"
#include "windowlevelfilter.h"
#include "thickslabfilter.h"
#include "displayshutterfilter.h"
#include "transferfunction.h"

#include "vtkImageData.h"
#include "vtkRunThroughFilter.h"

namespace udg {

ImagePipeline::ImagePipeline()
{
    // Filtre de thick slab + grayscale
    m_thickSlabProjectionFilter = new ThickSlabFilter();
    m_thickSlabProjectionFilter->setSlabThickness(1);

    m_windowLevelLUTFilter = new WindowLevelFilter();

    m_displayShutterFilter = new DisplayShutterFilter();
    m_displayShutterFilter->setInput(m_windowLevelLUTFilter->getOutput());

    m_outputFilter = vtkRunThroughFilter::New();
    m_outputFilter->SetInputConnection(m_windowLevelLUTFilter->getOutput().getVtkAlgorithmOutput());

    m_shutterData = 0;
}

ImagePipeline::~ImagePipeline()
{
    delete m_displayShutterFilter;
    delete m_windowLevelLUTFilter;
    delete m_thickSlabProjectionFilter;
    m_outputFilter->Delete();
}

void ImagePipeline::setInput(vtkImageData *input)
{
    m_input = input;

    if (m_thickSlabProjectionFilter->getSlabThickness() > 1)
    {
        m_thickSlabProjectionFilter->setInput(m_input);
        m_windowLevelLUTFilter->setInput(m_thickSlabProjectionFilter->getOutput());
    }
    else
    {
        m_windowLevelLUTFilter->setInput(m_input);
    }
}

void ImagePipeline::setInput(FilterOutput input)
{
    setInput(input.getVtkImageData());
}

void ImagePipeline::setShutterData(vtkImageData *shutterData)
{
    m_shutterData = shutterData;
    if (m_shutterData)
    {
        m_displayShutterFilter->setDisplayShutter(m_shutterData);
        m_outputFilter->SetInputConnection(m_displayShutterFilter->getOutput().getVtkAlgorithmOutput());
    }
    else
    {
        m_outputFilter->SetInputConnection(m_windowLevelLUTFilter->getOutput().getVtkAlgorithmOutput());
    }
}

void ImagePipeline::setSlice(int slice)
{
    m_thickSlabProjectionFilter->setFirstSlice(slice);
}

void ImagePipeline::setSlabProjectionMode(AccumulatorFactory::AccumulatorType type)
{
    m_thickSlabProjectionFilter->setAccumulatorType(static_cast<AccumulatorFactory::AccumulatorType>(type));
}

void ImagePipeline::setSlabStride(int step)
{
    m_thickSlabProjectionFilter->setStride(step);
}

void ImagePipeline::setProjectionAxis(const OrthogonalPlane &axis)
{
    m_thickSlabProjectionFilter->setProjectionAxis(axis);
}

void ImagePipeline::setSlabThickness(int numberOfSlices)
{
    int previousSlabThickness = m_thickSlabProjectionFilter->getSlabThickness();
    m_thickSlabProjectionFilter->setSlabThickness(numberOfSlices);

    if (m_thickSlabProjectionFilter->getSlabThickness() > 1 && previousSlabThickness == 1)
    {
        m_thickSlabProjectionFilter->setInput(m_input);
        m_windowLevelLUTFilter->setInput(m_thickSlabProjectionFilter->getOutput());
    }
    else if (m_thickSlabProjectionFilter->getSlabThickness() == 1)
    {
        m_windowLevelLUTFilter->setInput(m_input);
    }
}

vtkImageData* ImagePipeline::getSlabProjectionOutput()
{
    return m_thickSlabProjectionFilter->getOutput().getVtkImageData();
}

bool ImagePipeline::setWindowLevel(double window, double level)
{
    if ((m_windowLevelLUTFilter->getWindow() != window) || (m_windowLevelLUTFilter->getLevel() != level))
    {
        m_windowLevelLUTFilter->setWindow(window);
        m_windowLevelLUTFilter->setLevel(level);

        return true;
    }

    return false;
}

void ImagePipeline::getCurrentWindowLevel(double wl[2])
{
    wl[0] = m_windowLevelLUTFilter->getWindow();
    wl[1] = m_windowLevelLUTFilter->getLevel();
}

void ImagePipeline::setTransferFunction(const TransferFunction &transferFunction)
{
    m_windowLevelLUTFilter->setTransferFunction(transferFunction);
}

void ImagePipeline::clearTransferFunction()
{
    m_windowLevelLUTFilter->clearTransferFunction();
}

vtkAlgorithm* ImagePipeline::getVtkAlgorithm() const
{
    return m_outputFilter;
}

}
