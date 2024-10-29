#include "movingcoilsvmecardport.h"
#include <QtEndian>

using namespace MovingCoilsVmeCardPortNS;

MovingCoilsVmeCardPort::MovingCoilsVmeCardPort() : VmeCardBasePort()
{
    connectTimeout = 2000;
    disconnectTimeout = 2000;
    writeTimeout = 2000;
    readTimeout = 2000;
}


MovingCoilsVmeCardPort::~MovingCoilsVmeCardPort()
{
//    disconnect();
}

bool MovingCoilsVmeCardPort::isConnected()
{
    return mSocket.state() == QAbstractSocket::ConnectedState;
}

bool MovingCoilsVmeCardPort::isUnconnected()
{
    return mSocket.state() == QAbstractSocket::UnconnectedState;
}

bool MovingCoilsVmeCardPort::ctl_connect()
{
    mSocket.connectToHost(mServerIp, mServerPort);
    if (!mSocket.waitForConnected(connectTimeout))
    {
//        qCritical(tr("Connection was failed: %s").toLocal8Bit().constData(),
//                  mSocket.errorString().toLocal8Bit().constData());
        return false;
    }
    return true;
}

bool MovingCoilsVmeCardPort::ctl_connect(const QString &address, quint16 port)
{
    return ctl_connect(QHostAddress(address), port);
}

bool MovingCoilsVmeCardPort::ctl_connect(const QHostAddress &address, quint16 port)
{
    setServerIp(address);
    setServerPort(port);
    return ctl_connect();
}

bool MovingCoilsVmeCardPort::ctl_disconnect()
{
    if (!isConnected()) return true;

    //    sendDisconnectPackage();
    //    if (isUnconnected()) return true;

    kill_vsdc3_server();

    mSocket.disconnectFromHost();
    if (!isUnconnected() && (!mSocket.waitForDisconnected(disconnectTimeout)))
    {
//        qWarning(tr("Disconnection was failed: %s").toLocal8Bit().constData(),
//                 mSocket.errorString().toLocal8Bit().constData());
        return false;
    }
    return true;
}


QHostAddress MovingCoilsVmeCardPort::serverIp() const
{
    return mServerIp;
}

void MovingCoilsVmeCardPort::setServerIp(const QHostAddress &serverIp)
{
    mServerIp = serverIp;
}

void MovingCoilsVmeCardPort::setServerIp(const QString &serverIp)
{
    mServerIp.setAddress(serverIp);
}

quint16 MovingCoilsVmeCardPort::serverPort() const
{
    return mServerPort;
}

void MovingCoilsVmeCardPort::setServerPort(quint16 serverPort)
{
    mServerPort = serverPort;
}

void MovingCoilsVmeCardPort::to_network_long(QVector<quint32> &buf, int beginIndex, int endIndex)
{
    beginIndex = ((beginIndex<0) ? (0)          : (beginIndex));
    endIndex   = ((endIndex<0)   ? (buf.size()) : (endIndex));

    for (int i=beginIndex; i<endIndex; i++)
    {
        buf[i] = qToBigEndian<quint32>(buf.at(i));
    }
}

void MovingCoilsVmeCardPort::from_network_long(QVector<quint32> &buf, int beginIndex, int endIndex)
{
    beginIndex = ((beginIndex<0) ? (0)          : (beginIndex));
    endIndex   = ((endIndex<0)   ? (buf.size()) : (endIndex));

    for (int i=beginIndex; i<endIndex; i++)
    {
        buf[i] = qFromBigEndian<quint32>(buf.at(i));
    }
}

int MovingCoilsVmeCardPort::writeBuf(QVector<quint32> &buf)
{
    int res;
    int offset = 0;
    char* sendbuf = (char*)buf.constData();
    int buflen = buf.size()*sizeof(int);

    to_network_long(buf);

    while (offset < buflen)
    {
        res = mSocket.write( &sendbuf[offset], buflen-offset );
        if (res<0)
        {
//            qCritical(tr("Send failed with error: %s").toLocal8Bit().constData(),
//                      mSocket.errorString().toLocal8Bit().constData());
            return -1;
        }
        else
        {
            offset += res;
        }
    }

    if (!mSocket.waitForBytesWritten(writeTimeout))
    {
//        qCritical(tr("Send failed with error: %s").toLocal8Bit().constData(),
//                  mSocket.errorString().toLocal8Bit().constData());
        return -1;
    }

    return offset;
}

