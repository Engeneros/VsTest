#ifndef MOVINGCOILSVMECARDPORT_H
#define MOVINGCOILSVMECARDPORT_H

#include <QHostAddress>
#include <QTcpSocket>
#include "vmecardbaseport.h"

namespace MovingCoilsVmeCardPortNS
{

enum DeviceType
{
    SERVER_DEV_NUM,
    VsDC2_DEV_NUM,
    DAC_DEV_NUM,
    MOTOR_DEV_NUM,
    VsDC3_DEV_NUM
};

enum ServerCommand
{
    CMD_SERVER_GET_VERSION    = 0,
    CMD_SERVER_DISCONNECT     = 1
};

enum VsDC3Command
{
    CMD_VSDC3_SET_BASE_ADDR     = 0,
    CMD_VSDC3_READ_REG          = 1,
    CMD_VSDC3_WRITE_REG         = 2,
    CMD_VSDC3_READ_REG_MULT     = 3,
    CMD_VSDC3_RESET_VSDC3       = 4,
    CMD_VSDC3_SET_INT_NUM       = 5,
    CMD_VSDC3_CLEAR_BUFF        = 6,
    CMD_VSDC3_READ_MEM          = 0xA0,
    CMD_VSDC3_READ_MEM_FIRST    = 0xA1,
    CMD_VSDC3_READ_MEM_MIDDLE   = 0xA2,
    CMD_VSDC3_READ_MEM_LAST     = 0xA3,
    CMD_VSDC3_WRITE_REG_MULT    = 0xAA
};

//packet head
const int REQUEST_PACKET_HEADER_LENGTH = 4;
enum RequestPacketField
{
    RQFIELD_BUF_LEN      = 0,
    RQFIELD_DEV_TYPE     = 1,
    RQFIELD_DEV_ID       = 2,
    RQFIELD_CMD          = 3
};

const int ANSWER_PACKET_HEADER_LENGTH = 4;
enum AnswerPacketField
{
    ANFIELD_BUF_LEN      = 0,
    ANFIELD_DEV_ID       = 1,
    ANFIELD_CMD          = 2,
    ANFIELD_RETCODE      = 3,
};

//packet body
//WRITE REG
const int WRITE_PACKET_LENGTH = REQUEST_PACKET_HEADER_LENGTH + 2;
enum WriteRegCmd
{
    WRFIELD_ADDR    = 4,
    WRFIELD_DATA    = 5
};

//READ REG
const int READ_PACKET_LENGTH = REQUEST_PACKET_HEADER_LENGTH + 2;
enum ReadRegCmd
{
    RDFIELD_ADDR    = 4,
    RDFIELD_DATA    = 5
};

enum StatusCode
{
    STATUS_OK               =  0,
    STATUS_GENERAL_ERROR    = -1,
    STATUS_UNKNOWN_CMD      = -2,
    STATUS_INVALID_DEV_ID   = -3,
    STATUS_BAD_PARAM        = -4,
    STATUS_BAD_PARAM_CNT    = -5,
    STATUS_VME_ERROR        = -6
};

}

class MovingCoilsVmeCardPort : public VmeCardBasePort
{
public:
    MovingCoilsVmeCardPort();
    ~MovingCoilsVmeCardPort();

    bool ctl_connect();
    bool ctl_connect(const QHostAddress &address, quint16 port);
    bool ctl_connect(const QString &address, quint16 port);

    bool ctl_disconnect();

    bool isConnected();
    bool isUnconnected();

    quint16 serverPort() const;
    void setServerPort(quint16 serverPort);

    QHostAddress serverIp() const;
    void setServerIp(const QHostAddress &serverIp);
    void setServerIp(const QString &serverIp);
    void checkStatus(int status);

    void setBaseAddr(uint32_t _baseAddr, uint32_t _devInd) override;
    void vmeWriteReg(uint32_t addr, uint32_t wdata, uint32_t _devInd) override;
//    int vmeWriteMem(uint32_t addr, std::vector<uint32_t> & pwdata, uint32_t devInd);
    void vmeWriteRegMulty(uint32_t addr, std::vector<uint32_t> & pwdata, uint32_t _devInd) override;
    uint32_t vmeReadReg(uint32_t addr, uint32_t _devInd) override;
    void vmeReadMem(uint32_t addr, std::vector<uint32_t> & prdata, uint32_t _devInd) override;
    void vmeRdMemFirst(uint32_t addr, uint32_t nextRdCount, uint32_t _devInd) override;
    void vmeRdMemLast(uint32_t addr, std::vector<uint32_t> &prdata, uint32_t _devInd) override;
    void vmeRdMemMiddle(uint32_t addr, uint32_t nextRdCount, std::vector<uint32_t> &prdata, uint32_t _devInd) override;
    void vmeReadRegMulty(uint32_t addr, std::vector<uint32_t> & prdata, uint32_t _devInd) override;

private:
    bool net_talk(MovingCoilsVmeCardPortNS::DeviceType deviceType, const uint commandCode, const uint deviceID = 0,
                  const QVector<quint32> &writeBuff = QVector<quint32>(), QVector<quint32> *readBuff = 0);

    void to_network_long(QVector<quint32> &buf, int beginIndex = -1, int endIndex = -1);
    void from_network_long(QVector<quint32> &buf, int beginIndex = -1, int endIndex = -1);
    int writeBuf(QVector<quint32> &buf);
    int readBuf(QVector<quint32> &buf, int toRead);
    int readCharBuf(QVector<char> &buf, int maxLen);
    void kill_vsdc3_server(void);

public:
    int connectTimeout;
    int disconnectTimeout;
    int writeTimeout;
    int readTimeout;

private:

    QHostAddress mServerIp;
    quint16 mServerPort;
    QTcpSocket mSocket;
};

#endif // MOVINGCOILSVMECARDPORT_H
