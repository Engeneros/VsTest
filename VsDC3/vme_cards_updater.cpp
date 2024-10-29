#include "vme_cards_updater.h"
#include "assert.h"

vmeCardsUpdater::vmeCardsUpdater()
{
    pTypUser = NULL;
    pvmePort = NULL;
    reproRegsBaseAddr = -1ul;

    flashSize = -1ul;
    flashSectorSize = -1ul;
    flashPageSize = -1ul;
    fwWrSize = -1ul;
    flashSettingsLoaded = false;

    fwWrAddr = 0;
    flashWrPage = 0;
    pstr = "No Error";

    pwrFile = NULL;
    cmpIsEqual = false;
}

vmeCardsUpdater::vmeCardsUpdater(VmeCardBasePort * _pvmePort, TypUpdaterUserBase * _pTypUser) : vmeCardsUpdater()
{
    pvmePort = _pvmePort;
    pTypUser = _pTypUser;
}

void vmeCardsUpdater::setReproRegsBase(uint32_t _byte_addr)
{
    reproRegsBaseAddr = _byte_addr;
}

void vmeCardsUpdater::getFlashState(void)
{
    if(!isInitialaized())
        throw std::string("getFlashState: updater is not initialaized");
    flashState = pvmePort->vmeReadReg(regNumToAddr(EPCS_CMD));
}

bool vmeCardsUpdater::isFlashNotBusy(void)
{
   return !(flashState & EPCS_ST_BUSY);
}

bool vmeCardsUpdater::isFlashNoError(void)
{
    return !(flashState & EPCS_FAIL_MASK);
}

void vmeCardsUpdater::loadFlashSettings(void)
{
    if(!isInitialaized())
        throw std::string("loadFlashSettings: updater is not initialaized");
    if(flashInitialized())
        return;

    flashSize = pvmePort->vmeReadReg(regNumToAddr(EPCS_SIZE));
    flashSectorSize = pvmePort->vmeReadReg(regNumToAddr(EPCS_SECTOR_SIZE));
    flashPageSize = pvmePort->vmeReadReg(regNumToAddr(EPCS_BUFF_SIZE));
    pvmePort->vmeWriteReg(regNumToAddr(EPCS_CMD), uint32_t(EPCS_FAIL_MASK));

    if(validateFlashSettings())
    {
        flashSettingsLoaded = true;
        return;
    }

    flushFlashSettings();
    throw std::string("loadFlashSettings:badSettings!");
    return;
}

bool  vmeCardsUpdater::is_pow_of_two(uint32_t & val)
{
    return ((val & (val-1)) == 0);
}

bool vmeCardsUpdater::validateFlashSettings(void)
{
    return      (flashSize != 0)
            &&  (flashSectorSize != 0)
            &&  (flashPageSize != 0)
            &&  is_pow_of_two(flashSize)
            &&  is_pow_of_two(flashSectorSize)
            &&  is_pow_of_two(flashPageSize)
            &&  (flashSectorSize % flashPageSize == 0)
            &&  (flashSize % flashSectorSize == 0);
}

void vmeCardsUpdater::flushFlashSettings(void)
{
    flashSize = -1ul;
    flashSectorSize = -1ul;
    flashPageSize = -1ul;
    flashSettingsLoaded = false;
}

uint32_t vmeCardsUpdater::pageNumToAddr(uint32_t _page_num)
{
    return _page_num*flashPageSize*sizeof(uint32_t);
}

void vmeCardsUpdater::waitFlashIsNotBusy(void)
{
    uint32_t cnt = 10000;
    do
    {
        getFlashState();
        if(--cnt == 0)
            throw std::string("vmeCardsUpdater::waitFlashIsNotBusy() timeout");
    }
    while(!isFlashNotBusy());
    if(!isFlashNoError())
        throw std::string("vmeCardsUpdater::waitFlashIsNotBusy() flash error detected!");
    return;
}

