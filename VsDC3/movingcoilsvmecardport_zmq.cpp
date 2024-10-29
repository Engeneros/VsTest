#include "movingcoilsvmecardport_zmq.h"
#include <QtEndian>

using namespace MovingCoilsVmeCardPort_ZMQ_NS;

MovingCoilsVmeCardPortZMQ::MovingCoilsVmeCardPortZMQ() : VmeCardBasePort()
{
    connectTimeout = 2000;
    disconnectTimeout = 2000;
    writeTimeout = 2000;
    readTimeout = 2000;
    context = zmq_ctx_new ();
    requester = zmq_socket (context, ZMQ_PAIR);
}


MovingCoilsVmeCardPortZMQ::~MovingCoilsVmeCardPortZMQ()
{
    zmq_ctx_destroy (context);
//    disconnect();
}

bool MovingCoilsVmeCardPortZMQ::isConnected()
{
    //TODO:
//    return mSocket.state() == QAbstractSocket::ConnectedState;
    return true;
}

bool MovingCoilsVmeCardPortZMQ::isUnconnected()
{
    //
//    return mSocket.state() == QAbstractSocket::UnconnectedState;
    return false;
}

bool MovingCoilsVmeCardPortZMQ::ctl_connect()
{
    const std::string endpoint;

    //TODO:
    zmq_connect (requester, "tcp://192.168.1.128:6000");

//    mSocket.connectToHost(mServerIp, mServerPort);
//    if (!mSocket.waitForConnected(connectTimeout))
//    {
////        qCritical(tr("Connection was failed: %s").toLocal8Bit().constData(),
////                  mSocket.errorString().toLocal8Bit().constData());
//        return false;
//    }
    return true;
}

bool MovingCoilsVmeCardPortZMQ::ctl_connect(const QString &address, quint16 port)
{
    return ctl_connect(QHostAddress(address), port);
}

bool MovingCoilsVmeCardPortZMQ::ctl_connect(const QHostAddress &address, quint16 port)
{
    setServerIp(address);
    setServerPort(port);
    return ctl_connect();
}

bool MovingCoilsVmeCardPortZMQ::ctl_disconnect()
{
    kill_vsdc3_server();
    zmq_close (requester);
    return true;

//    if (!isConnected()) return true;

//    //    sendDisconnectPackage();
//    //    if (isUnconnected()) return true;

//    kill_vsdc3_server();

//    mSocket.disconnectFromHost();
//    if (!isUnconnected() && (!mSocket.waitForDisconnected(disconnectTimeout)))
//    {
////        qWarning(tr("Disconnection was failed: %s").toLocal8Bit().constData(),
////                 mSocket.errorString().toLocal8Bit().constData());
//        return false;
//    }
//    return true;
}


QHostAddress MovingCoilsVmeCardPortZMQ::serverIp() const
{
    return mServerIp;
}

void MovingCoilsVmeCardPortZMQ::setServerIp(const QHostAddress &serverIp)
{
    mServerIp = serverIp;
}

void MovingCoilsVmeCardPortZMQ::setServerIp(const QString &serverIp)
{
    mServerIp.setAddress(serverIp);
}

quint16 MovingCoilsVmeCardPortZMQ::serverPort() const
{
    return mServerPort;
}

void MovingCoilsVmeCardPortZMQ::setServerPort(quint16 serverPort)
{
    mServerPort = serverPort;
}

void MovingCoilsVmeCardPortZMQ::to_network_long(QVector<quint32> &buf, int beginIndex, int endIndex)
{
    beginIndex = ((beginIndex<0) ? (0)          : (beginIndex));
    endIndex   = ((endIndex<0)   ? (buf.size()) : (endIndex));

    for (int i=beginIndex; i<endIndex; i++)
    {
        buf[i] = qToBigEndian<quint32>(buf.at(i));
    }
}

void MovingCoilsVmeCardPortZMQ::from_network_long(QVector<quint32> &buf, int beginIndex, int endIndex)
{
    beginIndex = ((beginIndex<0) ? (0)          : (beginIndex));
    endIndex   = ((endIndex<0)   ? (buf.size()) : (endIndex));

    for (int i=beginIndex; i<endIndex; i++)
    {
        buf[i] = qFromBigEndian<quint32>(buf.at(i));
    }
}

int MovingCoilsVmeCardPortZMQ::writeBuf(QVector<quint32> &buf)
{
    char* sendbuf = (char*)buf.constData();
    char rbuf[4];
    int buflen = buf.size()*sizeof(int);

    to_network_long(buf);

    zmq_send (requester, sendbuf, 4*4, 0);
//    zmq_recv (requester, rbuf, 4, 0);
    if(buflen > 4*4)
        zmq_send (requester, (&sendbuf[4*4]), buflen-4*4, 0);

    return 0;
}

int MovingCoilsVmeCardPortZMQ::readBuf(QVector<quint32> &buf, int maxLen)
{
    int res;
    int offset = 0;
    int oldlen = buf.size();
    buf.resize(oldlen+maxLen);
    char *readbuf = (char*)&buf[oldlen];
    int buflen = maxLen*sizeof(int);

    zmq_recv (requester, readbuf, buflen, 0);

    from_network_long(buf, oldlen, oldlen+maxLen);

    return offset;
}