int MovingCoilsVmeCardPort::readBuf(QVector<quint32> &buf, int maxLen)
{
    int res;
    int offset = 0;
    int oldlen = buf.size();
    buf.resize(oldlen+maxLen);
    char *readbuf = (char*)&buf[oldlen];
    int buflen = maxLen*sizeof(int);

    while (offset < buflen)
    {
        if ((!mSocket.bytesAvailable()) && !mSocket.waitForReadyRead(3000))
        {
//            qCritical(tr("Recv failed with error: %s").toLocal8Bit().constData(),
//                      mSocket.errorString().toLocal8Bit().constData());
            return -1;
        }

        res = mSocket.read( &readbuf[offset], buflen-offset );
        if (res<0)
        {
//            qCritical(tr("Recv failed with error: %s").toLocal8Bit().constData(),
//                      mSocket.errorString().toLocal8Bit().constData());
            return -1;
        }
        else
        {
            offset += res;
        }
    }

    from_network_long(buf, oldlen, oldlen+maxLen);

    return offset;
}

int MovingCoilsVmeCardPort::readCharBuf(QVector<char> &buf, int maxLen)
{
    int res;
    int offset = 0;
    int oldlen = buf.size();
    buf.resize(oldlen+maxLen);
    char *readbuf = (char*)&buf[oldlen];
    int buflen = maxLen*sizeof(char);

    while (offset < buflen)
    {
        if ((!mSocket.bytesAvailable()) && !mSocket.waitForReadyRead(300000))
        {
//            qCritical(tr("Recv failed with error: %s").toLocal8Bit().constData(),
//                      mSocket.errorString().toLocal8Bit().constData());
            return -1;
        }

        res = mSocket.read( &readbuf[offset], buflen-offset );
        if (res<0)
        {
//            qCritical(tr("Recv failed with error: %s").toLocal8Bit().constData(),
//                      mSocket.errorString().toLocal8Bit().constData());
            return -1;
        }
        else
        {
            offset += res;
        }
    }

//    from_network_long(buf, oldlen, oldlen+maxLen);

    return offset;
}

// server functions
void MovingCoilsVmeCardPort::kill_vsdc3_server(void){
    //IS USED FOR BIVME-2
    //VsDC3 ETHERNET IGNORES THIS COMMAND
    QVector<quint32> dataSend;
    net_talk(SERVER_DEV_NUM, CMD_SERVER_DISCONNECT, 0, dataSend, NULL);
}

bool MovingCoilsVmeCardPort::net_talk(DeviceType deviceType, const uint commandCode, const uint deviceID,
                                 const QVector<quint32> &dataForWrite, QVector<quint32> *dataForRead)
{

    if (!isConnected())
    {
//        qCritical(tr("Socket is not connected").toLocal8Bit().constData());
        return false;
    }

    QVector<quint32> buf;

    buf.resize(REQUEST_PACKET_HEADER_LENGTH);
    buf[RQFIELD_DEV_TYPE] = deviceType;
    buf[RQFIELD_CMD]      = commandCode;
    buf[RQFIELD_DEV_ID]   = deviceID;

    if (!dataForWrite.isEmpty())
    {
        buf << dataForWrite;
    }

    buf[RQFIELD_BUF_LEN] = buf.size()*sizeof(int);

    writeBuf(buf);
    //    int bytesSended = writeBuf(buf);
    //    qDebug(tr("Bytes sended: %d").toLocal8Bit().constData(),
    //           bytesSended);
    if(dataForRead)
    {
        if (!isConnected())
        {
//            qCritical(tr("Socket is not connected").toLocal8Bit().constData());
            return false;
        }


        buf.clear();
        readBuf(buf, 1);

        quint32 buflen = buf.at(ANFIELD_BUF_LEN);
        if ( buflen < 4*sizeof(int) )
        {
//            qCritical(tr("Wrong BUF_LEN value: %d").toLocal8Bit().constData(),
//                      buflen);
            return false;
        }

        readBuf(buf, 3);
        buflen -= 4*sizeof(int);
        int errorCode = buf.at(ANFIELD_RETCODE);
        if( (errorCode == 0) && ((buflen % sizeof(int) == 0)) )
        {
            if(buflen != 0)
            {
                buflen /= sizeof(int);
                readBuf(buf, buflen);
            }
            *dataForRead << buf;
        }
        else
        {
         //Read And print Error Code

//            qCritical(tr("Server return Error\nError Code: %d").toLocal8Bit().constData(),
//                      errorCode);
            QVector<char> errString;

            errString.clear();
            readCharBuf(errString, buflen);
            errString.append('\0');
            throw std::string(errString.constData());
//            qCritical(tr("Server error: ").toLocal8Bit().constData());
//            qCritical(errString.constData());
            return false;
        }
    }
    return true;
}


void MovingCoilsVmeCardPort::checkStatus(int status)
{
    if (status!=0)
        throw std::string("checkStatus: Failed!");
}


