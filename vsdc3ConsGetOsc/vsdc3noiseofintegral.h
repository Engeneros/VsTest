#ifndef VSDC3NOISEOFINTEGRAL_H
#define VSDC3NOISEOFINTEGRAL_H

#include "MovingCoilsServer.h"

namespace VsDCDeviceTestsNS
{
}

class VsDC
{

};

class TestResult
{

};

class TestOfVsDC
{
public: TestResult makeTest(VsDC *pdev);
};


void NoiseInt(MovingCoilsServer * pServer);

#endif // VSDC3NOISEOFINTEGRAL_H
