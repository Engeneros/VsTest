#ifndef VME_CARDS_UPDATER_H
#define VME_CARDS_UPDATER_H

#include <stdint.h>
#include <string>
#include <sstream>
#include <vector>

#include "vmecardbaseport.h"
#include "dataprovider.h"
#include "typupdateruserbase.h"

enum ReproRegs
{
    EPCS_SIZE		 = 0,
    EPCS_SECTOR_SIZE = 1,
    EPCS_BUFF_SIZE   = 2,
    EPCS_ADDR        = 3,
    EPCS_BUFF_RD_ADDR= 4,
    EPCS_BUFF_WR_ADDR= 5,
    EPCS_BUFF_DATA	 = 6,
    EPCS_CMD		 = 7
};

enum ReproCmd
{
    EPCS_CMD_RD		= 0x1,
    EPCS_CMD_WR		= 0x2,
    EPCS_CMD_RD_ERR	= 0x4,
    EPCS_CMD_WR_ERR	= 0x8
};

enum ReproState
{
    EPCS_ST_BUSY    = 0x3,
    EPCS_FAIL_MASK  = 0xC
};

class vmeCardsUpdater
{
public:
    vmeCardsUpdater();
    vmeCardsUpdater(VmeCardBasePort * _pvmePort, TypUpdaterUserBase *_pTypUser = NULL);
    void setVmeCardPort(VmeCardBasePort * _pvmePort)
    {
        pvmePort = _pvmePort;
    }
    void setUpdaterUserI(TypUpdaterUserBase *_pTypUser)
    {
        pTypUser = _pTypUser;
    }
    void setReproRegsBase(uint32_t _byte_addr);
    void writeFirmware(DataProviderBase & _file);
    void readFirmware(DataProviderBase & _file, uint32_t _size = 0);
    bool compareFirmware(DataProviderBase & _file);
    std::string getFailedInfoString(void);

private:
    VmeCardBasePort * pvmePort;
    TypUpdaterUserBase * pTypUser;
    uint32_t reproRegsBaseAddr;
    uint32_t flashSize;
    uint32_t flashSectorSize;
    uint32_t flashPageSize;
    uint32_t flashState;

    uint32_t fwWrSize;
    uint32_t fwWrAddr;
    uint32_t flashWrPage;
    std::string pstr;
    bool flashSettingsLoaded;

    DataProviderBase * pwrFile;
    bool cmpIsEqual;
    uint32_t    failAddr;
    uint8_t     failReadedData;
    uint8_t     failShouldBeData;

    bool isInitialaized(void)
    {
        return ((pvmePort != NULL) && (reproRegsBaseAddr != -1ul));
    }
    bool flashInitialized(void)
    {
        return flashSettingsLoaded;
    }
    uint32_t regNumToAddr(uint32_t _rNum)
    {
        return reproRegsBaseAddr | (_rNum << 2);
    }
    uint32_t pageNumToAddr(uint32_t _page_num);
    void getFlashState(void);
    bool isFlashNoError(void);
    bool isFlashNotBusy(void);
    void loadFlashSettings(void);
    void waitFlashIsNotBusy(void);
    void readFlashPage(uint32_t _page_num, std::vector<uint32_t> &rdata);
    void writeFlashPage(uint32_t _page_num, std::vector<uint32_t> &wdata);

    void setBuffWithZero(std::vector<uint32_t> & buf);
    void reorderEPCSbits(uint8_t *dt);
    void reorderEPCSbuff(std::vector<uint32_t> & buf, uint32_t sz);
    uint32_t getRdPortionFromRemaining(const uint32_t & remains);
    bool validateFlashSettings(void);
    void flushFlashSettings(void);

    void setCurrentError(std::string & str)
    {
        pstr = str;
    }
    bool is_pow_of_two(uint32_t & val);
    //write steps functions
    void initWrFwSrc(DataProviderBase * pfile);
    void finitWrProc(void);
    bool doWrFwIter(void);
    uint32_t progressInPercent(void);
    //read steos functions
    void initRdFw(DataProviderBase * pfile);
    void finitRdProc(void);
    bool doRdIter(void);
    //compare step functions
    void initCmpFw(DataProviderBase * pfile);
    void finitCmpProc(void);
    bool doCmpIter(void);

};

#endif // VME_CARDS_UPDATER_H
