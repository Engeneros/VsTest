#include "datafromfileqt.h"

DataFromFileQt::DataFromFileQt() : DataProviderBase()
{
    _fil_path.clear();
    _fil_is_open = false;
    inStr = NULL;
    fwFile = NULL;
}

DataFromFileQt::DataFromFileQt(QString path) : DataFromFileQt()
{
    _fil_path.append(path);
}

DataFromFileQt::~DataFromFileQt()
{
    closeFile();
}

int DataFromFileQt::SetFilePath(QString path)
{
    _fil_path.clear();
    _fil_path.append(path);
    return 0;
}
bool DataFromFileQt:: validate_fil_path(void)
{
    //TODO
    return (_fil_path.size() != 0);
}

int DataFromFileQt::getFileSize(void)
{
    int ret = -1;
    if(_fil_is_open)
    {
        ret = fwFile->size();
    }
    else
    {
        if(openFile() != 0) return -1;
        ret = fwFile->size();
        if(closeFile() != 0) return -1;
    }
    return ret;
}

int DataFromFileQt::openFile(void)
{
    if(!validate_fil_path())
        return -1;
    //
    if(!_fil_is_open)
    {
        fwFile = new QFile(_fil_path);
        if(fwFile->open(QIODevice::ReadWrite))
        {
            inStr = new QDataStream(fwFile);
            _fil_is_open = true;
        }
        else
        {
            delete fwFile;
            fwFile = NULL;
        }
    }
    //
    if(_fil_is_open)
        return 0;
    else
        return -1;
}

int DataFromFileQt::closeFile(void)
{
    if(_fil_is_open)
    {
        fwFile->close();
        _fil_is_open = false;
        delete fwFile;
        fwFile = NULL;
        delete inStr;
        inStr = NULL;
    }
    return 0;
}

int DataFromFileQt::flushFile(void)
{
    bool was_open = _fil_is_open;
    if(_fil_is_open)
        closeFile();

    if(!validate_fil_path())
        return -1;

    fwFile = new QFile(_fil_path);

    if(fwFile == NULL)
        return -1;

    if(fwFile->remove())
    {
        if(was_open)
            return openFile();
        else
            return 0;
    }
    return -1;
}

int DataFromFileQt::setReadPosition(int byte_pos)
{
    return -1;
}

int DataFromFileQt::getFileChunk(int _bytes, uint8_t *pbuff)
{
    if(_fil_is_open && (inStr->readRawData((char*)pbuff, _bytes) == _bytes))
    {
        return 0;
    }
    return -1;
}

int DataFromFileQt::saveFileChunk(int _bytes, uint8_t *pbuff)
{
    if(_fil_is_open && (inStr->writeRawData((const char*)pbuff, _bytes) == _bytes))
        return 0;
    return -1;
}
