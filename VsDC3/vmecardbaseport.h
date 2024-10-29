#ifndef VMECARDBASEPORT_H
#define VMECARDBASEPORT_H

#include <vector>
#include <stdint.h>

//template<typename RegVType>
class VmeCardBasePort
{
public:
    VmeCardBasePort();

    virtual void setBaseAddr(uint32_t _baseAddr, uint32_t _devInd) = 0;
    virtual void vmeWriteReg(uint32_t addr, uint32_t wdata, uint32_t _devInd) = 0;
    virtual uint32_t vmeReadReg(uint32_t addr, uint32_t _devInd) = 0;

    virtual void setBaseAddr(uint32_t _baseAddr);
    virtual void vmeWriteReg(uint32_t addr, uint32_t wdata);
    virtual void vmeWriteMem(uint32_t addr, std::vector<uint32_t> & pwdata);
    virtual void vmeWriteRegMulty(uint32_t addr, std::vector<uint32_t> & pwdata);
    virtual uint32_t vmeReadReg(uint32_t addr);
    virtual void vmeReadMem(uint32_t addr, std::vector<uint32_t> & prdata);
    virtual void vmeReadRegMulty(uint32_t addr, std::vector<uint32_t> & prdata);

    virtual void vmeReadMem(uint32_t addr, std::vector<uint32_t> & prdata, uint32_t _devInd);
    virtual void vmeReadRegMulty(uint32_t addr, std::vector<uint32_t> & prdata, uint32_t _devInd);
    virtual void vmeWriteMem(uint32_t addr, std::vector<uint32_t> & pwdata, uint32_t _devInd);
    virtual void vmeWriteRegMulty(uint32_t addr, std::vector<uint32_t> & pwdata, uint32_t _devInd);

    virtual void vmeRdMemFirst(uint32_t addr, uint32_t nextRdCount, uint32_t _devInd) = 0;
    virtual void vmeRdMemLast(uint32_t addr, std::vector<uint32_t> &prdata, uint32_t _devInd) = 0;
    virtual void vmeRdMemMiddle(uint32_t addr, uint32_t nextRdCount, std::vector<uint32_t> &prdata, uint32_t _devInd) = 0;
//    union{
//        float fl;
//        uint32_t ut;
//    } readVal;
//    union{
//        float fl;
//        uint32_t ut;
//    } wrVal;
    //private:
};

#endif // VMECARDBASEPORT_H
