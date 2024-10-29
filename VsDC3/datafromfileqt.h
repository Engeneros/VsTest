#ifndef DATAFROMFILEQT_H
#define DATAFROMFILEQT_H

#include "dataprovider.h"
#include <QFile>
#include <qstring.h>
#include <QDataStream>

class DataFromFileQt : public DataProviderBase
{
public:
    DataFromFileQt();
    DataFromFileQt(QString path);
    ~DataFromFileQt();

    int SetFilePath(QString path);
    int getFileSize(void);
    int openFile(void);
    int closeFile(void);
    int setReadPosition(int byte_pos);
    int flushFile(void);
    int getFileChunk(int _bytes, uint8_t *pbuff);
    int saveFileChunk(int _bytes, uint8_t *pbuff);

private:
    QString _fil_path;
    bool _fil_is_open;
    QDataStream * inStr;
    QFile   *   fwFile;

    bool validate_fil_path();
};

#endif // DATAFROMFILEQT_H
