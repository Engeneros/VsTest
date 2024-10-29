#include "MovingCoilsServer.h"

#include <QtEndian>


using namespace MovingCoilsServerNS;


MovingCoilsServer::MovingCoilsServer(QObject *parent) :
    QObject(parent)
{
    connectTimeout = 2000;
    disconnectTimeout = 2000;
    writeTimeout = 2000;
    readTimeout = 2000;

    connect( &mSocket, SIGNAL( connected() ), this,  SLOT( ctrl_connected() ) );
    connect( &mSocket, SIGNAL( disconnected() ), this, SLOT( ctrl_closed() ) );
    connect( &mSocket, SIGNAL( error( QAbstractSocket::SocketError ) ), this,  SLOT( ctrl_error( QAbstractSocket::SocketError ) ) );
}

MovingCoilsServer::~MovingCoilsServer()
{
    disconnect();
}

bool MovingCoilsServer::isConnected()
{
    return mSocket.state() == QAbstractSocket::ConnectedState;
}

bool MovingCoilsServer::isUnconnected()
{
    return mSocket.state() == QAbstractSocket::UnconnectedState;
}

bool MovingCoilsServer::ctl_connect()
{
    mSocket.connectToHost(mServerIp, mServerPort);
    if (!mSocket.waitForConnected(connectTimeout))
    {
        qCritical(tr("Connection was failed: %s").toLocal8Bit().constData(),
                  mSocket.errorString().toLocal8Bit().constData());
        return false;
    }
    return true;
}

bool MovingCoilsServer::ctl_connect(const QString &address, quint16 port)
{
    return ctl_connect(QHostAddress(address), port);
}

bool MovingCoilsServer::ctl_connect(const QHostAddress &address, quint16 port)
{
    setServerIp(address);
    setServerPort(port);
    return ctl_connect();
}

bool MovingCoilsServer::ctl_disconnect()
{
    if (!isConnected()) return true;

    //    sendDisconnectPackage();
    //    if (isUnconnected()) return true;

    kill_vsdc3_server();

    mSocket.disconnectFromHost();
    if (!isUnconnected() && (!mSocket.waitForDisconnected(disconnectTimeout)))
    {
        qWarning(tr("Disconnection was failed: %s").toLocal8Bit().constData(),
                 mSocket.errorString().toLocal8Bit().constData());
        return false;
    }
    return true;
}

void MovingCoilsServer::ctrl_connected(){
    messenger("Control socket connected, Sib-Maks:"+mSocket.peerName());
}

void MovingCoilsServer::ctrl_closed(){
    messenger("Control socket closed:"+mSocket.peerName());
}

void MovingCoilsServer::ctrl_error( QAbstractSocket::SocketError code ){
    QString strTmp;
    strTmp.sprintf("error code: 0x%x ;", code);
    strTmp += mSocket.peerName();
    messenger(strTmp);
}

void MovingCoilsServer::messenger(const QString &str){
    emit serverMessages(str);
}

QHostAddress MovingCoilsServer::serverIp() const
{
    return mServerIp;
}

void MovingCoilsServer::setServerIp(const QHostAddress &serverIp)
{
    mServerIp = serverIp;
}

void MovingCoilsServer::setServerIp(const QString &serverIp)
{
    mServerIp.setAddress(serverIp);
}

quint16 MovingCoilsServer::serverPort() const
{
    return mServerPort;
}

void MovingCoilsServer::setServerPort(quint16 serverPort)
{
    mServerPort = serverPort;
}

void MovingCoilsServer::to_network_long(QVector<quint32> &buf, int beginIndex, int endIndex)
{
    beginIndex = ((beginIndex<0) ? (0)          : (beginIndex));
    endIndex   = ((endIndex<0)   ? (buf.size()) : (endIndex));

    for (int i=beginIndex; i<endIndex; i++)
    {
        buf[i] = qToBigEndian<quint32>(buf.at(i));
    }
}

void MovingCoilsServer::from_network_long(QVector<quint32> &buf, int beginIndex, int endIndex)
{
    beginIndex = ((beginIndex<0) ? (0)          : (beginIndex));
    endIndex   = ((endIndex<0)   ? (buf.size()) : (endIndex));

    for (int i=beginIndex; i<endIndex; i++)
    {
        buf[i] = qFromBigEndian<quint32>(buf.at(i));
    }
}

int MovingCoilsServer::writeBuf(QVector<quint32> &buf)
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
            qCritical(tr("Send failed with error: %s").toLocal8Bit().constData(),
                      mSocket.errorString().toLocal8Bit().constData());
            return -1;
        }
        else
        {
            offset += res;
        }
    }

    if (!mSocket.waitForBytesWritten(writeTimeout))
    {
        qCritical(tr("Send failed with error: %s").toLocal8Bit().constData(),
                  mSocket.errorString().toLocal8Bit().constData());
        return -1;
    }

    return offset;
}