int MovingCoilsVmeCardPortZMQ::readCharBuf(QVector<char> &buf, int maxLen)
{
    int res;
    int offset = 0;
    int oldlen = buf.size();
    buf.resize(oldlen+maxLen);
    char *readbuf = (char*)&buf[oldlen];
    int buflen = maxLen*sizeof(char);

    zmq_recv (requester, readbuf, buflen, 0);

//    while (offset < buflen)
//    {
//        if ((!mSocket.bytesAvailable()) && !mSocket.waitForReadyRead(300000))
//        {
////            qCritical(tr("Recv failed with error: %s").toLocal8Bit().constData(),
////                      mSocket.errorString().toLocal8Bit().constData());
//            return -1;
//        }

//        res = mSocket.read( &readbuf[offset], buflen-offset );
//        if (res<0)
//        {
////            qCritical(tr("Recv failed with error: %s").toLocal8Bit().constData(),
////                      mSocket.errorString().toLocal8Bit().constData());
//            return -1;
//        }
//        else
//        {
//            offset += res;
//        }
//    }

//    from_network_long(buf, oldlen, oldlen+maxLen);

    return offset;
}

// server functions
void MovingCoilsVmeCardPortZMQ::kill_vsdc3_server(void){
    //IS USED FOR BIVME-2
    //VsDC3 ETHERNET IGNORES THIS COMMAND
    QVector<quint32> dataSend;
    net_talk(SERVER_DEV_NUM, CMD_SERVER_DISCONNECT, 0, dataSend, NULL);
}

bool MovingCoilsVmeCardPortZMQ::net_talk(DeviceType deviceType, const uint commandCode, const uint deviceID,
                                 const QVector<quint32> &dataForWrite, QVector<quint32> *dataForRead)
{

    if (!isConnected())
    {
//        qCritical(tr("Socket is not connected").toLocal8Bit().constData());
        return false;
    }

    QVector<quint32> sbuf;
    QVector<quint32> rbuf;

    sbuf.resize(REQUEST_PACKET_HEADER_LENGTH);
    sbuf[RQFIELD_DEV_TYPE] = deviceType;
    sbuf[RQFIELD_CMD]      = commandCode;
    sbuf[RQFIELD_DEV_ID]   = deviceID;

    if (!dataForWrite.isEmpty())
    {
        sbuf << dataForWrite;
    }

    sbuf[RQFIELD_BUF_LEN] = sbuf.size()*sizeof(int);

    writeBuf(sbuf);

    if(dataForRead)
    {
        if (!isConnected())
        {
//            qCritical(tr("Socket is not connected").toLocal8Bit().constData());
            return false;
        }

        rbuf.clear();
        readBuf(rbuf, 4);
        quint32 buflen = rbuf.at(ANFIELD_BUF_LEN);

        if ( buflen < 4*sizeof(int) )
        {
            throw std::string("net_talk(): answer buflen < 4*sizeof(int)");
        }

        buflen -= 4*sizeof(int);
        int errorCode = rbuf.at(ANFIELD_RETCODE);
        if( (errorCode == 0) && ((buflen % sizeof(int) == 0)) )
        {
            if(buflen != 0)
            {
                buflen /= sizeof(int);
//                zmq_send (requester, &buflen, 4, 0);
                readBuf(rbuf, buflen);
            }
            *dataForRead << rbuf;
        }
        else
        {
            if(buflen != 0 )
            {
                std::vector<char> errStr;
                errStr.resize(buflen);
//                zmq_send (requester, &buflen, 4, 0);
                zmq_recv (requester, errStr.data(), buflen, 0);
                throw std::string("errStr");
            }
            else
            {
                throw std::string("net_talk(): error code != 0, no err string");
            }
        }
    }
    else
    {
        //kill server
        char b[4];
        zmq_recv (requester, b, 4, 0);
    }
    return true;
}


void MovingCoilsVmeCardPortZMQ::checkStatus(int status)
{
    if (status!=0)
        throw std::string("checkStatus: Failed!");
}


void MovingCoilsVmeCardPortZMQ::setBaseAddr(uint32_t _baseAddr, uint32_t _devInd)
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

void MovingCoilsVmeCardPortZMQ::vmeWriteReg(uint32_t addr, uint32_t wdata, uint32_t _devInd)
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

uint32_t MovingCoilsVmeCardPortZMQ::vmeReadReg(uint32_t addr, uint32_t _devInd)
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

void MovingCoilsVmeCardPortZMQ::vmeReadRegMulty(uint32_t addr, std::vector<uint32_t> & prdata, uint32_t _devInd)
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

void MovingCoilsVmeCardPortZMQ::vmeReadMem(uint32_t addr, std::vector<uint32_t> &prdata, uint32_t _devInd)
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
    for(quint32 ii=0; ii<prdata.size(); ++ii)
    {
        prdata[ii] = dataIn[ii];
    }
}

void MovingCoilsVmeCardPortZMQ::vmeWriteRegMulty(uint32_t addr, std::vector<uint32_t> & pwdata, uint32_t _devInd)
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
