#include "vsdccardtest.h"
#include "math.h"
#include "qt_windows.h"
#include <sstream>

VsDCTestConfig::VsDCTestConfig()
{
    nTimes = 0;
    stepsCount = 0;
    currInd = 0;
}

void VsDCTestConfig::configureMsmts(uint32_t msmtPerStep, uint32_t startDecade, uint32_t decadeNum, uint32_t stepPerDecade)
{
    nTimes = msmtPerStep;
    integrationTimes.clear();
    stepsCount = decadeNum*stepPerDecade+1;
    float intTimeTmp = pow(10, startDecade)*firstDecadeTime();
    currInd = 0;
    while(decadeNum-- > 0)
    {
        if(stepPerDecade == 2)
        {
            integrationTimes.push_back(intTimeTmp);
            integrationTimes.push_back(5.0*intTimeTmp);
        }
        else if(stepPerDecade == 3)
        {
            integrationTimes.push_back(intTimeTmp);
            integrationTimes.push_back(2.0*intTimeTmp);
            integrationTimes.push_back(5.0*intTimeTmp);
        }
        else if(stepPerDecade == 4)
        {
            integrationTimes.push_back(intTimeTmp);
            integrationTimes.push_back(2.0*intTimeTmp);
            integrationTimes.push_back(5.0*intTimeTmp);
            integrationTimes.push_back(7.0*intTimeTmp);
        }
        else if(stepPerDecade == 5)
        {
            integrationTimes.push_back(intTimeTmp);
            integrationTimes.push_back(2.0*intTimeTmp);
            integrationTimes.push_back(3.0*intTimeTmp);
            integrationTimes.push_back(5.0*intTimeTmp);
            integrationTimes.push_back(7.0*intTimeTmp);
        }
        else if(stepPerDecade == 10)
        {
            integrationTimes.push_back(intTimeTmp*1.0);
            integrationTimes.push_back(intTimeTmp*1.5);
            for(int ii=2;ii<stepPerDecade;++ii)
                integrationTimes.push_back(intTimeTmp*(float)ii);
        }
        else
        {
            float dInc = intTimeTmp/(float)(stepPerDecade+1);
            for(int ii=0;ii<stepPerDecade;++ii)
            {
                integrationTimes.push_back(intTimeTmp+dInc*ii);
            }
        }
        intTimeTmp *= 10.0;
    }
    integrationTimes.push_back(intTimeTmp);
}

bool VsDCTestConfig::getNextMsmt(float & timeOfInt, uint32_t & ntimes)
{
    if(currInd < integrationTimes.size())
    {
        timeOfInt = integrationTimes[currInd++];
        ntimes = nTimes;
        return true;
    }
    timeOfInt = 0.0;
    ntimes = 0;
    return false;
}

std::string VsDCTestConfig::confToString(void)
{
    std::string str;
    str.append("Step Num:\t");
    str.append(std::to_string(stepsCount));
    str.append("\nPer Step:\t");
    str.append(std::to_string(nTimes));
    str.append("\n");
    for(int ii=0; ii<stepsCount; ++ii)
    {
        str.append(std::to_string(integrationTimes[ii]));
        str.append("\n");
    }
    return str;
}

std::ostream& operator<<(std::ostream& os, VsDCTestConfig& dt)
{
    os << dt.confToString();
    return os;
}

float VsDCTestConfig::firstDecadeTime(void)
{
    return 1.0;
}

uint32_t VsDCTestConfig::getStepCount(void)
{
    return stepsCount;
}

//VsdcIntNoiseTest