void vmeCardsUpdater::readFlashPage(uint32_t _page_num, std::vector<uint32_t> & rdata)
{
    assert(isInitialaized() && flashInitialized());

    pvmePort->vmeWriteReg(regNumToAddr(EPCS_ADDR), pageNumToAddr(_page_num));
    pvmePort->vmeWriteReg(regNumToAddr(EPCS_BUFF_RD_ADDR), 0);
    pvmePort->vmeWriteReg(regNumToAddr(EPCS_CMD), EPCS_CMD_RD);
    waitFlashIsNotBusy();
    pvmePort->vmeReadRegMulty(regNumToAddr(EPCS_BUFF_DATA), rdata);

    return;
}

void vmeCardsUpdater::writeFlashPage(uint32_t _page_num, std::vector<uint32_t> & wdata)
{
    assert(isInitialaized() && flashInitialized());
    pvmePort->vmeWriteReg(regNumToAddr(EPCS_ADDR), pageNumToAddr(_page_num));
    pvmePort->vmeWriteReg(regNumToAddr(EPCS_BUFF_WR_ADDR), 0);
    pvmePort->vmeWriteRegMulty(regNumToAddr(EPCS_BUFF_DATA), wdata);
    pvmePort->vmeWriteReg(regNumToAddr(EPCS_CMD), EPCS_CMD_WR);
    waitFlashIsNotBusy();
    return;
}

void vmeCardsUpdater::setBuffWithZero(std::vector<uint32_t> &buf)
{
    for (uint32_t ii = 0; ii < buf.size(); ii++)
    {
        buf[ii] = -1ul;
    }
}

void    vmeCardsUpdater::reorderEPCSbits(uint8_t  *dt)
{
    uint8_t tmp_dt_out = 0;
    uint8_t tmp_dt_in = *dt;

    for(int ii=0;ii<7;ii++){
        if(tmp_dt_in & 1)
            tmp_dt_out += 1;
        tmp_dt_out <<= 1;
        tmp_dt_in >>= 1;
    }
    if(tmp_dt_in & 1)
        tmp_dt_out += 1;
    *dt = tmp_dt_out;
}

void vmeCardsUpdater::reorderEPCSbuff(std::vector<uint32_t> &buf, uint32_t sz)
{
    for(uint32_t ii=0; ii<sz; ii++)
    {
        reorderEPCSbits( ((uint8_t*)(buf.data())) + ii );
    }
}

void vmeCardsUpdater::initWrFwSrc(DataProviderBase * pfile)
{
    assert(pfile != NULL);
    pwrFile = pfile;
    loadFlashSettings();

    if(pwrFile->openFile() != 0) throw std::string("vmeCardsUpdater::initWrFwSrc() Can`t Open a File");
    fwWrSize = pwrFile->getFileSize();
    //
    if( fwWrSize == 0 ) throw std::string("vmeCardsUpdater::initWrFwSrc() File size is zero");
    fwWrAddr=fwWrSize;
    flashWrPage = 0;
    return;
}

bool vmeCardsUpdater::doWrFwIter(void)
{
    if( fwWrAddr == 0)
    {
        return false;
    }
    std::vector<uint32_t> read_mass(flashPageSize);
    uint32_t rdSize = getRdPortionFromRemaining(fwWrAddr);
    setBuffWithZero(read_mass);
    pwrFile->getFileChunk(rdSize, (uint8_t *)read_mass.data());
    reorderEPCSbuff(read_mass, rdSize);
    writeFlashPage(flashWrPage, read_mass);
    fwWrAddr -= rdSize;
    ++flashWrPage;
    return true;
}


void vmeCardsUpdater::finitWrProc(void)
{
    pwrFile->closeFile();
}

uint32_t vmeCardsUpdater::progressInPercent(void)
{
    return (100*(fwWrSize - fwWrAddr))/fwWrSize;
}

uint32_t vmeCardsUpdater::getRdPortionFromRemaining(const uint32_t & remains)
{
    //
    if(remains >= flashPageSize*sizeof(uint32_t))
        return flashPageSize*sizeof(uint32_t);
    else
        return remains;
}

void vmeCardsUpdater::writeFirmware(DataProviderBase & _file)
{
    initWrFwSrc(&_file);
    while(doWrFwIter())
    {
        if(pTypUser != NULL)
        {
            pTypUser->updateProgress(progressInPercent());
            if(pTypUser->abortTransaction())
            {
                break;
            }
        }
    }
    finitWrProc();
    return;
}

