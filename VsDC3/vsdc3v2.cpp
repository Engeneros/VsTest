#include "vsdc3v2.h"
#include "qt_windows.h"
#include <sstream>

void VsDC3v2::assignBaseAddr(uint32_t _baseAddr, uint32_t _chanNum, uint32_t _devInd)
{
    baseAddr = _baseAddr;
    chanNum = _chanNum;
    oscLen.resize(chanNum);
    devIndex = _devInd;
    pPort->setBaseAddr(_baseAddr, devIndex);
    dataTmp = pPort->vmeReadReg(TIME_QUANT, devIndex);
    adcSamplingTime = *reinterpret_cast<float*>(&dataTmp);
    precTimerRate = 10e-9;
}
uint32_t VsDC3v2::readSettings(uint32_t ch)
{
    auto dataTmp = pPort->vmeReadReg(ADC_SETT_REG(ch), devIndex);
    std::cout << std::hex << dataTmp << std::dec << std::endl;
}
void VsDC3v2::readDevIdReg(void)
{
    dataTmp = pPort->vmeReadReg(DEV_ID, devIndex);//TODO: howto handle BaseAddr?
    devId = (0xFFFF&(dataTmp >> 16));
    hwId = (0xFF&(dataTmp  >> 8));
    swVer = 0xFF&(dataTmp);
}
void VsDC3v2::readRefVoltages(void)
{
    refVoltages.clear();
    dataTmp = pPort->vmeReadReg(REFH, devIndex);
    refVoltages.push_back(*reinterpret_cast<float*>(&dataTmp));
    dataTmp = pPort->vmeReadReg(REFL, devIndex);
    refVoltages.push_back(*reinterpret_cast<float*>(&dataTmp));
    quants.clear();
    for(auto ch_idx = 0; ch_idx<chanNum; ++ch_idx) {
        dataTmp = pPort->vmeReadReg(ADC_QUAN(ch_idx), devIndex);
        quants.push_back(*reinterpret_cast<float*>(&dataTmp));
    }
}
float VsDC3v2::currRefVoltage(void)
{
    dataTmp = pPort->vmeReadReg(ADC_CSR_REG(0), devIndex);
    dataTmp &= AVCH_RANGE_MASK;
    if(dataTmp >= AVCH_RANGE_2V)
        dataTmp = pPort->vmeReadReg(REFH, devIndex);
    else
        dataTmp = pPort->vmeReadReg(REFL, devIndex);
    return *reinterpret_cast<float*>(&dataTmp);
}
void VsDC3v2::readTempReg(void)
{
    dataTmp = pPort->vmeReadReg(ADC0_CAL_TEMP, devIndex);
    calTemp = *reinterpret_cast<float*>(&dataTmp);
}
void VsDC3v2::calibAll(void)
{
    pPort->vmeWriteReg(GCR, CALIB_ALL, devIndex);
}
void VsDC3v2::pstartAll(void)
{
    pPort->vmeWriteReg(GCR, PSTART_ALL, devIndex);
}
void VsDC3v2::pstopAll(void)
{
    pPort->vmeWriteReg(GCR, PSTOP_ALL, devIndex);
}
void VsDC3v2::pstart(uint32_t _ch)
{
    pPort->vmeWriteReg(ADC_CSR_REG(_ch), PSTART, devIndex);
}
void VsDC3v2::pstop(uint32_t _ch)
{
    pPort->vmeWriteReg(ADC_CSR_REG(_ch), PSTOP, devIndex);
}
void VsDC3v2::calib(uint32_t _ch)
{
    pPort->vmeWriteReg(ADC_CSR_REG(_ch), CALIB, devIndex);
}
void VsDC3v2::configureInternalSyncTimer(void)
{

    int cycleNum = 1000;
    int period = 100000;
//    int phase = 4000000;

    pPort->vmeWriteReg(TG_SETTINGS,
//                                   TG_START1_BP_SRC |
//                                   TG_STOP0_BP_SRC |
                                   TG_START_TIMER_SRC |
                                   TG_CH0_ENA |
                                   TG_CH1_ENA |
                                   TG_CH2_ENA |
                                   TG_CH3_ENA |
                                   (cycleNum << 12)
                                   );
    pPort->vmeWriteReg(TG_TMR_PERIOD, period);
//    pPort->vmeWriteReg(TG_CH0_PHASE, 0);
//    pPort->vmeWriteReg(TG_CH1_PHASE, phase);
//    pPort->vmeWriteReg(TG_CH3_PHASE, phase);
//    pPort->vmeWriteReg(TG_CH2_PHASE, 0);

    pPort->vmeWriteReg(INT_BUFF_WRITE_POS, 0);
    pPort->vmeWriteReg(INT_BUFF_READ_POS, 0);
}
void VsDC3v2::deconfigureInternalSyncTimer(void)
{
    pPort->vmeWriteReg(TG_CSR, TG_STOP);
    Sleep(25);
    pPort->vmeWriteReg(TG_SETTINGS, 0);
}
void VsDC3v2::startSyncTimer(void)
{
    pPort->vmeWriteReg(TG_CSR, TG_RUN);
}
void VsDC3v2::stopSyncTimer(void)
{
    pPort->vmeWriteReg(TG_CSR, TG_STOP);
}
void VsDC3v2::configureAutoZero(void){
    uint32_t OSC_DRDY = 5;
    pPort->vmeWriteReg(AUZ_GND_MUX_DELAY,   OSC_DRDY*0);
    pPort->vmeWriteReg(AUZ_PAUSE_NUM,       OSC_DRDY*128);
    pPort->vmeWriteReg(AUZ_SW_NUM,          OSC_DRDY*128);
    pPort->vmeWriteReg(AUZ_FULL_NUM,        OSC_DRDY*580); //1260
}
void VsDC3v2::read_interleaved_measurements(void)
{
    QString filename;

    QString global_file_path = "G:\\work\\tmpdat\\VsDC3_chopp";

    QString filename_t = global_file_path + "\\dataNI_%08X.txt";

    filename.asprintf(filename_t.toLocal8Bit().constData(),baseAddr);
    FILE *fout;

    uint dataTmp;

    dataTmp = pPort->vmeReadReg(INT_BUFF_WRITE_POS);
    std::cout << std::dec << "INT_BUFF_WRITE_POS: " << dataTmp << std::dec << std::endl;

    auto cycleNum = dataTmp/2;

    uint    * ch0State       = new uint[2*cycleNum];
    float   * ch0SwTime     = new float[2*cycleNum];
    float   * ch0SwZero     = new float[2*cycleNum];
    float   * ch0FullZero   = new float[2*cycleNum];
    float   * ch0Integral   = new float[2*cycleNum];

    uint     * ch1State      = new uint[2*cycleNum];
    float   * ch1SwTime     = new float[2*cycleNum];
    float   * ch1SwZero     = new float[2*cycleNum];
    float   * ch1FullZero   = new float[2*cycleNum];
    float   * ch1Integral   = new float[2*cycleNum];

    float   * fullIntegral  = new float[2*cycleNum];



    const int intBufCellSize = 5;
    const int rdPortion = intBufCellSize * 2 * 100;
    int rdNum = intBufCellSize * 2 * cycleNum;
    int rdLen=0;
    uint rdAddr = (VME_INT_BUFF_MEM * 4);

    std::vector<uint32_t> rdData;

    uint ii=0,kk=0,jj=0,rr=0;

    for(; rdNum>0; rdNum-=rdPortion)
    {
        if(rdNum >= rdPortion)
        {
            rdLen = rdPortion;
        }
        else
        {
            rdLen = rdNum;
        }

        rdData.resize(rdLen);
        pPort->vmeReadMem(rdAddr, rdData, devIndex);
        rdAddr += 4*rdLen;

        for(jj=0;jj<(rdLen/intBufCellSize);jj++)
        {
            if(rdData[0+intBufCellSize*jj] & INT_BUFF_CHNUM_REG_MASK)
            {
                ch1State[ii]      = rdData[0+intBufCellSize*jj] & INT_BUFF_STATUS_MASK;
                ch1SwTime[ii]     = *(float*) &rdData[1+intBufCellSize*jj];
                ch1SwZero[ii]     = *(float*) &rdData[2+intBufCellSize*jj];
                ch1FullZero[ii]   = *(float*) &rdData[3+intBufCellSize*jj];
                ch1Integral[ii++]   = *(float*) &rdData[4+intBufCellSize*jj];
            }
            else
            {
                ch0State[kk]      = rdData[0+intBufCellSize*jj] & INT_BUFF_STATUS_MASK;
                ch0SwTime[kk]     = *(float*) &rdData[1+intBufCellSize*jj];
                ch0SwZero[kk]     = *(float*) &rdData[2+intBufCellSize*jj];
                ch0FullZero[kk]   = *(float*) &rdData[3+intBufCellSize*jj];
                ch0Integral[kk++]   = *(float*) &rdData[4+intBufCellSize*jj];
            }
            fullIntegral[rr++] = *(float*) &rdData[4+intBufCellSize*jj];
            if(!(rr%10000))
                std::cout << rr << std::endl;
        }
        rdData.clear();
    }

    std::cout << kk << "\t" << ii << std::endl;
    std::cout << ch1SwTime[0] << std::endl;
    //save data
    fout = fopen(filename.toLocal8Bit().constData(), "w");
    if(kk != ii)
        std::cout << "Reading Error!" << std::endl;
    if(kk < ii)
        ii = kk;
    {
        for(int jj=0;jj<ii;jj++){
            fprintf(fout, "%+G\t%+.7G\t%+.7G\t%+.7G\t%+.7G\t%+.7G\t%+.7G\t%+.7G\t%+.7G\n", ((1.0*jj)), ch0Integral[jj], ch1Integral[jj], ch0SwTime[jj], ch1SwTime[jj], ch0SwZero[jj], ch1SwZero[jj], ch0FullZero[jj], ch1FullZero[jj]);
        }
    }

//    double mean0 = Mean((float*)ch0Integral, 1, kk);
//    double rms0 = Rms(mean0, (float*)ch0Integral, 1, kk);
//    double mean1 = Mean((float*)ch1Integral, 1, ii);
//    double rms1 = Rms(mean1, (float*)ch1Integral, 1, ii);

//    cout << "INT0 avg:\t" << mean0 << " V;\tINT0 rms:\t" << rms0 << "V" << endl;
//    cout << "INT1 avg:\t" << mean1 << " V;\tINT1 rms:\t" << rms1 << "V" << endl;

//    fclose(fout);
//    PyPlotInter(filename, cycleNum);

    delete ch0State;
    delete ch0SwTime;
    delete ch0SwZero;
    delete ch0FullZero;
    delete ch0Integral;

    delete ch1State;
    delete ch1SwTime;
    delete ch1SwZero;
    delete ch1FullZero;
    delete ch1Integral;

    delete fullIntegral;

}
void VsDC3v2::readModifyWrite(uint32_t addr, uint32_t mask, uint32_t data)
{
    dataTmp = pPort->vmeReadReg(addr, devIndex);
    dataTmp &= (~mask);
    dataTmp |= (data & mask);
    pPort->vmeWriteReg(addr, dataTmp, devIndex);
}
void VsDC3v2::setStopSrc(uint32_t val, uint32_t ch)
{
    readModifyWrite(ADC_SETT_REG(ch), ADC_STOP_SRC_MUX, val);
}
void VsDC3v2::setStartSrc(uint32_t val, uint32_t ch)
{
    readModifyWrite(ADC_SETT_REG(ch), ADC_START_SRC_MUX, val);
}
void VsDC3v2::setInMux(uint32_t val, uint32_t ch)
{
    readModifyWrite(ADC_SETT_REG(ch), ADC_MUX_M_MASK, val);
}
void VsDC3v2::forceStop(void)
{
    for(auto ch=0ul; ch<chanNum; ++ch)
    {
        setStopSrc(ADC_STOP_PSTOP, ch);
    }
    pstopAll();
}
void VsDC3v2::setTimerMks(uint32_t uTmrMks, uint32_t ch)
{
    auto regVal = (uint32_t)(((double)uTmrMks)/(adcSamplingTime*1e6));
    setTimerReg(regVal, ch);
}
void VsDC3v2::setTimerSec(double dSec, uint32_t ch)
{
    auto regVal = (uint32_t)((dSec+adcSamplingTime/2)/(adcSamplingTime));
    setTimerReg(regVal, ch);
}
uint32_t VsDC3v2::getTimerReg(uint32_t ch)
{
//    std::cout << std::dec << "\nTimer:\t" << _val << std::endl;
    pPort->vmeReadReg(ADC_TIME_REG(ch), devIndex);
}
void VsDC3v2::setTimerReg(uint32_t _val, uint32_t ch)
{
//    std::cout << std::dec << "\nTimer:\t" << _val << std::endl;
    pPort->vmeWriteReg(ADC_TIME_REG(ch), _val, devIndex);
}
void VsDC3v2::setTimerReg(uint32_t _val)
{
    for(auto ch=0ul; ch<chanNum; ++ch)
        setTimerReg(_val, ch);
}
void VsDC3v2::setPrTimerReg(uint32_t _val, uint32_t ch)
{
    pPort->vmeWriteReg(ADC_PR_TIME_REG(ch), _val, devIndex);
}
void VsDC3v2::setPrTimerReg(uint32_t _val)
{
    for(auto ch=0ul; ch<chanNum; ++ch)
        setPrTimerReg(_val, ch);
}
void VsDC3v2::setPrTimerSec(double dSec, uint32_t ch)
{
    auto regVal = (uint32_t)(dSec/(double)adcSamplingTime);
    auto prRegVal = (uint32_t)(dSec/(double)precTimerRate);
    setTimerReg(regVal, ch);
    setPrTimerReg(prRegVal, ch);
//    dataTmp = pPort->vmeReadReg(ADC_PR_TIME_REG(ch), devIndex);
//    std::cout << dataTmp << std::endl;
}
std::vector<float> VsDC3v2::getOscilloscope(uint32_t ch)
{
    std::vector<float> oscData;
    uint32_t rdPortion = 1000;

    auto nn = pPort->vmeReadReg(ADC_ADC_WRITE(ch), devIndex);
    oscLen[ch] = nn;
    uint32_t rdCount = 0;

    std::vector<uint32_t> rdData;

    uint32_t chAddr = 4*VME_CH_OSC_MEM[ch];

    int kk=0;

    for(;;)
    {
        if(nn > rdPortion)
        {
            rdCount = rdPortion;
        }
        else
        {
            rdCount = nn;
        }

        if(rdCount>0)
        {
            rdData.resize(rdCount);
            pPort->vmeReadMem(chAddr, rdData, devIndex);
            chAddr += 4*rdCount;
            for(int ii=0; ii<rdCount; ii++)
            {
               oscData.push_back( *reinterpret_cast<float*>(&rdData[ii]) );
            }
            rdData.clear();
        }

        kk++;
        nn -= rdCount;
        if(nn==0) break;
    }
    return oscData;
}
std::vector<float> VsDC3v2::getOscilloscopeMultiThread(uint32_t ch)
{
    std::vector<float> oscData;
    uint32_t rdPortion = 500000;

    auto nn = pPort->vmeReadReg(ADC_ADC_WRITE(ch), devIndex);
    uint32_t rdCount = 0;

    std::vector<uint32_t> rdData;

    uint32_t chAddr = 4*VME_CH_OSC_MEM[ch];

    bool _last = false;
    bool _first = true;
    for(;;)
    {
        if(nn > rdPortion)
        {
            rdCount = rdPortion;
        }
        else
        {
            rdCount = nn;
        }
/*
        if(_last && _first)
        {
            rdData.resize(rdCount);
            pPort->vmeReadMem(chAddr, rdData, devIndex);
            for(int ii=0; ii<rdCount; ii++)
            {
               oscData.push_back( *reinterpret_cast<float*>(&rdData[ii]) );
            }
        }
        else
 */
        if(_first)
        {
            _first = false;
            pPort->vmeRdMemFirst(chAddr, rdCount, devIndex);

        }
        else
        {
            pPort->vmeRdMemMiddle(chAddr, rdCount, rdData, devIndex);
            for(int ii=0; ii<rdData.size(); ii++)
            {
               oscData.push_back( *reinterpret_cast<float*>(&rdData[ii]) );
            }
        }
        chAddr += 4*rdCount;
        nn -= rdCount;
        if(nn==0)
        {
            _last = true;
            break;
        }
    }
    if(_last)
    {
        pPort->vmeRdMemLast(chAddr, rdData, devIndex);
        for(int ii=0; ii<rdData.size(); ii++)
        {
           oscData.push_back( *reinterpret_cast<float*>(&rdData[ii]) );
        }
    }


    return oscData;
}
float VsDC3v2::readIntegral(uint32_t ch)
{
    dataTmp = pPort->vmeReadReg(ADC_ADC_INT(ch), devIndex);
    return *reinterpret_cast<float*>(&dataTmp);
}
float VsDC3v2::avg(std::vector<float> &data, uint32_t begin, uint32_t end)
{
    double tmp = 0.0;
    if((begin + end) >= data.size())
        return tmp;
    for(int ii=begin; ii<data.size()-end; ++ii)
    {
        tmp += data[ii];
    }
    if((data.size()-begin-end) > 1)
    {
        tmp /= (data.size()-begin-end);
    }
    return tmp;
}
float VsDC3v2::std(float mean, std::vector<float> &data, uint32_t begin, uint32_t end)
{
    double tmp = 0.0;

    if((begin + end) >= data.size())
        return tmp;

    for(int ii=begin; ii<data.size()-end; ++ii)
    {
        tmp += (data[ii]-mean)*(data[ii]-mean);
    }
    if((data.size()-begin-end) > 1)
    {
        tmp /= (data.size()-begin-end);
    }
    return sqrt(tmp);
}
void VsDC3v2::configureChannel(VsDCChSettings & _sett, uint32_t _ch)
{
    pPort->vmeWriteReg(ADC_SETT_REG(_ch), _sett.settReg, devIndex);
//    std::cout << std::dec << "\nTimer:\t" << _sett.timerReg << std::endl;
    pPort->vmeWriteReg(ADC_TIME_REG(_ch), _sett.timerReg, devIndex);
    pPort->vmeWriteReg(ADC_PR_TIME_REG(_ch), _sett.prTimerReg, devIndex);
    pPort->vmeWriteReg(ADC_ADC_AVGN(_ch), _sett.avg2inNReg, devIndex);
}
void VsDC3v2::setSettReg(uint32_t _sett, uint32_t _ch) {
    pPort->vmeWriteReg(ADC_SETT_REG(_ch), _sett, devIndex);
}
void VsDC3v2::setBpTTLMux(uint32_t _start, uint32_t _stop, uint32_t _ch)
{
    pPort->vmeWriteReg(ADC_BP_SYNC_MUX(_ch), ((_start << BP_START_SEL_OFFS) & BP_START_SEL_MASK) | ((_stop << BP_STOP_SEL_OFFS) & BP_STOP_SEL_MASK), devIndex);
}
void VsDC3v2::setBpStartMux(uint32_t _ttl, uint32_t _ch)
{
    readModifyWrite(ADC_BP_SYNC_MUX(_ch), BP_START_SEL_MASK, (_ttl << BP_START_SEL_OFFS));
}
void VsDC3v2::setBpStopMux(uint32_t _ttl, uint32_t _ch)
{
    readModifyWrite(ADC_BP_SYNC_MUX(_ch), BP_STOP_SEL_MASK, (_ttl << BP_STOP_SEL_MASK));
}
void VsDC3v2::setIrqVector(uint32_t _vec, uint32_t _ch)
{
    pPort->vmeWriteReg(ADC_INT_STATUS(_ch), _vec & ADC_INT_STATUS_MASK, devIndex);
}
void VsDC3v2::uploadRefVoltage(std::vector<float> refs)
{
    if(refs.size() != 2)
        throw std::string("Calib Voltages count is not enough");

    dataTmp = *(reinterpret_cast<uint32_t*>(&refs[0]));
    pPort->vmeWriteReg(REFH, dataTmp, devIndex);
    dataTmp = *(reinterpret_cast<uint32_t*>(&refs[1]));
    pPort->vmeWriteReg(REFL, dataTmp, devIndex);

    Sleep(1);//TODO: waiting
    pPort->vmeWriteReg(REFH, REWR, devIndex);
    Sleep(1);
    pPort->vmeWriteReg(REFH, SAVE, devIndex);
    Sleep(1);
    pPort->vmeWriteReg(REFH, LOAD, devIndex);
    Sleep(1000);
}
std::string VsDC3v2::printDevInfo(void)
{
    readDevIdReg();
    readTempReg();
    readTempReg();
    readRefVoltages();

    std::stringstream cout;
    cout << "ADDR: " << std::hex << baseAddr << "\tDEV ID: 0x" << devId;
    cout << "\tHW VER: 0x"   << hwId;
    cout << "\tSW VER: 0x"  << swVer;
    cout.precision(4);
    cout << "\nTemp: " << calTemp;
    if(!refVoltages.empty())
    {
        cout << "\tRefs: " << std::dec;
        for(auto const& ref :refVoltages)
        {
            cout.precision(7);
            cout << ref;
            cout << "V\t";
        }
    }
    cout << "\nQuants: ";
    for(auto const& q :quants) {
        cout.precision(7);
        cout << q;
        cout << "V\t";
    }
    cout << "\n";
    return cout.str();
}
std::string VsDC3v2::muxName(uint32_t code)
{
    std::string strTmp;
    switch(code & ADC_MUX_M_MASK)
    {
    case ADC_SIG:
        strTmp = "SIG";
        break;
    case ADC_GND:
        strTmp = "GND";
        break;
    case ADC_REF_H:
        strTmp = "REFH";
        break;
    case ADC_REF_L:
        strTmp = "REFL";
        break;
    }
    return strTmp;
}
std::string VsDC3v2::rangeName(uint32_t code)
{
    std::string strTmp;
    switch(code & AVCH_RANGE_MASK)
    {
    case AVCH_RANGE_ERROR:
        strTmp = "GainErr";
        break;
    case AVCH_RANGE_0V2:
        strTmp = "0V2";
        break;
    case AVCH_RANGE_0V5:
        strTmp = "0V5";
        break;
    case AVCH_RANGE_1V:
        strTmp = "1V";
        break;
    case AVCH_RANGE_2V:
        strTmp = "2V";
        break;
    case AVCH_RANGE_5V:
        strTmp = "5V";
        break;
    case AVCH_RANGE_10V:
        strTmp = "10V";
        break;
    }
    return strTmp;
}
uint32_t VsDC3v2::getState(uint32_t _ch)
{
    return pPort->vmeReadReg(ADC_CSR_REG(_ch), devIndex);
}
uint32_t VsDC3v2::getGlobalState(void)
{
    return pPort->vmeReadReg(GSR, devIndex);
}
void VsDC3v2::clrIntRdyBit(uint32_t _ch)
{
    return pPort->vmeWriteReg(ADC_CSR_REG(_ch), INTEGRAL_RDY);
}
std::vector<float> VsDC3v2::readCalibResults(uint32_t _ch)
{
    std::vector<float> calConstsList;
    //
    auto nn = pPort->vmeReadReg(ADC_SWOFF(_ch), devIndex);
    calConstsList.push_back( *reinterpret_cast<float*>(&nn) );
    //
    nn = pPort->vmeReadReg(ADC_OFFS(_ch), devIndex);
    calConstsList.push_back( *reinterpret_cast<float*>(&nn) );
    //
    nn = pPort->vmeReadReg(ADC_QUAN(_ch), devIndex);
    calConstsList.push_back( *reinterpret_cast<float*>(&nn) );
    //
    return calConstsList;
}
uint32_t VsDC3v2::chanCount(void)
{
    return chanNum;
}
void VsDC3v2::setDacValue(float _volt) {
    dataTmp = *(reinterpret_cast<uint32_t*>(&_volt));
    pPort->vmeWriteReg(CAL_DAC_VAL, dataTmp, devIndex);
}
float VsDC3v2::getDacValue(void) {
    dataTmp = pPort->vmeReadReg(CAL_DAC_VAL, devIndex);
    return *(reinterpret_cast<float*>(&dataTmp));
}