int MovingCoilsServer::readBuf(QVector<quint32> &buf, int maxLen)
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
            qCritical(tr("Recv failed with error: %s").toLocal8Bit().constData(),
                      mSocket.errorString().toLocal8Bit().constData());
            return -1;
        }

        res = mSocket.read( &readbuf[offset], buflen-offset );
        if (res<0)
        {
            qCritical(tr("Recv failed with error: %s").toLocal8Bit().constData(),
                      mSocket.errorString().toLocal8Bit().constData());
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

int MovingCoilsServer::readCharBuf(QVector<char> &buf, int maxLen)
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
            qCritical(tr("Recv failed with error: %s").toLocal8Bit().constData(),
                      mSocket.errorString().toLocal8Bit().constData());
            return -1;
        }

        res = mSocket.read( &readbuf[offset], buflen-offset );
        if (res<0)
        {
            qCritical(tr("Recv failed with error: %s").toLocal8Bit().constData(),
                      mSocket.errorString().toLocal8Bit().constData());
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
void MovingCoilsServer::kill_vsdc3_server(void){
    //IS USED FOR BIVME-2
    //VsDC3 ETHERNET IGNORES THIS COMMAND
    QVector<quint32> dataSend;
    net_talk(SERVER_DEV_NUM, CMD_SERVER_DISCONNECT, 0, dataSend, NULL);
}

bool MovingCoilsServer::net_talk(DeviceType deviceType, const uint commandCode, const uint deviceID,
                                 const QVector<quint32> &dataForWrite, QVector<quint32> *dataForRead)
{

    if (!isConnected())
    {
        qCritical(tr("Socket is not connected").toLocal8Bit().constData());
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
            qCritical(tr("Socket is not connected").toLocal8Bit().constData());
            return false;
        }


        buf.clear();
        readBuf(buf, 1);

        int buflen = buf.at(ANFIELD_BUF_LEN);
        if ( buflen < 4*sizeof(int) )
        {
            qCritical(tr("Wrong BUF_LEN value: %d").toLocal8Bit().constData(),
                      buflen);
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

            qCritical(tr("Server return Error\nError Code: %d").toLocal8Bit().constData(),
                      errorCode);
            QVector<char> errString;

            errString.clear();
            readCharBuf(errString, buflen);
            errString.append('\0');
            qCritical(tr("Server error: ").toLocal8Bit().constData());
            qCritical(errString.constData());
            return false;
        }
    }
    return true;
}

void MovingCoilsServer::setVsDC3BaseAddr(uint baseaddr)
{
    QVector<quint32> dataRecv, dataSend;
    dataSend.append(baseaddr);
    net_talk(VsDC3_DEV_NUM, CMD_VSDC3_SET_BASE_ADDR, /*deviceID*/ 0, dataSend, &dataRecv);

    if (dataRecv.size()<ANSWER_PACKET_HEADER_LENGTH)
    {
        qCritical(tr("Wrong response format: packet length %d bytes. It is less than %d bytes").toLocal8Bit().constData(),
                  dataRecv.size()*sizeof(int), ANSWER_PACKET_HEADER_LENGTH*sizeof(int));
        return;
    }

    checkStatus (dataRecv.at(ANFIELD_RETCODE));
}

void MovingCoilsServer::writeVsDC3Register(uint addr, uint data)
{
    QVector<quint32> dataRecv, dataSend;

    dataSend.append(addr);
    dataSend.append(data);

    net_talk(VsDC3_DEV_NUM, CMD_VSDC3_WRITE_REG, /*deviceID*/ 0, dataSend, &dataRecv);

    if (dataRecv.size()<ANSWER_PACKET_HEADER_LENGTH)
    {
        qCritical(tr("Wrong response format: packet length %d bytes. It is less than %d bytes").toLocal8Bit().constData(),
                  dataRecv.size()*sizeof(int), ANSWER_PACKET_HEADER_LENGTH*sizeof(int));
        return;
    }

    checkStatus (dataRecv.at(ANFIELD_RETCODE));

    //    qDebug(tr("Status: 0x%X").toLocal8Bit().constData(),               dataRecv.at(RFIELD_STATUS));
    //    qDebug(tr("Actual packet length: 0x%X").toLocal8Bit().constData(), dataRecv.size());
}

