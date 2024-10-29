#ifndef VSDC3_MEAS_HEADER_H
#define VSDC3_MEAS_HEADER_H

#include "qglobal.h"
#include "qstring.h"

namespace Vsdc3MeasHeader_NS
{
enum InMux {
    C_SIGNAL = 0,
    C_GROUND = 1,
    C_VREFHIGH = 2,
    C_VREFLOW = 3,
    C_CHOPP = 4
};

enum InRange {
    C_RANGE_2V = 0,
    C_RANGE_200mV = 1
};

}

class Vsdc3MeasHeader
{
public:
    vsdc3_meas_header();
    quint32 numPerStep;
    quint32 startDec;
    quint32 numOfDecs;
    quint32 stepPerDec;
    QString filePath;

    Vsdc3MeasHeader_NS::InRange inRange;
    Vsdc3MeasHeader_NS::InMux   inMux;

    quint32 msmt_num;
};

#endif // VSDC3_MEAS_HEADER_H