void vmeCardsUpdater::initRdFw(DataProviderBase * pfile)
{
    assert(pfile != NULL);
    pwrFile = pfile;
    loadFlashSettings();
    if(pwrFile->flushFile() != 0) throw std::string("vmeCardsUpdater::readFirmware(): Can't Flush file");
    if(pwrFile->openFile() != 0) throw std::string("vmeCardsUpdater::readFirmware(): Can't open a file");
    flashWrPage = 0;
    return;
}

void vmeCardsUpdater::finitRdProc(void)
{
    pwrFile->closeFile();
}

bool vmeCardsUpdater::doRdIter(void)
{
    if( fwWrAddr == 0)
    {
        return false;
    }
    std::vector<uint32_t> read_mass(flashPageSize);
    uint32_t rdSize = getRdPortionFromRemaining(fwWrAddr);
    setBuffWithZero(read_mass);
    readFlashPage(flashWrPage, read_mass);
    reorderEPCSbuff(read_mass, rdSize);
    pwrFile->saveFileChunk(rdSize, (uint8_t *)read_mass.data());
    fwWrAddr -= rdSize;
    ++flashWrPage;
    return true;
}

void vmeCardsUpdater::readFirmware(DataProviderBase & _file, uint32_t _size)
{
    if(_size==0) return;
    initRdFw(&_file);
    fwWrAddr=_size;
    while(doRdIter())
    {
        if(pTypUser != NULL)
        {
            pTypUser->updateProgress(progressInPercent());
            if(pTypUser->abortTransaction())
            {
                break;
            }
        }
    }
    finitRdProc();
    return;
}

void vmeCardsUpdater::initCmpFw(DataProviderBase * pfile)
{
    assert(pfile != NULL);
    pwrFile = pfile;
    loadFlashSettings();
    if(pwrFile->openFile() != 0) throw std::string("vmeCardsUpdater::compareFirmware(): Can't open a file");
    fwWrSize = pwrFile->getFileSize();
    //
    if( fwWrSize == 0 ) throw std::string("vmeCardsUpdater::compareFirmware(): File size is zero");
    //
    fwWrAddr=fwWrSize;
    flashWrPage = 0;
    cmpIsEqual = false;
}

void vmeCardsUpdater::finitCmpProc(void)
{
    pwrFile->closeFile();
}

bool vmeCardsUpdater::doCmpIter(void)
{
    if( fwWrAddr == 0)
    {
        cmpIsEqual = true;
        return false;
    }
    uint32_t rdSize = getRdPortionFromRemaining(fwWrAddr);
    std::vector<uint32_t> read_mass(flashPageSize);
    std::vector<uint32_t> load_mass(flashPageSize);

    readFlashPage(flashWrPage, read_mass);
    reorderEPCSbuff(read_mass, rdSize);
    pwrFile->getFileChunk(rdSize, (uint8_t *)load_mass.data());
    //
    uint32_t ii;
    for (ii = 0; ii < rdSize; ++ii)
        if( ((uint8_t *)read_mass.data())[ii] != ((uint8_t *)load_mass.data())[ii] )
            break;

    if(ii != rdSize)
    {
        failAddr = pageNumToAddr(flashWrPage) + ii;
        failReadedData      = ((uint8_t *)read_mass.data())[ii];
        failShouldBeData    = ((uint8_t *)load_mass.data())[ii];
        cmpIsEqual = false;
        return false;
    }
    //
    fwWrAddr -= rdSize;
    ++flashWrPage;
    return true;
}

bool vmeCardsUpdater::compareFirmware(DataProviderBase &_file)
{
    initCmpFw(&_file);
    while(doCmpIter())
    {
        if(pTypUser != NULL)
        {
            pTypUser->updateProgress(progressInPercent());
            if(pTypUser->abortTransaction())
            {
                break;
            }
        }
    }
    finitCmpProc();
    return cmpIsEqual;
}

std::string vmeCardsUpdater::getFailedInfoString(void)
{
    if(cmpIsEqual)
    {
         return std::string("Firmware is Ok!");
    }
    else
    {
        std::ostringstream oss;
        oss << "Failed:\taddr[" << failAddr << "]\t" << (int)failReadedData << " != " << (int)failShouldBeData;
        std::string var = oss.str();
        return var;
    }
}
