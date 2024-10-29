#include "vmecardbaseport.h"

VmeCardBasePort::VmeCardBasePort()
{

}

void VmeCardBasePort::vmeWriteReg(uint32_t addr, uint32_t wdata)
{
    vmeWriteReg(addr, wdata, 0);
}

uint32_t VmeCardBasePort::vmeReadReg(uint32_t addr)
{
    return  vmeReadReg(addr, 0);
}

void VmeCardBasePort::setBaseAddr(uint32_t _baseAddr)
{
    setBaseAddr(_baseAddr, 0);
}

void VmeCardBasePort::vmeWriteMem(uint32_t addr, std::vector<uint32_t> &pwdata)
{
    vmeWriteMem(addr, pwdata, 0);
}
void VmeCardBasePort::vmeWriteRegMulty(uint32_t addr, std::vector<uint32_t> &pwdata)
{
    vmeWriteRegMulty(addr, pwdata, 0);
}
void VmeCardBasePort::vmeReadMem(uint32_t addr, std::vector<uint32_t> &prdata)
{
    vmeReadMem(addr, prdata, 0);
}
void VmeCardBasePort::vmeReadRegMulty(uint32_t addr, std::vector<uint32_t> &prdata)
{
    vmeReadRegMulty(addr, prdata, 0);
}

void VmeCardBasePort::vmeWriteMem(uint32_t addr, std::vector<uint32_t> &pwdata, uint32_t devInd)
{
    for(uint32_t ii=0; ii<pwdata.size(); ++ii)
    {
        vmeWriteReg(addr, pwdata[ii], devInd);
        addr += 4;
    }
}

void VmeCardBasePort::vmeWriteRegMulty(uint32_t addr, std::vector<uint32_t> &pwdata, uint32_t devInd)
{
    for(uint32_t ii=0; ii<pwdata.size(); ++ii)
        vmeWriteReg(addr, pwdata[ii], devInd);

}

void VmeCardBasePort::vmeReadMem(uint32_t addr, std::vector<uint32_t> &prdata, uint32_t devInd)
{
    for(uint32_t ii=0; ii<prdata.size(); ++ii)
    {
        prdata[ii] = vmeReadReg(addr, devInd);
        addr += 4;
    }
}

void VmeCardBasePort::vmeReadRegMulty(uint32_t addr, std::vector<uint32_t> &prdata, uint32_t devInd)
{
    for(uint32_t ii=0; ii<prdata.size(); ++ii)
    {
        prdata[ii] = vmeReadReg(addr, devInd);
    }
}