void MovingCoilsVmeCardPort::setBaseAddr(uint32_t _baseAddr, uint32_t _devInd)
{
    QVector<quint32> dataRecv, dataSend;
    dataSend.append(_baseAddr);
    net_talk(VsDC3_DEV_NUM, CMD_VSDC3_SET_BASE_ADDR, _devInd, dataSend, &dataRecv);

    if (dataRecv.size()<ANSWER_PACKET_HEADER_LENGTH)
    {
//        qCritical(tr("Wrong response format: packet length %d bytes. It is less than %d bytes").toLocal8Bit().constData(),
//                  dataRecv.size()*sizeof(int), ANSWER_PACKET_HEADER_LENGTH*sizeof(int));
        throw std::string("setBaseAddr: Wrong response format: packet length");
    }

    checkStatus(dataRecv.at(ANFIELD_RETCODE));
}

void MovingCoilsVmeCardPort::vmeWriteReg(uint32_t addr, uint32_t wdata, uint32_t _devInd)
{
    QVector<quint32> dataRecv, dataSend;

    dataSend.append(addr);
    dataSend.append(wdata);

    net_talk(VsDC3_DEV_NUM, CMD_VSDC3_WRITE_REG, _devInd, dataSend, &dataRecv);

    if (dataRecv.size()<ANSWER_PACKET_HEADER_LENGTH)
    {
//        qCritical(tr("Wrong response format: packet length %d bytes. It is less than %d bytes").toLocal8Bit().constData(),
//                  dataRecv.size()*sizeof(int), ANSWER_PACKET_HEADER_LENGTH*sizeof(int));
        throw std::string("vmeWriteReg: Wrong response format: packet length");
    }

    checkStatus(dataRecv.at(ANFIELD_RETCODE));
}

uint32_t MovingCoilsVmeCardPort::vmeReadReg(uint32_t addr, uint32_t _devInd)
{
    QVector<quint32> dataRecv, dataSend;

    dataSend.append(addr);

    net_talk(VsDC3_DEV_NUM, CMD_VSDC3_READ_REG, _devInd, dataSend, &dataRecv);

    if (dataRecv.size()<READ_PACKET_LENGTH)
    {
//        qCritical(tr("Wrong response format: packet length %d bytes. It is less than %d bytes").toLocal8Bit().constData(),
//                  dataRecv.size()*sizeof(int), READ_PACKET_LENGTH*sizeof(int));
        throw std::string("vmeReadReg: Wrong response format: packet length");
    }

    checkStatus(dataRecv.at(ANFIELD_RETCODE));
    return dataRecv.at(RDFIELD_DATA);
}

void MovingCoilsVmeCardPort::vmeReadRegMulty(uint32_t addr, std::vector<uint32_t> & prdata, uint32_t _devInd)
{
    QVector<quint32> dataSend;
    QVector<quint32> dataIn;


    dataSend.append(addr);
    dataSend.append(prdata.size());

    net_talk(VsDC3_DEV_NUM, CMD_VSDC3_READ_REG_MULT, _devInd, dataSend, &dataIn);

    if ( (dataIn.size()) < ANSWER_PACKET_HEADER_LENGTH )
    {
 //       qCritical(tr("Wrong response format: packet length %d bytes. It is less than %d bytes").toLocal8Bit().constData(),
 //                 (data.size()-dataOldEnd)*sizeof(int), ANSWER_PACKET_HEADER_LENGTH*sizeof(int));
        throw std::string("vmeReadRegMulty: Wrong response format: packet length");
    }

    checkStatus(dataIn.at(ANFIELD_RETCODE));

    dataIn.erase(dataIn.begin(), dataIn.begin() + ANSWER_PACKET_HEADER_LENGTH + 1);
    for(quint32 ii=0; ii<prdata.size(); ++ii)
    {
        prdata[ii] = dataIn[ii];
    }
}