void VsdcIntNoiseTest::doTest(VsDCTestConfig sett, std::vector<VsDC3v2> &devList)
{
    float timeOfInt;
    uint32_t ntimes;
    uint32_t devCount = devList.size();

    //configure vsdc
    for(auto dev: devList)
    {
        //startSrcPStart
        //stopSrcTimer
        //muxToGnd or muxToSig
        dev.setPrTimerReg(0);
    }

    //Prepare results tables
    std::vector<std::string> msmt_str;
    msmt_str.push_back("Time, s");
    res.clear();
    for(uint32_t devInd=0; devInd<devCount; ++devInd)
    {
        res.push_back( VsDCTestResults(msmt_str, createFileName(devList[devInd].baseAddr, devList[devInd].getState(0)), devList[devInd].chanCount(), sett.getStepCount()) );
    }
    //Msmt cycle
    uint32_t step=0;
    while(sett.getNextMsmt(timeOfInt, ntimes))
    {
//        std::cout << timeOfInt << std::endl;
        for(auto dev: devList)
            dev.setTimerReg(timeOfInt);

        for(int ii=0; ii<ntimes; ++ii)
        {
            for(auto dev: devList)
                dev.pstartAll();
            Sleep(int(devList[0].adcSamplingTime*timeOfInt*1000.0)+10);
            for(uint32_t devInd=0; devInd<devCount; ++devInd)
            {
                for(int chan=0; chan<devList[devInd].chanCount(); ++chan)
                {
                    float Integral = devList[devInd].readIntegral(chan);
                    res[devInd].resRows[step][chan].push_back(Integral);
                }
            }
        }
        //
        for(uint32_t devInd=0; devInd<devCount; ++devInd)
        {
            res[devInd].results["Time, s"].push_back(devList[devInd].adcSamplingTime*timeOfInt);
            for(int chan=0; chan<devList[devInd].chanCount(); ++chan)
            {
                    float _avg = 0.0, _std = 0.0;
                    _avg = VsDC3v2::avg(res[devInd].resRows[step][chan]);
                    _std = VsDC3v2::std(_avg, res[devInd].resRows[step][chan]);
                    std::string str;
                    str.clear();
                    str = "Chan" + std::to_string(chan) + "_avg";
                    res[devInd].results[str].push_back(_avg);
                    str.clear();
                    str = "Chan" + std::to_string(chan) + "_std";
                    res[devInd].results[str].push_back(_std);
            }
        }
        //
        step++;
    }
}

std::string VsdcIntNoiseTest::resToString(void)
{
    std::string str;
    for(auto ind=0ul; ind<res.size(); ++ind)
        str.append(resToString(ind));
}

std::string VsdcIntNoiseTest::resToString(uint32_t ind)
{
    std::string str;
    str.append(res[ind].msmt_title);
    str.append("\n");

    for(auto& x : res[ind].results)
    {
        str.append(x.first);
        str.append("\t");
    }
    str.append("\n");
    for(int step=0; step<res[ind].results["Time, s"].size(); ++step)
    {
        for(auto& x : res[ind].results)
        {
            std::vector<float> &st = x.second;
            str.append(std::to_string(st[step]));
            str.append("\t");
        }
        str.append("\n");
    }
    return str;
}

std::string VsdcIntNoiseTest::createFileName(uint32_t baseAddr, uint32_t rangeCode)
{
    std::stringstream cout;
    cout << "vsdc_0x" << std::hex << (baseAddr >> 24)
         << "_" << VsDC3v2::rangeName(rangeCode)
         << "_NI.txt";
    return cout.str();
}

void VsdcIntNoiseTest::saveDataToFile(std::string dir)
{
    for(auto ind=0ul; ind<res.size(); ++ind)
        saveDataToFile(dir, ind);
}

void VsdcIntNoiseTest::saveDataToFile(std::vector<QString> dir)
{
    for(auto ind=0ul; ind<res.size(); ++ind)
        saveDataToFile(dir[ind].toStdString(), ind);
}


void VsdcIntNoiseTest::saveDataToFile(std::string dir, uint32_t ind)
{
    dir.append(res[ind].msmt_title);
    FILE *stream0 = fopen(dir.c_str(), "w");
    if(stream0 == 0) throw std::string("saveDataToFile(): Problems with Files");
    for(auto ln=0ul; ln<res[ind].results["Time, s"].size(); ++ln)
    {
        fprintf(stream0, "%+.7G", res[ind].results["Time, s"][ln]);
        uint32_t chanCountTmp = ((res[ind].results.size()-1)/2);
        for(int chan=0; chan<chanCountTmp; ++chan)
        {
            std::string str;
            str.clear();
            str = "Chan" + std::to_string(chan) + "_avg";
            fprintf(stream0, "\t%+.7G", res[ind].results[str][ln]);
            str.clear();
            str = "Chan" + std::to_string(chan) + "_std";
            fprintf(stream0, "\t%+.7G", res[ind].results[str][ln]);
        }
        fprintf(stream0, "\n");
    }
    fclose(stream0);
}

std::string VsdcIntNoiseTest::createRawFileName(uint32_t baseAddr, uint32_t rangeCode)
{
    std::stringstream cout;
    cout << "vsdc_0x" << std::hex << (baseAddr << 24)
         << "_" << VsDC3v2::rangeName(rangeCode)
         << "_NI_tmp.txt";
    return cout.str();
}

