#include "autotest.h"
#include "temporaldimensionfillerstep.h"

#include "image.h"
#include "series.h"

using namespace udg;

class TestingTemporalDimensionFillerStep : public TemporalDimensionFillerStep {
public:
   using TemporalDimensionFillerStep::VolumeInfo;
   using TemporalDimensionFillerStep::TemporalDimensionInternalInfo;
};

class test_TemporalDimensionFillerStep : public QObject {
Q_OBJECT

private slots:

    void postProcessing_ShouldSetExpectedPhaseNumberWhenAllSlicesHaveTheSameNumberOfPhases_data();
    void postProcessing_ShouldSetExpectedPhaseNumberWhenAllSlicesHaveTheSameNumberOfPhases();

    void postProcessing_ShouldSetSinglePhaseWhenNotAllSlicesHaveTheSameNumberOfPhases_data();
    void postProcessing_ShouldSetSinglePhaseWhenNotAllSlicesHaveTheSameNumberOfPhases();

    void postProcessing_ShouldSetSinglePhaseWhenThereAreMultipleAcquisitionNumbers_data();
    void postProcessing_ShouldSetSinglePhaseWhenThereAreMultipleAcquisitionNumbers();

    void postProcessing_ShouldSetSinglePhaseWhenTheVolumeIsNotProcessed_data();
    void postProcessing_ShouldSetSinglePhaseWhenTheVolumeIsNotProcessed();

private:

    template <typename Function>
    QSharedPointer<TestingTemporalDimensionFillerStep> createStep(int numberOfSlices, const Function &numberOfPhasesForSlice);
    template <typename Function>
    void testPostProcessing(const Function &expectedPhaseNumber);

};

Q_DECLARE_METATYPE(QSharedPointer<TestingTemporalDimensionFillerStep>)

void test_TemporalDimensionFillerStep::postProcessing_ShouldSetExpectedPhaseNumberWhenAllSlicesHaveTheSameNumberOfPhases_data()
{
    QTest::addColumn< QSharedPointer<TestingTemporalDimensionFillerStep> >("step");
    QTest::addColumn<int>("numberOfPhases");

    // Emulate a lambda function with a struct with function operator
    struct { int operator()(int) const { return 10; } } lambda1;
    struct { int operator()(int) const { return 12; } } lambda2;

    QTest::newRow("one slice") << createStep(1, lambda1) << 10;
    QTest::newRow("10 slices") << createStep(10, lambda2) << 12;
}

void test_TemporalDimensionFillerStep::postProcessing_ShouldSetExpectedPhaseNumberWhenAllSlicesHaveTheSameNumberOfPhases()
{
    QFETCH(int, numberOfPhases);

    // Emulate a lambda function with a struct with function operator
    struct {
        int numberOfPhases;
        int operator()(int i) const { return i % numberOfPhases; }
    } lambda = { numberOfPhases };

    testPostProcessing(lambda);
}

void test_TemporalDimensionFillerStep::postProcessing_ShouldSetSinglePhaseWhenNotAllSlicesHaveTheSameNumberOfPhases_data()
{
    QTest::addColumn< QSharedPointer<TestingTemporalDimensionFillerStep> >("step");

    // Emulate a lambda function with a struct with function operator
    struct { int operator()(int i) const { return i + 1; } } lambda;

    QTest::newRow("differents numbers of phases per slice") << createStep(10, lambda);
}

void test_TemporalDimensionFillerStep::postProcessing_ShouldSetSinglePhaseWhenNotAllSlicesHaveTheSameNumberOfPhases()
{
    // Emulate a lambda function with a struct with function operator
    struct { int operator()(int) const { return 0; } } lambda;

    testPostProcessing(lambda);
}

