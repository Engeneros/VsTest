#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include <stdint.h>

class DataProviderBase
{
public:
    DataProviderBase();
    virtual int getFileSize(void) = 0;
    virtual int openFile(void) = 0;
    virtual int closeFile(void) = 0;
    virtual int setReadPosition(int byte_pos) = 0;
    virtual int flushFile(void) = 0;
    virtual int getFileChunk(int _bytes, uint8_t *pbuff) = 0;
    virtual int saveFileChunk(int _bytes, uint8_t *pbuff) = 0;
};

#endif // DATAPROVIDER_H
