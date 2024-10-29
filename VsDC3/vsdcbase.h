#ifndef VSDCBASE_H
#define VSDCBASE_H

#include <stdint.h>

//class regFields
//{
//public:
//    vsdcRegFields()
//    {
//        mask =0;
//        offset = 0;
//    }
//    std::map<std::string, uint32_t> regFieldsVals;
//    uint32_t mask;
//    uint32_t offset;
//    std::string settings;
//    void setSettings(std::string settName)
//    {
//        if(regFieldsVals.count(settName))
//            settings = settName;
//        else throw "regFields: No key fpund!";
//    }
//    uint32_t settsToRegVal()
//    {
//        return regFieldsVals[settings] << offset;
//    }
//    std::string getSettName(uint32_t regVal)
//    {
//        auto val = (regVal >> offset) & mask;
//        auto itr = regFieldsVals.find(val);
//        return itr->first;
//    }
//};

//class regBase
//{
//public:
//    regBase();
//    std::vector<vsdcRegFields> _regFields;
//    uint32_t regVal(void)
//    {
//        uint32_t tmp = 0;
//        for(auto value : _regFields)
//        {
//            tmp |= value.settsToRegVal();
//        }
//        return tmp;
//    }
//};

class vsdcBase
{
public:
    vsdcBase();
    virtual void pstartAll(void) = 0;
    virtual void pstart(uint32_t ch) = 0;
    virtual void pstopAll(void) = 0;
    virtual void pstop(uint32_t ch) = 0;
    virtual void calibAll(void) = 0;
    virtual void calib(uint32_t ch) = 0;
    virtual float readIntegral(uint32_t ch) = 0;
    virtual void setInputMux() = 0;
    virtual float getTimeQuant() = 0;
    virtual float getPrTimerQuant() = 0;
    virtual void setTimer(uint32_t count) = 0;
    virtual void setTimer(uint32_t count, uint32_t ch) = 0;
    virtual void setPrecisionTimer(uint32_t count) = 0;
    virtual void setPrecisionTimer(uint32_t count, uint32_t ch) = 0;
    virtual uint32_t chanCount(void) = 0;
    virtual uint32_t GetUid() = 0;
};

class VsDCinRam : public vsdcBase
{
public:
    VsDCinRam();
     void pstartAll(void);
     void pstart(uint32_t ch);
     void pstopAll(void);
     void pstop(uint32_t ch);
     void calibAll(void);
     void calib(uint32_t ch);
     float readIntegral(uint32_t ch);
     void setInputMux();
     float getTimeQuant();
     float getPrTimerQuant();
     void setTimer(uint32_t count);
     void setTimer(uint32_t count, uint32_t ch);
     void setPrecisionTimer(uint32_t count);
     void setPrecisionTimer(uint32_t count, uint32_t ch);
     uint32_t chanCount(void);
     uint32_t GetUid();
};

#endif // VSDCBASE_H