void MovingCoilsServer::writeVsDC3Registers(uint addr, QVector<quint32> &data)
{
    QVector<quint32> dataRecv, dataSend;

    dataSend.append(addr);
    dataSend << data;

    net_talk(VsDC3_DEV_NUM, CMD_VSDC3_WRITE_REG_MULT, /*deviceID*/ 0, dataSend, &dataRecv);

    if (dataRecv.size()<ANSWER_PACKET_HEADER_LENGTH)
    {
        qCritical(tr("Wrong response format: packet length %d bytes. It is less than %d bytes").toLocal8Bit().constData(),
                  dataRecv.size()*sizeof(int), ANSWER_PACKET_HEADER_LENGTH*sizeof(int));
        return;
    }

    checkStatus (dataRecv.at(ANFIELD_RETCODE));

    //    qDebug(tr("Status: 0x%X").toLocal8Bit().constData(),               dataRecv.at(RFIELD_STATUS));
    //    qDebug(tr("Actual packet length: 0x%X").toLocal8Bit().constData(), dataRecv.size());
}


uint MovingCoilsServer::readVsDC3Register(uint addr)
{
    QVector<quint32> dataRecv, dataSend;

    dataSend.append(addr);

    net_talk(VsDC3_DEV_NUM, CMD_VSDC3_READ_REG,  /*deviceID*/ 0, dataSend, &dataRecv);

    if (dataRecv.size()<READ_PACKET_LENGTH)
    {
        qCritical(tr("Wrong response format: packet length %d bytes. It is less than %d bytes").toLocal8Bit().constData(),
                  dataRecv.size()*sizeof(int), READ_PACKET_LENGTH*sizeof(int));
        return 0;
    }

    checkStatus(dataRecv.at(ANFIELD_RETCODE));

    //    qDebug(tr("Status: 0x%X").toLocal8Bit().constData(),               dataRecv.at(RFIELD_STATUS));
    //    qDebug(tr("Actual packet length: 0x%X").toLocal8Bit().constData(), dataRecv.size());
    //    qDebug(tr("Data: 0x%X").toLocal8Bit().constData(),                 dataRecv.at(RFIELD_DATA));

    return dataRecv.at(RDFIELD_DATA);
}

void MovingCoilsServer::sendCommandToServer(uint command, QVector<quint32> &data)
{
    QVector<quint32> dataRb;
    net_talk(SERVER_DEV_NUM, command, 0, data, &dataRb);
    checkStatus(dataRb.at(ANFIELD_RETCODE));
}

void MovingCoilsServer::readVsDC3Registers(uint addr, QVector<quint32> &data, uint dataLen)
{
    QVector<quint32> dataSend;
    int dataOldEnd = data.size();

    dataSend.append(addr);
    dataSend.append(dataLen);

    net_talk(VsDC3_DEV_NUM, CMD_VSDC3_READ_MEM, /*deviceID*/ 0, dataSend, &data);

    if ( (data.size()-dataOldEnd) < ANSWER_PACKET_HEADER_LENGTH )
    {
        qCritical(tr("Wrong response format: packet length %d bytes. It is less than %d bytes").toLocal8Bit().constData(),
                  (data.size()-dataOldEnd)*sizeof(int), ANSWER_PACKET_HEADER_LENGTH*sizeof(int));
        return;
    }

    checkStatus(data.at(ANFIELD_RETCODE));

    //    qDebug(tr("Status: 0x%X").toLocal8Bit().constData(),               data.at(RFIELD_STATUS));
    //    qDebug(tr("Actual packet length: 0x%X").toLocal8Bit().constData(), data.size()-dataOldEnd);

    data.erase(data.begin()+dataOldEnd, data.begin()+dataOldEnd+ANSWER_PACKET_HEADER_LENGTH);
}

void MovingCoilsServer::modifyVsDC3Register(uint addr, uint data, uint mask)
{
    uint old_data = readVsDC3Register(addr);
    data = ((old_data &~ mask) | (data & mask));
    writeVsDC3Register(addr, data);
}

bool MovingCoilsServer::checkStatus(int status)
{
    if (status==0)
    {
        //        qDebug(tr("Status - ok").toLocal8Bit().constData());
        return true;
    }
    else if (status<0)
    {
        qCritical(tr("Status - error: %d: %s").toLocal8Bit().constData(), status, errorString(status).toLocal8Bit().constData());
        return false;
    }
    else if (status>0)
    {
        qWarning(tr("Status - warning: %d: %s").toLocal8Bit().constData(), status, errorString(status).toLocal8Bit().constData());
        return true;
    }
    return false;
}

QString MovingCoilsServer::errorString(int status)
{
    switch (status)
    {
    case STATUS_OK:             return tr("Ok");
    case STATUS_GENERAL_ERROR:  return tr("General error");
    case STATUS_UNKNOWN_CMD:    return tr("Unknown command");
    case STATUS_INVALID_DEV_ID: return tr("Invalid device ID");
    case STATUS_BAD_PARAM:      return tr("Bad parameter");
    case STATUS_BAD_PARAM_CNT:  return tr("Bad parameters count");
    case STATUS_VME_ERROR:      return tr("VME error");
    default:                    return tr("Unknown");
    }
}