void test_TemporalDimensionFillerStep::postProcessing_ShouldSetSinglePhaseWhenThereAreMultipleAcquisitionNumbers_data()
{
    QTest::addColumn< QSharedPointer<TestingTemporalDimensionFillerStep> >("step");

    // Emulate a lambda function with a struct with function operator
    struct { int operator()(int) const { return 10; } } lambda;

    QSharedPointer<TestingTemporalDimensionFillerStep> step = createStep(10, lambda);
    (*step->TemporalDimensionInternalInfo.begin())->value(0)->multipleAcquisitionNumber = true; // set multipleAcquisitionNumber to true for volume 0
    QTest::newRow("multiple acquisition numbers") << step;
}

void test_TemporalDimensionFillerStep::postProcessing_ShouldSetSinglePhaseWhenThereAreMultipleAcquisitionNumbers()
{
    // Emulate a lambda function with a struct with function operator
    struct { int operator()(int) const { return 0; } } lambda;

    testPostProcessing(lambda);
}

void test_TemporalDimensionFillerStep::postProcessing_ShouldSetSinglePhaseWhenTheVolumeIsNotProcessed_data()
{
    QTest::addColumn< QSharedPointer<TestingTemporalDimensionFillerStep> >("step");

    // Emulate a lambda function with a struct with function operator
    struct { int operator()(int) const { return 10; } } lambda;

    QSharedPointer<TestingTemporalDimensionFillerStep> step = createStep(10, lambda);
    delete (*step->TemporalDimensionInternalInfo.begin())->take(0); // esborrem el VolumeInfo del volum 0
    QTest::newRow("volume not processed") << step;
}

void test_TemporalDimensionFillerStep::postProcessing_ShouldSetSinglePhaseWhenTheVolumeIsNotProcessed()
{
    // Emulate a lambda function with a struct with function operator
    struct { int operator()(int) const { return 0; } } lambda;

    testPostProcessing(lambda);
}

template <typename Function>
QSharedPointer<TestingTemporalDimensionFillerStep> test_TemporalDimensionFillerStep::createStep(int numberOfSlices,
                                                                                                const Function &numberOfPhasesForSlice)
{
    Series *series = new Series(this);
    TestingTemporalDimensionFillerStep::VolumeInfo *volumeInfo = new TestingTemporalDimensionFillerStep::VolumeInfo();
    volumeInfo->numberOfPhases = numberOfPhasesForSlice(0);
    volumeInfo->multipleAcquisitionNumber = false;

    for (int i = 0; i < numberOfSlices; i++)
    {
        int numberOfPhases = numberOfPhasesForSlice(i);

        for (int j = 0; j < numberOfPhases; j++)
        {
            Image *image = new Image();
            image->setVolumeNumberInSeries(0);
            image->setSOPInstanceUID(QString("%1.%2").arg(i).arg(j));
            series->addImage(image);
        }

        volumeInfo->phasesPerPositionHash.insert(QString("0\\0\\%1").arg(i), numberOfPhases);
    }

    QHash<int, TestingTemporalDimensionFillerStep::VolumeInfo*> *volumeHash = new QHash<int, TestingTemporalDimensionFillerStep::VolumeInfo*>();
    volumeHash->insert(0, volumeInfo);

    QSharedPointer<TestingTemporalDimensionFillerStep> step(new TestingTemporalDimensionFillerStep());
    step->TemporalDimensionInternalInfo.insert(series, volumeHash);

    return step;
}

template <typename Function>
void test_TemporalDimensionFillerStep::testPostProcessing(const Function &expectedPhaseNumber)
{
    QFETCH(QSharedPointer<TestingTemporalDimensionFillerStep>, step);

    QList<Series*> seriesList = step->TemporalDimensionInternalInfo.keys();

    step->postProcessing();

    int i = 0;

    foreach (Series *series, seriesList)
    {
        foreach (Image *image, series->getImages())
        {
            QCOMPARE(image->getPhaseNumber(), expectedPhaseNumber(i));
            i++;
        }
    }
}

DECLARE_TEST(test_TemporalDimensionFillerStep)

#include "test_temporaldimensionfillerstep.moc"