void VsdcIntNoiseTest::saveRawDataToFile(std::string dir)
{
    for(auto ind=0ul; ind<res.size(); ++ind)
        saveRawDataToFile(dir, ind);
}

void VsdcIntNoiseTest::saveRawDataToFile(std::vector<QString> dir)
{
    for(auto ind=0ul; ind<res.size(); ++ind)
        saveRawDataToFile(dir[ind].toStdString(), ind);
}

void VsdcIntNoiseTest::saveRawDataToFile(std::string dir, uint32_t ind)
{
    dir.append(res[ind].msmt_title);
    dir.append(".tmp");
    FILE *stream0 = fopen(dir.c_str(), "w");
    if(stream0 == 0) throw std::string("saveDataToFile(): Problems with Files");
    uint32_t id=0;
    for(auto step=0ul; step<res[ind].resRows.size(); ++step)
    {
        for(auto col=0ul; col<res[ind].resRows[step][0].size(); ++col)
        {
            fprintf(stream0, "%d\t%+.7G", id++, res[ind].results["Time, s"][step]);
            for(auto chan=0ul; chan<res[ind].resRows[step].size(); ++chan)
            {
                fprintf(stream0, "\t%+.7G", res[ind].resRows[step][chan][col]);
            }
            fprintf(stream0, "\n");
        }
    }
    fclose(stream0);
}

//VsdcIntErrorTest
void VsdcIntErrorTest::doTest(VsDCTestConfig sett, std::vector<VsDC3v2> &devList)
{
    float timeOfInt;
    uint32_t ntimes;
    uint32_t devCount = devList.size();

    //configure vsdc
    for(auto dev: devList)
    {
        //startSrcPStart
        //stopSrcTimer
        //muxToGnd or muxToSig
        dev.setPrTimerReg(0);
    }

    //Prepare results tables
    std::vector<std::string> msmt_str;
    msmt_str.push_back("Time, s");
    msmt_str.push_back("Theor, Vs");//Difference!!
    res.clear();
    for(auto dev: devList/*uint32_t devInd=0; devInd<devCount; ++devInd*/)
    {
        res.push_back( VsDCTestResults(msmt_str, createFileName(dev.baseAddr, dev.getState(0)), dev.chanCount(), sett.getStepCount()) );
    }
    for(uint32_t devInd=0; devInd<devCount; ++devInd)//Difference!!
    {
        res[devInd].currRef = devList[devInd].currRefVoltage();//Difference!!
    }
    //Msmt cycle
    uint32_t step=0;
    while(sett.getNextMsmt(timeOfInt, ntimes))
    {
        for(auto dev: devList)
        {
            dev.setPrTimerReg(timeOfInt);//Difference!!
            dev.setTimerReg(timeOfInt*(dev.precTimerRate/dev.adcSamplingTime));//Difference!!
        }

        for(int ii=0; ii<ntimes; ++ii)
        {
            for(auto dev: devList)
                dev.pstartAll();
            Sleep(int(devList[0].precTimerRate*timeOfInt*1000.0)+10);//modified
            for(uint32_t devInd=0; devInd<devCount; ++devInd)
            {
                for(int chan=0; chan<devList[devInd].chanCount(); ++chan)
                {
                    float Integral = devList[devInd].readIntegral(chan);
                    res[devInd].resRows[step][chan].push_back(Integral);
                }
            }
        }
        //
        for(uint32_t devInd=0; devInd<devCount; ++devInd)
        {
            res[devInd].results["Time, s"].push_back(devList[devInd].precTimerRate*timeOfInt);//modified
            res[devInd].results["Theor, Vs"].push_back(devList[devInd].precTimerRate*timeOfInt*res[devInd].currRef);//added
            for(int chan=0; chan<devList[devInd].chanCount(); ++chan)
            {
                    float _avg = 0.0, _std = 0.0;
                    _avg = VsDC3v2::avg(res[devInd].resRows[step][chan]);
                    _std = VsDC3v2::std(_avg, res[devInd].resRows[step][chan]);
                    std::string str;
                    str.clear();
                    str = "Chan" + std::to_string(chan) + "_avg";
                    res[devInd].results[str].push_back(_avg);
                    str.clear();
                    str = "Chan" + std::to_string(chan) + "_std";
                    res[devInd].results[str].push_back(_std);
            }
        }
        //
        step++;
    }
    //flush settings
    for(auto dev: devList)
    {
        dev.setPrTimerReg(0);
    }
}
std::string VsdcIntErrorTest::resToString(void)
{
    std::string str;
    for(auto ind=0ul; ind<res.size(); ++ind)
        str.append(resToString(ind));
}
std::string VsdcIntErrorTest::resToString(uint32_t ind)
{
    std::string str;
    str.append(res[ind].msmt_title);
    str.append("\n");

    for(auto& x : res[ind].results)
    {
        str.append(x.first);
        str.append("\t");
    }
    str.append("\n");
    for(int step=0; step<res[ind].results["Time, s"].size(); ++step)
    {
        for(auto& x : res[ind].results)
        {
            std::vector<float> &st = x.second;
            str.append(std::to_string(st[step]));
            str.append("\t");
        }
        str.append("\n");
    }
    return str;
}

