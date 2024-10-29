#ifndef VSDCCARDTEST_H
#define VSDCCARDTEST_H

#include <stdint.h>
#include <iostream>
#include <vector>
#include <map>
#include "vsdc3v2.h"

class VsDCTestConfig
{
public:
    VsDCTestConfig();
    void configureMsmts(uint32_t msmtPerStep, uint32_t startDecade, uint32_t decadeNum, uint32_t stepPerDecade);
    bool getNextMsmt(float & timeOfInt, uint32_t & ntimes);
    std::string confToString(void);
    uint32_t getStepCount(void);
    friend std::ostream& operator<<(std::ostream& os, VsDCTestConfig& dt);
private:
    float firstDecadeTime(void);
    std::vector<float> integrationTimes;
    uint32_t nTimes;
    uint32_t stepsCount;
    uint32_t currInd;
};

class VsDCTestResults
{
public:
    VsDCTestResults(std::vector<std::string> &colNames, std::string msmtName, uint32_t chCount, uint stepCount)
    {
        msmt_title.assign(msmtName);
        //
        std::string str;
        for(int chan=0; chan<chCount; ++chan)
        {
            str = "Chan" + std::to_string(chan) + "_avg";
            colNames.push_back(str);
            str.clear();
            str = "Chan" + std::to_string(chan) + "_std";
            colNames.push_back(str);
            str.clear();
        }
        for(auto cols: colNames)
        {
            results[cols] = std::vector<float>();
        }
        //
        for(int jj=0; jj<stepCount; ++jj)
        {
            resRows.push_back(std::vector<std::vector<float>>());
            for(int chan=0; chan<chCount; ++chan)
            {
                resRows[jj].push_back(std::vector<float>());
            }
        }
    }
    std::map<std::string, std::vector<float>> results;
    std::vector<std::vector<std::vector<float>>> resRows;
    std::string msmt_title;
    float currRef;
};

class VsdcIntNoiseTest
{
public:
    void doTest(VsDCTestConfig sett, std::vector<VsDC3v2> &devList);
    std::string resToString(void);
    std::string resToString(uint32_t ind);
    std::string createFileName(uint32_t baseAddr, uint32_t rangeCode);
    void saveDataToFile(std::string dir);
    void saveDataToFile(std::vector<QString> dir);
    void saveDataToFile(std::string dir, uint32_t ind);
    std::string createRawFileName(uint32_t baseAddr, uint32_t rangeCode);
    void saveRawDataToFile(std::string dir);
    void saveRawDataToFile(std::vector<QString> dir);
    void saveRawDataToFile(std::string dir, uint32_t ind);
protected:
    std::vector<VsDCTestResults>    res;
//    std::map<std::string, std::vector<float>> results;
//    std::vector<std::vector<std::vector<float>>> resRows;
//    std::string msmt_title;
};


class VsdcIntErrorTest
{
public:
    void doTest(VsDCTestConfig sett, std::vector<VsDC3v2> &devList);
    std::string resToString(void);
    std::string resToString(uint32_t ind);
    std::string createFileName(uint32_t baseAddr, uint32_t rangeCode);
    void saveDataToFile(std::string dir);
    void saveDataToFile(std::vector<QString> dir);
    void saveDataToFile(std::string dir, uint32_t ind);
    std::string createRawFileName(uint32_t baseAddr, uint32_t rangeCode);
    void saveRawDataToFile(std::string dir);
    void saveRawDataToFile(std::vector<QString> dir);
    void saveRawDataToFile(std::string dir, uint32_t ind);
protected:
    std::vector<VsDCTestResults>    res;

};


#endif // VSDCCARDTEST_H
