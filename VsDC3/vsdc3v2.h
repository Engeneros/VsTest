#ifndef VSDC3V2_H
#define VSDC3V2_H

#include <stdint.h>
#include <iostream>
#include <vector>

#include <qvector.h>
#include "vsdc3RegDefs.h"

#include "vmecardbaseport.h"

static const QString RANGES[8] = {"GAIN_ERROR", "RANGE_0V2", "RANGE_0V5", "RANGE_1V", "RANGE_2V", "RANGE_5V", "RANGE_10V", "IMPOSIBRU!!!"};

class VsDCChSettings
{
public:
    VsDCChSettings(quint32 _settReg, quint32 _timerReg, quint32 _avg2inNReg = 0,
                   quint32 _prTimerReg=0)
    {
        settReg = _settReg;
        timerReg = _timerReg;
        avg2inNReg = _avg2inNReg;
        prTimerReg = _prTimerReg;
    }
    quint32 settReg;
    quint32 timerReg;
    quint32 avg2inNReg;
    quint32 prTimerReg;
};


//class VsDC_Base
//{

//};

class VsDC3v2// : VsDC_Base
{
public:
    VsDC3v2()
    {
        pPort = NULL;
        baseAddr = 0x00;
        chanNum = 0;
    }
    VsDC3v2(VmeCardBasePort *_pPort) : VsDC3v2()
    {
        changePort(_pPort);
    }
    void changePort(VmeCardBasePort *_pPort)
    {
        pPort = _pPort;
    }

    void assignBaseAddr(uint32_t _baseAddr, uint32_t _chanNum, uint32_t _devInd);
    uint32_t readSettings(uint32_t ch);
    void readDevIdReg(void);
    void readRefVoltages(void);
    float currRefVoltage(void);
    void readTempReg(void);
    void calibAll(void);
    void pstartAll(void);
    void pstopAll(void);
    void pstart(uint32_t _ch);
    void pstop(uint32_t _ch);
    void calib(uint32_t _ch);
    void setStopSrc(uint32_t val, uint32_t ch);
    void setStartSrc(uint32_t val, uint32_t ch);
    void setInMux(uint32_t val, uint32_t ch);
    void forceStop(void);
    //
    void setTimerMks(uint32_t uTmrMks, uint32_t ch);
    void setTimerSec(double dSec, uint32_t ch);
    void setTimerReg(uint32_t _val, uint32_t ch);
    void setTimerReg(uint32_t _val);
    uint32_t getTimerReg(uint32_t ch);
    //
//    void applyToAllChannels(void (*func)(uint32_t, uint32_t) , uint32_t _val)
//    {
//        for(auto chan=0ul; chan<chanNum; ++chan)
//        {
//            func(_val, chan);
//        }
//    }
    void setPrTimerReg(uint32_t _val);
    void setPrTimerReg(uint32_t _val, uint32_t ch);
    void setPrTimerSec(double dSec, uint32_t ch);
    //
    std::vector<float> getOscilloscope(uint32_t ch);
    std::vector<float> getOscilloscopeMultiThread(uint32_t ch);
    float readIntegral(uint32_t ch);
    static float avg(std::vector<float> &data, uint32_t begin=0, uint32_t end=0);
    static float std(float mean, std::vector<float> &data, uint32_t begin=0, uint32_t end=0);
    //
    void configureChannel(VsDCChSettings & _sett, uint32_t _ch);
    void deconfigureInternalSyncTimer(void);
    void uploadRefVoltage(std::vector<float> refs);
    std::string printDevInfo(void);
    uint32_t chanCount(void);
    void setSettReg(uint32_t _sett, uint32_t _ch);
    void setBpTTLMux(uint32_t _start, uint32_t _stop, uint32_t _ch);
    void setBpStartMux(uint32_t _ttl, uint32_t _ch);
    void setBpStopMux(uint32_t _ttl, uint32_t _ch);
    void setIrqVector(uint32_t _vec, uint32_t _ch);
    uint32_t getGlobalState(void);
    uint32_t getState(uint32_t _ch);
    void clrIntRdyBit(uint32_t _ch);
    std::vector<float> readCalibResults(uint32_t _ch);
    void setDacValue(float _volt);
    float getDacValue(void);
    void configureInternalSyncTimer(void);
    void startSyncTimer(void);
    void stopSyncTimer(void);
    void configureAutoZero(void);
    void read_interleaved_measurements(void);
    //
    static std::string muxName(uint32_t code);
    static std::string rangeName(uint32_t code);
    //protected:
    quint32 devId;
    quint32 hwId;
    quint32 swVer;
    float calTemp;
    std::vector<float> refVoltages;
    std::vector<float> quants;
    std::vector<uint32_t> oscLen;
    float adcSamplingTime;
    float precTimerRate;
    uint32_t baseAddr;

private:
    VmeCardBasePort *pPort;
//    uint32_t baseAddr;
    uint32_t chanNum;
    uint32_t dataTmp;
    uint32_t devIndex;
    void readModifyWrite(uint32_t addr, uint32_t mask, uint32_t data);
};


#endif // VSDC3V2_H
