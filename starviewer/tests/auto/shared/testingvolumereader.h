#ifndef TESTINGVOLUMEREADER_H
#define TESTINGVOLUMEREADER_H

#include "volumereader.h"

using namespace udg;

namespace udg {

class VolumePixelData;

}

namespace testing {

/// De moment nom�s serveix per comprovar si es crida read() o no i per simular que llegeix correctament.
class TestingVolumeReader : public udg::VolumeReader {

public:

    TestingVolumeReader(bool &readHasBeenCalled, QObject *parent = 0);

    virtual void read(Volume *volume);

public:

    /// Pixel data que el read() assigna al volum. Si �s nul no fa l'assignaci�.
    VolumePixelData *m_readPixelData;

private:

    /// Es posar� a cert si es crida read().
    bool &m_readHasBeenCalled;

};

}

#endif // TESTINGVOLUMEREADER_H