std::string VsdcIntErrorTest::createFileName(uint32_t baseAddr, uint32_t rangeCode)
{
    std::stringstream cout;
    cout << "vsdc_0x" << std::hex << (baseAddr >> 24)
         << "_" << VsDC3v2::rangeName(rangeCode)
         << "_TLT.txt";
    return cout.str();
}

void VsdcIntErrorTest::saveDataToFile(std::string dir)
{
    for(auto ind=0ul; ind<res.size(); ++ind)
        saveDataToFile(dir, ind);
}




void VsdcIntErrorTest::saveDataToFile(std::vector<QString> dir)
{
    for(auto ind=0ul; ind<res.size(); ++ind)
        saveDataToFile(dir[ind].toStdString(), ind);
}


void VsdcIntErrorTest::saveDataToFile(std::string dir, uint32_t ind)
{
    dir.append(res[ind].msmt_title);
    FILE *stream0 = fopen(dir.c_str(), "w");
    if(stream0 == 0) throw std::string("VsdcIntErrorTest::saveDataToFile(): Problems with Files");
    for(auto ln=0ul; ln<res[ind].results["Time, s"].size(); ++ln)
    {
        fprintf(stream0, "%+.7G\t%.7+G", res[ind].results["Time, s"][ln], res[ind].results["Theor, Vs"][ln]);//added results["Theor, Vs"]
        uint32_t chanCountTmp = ((res[ind].results.size()-2)/2);//modified 1 to 2
        for(int chan=0; chan<chanCountTmp; ++chan)
        {
            std::string str;
            str.clear();
            str = "Chan" + std::to_string(chan) + "_avg";
            fprintf(stream0, "\t%+.7G", res[ind].results[str][ln]);
            str.clear();
            str = "Chan" + std::to_string(chan) + "_std";
            fprintf(stream0, "\t%+.7G", res[ind].results[str][ln]);
        }
        fprintf(stream0, "\n");
    }
    fclose(stream0);
}

std::string VsdcIntErrorTest::createRawFileName(uint32_t baseAddr, uint32_t rangeCode)
{
    std::stringstream cout;
    cout << "vsdc_0x" << std::hex << baseAddr
         << "_" << VsDC3v2::rangeName(rangeCode)
         << "_TLT_tmp.txt";
    return cout.str();
}

void VsdcIntErrorTest::saveRawDataToFile(std::string dir)
{
    for(auto ind=0ul; ind<res.size(); ++ind)
        saveRawDataToFile(dir, ind);
}


void VsdcIntErrorTest::saveRawDataToFile(std::vector<QString> dir)
{
    for(auto ind=0ul; ind<res.size(); ++ind)
        saveRawDataToFile(dir[ind].toStdString(), ind);
}


void VsdcIntErrorTest::saveRawDataToFile(std::string dir, uint32_t ind)
{
    dir.append(res[ind].msmt_title);
    dir.append(".tmp");
    FILE *stream0 = fopen(dir.c_str(), "w");
    if(stream0 == 0) throw std::string("VsdcIntErrorTest::saveRawDataToFile(): Problems with Files");
    uint32_t id=0;
    for(auto step=0ul; step<res[ind].resRows.size(); ++step)
    {
        for(auto col=0ul; col<res[ind].resRows[step][0].size(); ++col)
        {
            fprintf(stream0, "%d\t%+.7G\t%+.7G", id++, res[ind].results["Time, s"][step], res[ind].results["Theor, Vs"][step]);//added results["Theor, Vs"]
            for(auto chan=0ul; chan<res[ind].resRows[step].size(); ++chan)
            {
                fprintf(stream0, "\t%+.7G", res[ind].resRows[step][chan][col]);
            }
            fprintf(stream0, "\n");
        }
    }
    fclose(stream0);
}