void MovingCoilsVmeCardPort::vmeRdMemFirst(uint32_t addr, uint32_t nextRdCount, uint32_t _devInd)
{
    QVector<quint32> dataSend;
    QVector<quint32> dataIn;


    dataSend.append(addr);
    dataSend.append(nextRdCount);

    net_talk(VsDC3_DEV_NUM, CMD_VSDC3_READ_MEM_FIRST, _devInd, dataSend, &dataIn);

    if ( (dataIn.size()) < ANSWER_PACKET_HEADER_LENGTH )
    {
 //       qCritical(tr("Wrong response format: packet length %d bytes. It is less than %d bytes").toLocal8Bit().constData(),
 //                 (data.size()-dataOldEnd)*sizeof(int), ANSWER_PACKET_HEADER_LENGTH*sizeof(int));
        throw std::string("vmeReadMem: Wrong response format: packet length");
    }

    checkStatus(dataIn.at(ANFIELD_RETCODE));

}
void MovingCoilsVmeCardPort::vmeRdMemLast(uint32_t addr, std::vector<uint32_t> &prdata, uint32_t _devInd)
{
    QVector<quint32> dataSend;
    QVector<quint32> dataIn;


//    dataSend.append(addr);
//    dataSend.append(nextRdCount);

    net_talk(VsDC3_DEV_NUM, CMD_VSDC3_READ_MEM_LAST, _devInd, dataSend, &dataIn);

    if ( (dataIn.size()) < ANSWER_PACKET_HEADER_LENGTH )
    {
 //       qCritical(tr("Wrong response format: packet length %d bytes. It is less than %d bytes").toLocal8Bit().constData(),
 //                 (data.size()-dataOldEnd)*sizeof(int), ANSWER_PACKET_HEADER_LENGTH*sizeof(int));
        throw std::string("vmeReadMem: Wrong response format: packet length");
    }

    checkStatus(dataIn.at(ANFIELD_RETCODE));

    prdata.clear();
//    uint32_t rdLen = dataIn[5];
    dataIn.erase(dataIn.begin(), dataIn.begin() + ANSWER_PACKET_HEADER_LENGTH + 2);
    prdata.resize(dataIn.size());
    for(quint32 ii=0; ii<prdata.size(); ++ii)
    {
        prdata[ii] = dataIn[ii];
    }
}
void MovingCoilsVmeCardPort::vmeRdMemMiddle(uint32_t addr, uint32_t nextRdCount, std::vector<uint32_t> &prdata, uint32_t _devInd)
{
    QVector<quint32> dataSend;
    QVector<quint32> dataIn;


    dataSend.append(addr);
    dataSend.append(nextRdCount);

    net_talk(VsDC3_DEV_NUM, CMD_VSDC3_READ_MEM_MIDDLE, _devInd, dataSend, &dataIn);

    if ( (dataIn.size()) < ANSWER_PACKET_HEADER_LENGTH )
    {
 //       qCritical(tr("Wrong response format: packet length %d bytes. It is less than %d bytes").toLocal8Bit().constData(),
 //                 (data.size()-dataOldEnd)*sizeof(int), ANSWER_PACKET_HEADER_LENGTH*sizeof(int));
        throw std::string("vmeReadMem: Wrong response format: packet length");
    }

    checkStatus(dataIn.at(ANFIELD_RETCODE));

    prdata.clear();
//    uint32_t rdLen = dataIn[5];
    dataIn.erase(dataIn.begin(), dataIn.begin() + ANSWER_PACKET_HEADER_LENGTH + 2);
    prdata.resize(dataIn.size());
    for(quint32 ii=0; ii<dataIn.size(); ++ii)
    {
        prdata[ii] = dataIn[ii];
    }
}

void MovingCoilsVmeCardPort::vmeReadMem(uint32_t addr, std::vector<uint32_t> &prdata, uint32_t _devInd)
{
    QVector<quint32> dataSend;
    QVector<quint32> dataIn;


    dataSend.append(addr);
    dataSend.append(prdata.size());

    net_talk(VsDC3_DEV_NUM, CMD_VSDC3_READ_MEM, _devInd, dataSend, &dataIn);

    if ( (dataIn.size()) < ANSWER_PACKET_HEADER_LENGTH )
    {
 //       qCritical(tr("Wrong response format: packet length %d bytes. It is less than %d bytes").toLocal8Bit().constData(),
 //                 (data.size()-dataOldEnd)*sizeof(int), ANSWER_PACKET_HEADER_LENGTH*sizeof(int));
        throw std::string("vmeReadMem: Wrong response format: packet length");
    }

    checkStatus(dataIn.at(ANFIELD_RETCODE));

    dataIn.erase(dataIn.begin(), dataIn.begin() + ANSWER_PACKET_HEADER_LENGTH + 1);
    for(quint32 ii=0; ii<dataIn.size(); ++ii)
    {
        prdata[ii] = dataIn[ii];
    }
}

void MovingCoilsVmeCardPort::vmeWriteRegMulty(uint32_t addr, std::vector<uint32_t> & pwdata, uint32_t _devInd)
{
    QVector<quint32> dataRecv, dataSend;

    dataSend.append(addr);
    dataSend.append(pwdata.size());

    for(quint32 ii=0; ii<pwdata.size(); ++ii)
    {
        dataSend.append(pwdata[ii]);
    }

    net_talk(VsDC3_DEV_NUM, CMD_VSDC3_WRITE_REG_MULT, _devInd, dataSend, &dataRecv);

    if (dataRecv.size()<ANSWER_PACKET_HEADER_LENGTH)
    {
//        qCritical(tr("Wrong response format: packet length %d bytes. It is less than %d bytes").toLocal8Bit().constData(),
//                  dataRecv.size()*sizeof(int), ANSWER_PACKET_HEADER_LENGTH*sizeof(int));
        throw std::string("vmeWriteRegMulty: Wrong response format: packet length");;
    }

    checkStatus (dataRecv.at(ANFIELD_RETCODE));
}
