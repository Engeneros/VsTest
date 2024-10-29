#include <QCoreApplication>
#include <iostream>

#include <sstream>
#include <math.h>
#include <conio.h>

//#include "MovingCoilsServer.h"
#include <qt_windows.h>


#include <QProcess>

#include "vsdc3v2.h"
#include "vsdccardtest.h"
#include "movingcoilsvmecardport.h"

//#include "vsdc3noiseofintegral.h"
//#include "vsdc3abserror.h"

//#include <QDateTime>
//#include <QtGlobal>
#include <QTime>

//using namespace std;

bool connectToServer(MovingCoilsVmeCardPort &srv, QString ip, quint16 port);
bool disconnectFromServer(MovingCoilsVmeCardPort &srv);

void Plot_NI(QString dirname);
void Plot_NI(QString dirname, QString filename);
void Plot_TLT(QString dirname);
void Plot_TLT(QString dirname, QString filename);

bool apply_settings(void);
bool make_interleaved_measurement(uint mux, uint cycleNum, uint avgn);
bool make_osc_measurement(float t_int, uint sett, uint avgn, uint prTimer, uint startNum, uint plot_flag);
void make_coil_calibration(void);
bool get_ch_data(uint chInd, uint rdNum, float *data_osc);
bool get_data(uint rdnum, uint avgn, uint plot_flag);
void Plot_NO(QString filename);
void PyPlotChan(QString filename, QString fil, int nstart, int nstop, int avgn, int chCount);
void PyPlot(QString filename, int nstart, int nstop, int avgn);
void Plot_NB(void);
void Plot_TM(void);
void Plot_SWZ(void);
void Plot_FZ(void);
bool read_interleaved_measurements(quint32 sett0, quint32 sett1, quint32 cycleNum, quint32 base);
bool wait_untill_the_end(uint cycleNum);
bool config_interleaved_measurement(uint mux0, uint mux1, uint cycleNum, uint avgn);
bool startSynchroBoard(void);
bool stopSynchroBoard(void);
void PyPlotInter(QString filename, int cycleNum);
void print_osc_read_stat(void);

void printVsDCSettReg(void);
void printVsDCStatusReg(void);
void clearVsDC_CSR_Reg(void);
void printVsDCIsrVectorReg(void);
void printVsDCIntegral(void);
void printVsDCIntBuffviaRegs(void);
void printVsDCIntBuffviaMem(void);
void print_vsdc_range(void);
void printVsDCsyncBPmux(void);
void printVsDCAUZregs(void);
void printVsDCIqrLineReg(void);

//MovingCoilsServer mServer;
quint32 F_DRDY = 5000000;
quint32 OSC_DRDY;

uint period;

QString global_file_path = "D:\\DC\\!W\\Prj\\VSDC3-1\\python\\"; //"G:\\work\\tmpdat\\VsDC3_chopp";
QString python_file_path = "D:\\DC\\!W\\Prj\\VSDC3-1\\python\\50Hz_test_plot.py";//"G:\\work\\tmpdat\\VsDC3_chopp\\50Hz_test_plot.py";


class vsdcProductionTable
{
public:
    vsdcProductionTable(quint32 _base_addr, std::initializer_list<float> _refs, quint32 _uinqe_id, quint32 _chans)
    {
        base_addr = _base_addr;
        refVoltages = std::vector<float>(_refs);
        uinqe_id = _uinqe_id;
        chanNum = _chans;
    }
    quint32 base_addr;
    std::vector<float> refVoltages;
    quint32 uinqe_id;
    quint32 chanNum;
};

int DO_WRITE_CALIB = 0;
int DO_VOLT_INL_TEST = 0;

//std::vector<vsdcProductionTable> devList = {
////    {0x02000000,   {1.8,   0.168},       0x302,  2},
////    {0x04000000,   {1.80003,   0.168035},       0x304,  2},
////    {0x06000000,   {1.8,   0.168},       0x306,  2},
////    {0x22000000,   {1.800061,   0.1680666},       0x310,  2},
////    {0x24000000,   {1.800061,   0.1680666},       0x310,  2},
////    {0x10000000,   {1.79999,   0.168054},  0x312,  2},
////    {0x14000000,   {1.79997,   0.168071},  0x315,  2},
////    {0x16000000,   {1.80019,   0.168059},  0x316,  2},
////    {0x18000000,   {1.80010,   0.168076},  0x31A,  2},
////    {0x1A000000,   {1.800035,  0.168043},  0x31C,  2},
////    {0x1C000000,   {1.79996,   0.168033},  0x31E,  2},
////    {0x1E000000,   {1.80009,   0.168069},  0x320,  2},
////    {0x20000000,   {1.79993,   0.168030},  0x322,  2},
////      {0x22000000,   {1.799917 + 1.799878, 0.336}, 0x322, 2},
////    {0x20000000,    {1.774240,  0.161298},      0x400,  4},
////    {0x44000000,    {1.773635,  0.161268},       0x440,  4},
////    {0x48000000,    {1.774354,  0.161336},       0x480,  4},
////    {0x4C000000,    {1.77455,   0.161349},       0x4C0,  4},
////    {0x50000000,    {1.77416,   0.161316},       0x500,  4},
////    {0x06000000,   {1.79996,        0.168065},       0x322,  2},
////    {0xC0000000,   {1.80007,   0.168071},       0x310,  2},
//    //DAC dev
//    {0x22000000,    {1.80007,   0.168071},       0x310,  2},
//};

//std::vector<vsdcProductionTable> devList = {
//    {0x22000000,    {1.80018,   0.168061},       0x501,  2},
//    {0x24000000,    {1.80005,   0.168070},       0x502,  2},
//    //DAC dev
////    {0x22000000,    {1.80018,   0.168061},       0x501,  2},
//};


std::vector<vsdcProductionTable> devList = {
//    {0x20000000,    {1.774245,   0.161300},       0x601,  4},
    //DAC dev
//    {0x34000000,    {1.80018,   0.168061},       0x502,  2},
      {0x34000000,    {1.80005,   0.168014},       0x503,  2},
//      {0x36000000,    {1.80012,   0.168117},       0x504,  2},
//      {0x38000000,    {1.80022,   0.168121},       0x505,  2},
//      {0x3A000000,    {1.80015,   0.168117},       0x506,  2},
//      {0x40000000,    {1.80018,   0.168061},       0x507,  2},
//      {0x42000000,    {1.80018,   0.168061},       0x508,  2},
};

//std::vector<vsdcProductionTable> dacDevList = {
//    {0x22000000,    {1.80007,   0.168071},       0x310,  2},
//};


//std::vector<vsdcProductionTable> devList = {
//    {0x10000000,   {1.800056,   0.168},       0x204,  2},
//};

//std::vector<vsdcProductionTable> devList = {
//    {0x10000000,   {1.80007,   0.168},       0x204,  2},
//};



MovingCoilsVmeCardPort mServer;
VsDC3v2 vsdc3Dev(&mServer);

VsDC3v2 VsDCFactory(vsdcProductionTable _inst, uint32_t _ind)
{
    return VsDC3v2(&mServer);
}

QString makeOscDevFilName(uint32_t baseAddr, uint32_t sett, uint32_t avgn, uint32_t rangeCode)
{
    QString filename_t = QString("vsdc_0x%1_").arg(baseAddr >> 24, 0, 16);
    filename_t += QString::fromUtf8(VsDC3v2::muxName(sett));
    QString avgStr = QString("_avg%1_").arg(avgn, 0, 10);
    filename_t += avgStr;
    filename_t += QString::fromUtf8(VsDC3v2::rangeName(rangeCode));
    filename_t += "_ch_";

//    filename.sprintf(filename_t.toLocal8Bit().constData(), vsdc3Dev.baseAddr, chan);
//    QString filename;
//    filename.asprintf(filename_t.toLocal8Bit().constData(), baseAddr);
//    std::cout << filename.toStdString() << std::endl;
//    filename += QString::fromUtf8(VsDC3v2::muxName(sett).c_str());
//    filename += "_avg%d_";
//    filename.asprintf(filename.toLocal8Bit().constData(), avgn);
//    filename += QString::fromUtf8(VsDC3v2::rangeName(rangeCode).c_str());
//    filename += "_ch_";
    return filename_t;
}

QString makeOscChanFilName(QString devFil, uint32_t ch_num)
{
    QString chStr = QString("%1.oscdat").arg(ch_num, 0, 10);
    return devFil + chStr;
//    devFil += "%d.oscdat";
//    devFil.asprintf(devFil.toLocal8Bit().constData(), ch_num);
//    return devFil;
}

class VsDCOscSettings
{
    public:
    VsDCOscSettings(uint32_t _sett, float _timerSec, uint32_t _avgN, uint32_t _plot_start=32, int32_t _plot_stop=-128, uint32_t _stat_srart=100, uint32_t _stat_stop=128)
    {
        sett = _sett;
        timerSec = _timerSec;
        avgN = _avgN;
        plot_start = _plot_start;
        plot_stop = _plot_stop;
        stat_srart = _stat_srart;
        stat_stop = _stat_stop;
    }
    uint32_t sett;
    float timerSec;
    uint32_t avgN;
    uint32_t plot_start;
    int32_t  plot_stop;
    uint32_t stat_srart;
    uint32_t stat_stop;

};



std::string makeOscStarts(VsDCOscSettings oscSett, std::vector<VsDC3v2> &vsdcList)
{
    std::stringstream cout;
    VsDCChSettings conf = VsDCChSettings(oscSett.sett, 0, oscSett.avgN, 0);

//    quint32 xsett_0    = ADC_GAIN02
//                        | ADC_REF_L
////                        | ADC_GND
//    //                        | ADC_START_BP
//    //                    | ADC_STOP_BP
//    //                        | ADC_IRQ_EN
//    //                        | ADC_SINGLE
//                        | ADC_SET_AUTOZERO
//                ;
//    VsDCChSettings xconf_0 = VsDCChSettings(xsett_0, 0, oscSett.avgN, 0);

//    quint32 xsett_1    = ADC_GAIN02
//                        | ADC_SIG
////                        | ADC_GND
//    //                        | ADC_START_BP
//    //                    | ADC_STOP_BP
//    //                        | ADC_IRQ_EN
//    //                        | ADC_SINGLE
//                        | ADC_SET_AUTOZERO
//                ;
//    VsDCChSettings xconf_1 = VsDCChSettings(xsett_1, 0, oscSett.avgN, 0);


    for(auto vsdc3Dev : vsdcList)
    {
        //
        auto chan=0;
//        {
//            vsdc3Dev.configureChannel(xconf_0, chan);
//        }
//        for(chan=1; chan<vsdc3Dev.chanCount(); ++chan)
//        chan=1;
        for(chan=0; chan<vsdc3Dev.chanCount(); ++chan)
        {
            vsdc3Dev.configureChannel(conf, chan);
//            vsdc3Dev.configureChannel(xconf_1, chan);
        }

        for(chan=0; chan<vsdc3Dev.chanCount(); ++chan)
        {
            Sleep(250);
            vsdc3Dev.setTimerSec(oscSett.timerSec, chan);
        }

//        vsdc3Dev.configureAutoZero();


    }
    //
    for(auto vsdc3Dev : vsdcList)
    {
        vsdc3Dev.pstartAll();
//        Sleep(100);
//        vsdc3Dev.setDacValue(-1.8);
    }
    //
    Sleep(oscSett.timerSec*1000+100);

    return cout.str();
}

std::string makeOscRead(VsDCOscSettings oscSett, std::vector<VsDC3v2> &vsdcList)
{
    std::stringstream cout;
    QString dir_path = global_file_path;// "D:\\DC\\!W\\Prj\\VSDC3-1\\python\\;"//"g:\\work\\tmpdat\\VsDC3_chopp\\";
    QString filename;
    bool do_SAVE = true;

    for(auto vsdc3Dev : vsdcList)
    {
        cout << std::hex << "0x" << vsdc3Dev.baseAddr << std::dec << " AvgN(" << oscSett.avgN << ")\t" << vsdc3Dev.muxName(oscSett.sett) << " ";
        for(auto chan=0; chan<vsdc3Dev.chanCount(); ++chan)
        {
            auto osc_data = vsdc3Dev.getOscilloscope(chan);
            //print some info
            auto mean = VsDC3v2::avg(osc_data, oscSett.stat_srart, oscSett.stat_stop);
            auto std = VsDC3v2::std(mean, osc_data, oscSett.stat_srart, oscSett.stat_stop);
            auto valVs = vsdc3Dev.readIntegral(chan);
            cout << "\nosc Len:\t" << vsdc3Dev.oscLen[chan] << "\n";
            cout << "\nCH" << chan << ":\t" << mean << "V\t" << std << "V\tINT:" << valVs << "Vs\t" << valVs/oscSett.timerSec;
            //do something with osc data
            if(do_SAVE)
            {
//                        filename.sprintf(filename_t.toLocal8Bit().constData(), vsdc3Dev.baseAddr, chan);
                                       //(uint32_t baseAddr, uint32_t sett, uint32_t avgn, uint32_t rangeCode, uint32_t ch_num)
                filename = makeOscChanFilName(makeOscDevFilName(vsdc3Dev.baseAddr, oscSett.sett, oscSett.avgN, vsdc3Dev.getState(chan)), chan);
                FILE *stream0 = fopen((dir_path+filename).toLocal8Bit().constData(), "w");
                for(uint ii=0; ii<osc_data.size(); ++ii)
                {
                    fprintf( stream0, "%+.7G\t%+.7G\n",  1.0*ii*vsdc3Dev.adcSamplingTime*(1<<oscSett.avgN), osc_data[ii] );
                }
                fclose(stream0);
            }
        }
        cout << std::endl;
    }
    //plot
    if(1)
    {
        QString plot_name;
        for(auto vsdc3Dev : vsdcList)
        {            
            plot_name = makeOscDevFilName(vsdc3Dev.baseAddr, oscSett.sett, oscSett.avgN, vsdc3Dev.getState(0));
            PyPlotChan(global_file_path, plot_name,  oscSett.plot_start, oscSett.plot_stop, oscSett.avgN, vsdc3Dev.chanCount()); //(dir_path, fil_name, ns = 0, ne = -1, navg = 0, ch_count = 2)
        }
    }
    return cout.str();
}

std::string makeOscTest(VsDCOscSettings oscSett, std::vector<VsDC3v2> &vsdcList)
{
    makeOscStarts(oscSett, vsdcList);
    return makeOscRead(oscSett, vsdcList);
}

void shortOscTest(float &_avg, float &_std, VsDC3v2 &vsdc3Dev, uint32_t sett, uint32_t timer, uint32_t start_ind, uint32_t stop_ind)
{
    auto conf = VsDCChSettings(sett, timer, 0, 0);
    for(auto chan=0; chan<vsdc3Dev.chanCount(); ++chan)
    {
        vsdc3Dev.configureChannel( conf, chan);
    }
    vsdc3Dev.pstartAll();
    Sleep((timer*vsdc3Dev.adcSamplingTime*1000)+10);
    uint32_t chan=0;
//    for(auto chan=0; chan<vsdc3Dev.chanCount(); ++chan)
    {
        auto osc_data = vsdc3Dev.getOscilloscope(chan);
        //print some info
        _avg = VsDC3v2::avg(osc_data, start_ind, stop_ind);
        _std = VsDC3v2::std(_avg, osc_data, start_ind, stop_ind);
    }
}

bool abortTransaction(void)
{
    if(kbhit() != 0)
    {
        while(kbhit())
        {
            char c = getch();
        }
        return true;
    }
    return false;
}
//D:\DC\!W\\Prj\VSDC3-1\python\HP34410A.py
float readHPVoltage(void)
{
    QStringList arguments { global_file_path + "HP34410A.py" };
    QProcess p;
    p.start("python", arguments);
    p.waitForFinished();

    float val;
    QString fil = global_file_path +  "hpVoltage";
    FILE *streamX = fopen(fil.toLocal8Bit().constData(), "r");
    fscanf(streamX, "%G", &val);
    fclose(streamX);
    return val;
}

uint32_t vsdc3IqrVectorFactory( uint32_t _addr, uint32_t _chanCount, uint32_t _chanNum)
{
    uint32_t vector = 0xFE & (_addr >> 24) + _chanNum;
    return vector;
}

int main(int argc, char *argv[])
{
//    using namespace MovingCoilsServerNS;

//    QString dev_ip      = "192.168.1.128";
//    quint16 dev_port    = 6000;

    QString dev_ip      = "192.168.1.6";
    quint16 dev_port    = 6000;

//    QString dev_ip      = "192.168.1.137";
//    quint16 dev_port    = 6000;

//    QString dev_ip      = "192.168.1.127";
//    quint16 dev_port    = 10001;

//    quint32 volt_set_base_addr   = (0x44000000); OSC_DRDY = 1;
//    quint32 volt_set_base_addr   = (0x10000000); OSC_DRDY = 1;

    float time_q;
    //    QString dev_ip      = "192.168.1.137";
    //    quint16 dev_port    = 10001;

    int devInd = 0;

    quint32 base_addr   = devList[devInd].base_addr;

    std::vector<VsDC3v2> vsdcList;
//    std::array<VsDC3v2, devList.size()> vsdcList;

//    cout << "Start!" << endl;



    //CONNECT TO SERVER
    bool res = connectToServer(mServer, dev_ip, dev_port);
    if (!res) exit(1);
    std::cout << "connect!" << std::endl;
//    auto ind=0;
//    vsdcList.push_back(VsDCFactory(devList[ind], ind));
//    vsdcList[ind].assignBaseAddr(devList[ind].base_addr, devList[ind].chanNum, ind);
//    disconnectFromServer(mServer);
//    return 0;

    try
    {
        //Initialize list
    //    for(auto const& vsdc : vsdcList)

        for(auto ind=0ul; ind<devList.size(); ++ind)
        {
            vsdcList.push_back(VsDCFactory(devList[ind], ind));
            std::cout << "calibration" << std::endl;
            vsdcList[ind].assignBaseAddr(devList[ind].base_addr, devList[ind].chanNum, ind);
        }
        //WRITE CALIB CONSTS

        if(DO_WRITE_CALIB)
        {
            if(1)
            {
                int cxx;
                for(auto vsdc3Dev : vsdcList)
                {
                    vsdc3Dev.setDacValue(0.168);
                }
                 std::cout << "dac = 0.168 press any key5" << std::endl;
                std::cin >> cxx;
                for(auto vsdc3Dev : vsdcList)
                {
                    vsdc3Dev.setDacValue(1.8);
                }
                std::cout << "dac = 1.8 press any key6" << std::endl;
                std::cin >> cxx;
            }
            for(auto ind=0ul; ind < devList.size(); ++ind)
            {
                vsdcList[ind].uploadRefVoltage(devList[ind].refVoltages);
            }
            throw std::string("Calib Voltages");

        }

        //CALIB DEVICE
        if(1) {
            for(auto vsdc3Dev : vsdcList) {
                vsdc3Dev.calibAll();
            }
            Sleep(1000);
        }
        //GET DEV INFO
        if(1)
        {
    //        for(int devInd=0; devInd<devList.size(); ++devInd)
            for(auto vsdc3Dev : vsdcList)
            {
                std::cout << vsdc3Dev.printDevInfo();
            }

        }
        //CALIB DEVICE
        if(1) {
            for(auto vsdc3Dev : vsdcList) {
                vsdc3Dev.calibAll();
//                vsdc3Dev.pstartAll();
            }
            Sleep(1600);
//            {
//                    auto resStr = makeOscRead(VsDCOscSettings(ADC_GAIN2 | ADC_GND, 0.01, 0, 0, 0, 100, 128), vsdcList);
//                    std::cout << resStr;
//            }
        }

        //SET DAC
        if(0) {
            for(auto vsdc3Dev : vsdcList)
            {
                vsdc3Dev.setDacValue(1.0);
                std::cout << vsdc3Dev.getDacValue() << std::endl;
                Sleep(10000);
            }
        }

        //CONFIGURE DEVICE CHANNELS FOR TANGO DEVICE SERVER TESTing
        if(0) {

            //quint32 settReg, quint32 timerReg, quint32 avg2inNReg = 0, quint32 prTimerReg=0
            auto vsdc3Dev = vsdcList[0];

            int chan=0;


            VsDCChSettings conf[] = {VsDCChSettings(ADC_GAIN2 | ADC_REF_H, 20000000, 10, 0),
                                     VsDCChSettings(ADC_GAIN2 | ADC_REF_H, 20000000, 10, 0)
                                    };
//            std::cout << std::hex << "state:\t" << vsdc3Dev.getGlobalState() << "\t";
//            for(chan=0; chan<vsdc3Dev.chanCount(); ++chan)
//            {
//                vsdc3Dev.configureChannel(conf[chan], chan);
//                std::cout << "chan[" << chan << "]\t" << vsdc3Dev.getState(chan) << "\t";
//            }
//            vsdc3Dev.pstopAll();

//            conf[0] = VsDCChSettings(ADC_GAIN2 | ADC_REF_H, 20000000, 10, 1000000000);
//            conf[1] = VsDCChSettings(ADC_GAIN2 | ADC_REF_H, 20000000, 10, 1000000000);

            conf[0] = VsDCChSettings(ADC_GAIN2 | ADC_REF_L, 20000000, 10, 1000000000);
            conf[1] = VsDCChSettings(ADC_GAIN2 | ADC_REF_L, 20000000, 10, 1000000000);



            for(chan=0; chan<vsdc3Dev.chanCount(); ++chan)
            {
                vsdc3Dev.configureChannel(conf[0], chan);
                std::cout << std::hex << "chan[" << chan << "]\t" << vsdc3Dev.getState(chan) << std::dec << "\t";
            }
//            vsdc3Dev.setDacValue(1.6);
            Sleep(3000);
            vsdc3Dev.pstartAll();
            Sleep(10000);

//            for(chan=0; chan<vsdc3Dev.chanCount(); ++chan)
//            {
//                vsdc3Dev.setSettReg(ADC_GAIN2 | ADC_GND, chan);
//            }

//            vsdc3Dev.setDacValue(0.0);


            Sleep(10000);
            Sleep(150);

            makeOscRead(VsDCOscSettings(conf[0].settReg, 1e-6*conf[0].timerReg, conf[0].avg2inNReg, 0, 0, 100, 128), vsdcList);
        }
        //Make Temperature Stability Test
        if(0)
        {
            auto vsdc3Dev = vsdcList[0];
            std::stringstream cout;
            QString tmp = QString (global_file_path) + "refVvsTemp_0x";
            cout << tmp.toStdString() << std::hex << "0x" << vsdc3Dev.baseAddr << ".txt";
//            FILE *stream0 = fopen(cout.str().c_str(), "w");
//            fclose(stream0);
            uint32_t ind=0;
            while(!abortTransaction())
            {
                FILE *stream0 = fopen(cout.str().c_str(), "a");
                vsdc3Dev.readTempReg();
                vsdc3Dev.readTempReg();
                auto temp = vsdc3Dev.calTemp;
                auto refVoltage = readHPVoltage();
                float _avg, _std;
                fprintf(stream0, "%d\t%s\t", ind++, (QTime::currentTime().toString("HH:mm:ss")).toLocal8Bit().constData());
                fprintf(stream0, "%G\t%.7G\t", temp, refVoltage);
                shortOscTest(_avg, _std, vsdc3Dev, ADC_GAIN2 | ADC_REF_H, 2000, 1000, 128);
                fprintf(stream0, "%.7G\t%.7G\t", _avg, _std);
                shortOscTest(_avg, _std, vsdc3Dev, ADC_GAIN2 | ADC_SIG, 2000, 1000, 128);
                fprintf(stream0, "%.7G\t%.7G\t", _avg, _std);
                shortOscTest(_avg, _std, vsdc3Dev, ADC_GAIN2 | ADC_GND, 2000, 1000, 128);
                fprintf(stream0, "%.7G\t%.7G\n", _avg, _std);
                fclose(stream0);
                std::cout << ind << "\t" << temp << "\t" << refVoltage << std::endl;
                Sleep(2000);
            }
        }
        //Test Front Panel START/STOP
        if(0)
        {
            int cxx;
            int chan;
            VsDCChSettings conf[] = {VsDCChSettings(ADC_GAIN2 | ADC_REF_H | ADC_START_SYNCA | ADC_STOP_TIMER, 10000, 0, 0)};

            for(auto vsdc3Dev : vsdcList)
            {
                conf[0] = {VsDCChSettings(ADC_GAIN2 | ADC_REF_H | ADC_START_SYNCA | ADC_STOP_TIMER, 10000, 0, 0)};
                for(chan=0; chan<vsdc3Dev.chanCount(); ++chan)
                {
                    vsdc3Dev.configureChannel(conf[0], chan);
                }
                std::cout << std::hex << vsdc3Dev.baseAddr << std::dec <<  "\tSYNC_A\n";
                std::cout << "press any key0" << std::endl;
                std::cin >> cxx;

                conf[0] = {VsDCChSettings(ADC_GAIN2 | ADC_REF_H | ADC_START_SYNCB | ADC_STOP_TIMER, 10000, 0, 0)};
                for(chan=0; chan<vsdc3Dev.chanCount(); ++chan)
                {
                    vsdc3Dev.configureChannel(conf[0], chan);
                }
                std::cout << std::hex << vsdc3Dev.baseAddr << std::dec <<  "\tSYNC_B\n";
                 std::cout << "press any key1" << std::endl;
                std::cin >> cxx;

                conf[0] = {VsDCChSettings(ADC_GAIN2 | ADC_REF_H | ADC_START_SYNCC | ADC_STOP_TIMER, 10000, 0, 0)};
                for(chan=0; chan<vsdc3Dev.chanCount(); ++chan)
                {
                    vsdc3Dev.configureChannel(conf[0], chan);
                }
                std::cout << std::hex << vsdc3Dev.baseAddr << std::dec <<  "\tSYNC_C\n";
                 std::cout << "press any key2" << std::endl;
                std::cin >> cxx;

                conf[0] = {VsDCChSettings(ADC_GAIN2 | ADC_REF_H | ADC_START_SYNCD | ADC_STOP_TIMER, 10000, 0, 0)};
                for(chan=0; chan<vsdc3Dev.chanCount(); ++chan)
                {
                    vsdc3Dev.configureChannel(conf[0], chan);
                }
                std::cout << std::hex << vsdc3Dev.baseAddr << std::dec <<  "\tSYNC_D\n";
                 std::cout << "press any key3" << std::endl;
                std::cin >> cxx;

                conf[0] = {VsDCChSettings(ADC_GAIN2 | ADC_REF_H | ADC_START_PSTART | ADC_STOP_TIMER, 10000, 0, 0)};
                for(chan=0; chan<vsdc3Dev.chanCount(); ++chan)
                {
                    vsdc3Dev.configureChannel(conf[0], chan);
                }

            }
        }
        //Test BP START/STOP
        if(0)
        {
            int cxx;
            int ttl_line;
            int chan;
            VsDCChSettings conf[] = {VsDCChSettings(ADC_GAIN2 | ADC_REF_H | ADC_START_BP | ADC_STOP_TIMER, 10000, 0, 0)};

            for(auto vsdc3Dev : vsdcList)
            {
                vsdc3Dev.configureChannel(conf[0], 0);
                for(ttl_line=0; ttl_line<8; ++ttl_line)
                {
                    vsdc3Dev.setBpTTLMux(ttl_line, ttl_line, 0);
                    std::cout << std::hex << vsdc3Dev.baseAddr << std::dec <<  "\tTTL\t" << ttl_line << std::endl;
                     std::cout << "press any key4" << std::endl;
                    std::cin >> cxx;
                }

            }
        }
        //Make Oscilloscope Testing
        if(1)
        {
            std::vector<VsDCOscSettings> oscSettList;
//            quint32 sett    = ADC_GAIN2
//                            | ADC_REF_H
//    //                        | ADC_START_BP
//    //                    | ADC_STOP_BP
//    //                        | ADC_IRQ_EN
//    //                        | ADC_SINGLE
    //                    | ADC_SET_AUTOZERO
//                    ;
//            float timerSec = 0.001;
            uint32_t avgN = 0;
//            uint32_t    plot_start=32;
//            int32_t     plot_stop=-128;
//            uint32_t    stat_srart=100, stat_stop=128;
            if(0)
            {
//(uint32_t _sett, float _timerSec, uint32_t _avgN, uint32_t _plot_start=32, int32_t _plot_stop=-128, uint32_t _stat_srart=100, uint32_t _stat_stop=128)

                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_GND,   0.01, 0, 0, 0, 100, 128));
                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_SIG,   0.1, 4, 0, 0, 100, 128));
                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_H, 0.01, 0, 0, 0, 100, 128));


//                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_L, 0.01, 0, 0, 0, 100, 128));
//                  avgN=0;  oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_GND,   0.01, 0, 100, -128, 100, 128));
//                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_SIG,   0.01, 0, 100, -128, 100, 128));
//                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_H, 0.01, 0, 100, -128, 100, 128));
//                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_L, 0.01, 0, 100, -128, 100, 128));
//                avgN = 8;  oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_GND,   1.1, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 1+99/(1<<avgN), 127/(1<<avgN)+1));
//                avgN = 8;  oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_SIG,   10.1, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 1+99/(1<<avgN), 127/(1<<avgN)+1));

                avgN = 8;  oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_H, 10.1, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 1+99/(1<<avgN), 127/(1<<avgN)+1));
                avgN = 8;  oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_GND, 10.1, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 1+99/(1<<avgN), 127/(1<<avgN)+1));

            }
            if(0)
            {
                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_GND,   0.01, 0, 0/*32*/, 0/*-128*/, 100, 800));
                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_SIG,   0.01, 0, 0, 0, 100, 128));
                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_REF_H, 0.01, 0, 0/*32*/, 0/*-128*/, 100, 800));


//                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_GND,   0.01, 0, 0, 0, 100, 128));
//                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_SIG,   0.01, 0, /*0*/32, /*0*/-128, 100, 128));
//                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_GND,   0.01, 0, 0, 0, 100, 128));
//                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_REF_L, 0.01, 0, 0, 0, 100, 128));
//                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_GND,   0.01, 0, 32, -128, 100, 128));
//                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_REF_H,   0.1, 0, 32, -128, 100, 128));
//                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_GND,   0.01, 0, 32, -128, 100, 128));
//                avgN = 8;  oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_SIG,    1.1, avgN, 0/*1+31/(1<<avgN)*/, 0/*-127/(1<<avgN)-1*/, 1+99/(1<<avgN), 127/(1<<avgN)+1));

                avgN = 8;  oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_REF_H, 10.1, avgN, /*0*/1+31/(1<<avgN), /*0*/-127/(1<<avgN)-1, 1+99/(1<<avgN), 127/(1<<avgN)+1));
                avgN = 8;  oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_GND,   10.1, avgN, 0/*1+31/(1<<avgN)*/, 0/*-127/(1<<avgN)-1*/, 1+99/(1<<avgN), 127/(1<<avgN)+1));
            }
            avgN = 10;  oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_GND,   2.1, avgN, 0/*1+31/(1<<avgN)*/, 0/*-127/(1<<avgN)-1*/, 1+99/(1<<avgN), 127/(1<<avgN)+1));

            std::string dir = global_file_path.toStdString() + "osc_summery.txt";
            FILE *stream0 = fopen(dir.c_str(), "w");

            for(auto setti : oscSettList)
            {
//                auto resStr = makeOscStarts(setti, vsdcList);
                auto resStr = makeOscTest(setti, vsdcList);

                Sleep(1500);
                std::cout << resStr;
                fprintf(stream0, "%s", resStr.c_str());
            }

//            {
//                auto resStr = makeOscRead(oscSettList[0], vsdcList);
//                std::cout << resStr;
//            }

            fclose(stream0);
        }
//        Range 2V
        //Make Noise Of Int 2V
        if(0)
        {
            VsDCChSettings conf = VsDCChSettings((ADC_GAIN2 | ADC_GND), 0, 0, 0);
            for(auto vsdc3Dev : vsdcList)
            {
                for(int chan=0; chan<vsdc3Dev.chanCount(); ++chan)
                {
                    vsdc3Dev.configureChannel(conf, chan);
                }
            }
            VsdcIntNoiseTest noiseTest;
            VsDCTestConfig test_conf;
            test_conf.configureMsmts(40, 0, 6, 3);//(100, 0, 7, 4);//(80, 0, 6, 4);//(30, 0, 7, 3);
            if(1)
            {
                for(auto vsdc3Dev : vsdcList)
                    vsdc3Dev.calibAll();
                Sleep(1500);
            }
            noiseTest.doTest(test_conf, vsdcList);
            std::string filName = global_file_path.toStdString();
            noiseTest.saveDataToFile(filName);
            noiseTest.saveRawDataToFile(filName);
            Sleep(1500);
            Plot_NI(QString::fromUtf8(filName.c_str()));
        }        
        //Volt*Second Linearity Error Test
        //2V
        if(0)
        {
            VsDCChSettings conf = VsDCChSettings((ADC_GAIN2 | ADC_REF_H), 0, 0, 0);
            for(auto vsdc3Dev : vsdcList)
            {
                for(int chan=0; chan<vsdc3Dev.chanCount(); ++chan)
                {
                    vsdc3Dev.configureChannel(conf, chan);
                }
            }

            VsdcIntErrorTest errorTest;
            VsDCTestConfig test_conf;
            test_conf.configureMsmts(40, 1, 7, 3);//(30, 1, 8, 3);
            if(1)
            {
                for(auto vsdc3Dev : vsdcList)
                    vsdc3Dev.calibAll();
                Sleep(1500);
            }
            errorTest.doTest(test_conf, vsdcList);
            std::string filName = global_file_path.toStdString();
            errorTest.saveDataToFile(filName);
            errorTest.saveRawDataToFile(filName);
            Sleep(1500);
            Plot_TLT(QString::fromUtf8(filName.c_str()));
        }
//        Range 0.2V
        //Make Noise Of Int 0.2V
        if(0)
        {
            VsDCChSettings conf = VsDCChSettings((ADC_GAIN02 | ADC_GND), 0, 0, 0);
            for(auto vsdc3Dev : vsdcList)
            {
                for(int chan=0; chan<vsdc3Dev.chanCount(); ++chan)
                {
                    vsdc3Dev.configureChannel(conf, chan);
                }
            }
            VsdcIntNoiseTest noiseTest;
            VsDCTestConfig test_conf;
            test_conf.configureMsmts(40, 0, 6, 3);//(100, 0, 7, 4);//(80, 0, 6, 4);//(30, 0, 7, 3);
            if(1)
            {
                for(auto vsdc3Dev : vsdcList)
                    vsdc3Dev.calibAll();
                Sleep(1500);
            }
            noiseTest.doTest(test_conf, vsdcList);
            std::string filName = global_file_path.toStdString();
            noiseTest.saveDataToFile(filName);
            noiseTest.saveRawDataToFile(filName);
            Sleep(1500);
            Plot_NI(QString::fromUtf8(filName.c_str()));
        }
        //Volt*Second Linearity Error Test
        //0.2V
        if(0)
        {
            VsDCChSettings conf = VsDCChSettings((ADC_GAIN02 | ADC_REF_H), 0, 0, 0);
            for(auto vsdc3Dev : vsdcList)
            {
                for(int chan=0; chan<vsdc3Dev.chanCount(); ++chan)
                {
                    vsdc3Dev.configureChannel(conf, chan);
                }
            }

            VsdcIntErrorTest errorTest;
            VsDCTestConfig test_conf;
            test_conf.configureMsmts(40, 1, 7, 3);//(30, 1, 8, 3);
            if(1)
            {
                for(auto vsdc3Dev : vsdcList)
                    vsdc3Dev.calibAll();
                Sleep(1500);
            }
            errorTest.doTest(test_conf, vsdcList);
            std::string filName = global_file_path.toStdString();
            errorTest.saveDataToFile(filName);
            errorTest.saveRawDataToFile(filName);
            Sleep(1500);
            Plot_TLT(QString::fromUtf8(filName.c_str()));
        }

        //VsDC3v2 INL TESTING
        if(0)
        {
            auto vsdc3Dev = vsdcList[0];
            auto vsdc3DacDev = vsdcList[1];

            float dac_volt = 0.0;
            quint32 avgn = 6;
            auto msmt_time = 0.5;//seconds

            QVector<double> chVoltAvg[vsdc3Dev.chanCount()];
            QVector<double> chVoltInt[vsdc3Dev.chanCount()];
            QVector<double> hp3458AVolt;
            QVector<double> dacSetVolt;


            QString filename_t = global_file_path + "dataLin.txt";
            QString filename;
            filename.asprintf(filename_t.toLocal8Bit().constData());
            FILE *stream0 = fopen(filename.toLocal8Bit().constData(), "w");

            {
                fprintf( stream0, "Index\tDAC\tHP\t");
                for(uint ch_ii=0; ch_ii<vsdc3Dev.chanCount();++ch_ii)
                {
                    fprintf( stream0, "CH[%i]Avg\tCH[%i]Int\t",  ch_ii, ch_ii);
                }
                fprintf( stream0, "\n");
                fclose(stream0);
            }


//            VsDCChSettings conf = VsDCChSettings((ADC_GAIN2 | ADC_REF_H), (int)(msmt_time*1000000), avgn, 0); //quint32 _settReg, quint32 _timerReg, quint32 _avg2inNReg = 0, quint32 _prTimerReg=0
//            VsDCChSettings conf = VsDCChSettings((ADC_GAIN2 | ADC_SIG), (int)(msmt_time*1000000), avgn, 0); //quint32 _settReg, quint32 _timerReg, quint32 _avg2inNReg = 0, quint32 _prTimerReg=0
            VsDCChSettings conf = VsDCChSettings((ADC_GAIN02 | ADC_SIG), (int)(msmt_time*1000000), avgn, 0);
//            VsDCChSettings pstart_conf = VsDCChSettings((ADC_GAIN2 | ADC_REF_H | ADC_START_PSTART | ADC_STOP_PSTOP), (int)(msmt_time*1000000), avgn, 0);

            for(int chan=0; chan<vsdc3Dev.chanCount(); ++chan)
            {
                vsdc3Dev.configureChannel(conf, chan);
            }

            {
                Sleep(2000);
                vsdc3Dev.calibAll();
                Sleep(2000);
            }


            bool first_step = true;
            for(dac_volt = -0.2; dac_volt < 0.202; dac_volt += 0.01)
//            for(dac_volt = -2; dac_volt < 2.02; dac_volt += 0.1)
//            for(dac_volt = -2; dac_volt < 2.02; dac_volt += 0.0000625)
//            for(dac_volt = -1.005; dac_volt < 0.800; dac_volt += 0.0000625)
            {
//                for(int chan=0; chan<vsdc3Dev.chanCount(); ++chan)
//                {
//                    vsdc3Dev.configureChannel(pstart_conf, chan);
//                }

                vsdc3DacDev.setDacValue(dac_volt);
                auto fvolt = vsdc3DacDev.getDacValue();
                dacSetVolt.append(fvolt);
                if(first_step) {
                    first_step = false;
                    Sleep(2000);
                }
//
//                vsdc3Dev.pstartAll();
//                int ctttgh;
//                std::cin >> ctttgh;
//                vsdc3Dev.pstopAll();
//                //
//                for(int chan=0; chan<vsdc3Dev.chanCount(); ++chan)
//                {
//                    vsdc3Dev.configureChannel(conf, chan);
//                }
//                //
//                Sleep(10);
//                //
//                vsdc3Dev.setDacValue(dac_volt);


                Sleep(2000);
                //
                vsdc3Dev.pstartAll();
                Sleep((int)(msmt_time*1000)+100);

                if(0)
                {
                    std::cout << "Enter HP voltage" << std::endl;
                    float tmp;
                    std::cin >> tmp;
                    hp3458AVolt.append(tmp);
                }
                else if(1)
                {
                    QStringList arguments { global_file_path + "HP34410A.py" };
                    QProcess p;
                    p.start("python", arguments);
                    p.waitForFinished();

                    float val;
                    QString fil = global_file_path + "hpVoltage";
                    FILE *streamX = fopen(fil.toLocal8Bit().constData(), "r");
                    fscanf(streamX, "%G", &val);
                    hp3458AVolt.append(val);
                    fclose(streamX);
                }
                else
                {
                    hp3458AVolt.append(dac_volt);
                }
                std::cout << std::endl;
                std::cout << "HP: " << hp3458AVolt.last() << "\t";
                for(int chan=0; chan<vsdc3Dev.chanCount(); ++chan)
                {
                    chVoltInt[chan].append(vsdc3Dev.readIntegral(chan)/msmt_time);
                    std::cout << "Volt["<<chan<<"]: " << chVoltInt[chan].last() << "\t";

                    //READ MEMORY
                    if(1)
                    {
                        //                for(auto chan=0; chan<vsdc3Dev.chanCount(); ++chan)
                        //                {
                        //                    auto osc_data = vsdc3Dev.getOscilloscope(chan);
                        //                    //print some info
                        //                    auto mean = VsDC3v2::avg(osc_data, oscSett.stat_srart, oscSett.stat_stop);
                        //                    auto std = VsDC3v2::std(mean, osc_data, oscSett.stat_srart, oscSett.stat_stop);
//                        uint nn_tmp = mServer.readVsDC3Register( ADC_ADC_WRITE(ch_ii));
//                        float * data_osc = new float[nn_tmp];

                        auto osc_data = vsdc3Dev.getOscilloscope(chan);
                        auto mean0 = VsDC3v2::avg(osc_data, 500, 200);
                        auto std0 = VsDC3v2::std(mean0, osc_data, 500, 200);
                        chVoltAvg[chan].append(mean0);
                        std::cout << "ADC[" << chan << "]\tavg:\t" << mean0 << "V;\trms:\t" << std0 << "V;" << std::endl;
//                        get_ch_data(ch_ii, nn_tmp, data_osc);

//                        {
//                            double mean0 = Mean(data_osc, 500, nn_tmp-200);
//                            double rms0 = Rms(mean0, data_osc, 500, nn_tmp-200);
//                            chVoltAvg[ch_ii].append(mean0);
//                            cout << "ADC[" << ch_ii << "]\tavg:\t" << mean0 << "V;\trms:\t" << rms0 << "V;" << endl;
//                        }
//                        delete data_osc;
                    }
                    else
                    {
                        chVoltAvg[chan].append(chVoltInt[chan].last());
                    }
                }
                std::cout << std::endl;
                //SAVE DATA
                {
                    stream0 = fopen(filename.toLocal8Bit().constData(), "a");
                    fprintf( stream0, "%i\t%+.7G\t%+.7G\t", dacSetVolt.size(), dacSetVolt.last(), hp3458AVolt.last());
                    for(uint ch_ii=0; ch_ii<vsdc3Dev.chanCount();++ch_ii)
                    {
    //                    fprintf( stream0, "%+.7G\t%+.7G\t",  chVoltAvg[ch_ii].last(), chVoltInt[ch_ii].last());
                        fprintf( stream0, "%+.7G\t%+.7G\t",  chVoltAvg[ch_ii].last(), chVoltInt[ch_ii].last());
                    }
                    fprintf( stream0, "\n");
                    fclose(stream0);
                }
                //get_data(0, avgn, 0);
            }
        }
        //find bug with Offset
        if(0)
        {
            quint32 sett    = 0
                            | ADC_GAIN2
//                            | ADC_GAIN02
//                            | ADC_SIG
//                            | ADC_GND
                            | ADC_REF_H
    //                        | ADC_START_BP
//                            | ADC_START_SYNCA
//                            | ADC_AUCAL
    //                        | ADC_STOP_BP
    //                        | ADC_IRQ_EN
//                            | ADC_SINGLE
    //                    | ADC_SET_AUTOZERO
                        ;

            uint32_t testDevInd = 0;
            uint32_t testChanNum = 3;
            uint32_t TestCycCnt = 150;

            float timerSec = 1.001;
            uint32_t    plot_start=0;
            int32_t     plot_stop=0;//-128;
            uint32_t    stat_srart=100, stat_stop=128;
            uint32_t avg2inNReg = 0;

//            (quint32 _settReg, quint32 _timerReg, quint32 _avg2inNReg = 0,
//                               quint32 _prTimerReg=0)
            VsDCChSettings conf = VsDCChSettings(sett, 10, 0, 0);

            auto vsdc3Dev = vsdcList[testDevInd];

            for(auto chan=0; chan<vsdc3Dev.chanCount(); ++chan)
            {
                vsdc3Dev.configureChannel(conf, chan);
                vsdc3Dev.setTimerSec(timerSec, chan);
                vsdc3Dev.setPrTimerReg(0, chan);
            }

            for(auto xchan=0; xchan<vsdc3Dev.chanCount(); ++xchan) {
                for(auto chan=0; chan<vsdc3Dev.chanCount(); ++chan)
                     std::cout << std::hex << "0x" << vsdc3Dev.getState(chan) << "\t";
                std::cout << std::hex << "0x" << vsdc3Dev.getGlobalState() << "\n";

                //vsdc3Dev.pstartAll();
                vsdc3Dev.pstart(xchan);

                for(auto chan=0; chan<vsdc3Dev.chanCount(); ++chan)
                     std::cout << std::hex << "0x" << vsdc3Dev.getState(chan) << "\t";
                std::cout << std::hex << "0x" << vsdc3Dev.getGlobalState() << "\n";

                Sleep(1100);
            }

//            for(int ii=0; ii<100; ++ii) {
//                vsdc3Dev.calibAll();
//                Sleep(100);
//                auto state_tmp = vsdc3Dev.getState(0);
//                if(!(state_tmp & 0x04)){
//                    std::cout << std::hex << "ERR!0x04\t0x" << state_tmp << std::dec << std::endl;
//                    std::cout << std::hex << "0x" << vsdc3Dev.getState(0) << std::dec << std::endl;
//                }

//                Sleep(1800);
//                state_tmp = vsdc3Dev.getState(0);
//                if(!(state_tmp & 0x02)){
//                    std::cout << ii << std::hex << "ERR!0x02\t0x" << state_tmp << std::dec << std::endl;
//                    std::cout << std::hex << "0x" << vsdc3Dev.getState(0) << std::dec << std::endl;
//                }
//            }


//================
//            for(auto cyc_cnt = 0ul; cyc_cnt <= TestCycCnt; ++cyc_cnt)//Get osc and save to file
//            {
//                if(!(sett & ADC_START_SYNCA))//PSTART
//                {
//                    vsdc3Dev.pstartAll();
//                    Sleep(timerSec*1000+10);
//                }
//                else //SYNC_START
//                {
//                    do
//                    {
//                        if(vsdc3Dev.getState(testChanNum) & INTEGRAL_RDY)
//                        {
//                            vsdc3Dev.clrIntRdyBit(testChanNum);
//                            break;
//                        }
////                        Sleep(1000);
//                    }
//                    while(1);
//                }

//                //
//                {
//                    auto osc_data = vsdc3Dev.getOscilloscope(testChanNum);
//                    //print some info
//                    auto mean = VsDC3v2::avg(osc_data, stat_srart, stat_stop);
//                    auto std = VsDC3v2::std(mean, osc_data, stat_srart, stat_stop);
//                    std::cout << "CH" << testChanNum << ":\t" << mean << "V\t" << std << "V\t" << vsdc3Dev.readIntegral(testChanNum) << "Vs";
//                    if(fabs(mean) > 2.0e-5)
//                    {
//                        std::cout << "OOPS!";
//                    }
//                    std::cout << std::endl;

//                }
//            }//cyc_cnt

//            {
//                auto calConsts = vsdc3Dev.readCalibResults(testChanNum);
//                //
//                std::cout << "CH" << testChanNum << ":\t";
//                std::cout << calConsts[1] - calConsts[0] << "\t";
//                for(auto fl_cnst : calConsts)
//                {
//                    std::cout << fl_cnst << "V\t";
//                }
//                std::cout << std::endl;
//            }
//            //==============
        }
        //make oscilloscope
        if(0)
        {
            quint32 sett    = ADC_GAIN2
                              | ADC_GND
//                            | ADC_REF_H
//                              | ADC_START_BP
//                              | ADC_START_SYNCA
        //                    | ADC_STOP_BP
        //                        | ADC_IRQ_EN
        //                        | ADC_SINGLE
        //                    | ADC_SET_AUTOZERO
                    ;
            float timerSec = 0.001;
            uint32_t    plot_start=0;
            int32_t     plot_stop=0;//-128;
            uint32_t    stat_srart=100, stat_stop=128;
            uint32_t avg2inNReg = 0;
            //
            QString dir_path = global_file_path ;
        //            QString filename_t = "vsdc_0x%x_ch_%d.txt";
            QString filename;
            //
            bool do_SAVE = true;
            bool do_PLOT = true;
            //load settings
            //quint32 settReg, quint32 timerReg, quint32 avg2inNReg = 0, quint32 prTimerReg=0
            VsDCChSettings conf = VsDCChSettings(sett, 0, avg2inNReg, 0);
            if(1)//Configure and start
            {
                for(auto vsdc3Dev : vsdcList)
                {
                    for(auto chan=0; chan<vsdc3Dev.chanCount(); ++chan)
                    {
                        vsdc3Dev.configureChannel(conf, chan);
                        vsdc3Dev.setTimerSec(timerSec, chan);
                        vsdc3Dev.setPrTimerReg(0, chan);
                    }
                }
                //wait for settings to settle
                Sleep(500);
                // == PSTART
                for(auto cyc=0ul; cyc<1; ++cyc)
                {
                    for(auto vsdc3Dev : vsdcList)
                    {
                        vsdc3Dev.pstartAll();
                    }
                    //
                    Sleep(timerSec*1000+10);
                }
            }//Configure and start

            if(1)//Get osc and save to file
            {
                for(auto vsdc3Dev : vsdcList)
                {
                    for(auto chan=0; chan<vsdc3Dev.chanCount(); ++chan)
                    {
                        auto osc_data = vsdc3Dev.getOscilloscope(chan);
                        //print some info
                        auto mean = VsDC3v2::avg(osc_data, stat_srart, stat_stop);
                        auto std = VsDC3v2::std(mean, osc_data, stat_srart, stat_stop);
                        std::cout << "CH" << chan << ":\t" << mean << "V\t" << std << "V\t";
                        //do something with osc data
                        if(do_SAVE)
                        {
            //                        filename.sprintf(filename_t.toLocal8Bit().constData(), vsdc3Dev.baseAddr, chan);
                                                   //(uint32_t baseAddr, uint32_t sett, uint32_t avgn, uint32_t rangeCode, uint32_t ch_num)
                            filename = makeOscChanFilName(makeOscDevFilName(vsdc3Dev.baseAddr, sett, avg2inNReg, vsdc3Dev.getState(chan)), chan);
                            FILE *stream0 = fopen((dir_path+filename).toLocal8Bit().constData(), "w");
                            for(uint ii=0; ii<osc_data.size(); ++ii)
                            {
                                fprintf( stream0, "%+.7G\t%+.7G\n",  1.0*ii*vsdc3Dev.adcSamplingTime*(1<<avg2inNReg), osc_data[ii] );
                            }
                            fclose(stream0);
                        }
                    }
                    std::cout << std::endl;
                }

                if(do_PLOT)//plot
                {
                    QString plot_name;

                    for(auto vsdc3Dev : vsdcList)
                    {

                        plot_name = makeOscDevFilName(vsdc3Dev.baseAddr, sett, avg2inNReg, vsdc3Dev.getState(0));//.sprintf(plot_name_t.toLocal8Bit().constData(), vsdc3Dev.baseAddr);
                        PyPlotChan(global_file_path, plot_name,  plot_start, plot_stop, avg2inNReg, vsdc3Dev.chanCount()); //(dir_path, fil_name, ns = 0, ne = -1, navg = 0, ch_count = 2)
                    }
                }//plot
            }//Get osc and save to file
        }//make oscilloscope

    }
    catch(std::string &error)
    {
        //TODO: findout which operation failed
        std::cout << "handled exception catched\t" << error << std::endl;
    }
    catch(...)
    {
        std::cout << "something bad happend" << std::endl;
    }

/*
    //Check IRQ
    if(0)
    {
        //read current settings
        cout << "\tINITIAL SETTINGS" << endl;
        cout << "INT LINE:\t" << mServer.readVsDC3Register(INT_LINE) << endl;
        printVsDCIsrVectorReg();
        printVsDCSettReg();
        clearVsDC_CSR_Reg();
        //set settings
        cout << "SET SETTINGS" << endl;
        mServer.writeVsDC3Register(INT_LINE, 5);
        for(int ii=0; ii<VSDC_CH_COUNT; ++ii)
        {
            mServer.writeVsDC3Register(ADC_INT_STATUS(ii), 10+ii);
            mServer.writeVsDC3Register(ADC_SETT_REG(ii), 0x800);
            mServer.writeVsDC3Register(ADC_TIME_REG(ii), 1000000);
        }
        cout << "\tStart Calib:" << endl;
        //calib
        mServer.writeVsDC3Register(GCR, CALIB_ALL);
        printVsDCSettReg();
        Sleep(1000);
        //read again
        cout << "\tCHECK SETTINGS" << endl;
        printVsDCStatusReg();
        printVsDCSettReg();
        printVsDCIsrVectorReg();
        //start
        mServer.writeVsDC3Register(GCR, 0x01);
        Sleep(1000);
        cout << "\tSTART AND CHEC STATUS" << endl;
        printVsDCStatusReg();
        printVsDCIsrVectorReg();
    }

    //Check Status Bits
    if(0)
    {
        uint32_t tfd;
//        mServer.writeVsDC3Register(ADC0_CSR_REG, 0x11E0);
//        mServer.writeVsDC3Register(ADC1_CSR_REG, 0x11E0);
//        tfd = mServer.readVsDC3Register(ADC0_ADC_INT);
//        tfd = mServer.readVsDC3Register(ADC1_ADC_INT);


//        mServer.writeVsDC3Register(ADC0_TIME_REG, 200000);
//        mServer.writeVsDC3Register(ADC1_TIME_REG, 200000);
        cout << "CALIB\t" << endl;
        for(int ii=0; ii<15; ++ii)
        {
            cout << hex << "0x" << mServer.readVsDC3Register(GSR) << "\t0x" << mServer.readVsDC3Register(ADC1_CSR_REG) << "\t0x" << mServer.readVsDC3Register(ADC0_CSR_REG) << endl;
        }
        mServer.writeVsDC3Register(GCR, CALIB_ALL);
        for(int ii=0; ii<15; ++ii)
        {
//            mServer.writeVsDC3Register(GCR, PSTART_ALL);
            cout << hex << "0x" << mServer.readVsDC3Register(GSR) << "\t0x" << mServer.readVsDC3Register(ADC1_CSR_REG) << "\t0x" << mServer.readVsDC3Register(ADC0_CSR_REG) << endl;
//            mServer.writeVsDC3Register(ADC0_CSR_REG, PSTART);

        }
        cout << "PSTART #1\t" << endl;

        mServer.writeVsDC3Register(GCR, PSTART_ALL);
        for(int ii=0; ii<15; ++ii)
        {
            cout << hex << "0x" << mServer.readVsDC3Register(GSR) << "\t0x" << mServer.readVsDC3Register(ADC1_CSR_REG) << "\t0x" << mServer.readVsDC3Register(ADC0_CSR_REG) << endl;
        }
//        tfd = mServer.readVsDC3Register(ADC0_ADC_INT);
//        tfd = mServer.readVsDC3Register(ADC1_ADC_INT);
        cout << "PSTART #2\t" << endl;
        mServer.writeVsDC3Register(GCR, PSTART_ALL);
        for(int ii=0; ii<15; ++ii)
        {
            cout << hex << "0x" << mServer.readVsDC3Register(GSR) << "\t0x" << mServer.readVsDC3Register(ADC1_CSR_REG) << "\t0x" << mServer.readVsDC3Register(ADC0_CSR_REG) << endl;
        }
//        mServer.writeVsDC3Register(ADC0_CSR_REG, 0x11E0);
//        mServer.writeVsDC3Register(ADC1_CSR_REG, 0x11E0);
//        cout << hex << "0x" << mServer.readVsDC3Register(GSR) << "\t0x" << mServer.readVsDC3Register(ADC1_CSR_REG) << "\t0x" << mServer.readVsDC3Register(ADC0_CSR_REG) << endl;
    }
*/

/*

    //MEASURE TEMPERATURE STABILITY
    if(0)
    {
        for(int kb=0;kb<200;kb++)
        {
            dataTmp = mServer.readVsDC3Register(ADC0_CAL_TEMP);
            cout << kb << "\t| Temperature:\t | " << *(float *) (&dataTmp) << endl;
            Sleep(3300);
        }
    }


    //VsDC4 LINEARITY TEST
    if(0)
    {
        float dac_volt = 0.9;
        quint32 avgn = 6;

        QVector<double> chVoltAvg[VSDC_CH_COUNT];
        QVector<double> chVoltInt[VSDC_CH_COUNT];
        QVector<double> hp3458AVolt;
        QVector<double> dacSetVolt;

        QString filename_t =  "g:\\work\\tmpdat\\VsDC3_chopp\\dataLin.txt";
        QString filename;
        filename.sprintf(filename_t.toLocal8Bit().constData());
        FILE *stream0 = fopen(filename.toLocal8Bit().constData(), "w");

        {
            fprintf( stream0, "Index\tDAC\tHP\t");
            for(uint ch_ii=0; ch_ii<VSDC_CH_COUNT;++ch_ii)
            {
                fprintf( stream0, "CH[%i]Avg\tCH[%i]Int\t",  ch_ii, ch_ii);
            }
            fprintf( stream0, "\n");
            fclose(stream0);
        }


        for(uint ii=0; ii<VSDC_CH_COUNT;++ii)
        {
            mServer.writeVsDC3Register(ADC_SETT_REG(ii), ADC_GAIN2 + ADC_SIG);
            mServer.writeVsDC3Register(ADC_TIME_REG(ii), 500000);
            mServer.writeVsDC3Register(ADC_PR_TIME_REG(ii), 0);
            mServer.writeVsDC3Register(ADC_ADC_AVGN(ii), avgn);
        }

        for(dac_volt = -1.95; dac_volt < 1.951; dac_volt += 0.25)
        {
            for(uint ii=0; ii<VSDC_CH_COUNT;++ii)
            {
                mServer.writeVsDC3Register(ADC_ADC_WRITE(ii), 0);
            }

            mServer.writeVsDC3Register(INT_BUFF_WRITE_POS, 0);
            mServer.writeVsDC3Register(INT_BUFF_READ_POS, 0);

            //SET DAC Voltage
            {
                mServer.setVsDC3BaseAddr(volt_set_base_addr);
                mServer.writeVsDC3Register(CAL_DAC_VAL, *(quint32*)(&dac_volt) );
                dataTmp = mServer.readVsDC3Register(CAL_DAC_VAL);
                cout << "DAC Voltage:\t" << *(float *) (&dataTmp) << endl;
                mServer.setVsDC3BaseAddr(base_addr);
            }
            //
            dacSetVolt.append(*(float *) (&dataTmp));

            Sleep(2000);

            mServer.writeVsDC3Register(GCR, PSTART_ALL);

            Sleep(800);

            if(0)
            {
                cout << "Enter HP voltage" << endl;
                float tmp;
                cin >> tmp;
                hp3458AVolt.append(tmp);
            }
            else if(1)
            {
                QStringList arguments { "g:\\work\\tmpdat\\VsDC3_chopp\\HP34410A.py" };
                QProcess p;
                p.start("python", arguments);
                p.waitForFinished();

                float val;
                QString fil = "g:\\work\\tmpdat\\VsDC3_chopp\\hpVoltage";
                FILE *streamX = fopen(fil.toLocal8Bit().constData(), "r");
                fscanf(streamX, "%G", &val);
                hp3458AVolt.append(val);
                fclose(streamX);
            }
            else
            {
                hp3458AVolt.append(dac_volt);
            }

            for(uint ch_ii=0; ch_ii<VSDC_CH_COUNT;++ch_ii)
            {
                dataTmp =  mServer.readVsDC3Register( ADC_ADC_INT(ch_ii));
                chVoltInt[ch_ii].append(*(float *) (&dataTmp)/0.5);
                cout << "\n" << dec << "Int["<<ch_ii<<"]: " << *(float *) (&dataTmp) << "\tVolt[" << ch_ii << "]: " << *(float *) (&dataTmp)/0.5 << dec << "\t";

                //READ MEMORY
                if(0)
                {
                    uint nn_tmp = mServer.readVsDC3Register( ADC_ADC_WRITE(ch_ii));
                    float * data_osc = new float[nn_tmp];

                    get_ch_data(ch_ii, nn_tmp, data_osc);

                    {
                        double mean0 = Mean(data_osc, 500, nn_tmp-200);
                        double rms0 = Rms(mean0, data_osc, 500, nn_tmp-200);
                        chVoltAvg[ch_ii].append(mean0);
                        cout << "ADC[" << ch_ii << "]\tavg:\t" << mean0 << "V;\trms:\t" << rms0 << "V;" << endl;
                    }
                    delete data_osc;
                }
                else
                {
                    chVoltAvg[ch_ii].append(chVoltInt[ch_ii].last());
                }
            }
            cout << endl;
            //SAVE DATA
            {
                stream0 = fopen(filename.toLocal8Bit().constData(), "a");
                fprintf( stream0, "%i\t%+.7G\t%+.7G\t", dacSetVolt.size(), dacSetVolt.last(), hp3458AVolt.last());
                for(uint ch_ii=0; ch_ii<VSDC_CH_COUNT;++ch_ii)
                {
//                    fprintf( stream0, "%+.7G\t%+.7G\t",  chVoltAvg[ch_ii].last(), chVoltInt[ch_ii].last());
                    fprintf( stream0, "%+.7G\t%+.7G\t",  chVoltAvg[ch_ii].last(), chVoltInt[ch_ii].last());
                }
                fprintf( stream0, "\n");
                fclose(stream0);
            }
            //get_data(0, avgn, 0);
        }
    }

    //Check ADC-REF of VsDC3v2 by VsDC3-old
    if(0)
    {
        uint avgn_t = 2;
        //configure VsDC3-legacy
        for(uint ii=0; ii<VSDC_CH_COUNT;++ii)
        {
            mServer.writeVsDC3Register(ADC_SETT_REG(ii), ADC_GAIN2 + ADC_SIG);
            mServer.writeVsDC3Register(ADC_TIME_REG(ii), 100000);
            mServer.writeVsDC3Register(ADC_ADC_AVGN(ii), avgn_t);
            mServer.writeVsDC3Register(ADC_PR_TIME_REG(ii), 0);
        }
        //configure VsDC3v2
        uint dut_ch_count = 4;
        mServer.setVsDC3BaseAddr(volt_set_base_addr);
        for(uint ii=0; ii<dut_ch_count;++ii)
        {
            mServer.writeVsDC3Register(ADC_SETT_REG(ii), ADC_GAIN2 + ADC_SIG);
            mServer.writeVsDC3Register(ADC_TIME_REG(ii), 100000);
            mServer.writeVsDC3Register(ADC_ADC_AVGN(ii), 0);
            mServer.writeVsDC3Register(ADC_PR_TIME_REG(ii), 0);
//            {
//                mServer.writeVsDC3Register(GCR, CALIB_ALL);
//                Sleep(1000);
//            }
        }
        mServer.setVsDC3BaseAddr(base_addr);
        //start VsDC3-legacy
        {
            mServer.writeVsDC3Register(GCR, PSTART_ALL);
        }
        //start VsDC3v2
        {
            Sleep(50);
            mServer.setVsDC3BaseAddr(volt_set_base_addr);
            mServer.writeVsDC3Register(GCR, PSTART_ALL);
            mServer.setVsDC3BaseAddr(base_addr);
        }
        Sleep(1000);
        //get data
        //READ MEMORY
        get_data(0, avgn_t, 1);
//        Sleep(1000);
//        mServer.setVsDC3BaseAddr(volt_set_base_addr);
//        get_data(1, 0, 1);
//        mServer.setVsDC3BaseAddr(base_addr);
    }



    //MAKE NOISE TEST
    if(0)
    {
        NoiseInt(&mServer);
    }

    //MAKE ERROR OF INTEGRAL TEST
    if(0)
    {
        ErrorOfTntegral(&mServer);
    }

    //GET SEVERAL WAVEFORMS
    if(0)
    {
        //
        quint32 cycNum = 10;
        quint32 avgn = 0;
        float T_int = 0.0005; //s
            uint sett   = ADC_GAIN02
                        + ADC_GND
//                        + ADC_SET_AUTOZERO
;
        uint PY_PLOT = 0;
        for(int jjj=0; jjj<1; jjj++)
        {
            //            {
            //                cout << "Enter Voltage: " << endl;
            //                float dac_volt;
            //                cin >> dac_volt;
            //                mServer.writeVsDC3Register(ADC0_DAC_VOLT, *(quint32*)(&dac_volt) );
            //                dataTmp = mServer.readVsDC3Register(ADC0_DAC_VOLT);
            //                cout << "DAC Voltage:\t" << *(float *) (&dataTmp) << endl;
            //                Sleep(100);
            //            }
            //
            if(0)
            {
                mServer.writeVsDC3Register(AUZ_PAUSE_NUM,   OSC_DRDY*100);
                mServer.writeVsDC3Register(AUZ_SW_NUM,      OSC_DRDY*128);
                mServer.writeVsDC3Register(AUZ_FULL_NUM,    OSC_DRDY*248);//12680
            }
            if(0)
            {
                sett &= (~ADC_MUX_M_MASK);     sett |= ADC_GND;
                mServer.writeVsDC3Register(ADC0_SETT_REG, sett);
                mServer.writeVsDC3Register(ADC1_SETT_REG, sett);
                sett &= (~ADC_MUX_M_MASK);     sett |= ADC_REF_H;
                Sleep(200);
            }

            make_osc_measurement(T_int, sett, avgn, 0, cycNum, PY_PLOT);//(uint rdnum, uint avgn, uint prTimer, uint startNum, uint plot_flag)
        }
    }

    //GET OSCILLOSCOPE
    if(0)
    {
        //
        quint32 cycNum = 1;
        quint32 avgn = 0;
        float T_int = 0.005; //s
        quint32 prTimer = 0;
        uint sett   = ADC_GAIN2
                    | ADC_GND
//                    | ADC_START_BP
//                    | ADC_STOP_BP
//                    | ADC_IRQ_EN
//                    | ADC_SINGLE
//                    | ADC_SET_AUTOZERO
                ;
        uint PY_PLOT = 1;

//        printVsDCIntegral();

//        for(int ii=0; ii<VSDC_CH_COUNT; ++ii)
//        {
//            mServer.writeVsDC3Register(ADC_INT_STATUS(ii), 10+ii);
//        }

//        mServer.writeVsDC3Register(ADC_BP_SYNC_MUX(0), (0 << BP_START_SEL_OFFS) | (0 << BP_STOP_SEL_OFFS));
//        mServer.writeVsDC3Register(ADC_BP_SYNC_MUX(1), (1 << BP_START_SEL_OFFS) | (1 << BP_STOP_SEL_OFFS));
//        mServer.writeVsDC3Register(ADC_BP_SYNC_MUX(2), (4 << BP_START_SEL_OFFS) | (5 << BP_STOP_SEL_OFFS));
//        mServer.writeVsDC3Register(ADC_BP_SYNC_MUX(3), (6 << BP_START_SEL_OFFS) | (7 << BP_STOP_SEL_OFFS));

//        printVsDCAUZregs();
        if(0)
        {
            mServer.writeVsDC3Register(AUZ_PAUSE_NUM,   5*128);
            mServer.writeVsDC3Register(AUZ_SW_NUM,      5*128);
            mServer.writeVsDC3Register(AUZ_FULL_NUM,    5*128);
        }

//        printVsDCsyncBPmux();

        for(int jjj=0; jjj<1; jjj++)
        {

            //
            if(0)
            {
                mServer.writeVsDC3Register(AUZ_PAUSE_NUM,   OSC_DRDY*100);
                mServer.writeVsDC3Register(AUZ_SW_NUM,      OSC_DRDY*128);
                mServer.writeVsDC3Register(AUZ_FULL_NUM,    OSC_DRDY*248); //1260
            }
            if(0)
            {
                sett &= (~ADC_MUX_M_MASK);     sett |= ADC_GND;
                mServer.writeVsDC3Register(ADC0_SETT_REG, sett);
                mServer.writeVsDC3Register(ADC1_SETT_REG, sett);
                sett &= (~ADC_MUX_M_MASK);     sett |= ADC_REF_H;
                Sleep(200);
            }

            make_osc_measurement(T_int, sett, avgn, prTimer, cycNum, PY_PLOT);//(uint rdnum, uint avgn, uint prTimer, uint startNum, uint plot_flag)
        }

    }

    //VSDC3 PRODUCTION TEST
    //GET OSCILLOSCOPE
    if(1)
    {
        for(int ii=0; ii<devList.size(); ++ii)
        {
            mServer.setVsDC3BaseAddr(devList[ii].base_addr);

//            printVsDCsyncBPmux();
//            printVsDCIntBuffviaMem();
    //        printVsDCIntBuffviaRegs();
            mServer.writeVsDC3Register(ADC_BP_SYNC_MUX(0), (0 << BP_START_SEL_OFFS) | (1 << BP_STOP_SEL_OFFS));
            mServer.writeVsDC3Register(ADC_BP_SYNC_MUX(1), (0 << BP_START_SEL_OFFS) | (1 << BP_STOP_SEL_OFFS));
//            mServer.writeVsDC3Register(ADC_BP_SYNC_MUX(2), (0 << BP_START_SEL_OFFS) | (1 << BP_STOP_SEL_OFFS));
//            mServer.writeVsDC3Register(ADC_BP_SYNC_MUX(3), (0 << BP_START_SEL_OFFS) | (1 << BP_STOP_SEL_OFFS));

            uint xii=0;
//            for(xii=0; xii<VSDC_CH_COUNT; ++xii)
            {
//                mServer.writeVsDC3Register(ADC_INT_STATUS(ii), 10+ii);
                mServer.writeVsDC3Register(ADC_TIME_REG(xii), 10000);
                mServer.writeVsDC3Register(ADC_SETT_REG(xii), ADC_GAIN2 + ADC_REF_H + ADC_START_BP + ADC_STOP_BP + ADC_IRQ_EN + ADC_SET_AUTOZERO + ADC_SINGLE);
            }
        }

    }

    //SYNC & BACKPLANE & SINGLE
    if(0)
    {
        mServer.writeVsDC3Register(INT_BUFF_WRITE_POS, 0);
        mServer.writeVsDC3Register(INT_BUFF_READ_POS, 0);
        for(uint ii=0; ii<VSDC_CH_COUNT;++ii)
        {
            mServer.writeVsDC3Register(ADC_TIME_REG(ii), 1000000);
            mServer.writeVsDC3Register(ADC_SETT_REG(ii), ADC_GAIN02 + ADC_REF_L + ADC_SET_AUTOZERO + ADC_SINGLE);
        }

        cout << "PSTART: 1\n";
        mServer.writeVsDC3Register(GCR, PSTART_ALL);
        printVsDCStatusReg();
        printVsDCStatusReg();
        printVsDCStatusReg();
        Sleep(1010);
        printVsDCStatusReg();
        printVsDCStatusReg();
        getchar();

        printVsDCStatusReg();
        cout << "PSTART: 2\n";
        mServer.writeVsDC3Register(GCR, PSTART_ALL);
        getchar();

        printVsDCStatusReg();
        cout << "PSTART: 3\n";
        printVsDCIntegral();
        mServer.writeVsDC3Register(GCR, PSTART_ALL);
        getchar();
        printVsDCIntegral();
        printVsDCStatusReg();
    }

    //TEST INT BUFF VIA REGS READING
    if(0)
    {
        //Configure Integrator
        uint avgn_t = 0;
        for(uint ii=0; ii<VSDC_CH_COUNT;++ii)
        {
            mServer.writeVsDC3Register(ADC_SETT_REG(ii), ADC_GAIN2 + ADC_REF_H);
            mServer.writeVsDC3Register(ADC_TIME_REG(ii), 100);
            mServer.writeVsDC3Register(ADC_ADC_AVGN(ii), avgn_t);
            mServer.writeVsDC3Register(ADC_PR_TIME_REG(ii), 0);
        }

        mServer.writeVsDC3Register(INT_BUFF_WRITE_POS, 0);
        mServer.writeVsDC3Register(INT_BUFF_READ_POS, 0);

        cout << "WrPos:\t" << mServer.readVsDC3Register(INT_BUFF_WRITE_POS) << "\tRdPos:\t" << mServer.readVsDC3Register(INT_BUFF_READ_POS) << endl;

        for(uint32_t cyc=0; cyc<4; ++cyc)
        {
            mServer.writeVsDC3Register(GCR, PSTART_ALL);
            Sleep(100);
        }

        cout << "WrPos:\t" << mServer.readVsDC3Register(INT_BUFF_WRITE_POS) << "\tRdPos:\t" << mServer.readVsDC3Register(INT_BUFF_READ_POS) << endl;

        //Read Back Integral Buffer
        printVsDCIntBuffviaMem();
        cout<<endl;
        //Read Back Integral Buffer
        printVsDCIntBuffviaRegs();


    }

    //TEST INT BUFF VIA MEMORY READING
    if(0)
    {
        //Configure Integrator
        uint avgn_t = 0;
        for(uint ii=0; ii<VSDC_CH_COUNT;++ii)
        {
            mServer.writeVsDC3Register(ADC_SETT_REG(ii), ADC_GAIN2 + ADC_REF_H);
            mServer.writeVsDC3Register(ADC_TIME_REG(ii), 10000);
            mServer.writeVsDC3Register(ADC_ADC_AVGN(ii), avgn_t);
            mServer.writeVsDC3Register(ADC_PR_TIME_REG(ii), 0);
        }

        mServer.writeVsDC3Register(INT_BUFF_WRITE_POS, 0);
        mServer.writeVsDC3Register(INT_BUFF_READ_POS, 0);

        cout << "WrPos:\t" << mServer.readVsDC3Register(INT_BUFF_WRITE_POS) << "\tRdPos:\t" << mServer.readVsDC3Register(INT_BUFF_READ_POS) << endl;

        mServer.writeVsDC3Register(GCR, PSTART_ALL);
        getchar();

        cout << "WrPos:\t" << mServer.readVsDC3Register(INT_BUFF_WRITE_POS) << "\tRdPos:\t" << mServer.readVsDC3Register(INT_BUFF_READ_POS) << endl;

        //Read Back Integral Buffer
        {
            uint integralCount = mServer.readVsDC3Register(INT_BUFF_WRITE_POS);
            cout << "Integral buffer count:\t" << integralCount << endl;
            uint rdAddr = (VME_INT_BUFF_MEM * 4);
            QVector<quint32> rdData;
            int chState;
            float chSwTime, chSwZero, chFullZero, chIntegral;

            //
            for(int jj=0; jj<integralCount; ++jj)
            {
                mServer.readVsDC3Registers(rdAddr, rdData, 5);
                rdAddr += 5*4;

                rdData.erase(rdData.begin());
                chState      = rdData[0] & INT_BUFF_STATUS_MASK;
                chSwTime     = *(float*) &rdData[1];
                chSwZero     = *(float*) &rdData[2];
                chFullZero   = *(float*) &rdData[3];
                chIntegral   = *(float*) &rdData[4];
                cout << ((rdData[0] & INT_BUFF_CHNUM_REG_MASK) >> INT_BUFF_CHNUM_OFFSET) << "\tstate: 0x" << hex << chState << dec << "\t" << chSwTime << " s\t" << chIntegral << " V*s" << endl;
                rdData.clear();

            }
        }
    }

    //CHECK SYNC & BACKPLANE START/STOP FROM SYNC BOARD
    if(0)
    {
        //Configure Integrator
        {
            mServer.writeVsDC3Register(ADC0_SETT_REG, ADC_GAIN2 + ADC_SIG + ADC_START_PSTART + ADC_STOP_PSTOP);
            mServer.writeVsDC3Register(ADC1_SETT_REG, ADC_GAIN2 + ADC_SIG + ADC_START_PSTART + ADC_STOP_PSTOP);

            mServer.writeVsDC3Register(INT_BUFF_WRITE_POS, 0);
            mServer.writeVsDC3Register(INT_BUFF_READ_POS, 0);

//            mServer.writeVsDC3Register(ADC0_ADC_WRITE, 0);
//            mServer.writeVsDC3Register(ADC1_ADC_WRITE, 0);

        }


        mServer.writeVsDC3Register(ADC0_CSR_REG, PSTART);
        Sleep(100);
        mServer.writeVsDC3Register(ADC0_CSR_REG, PSTOP);
        mServer.writeVsDC3Register(ADC1_CSR_REG, PSTART);
        Sleep(100);
        mServer.writeVsDC3Register(ADC1_CSR_REG, PSTOP);
        //Read Back Integral Buffer
        {
            uint integralCount = mServer.readVsDC3Register(INT_BUFF_WRITE_POS);
            cout << "Integral buffer count:\t" << integralCount << endl;
            uint rdAddr = (VME_INT_BUFF_MEM * 4);
            QVector<quint32> rdData;
            int chState;
            int intBufCellSize = 5;
            float chSwTime, chSwZero, chFullZero, chIntegral;
            //
            for(int jj=0; jj<integralCount; ++jj)
            {
                mServer.readVsDC3Registers(rdAddr, rdData, 5);
                rdAddr += 5*4;

                rdData.erase(rdData.begin());
                chState      = rdData[0] & INT_BUFF_STATUS_MASK;
                chSwTime     = *(float*) &rdData[1];
                chSwZero     = *(float*) &rdData[2];
                chFullZero   = *(float*) &rdData[3];
                chIntegral   = *(float*) &rdData[4];
                cout << (((rdData[0] & INT_BUFF_CHNUM_REG_MASK)) ? "CH1:\t" : "CH0:\t") << "state: 0x" << hex << chState << dec << "\t" << chSwTime << " s" << endl;
                rdData.clear();

            }
        }
    }

    //INTERLEAVED MEASUREMENTS FROM INTERNAL TIMER
    if(0)
    {
        quint32 meas_num = 500;

        uint sett0 = ADC_GAIN2 | ADC_REF_L  + ADC_SET_AUTOZERO;
        uint sett1 = ADC_GAIN2 | ADC_SIG    + ADC_SET_AUTOZERO;
        uint avgn = 0;
        period = 50000;   //1000000;//250000;//500000;//250000; //20e-9


        if(1)
        {
            quint32 OSC_DRDY_RB = OSC_DRDY;
            OSC_DRDY = 5;
            mServer.writeVsDC3Register(AUZ_GND_MUX_DELAY,   OSC_DRDY*0);
            mServer.writeVsDC3Register(AUZ_PAUSE_NUM,   OSC_DRDY*128);
            mServer.writeVsDC3Register(AUZ_SW_NUM,      OSC_DRDY*128);
            mServer.writeVsDC3Register(AUZ_FULL_NUM,    OSC_DRDY*580); //1260
            OSC_DRDY = OSC_DRDY_RB;
        }

        if(0)
        {
            if(base_addr   == (0x02000000))
            {
                mServer.writeVsDC3Register(AUZ_GND_MUX_DELAY,   1);
                mServer.writeVsDC3Register(AUZ_PAUSE_NUM,   OSC_DRDY*256);
                mServer.writeVsDC3Register(AUZ_SW_NUM,      OSC_DRDY*128);
                mServer.writeVsDC3Register(AUZ_FULL_NUM,    OSC_DRDY*1400);
            }
            else
            {
                mServer.writeVsDC3Register(AUZ_PAUSE_NUM,   OSC_DRDY*100);
                mServer.writeVsDC3Register(AUZ_SW_NUM,      OSC_DRDY*128);
                mServer.writeVsDC3Register(AUZ_FULL_NUM,    OSC_DRDY*220);
            }
            //            mServer.writeVsDC3Register(CAL_PAUSE,       1024);
            //            mServer.writeVsDC3Register(SW_GND_NUM,      1024);
            //            mServer.writeVsDC3Register(GND_NUM,         1024);
            //            mServer.writeVsDC3Register(GAIN_NUM,        1024);
        }
        if(0)
        {
            mServer.writeVsDC3Register(GCR, CALIB_ALL);
            Sleep(500);
        }

        config_interleaved_measurement(sett0, sett1, 2*meas_num, 0);

        startSynchroBoard();

        wait_untill_the_end(2*meas_num);//number of measurement

        stopSynchroBoard();

        read_interleaved_measurements(sett0, sett1, meas_num, base_add);

        dataTmp =  mServer.readVsDC3Register(ADC0_ADC_INT);
        cout << "Integral0:\t" << *(float *) (&dataTmp) << "Vs"  << endl;//<< "\t Avg voltage: " << (*(float *) (&dataTmp)) / ((float)timerTmp/((float)F_DRDY)) << "\t delta: " << (*(float *) (&dataTmp)) / ((float)timerTmp/((float)F_DRDY)) << endl;
        dataTmp =  mServer.readVsDC3Register(ADC1_ADC_INT);
        cout << "Integral1:\t" << *(float *) (&dataTmp) << "Vs" << endl;//"\t Avg voltage: " << (*(float *) (&dataTmp)) / ((float)timerTmp/((float)F_DRDY)) << "\t delta: " << (*(float *) (&dataTmp)) / ((float)timerTmp/((float)F_DRDY)) << endl;

        get_data(100, avgn, 1);

        print_osc_read_stat();
    }

    //SIMPLE OSC READ TEST
    int cyc = 1;
//    do
//    {
//        uint test_data = cyc+101;
//        mServer.writeVsDC3Register(ADC_TIME_REG(0), test_data);
//        uint data = mServer.readVsDC3Register(ADC_TIME_REG(0));

//        if(data != test_data )
//        {
//            cout << cyc << ": Fail! " << data << " != " << test_data << " " << mServer.readVsDC3Register(ADC_TIME_REG(0)) << endl;
//        }
//    }
//    while(++cyc<100);
    if(0)
    do
    {
        uint osc_num = 100;
        bool do_PLOT = true;
        //configure
        if(1)
        {
            mServer.writeVsDC3Register(ADC_INT_STATUS(0), 10);
            mServer.writeVsDC3Register(ADC_INT_STATUS(1), 11);
            mServer.writeVsDC3Register(ADC_TIME_REG(0), osc_num);
            mServer.writeVsDC3Register(ADC_TIME_REG(1), osc_num);
            mServer.writeVsDC3Register(ADC_SETT_REG(0), ADC_GAIN2 | ADC_SIG | ADC_IRQ_EN | ADC_SINGLE);
            mServer.writeVsDC3Register(ADC_SETT_REG(1), ADC_GAIN2 | ADC_GND | ADC_IRQ_EN | ADC_SINGLE);
            if(VSDC_CH_COUNT == 4)
            {
                mServer.writeVsDC3Register(ADC_INT_STATUS(2), 12);
                mServer.writeVsDC3Register(ADC_INT_STATUS(3), 13);
                mServer.writeVsDC3Register(ADC_TIME_REG(2), osc_num);
                mServer.writeVsDC3Register(ADC_TIME_REG(3), osc_num);
                mServer.writeVsDC3Register(ADC_SETT_REG(2), ADC_GAIN2 | ADC_GND   | ADC_IRQ_EN | ADC_SINGLE);
                mServer.writeVsDC3Register(ADC_SETT_REG(3), ADC_GAIN2 | ADC_REF_L | ADC_IRQ_EN | ADC_SINGLE);
            }
            Sleep(200);
        }

        //start pstart
        if(1)
        {
            mServer.writeVsDC3Register(GCR, PSTART_ALL);
//            mServer.writeVsDC3Register(ADC_CSR_REG(1), PSTART);
            Sleep(osc_num * 1000 * time_q + 100);
        }
        //READ MEMORY
        if(1)
        {
            QString filename_t = "g:\\work\\tmpdat\\VsDC3_chopp\\data_ch_%d.txt";
            QString filename;

            for(int ch_ii=0; ch_ii<VSDC_CH_COUNT; ++ch_ii)
//            int ch_ii=3;
            {
                uint nn_tmp = mServer.readVsDC3Register( ADC_ADC_WRITE(ch_ii));
                cout << "Points: " << nn_tmp << endl;
                float * data_osc = new float[nn_tmp];

                get_ch_data(ch_ii, nn_tmp, data_osc);

                if(do_PLOT)
                {
                    filename.sprintf(filename_t.toLocal8Bit().constData(), ch_ii);
                    FILE *stream0 = fopen(filename.toLocal8Bit().constData(), "w");
                    for(uint ii=0; ii<nn_tmp; ++ii)
                    {
                        fprintf( stream0, "%+.7G\t%+.7G\n",  1.0*ii*time_q, data_osc[ii] );
                    }
                    fclose(stream0);
                }
                delete data_osc;
            }
            //PLOT
            if(do_PLOT)
            {
                PyPlotChan("g:\\work\\tmpdat\\VsDC3_chopp", 0, 0, 0, VSDC_CH_COUNT);
            }
        }
//        printVsDCIqrLineReg();
//        printVsDCIsrVectorReg();
//        get_data(0, 0, 1);
//        printVsDCIntegral();
//        printVsDCIntBuffviaMem();

    }
    while(--cyc>0);

    if(0)
    {
        dataTmp =  mServer.readVsDC3Register(INT_BUFF_SW_TIME);
        cout << "\nSw on Time:\t" << *(float *) (&dataTmp) << "s" << endl;

        dataTmp =  mServer.readVsDC3Register(INT_BUFF_INTEGRAL);
        cout << "Integral:\t" << *(float *) (&dataTmp) << "s" << endl;

        dataTmp =  mServer.readVsDC3Register(INT_BUFF_SW_TIME);
        cout << "\nSw on Time:\t" << *(float *) (&dataTmp) << "s" << endl;

        dataTmp =  mServer.readVsDC3Register(INT_BUFF_INTEGRAL);
        cout << "Integral:\t" << *(float *) (&dataTmp) << "s" << endl;
    }


    if(0)
    {
//        uint sett = ADC_GAIN02 + ADC_REF_H + ADC_START_SYNCD+ ADC_SET_AUTOZERO;
//        mServer.writeVsDC3Register(ADC0_SETT_REG, sett);
//        mServer.writeVsDC3Register(ADC1_SETT_REG, sett);
//        mServer.writeVsDC3Register(ADC0_TIME_REG, 20000);
//        mServer.writeVsDC3Register(ADC1_TIME_REG, 20000);

        //        Sleep(1000);

        get_data(1, 0, 1);
    }
    //    QString filename_t = "g:\\work\\tmpdat\\VsDC3_chopp\\dataNO_%d.txt";
    //    QString filename;
    //    filename.sprintf(filename_t.toLocal8Bit().constData(),(1));
    //    PyPlot(filename, 30, 100);
*/
    disconnectFromServer(mServer);
    //    QCoreApplication a(argc, argv);
    return 0;//a.exec();
}
/*
void printVsDCIntBuffviaRegs(void)
{
    uint32_t integralCount = mServer.readVsDC3Register(INT_BUFF_WRITE_POS);
    float chSwTime, chSwZero, chFullZero, chIntegral;
    uint32_t chState;
    uint32_t data_tmp;
    //Read back Integral buffer via REGS
    for(int jj=0; jj<integralCount; ++jj)
    {
        mServer.writeVsDC3Register(INT_BUFF_READ_POS, jj);
        chState      = mServer.readVsDC3Register(INT_BUFF_STATUS);
        data_tmp = mServer.readVsDC3Register(INT_BUFF_SW_TIME);
        chSwTime = *(float *)(&data_tmp);
        data_tmp = mServer.readVsDC3Register(INT_BUFF_SW_ZERO);
        chSwZero = *(float *)(&data_tmp);
        data_tmp = mServer.readVsDC3Register(INT_BUFF_FULL_ZERO);
        chFullZero = *(float *)(&data_tmp);
        data_tmp = mServer.readVsDC3Register(INT_BUFF_INTEGRAL);
        chIntegral = *(float *)(&data_tmp);
        //
//                cout << "WrPos:\t" << mServer.readVsDC3Register(INT_BUFF_WRITE_POS) << "\tRdPos:\t" << mServer.readVsDC3Register(INT_BUFF_READ_POS) << endl;
        if(chState & INT_BUFF_IS_BUSY)
            cout << "Error happend:\t" << hex << chState << dec << endl;
        else
            cout << ((chState & INT_BUFF_CHNUM_REG_MASK) >> INT_BUFF_CHNUM_OFFSET)
                 << "\tstate: 0x" << hex << (chState & INT_BUFF_STATUS_MASK) << dec
                 << "\t" << chSwTime << " s\t"
                 << chIntegral << " V*s" << endl;
        //
    }
}

void printVsDCIntBuffviaMem(void)
{
    uint integralCount = mServer.readVsDC3Register(INT_BUFF_WRITE_POS);
    cout << "Integral buffer count:\t" << integralCount << endl;
    uint rdAddr = (VME_INT_BUFF_MEM * 4);
    QVector<quint32> rdData;
    int chState;
    float chSwTime, chSwZero, chFullZero, chIntegral;

    //
    for(int jj=0; jj<integralCount; ++jj)
    {
        mServer.readVsDC3Registers(rdAddr, rdData, 5);
        rdAddr += 5*4;

        rdData.erase(rdData.begin());
        chState      = rdData[0];
        chSwTime     = *(float*) &rdData[1];
        chSwZero     = *(float*) &rdData[2];
        chFullZero   = *(float*) &rdData[3];
        chIntegral   = *(float*) &rdData[4];
        if(chState & INT_BUFF_IS_BUSY)
            cout << "Error happend:\t" << hex << chState << dec << endl;
        else
            cout << ((chState & INT_BUFF_CHNUM_REG_MASK) >> INT_BUFF_CHNUM_OFFSET)
                 << "\tstate: 0x" << hex << (chState & INT_BUFF_STATUS_MASK) << dec
                 << "\t" << chSwTime << " s\t"
                 << chIntegral << " V*s" << endl;
        //
        rdData.clear();
    }
}

void printVsDCSettReg(void)
{
    cout << "SETT REG:" << hex;
    for(int ii=0; ii<VSDC_CH_COUNT; ++ii)
    {
        cout << "\t0x" << mServer.readVsDC3Register(ADC_SETT_REG(ii));
    }
    cout << dec << endl;
}

void printVsDCStatusReg(void)
{
    cout << "CSR REG:" << hex;
    for(int ii=0; ii<VSDC_CH_COUNT; ++ii)
    {
        cout << "\t0x" << mServer.readVsDC3Register(ADC_CSR_REG(ii));
    }
    cout << dec << endl;
}

void printVsDCIsrVectorReg(void)
{
    cout << "VECTOR:\t";
    for(int ii=0; ii<VSDC_CH_COUNT; ++ii)
    {
        cout << "\t" << mServer.readVsDC3Register(ADC_INT_STATUS(ii));
    }
    cout << endl;
}

void printVsDCIqrLineReg(void)
{
    cout << "IRQ LINE:\t" << mServer.readVsDC3Register(INT_LINE) << endl;
}

void printVsDCIntegral(void)
{
    uint32_t data_tmp;
    cout << "INT:";
    for(int ii=0; ii<VSDC_CH_COUNT; ++ii)
    {
        data_tmp = mServer.readVsDC3Register(ADC_ADC_INT(ii));
        cout << "\t" << *((float *)(&data_tmp)) << "Vs";
    }
    cout << endl;
}

void clearVsDC_CSR_Reg(void)
{
    uint32_t csr_tmp[VSDC_CH_COUNT];
    for(int ii=0; ii<VSDC_CH_COUNT; ++ii)
    {
        csr_tmp[ii] = mServer.readVsDC3Register(ADC_CSR_REG(ii));
        mServer.writeVsDC3Register(ADC_CSR_REG(ii), csr_tmp[ii] & CSR_W1C_MASK);
    }
    cout << "CSR REG:" << hex;
    for(int ii=0; ii<VSDC_CH_COUNT; ++ii)
    {
        cout << "\t0x" << csr_tmp[ii];
    }
    cout << endl;
    cout << "CLEARED CSR:";
    for(int ii=0; ii<VSDC_CH_COUNT; ++ii)
    {
        cout << "\t0x" << mServer.readVsDC3Register(ADC_CSR_REG(ii));
    }
    cout << dec << endl;
}

void printVsDCsyncBPmux(void)
{
    uint32_t data_tmp;
    cout << "BP MUX:\n";
    for(int ii=0; ii<VSDC_CH_COUNT; ++ii)
    {
        data_tmp = mServer.readVsDC3Register(ADC_BP_SYNC_MUX(ii));
        cout << ii << " start: TTL[" << (0x7 & (data_tmp >> BP_START_SEL_OFFS)) << "]\tstop: TTL[" << (0x7 & (data_tmp >> BP_STOP_SEL_OFFS)) << "]\n";
    }
}

void printVsDCAUZregs(void)
{
    cout << "AUZ_GND_MUX_DELAY:\t" << mServer.readVsDC3Register(AUZ_GND_MUX_DELAY) << endl;
    cout << "AUZ_PAUSE_NUM:\t\t" << mServer.readVsDC3Register(AUZ_PAUSE_NUM) << endl;
    cout << "AUZ_SW_NUM:\t\t" << mServer.readVsDC3Register(AUZ_SW_NUM) << endl;
    cout << "AUZ_FULL_NUM:\t\t" << mServer.readVsDC3Register(AUZ_FULL_NUM) << endl;
}

*/
bool connectToServer(MovingCoilsVmeCardPort & srv, QString ip, quint16 port)
{
    if (srv.isConnected())
    {
        srv.ctl_disconnect();
    }

    srv.setServerIp(ip);
    srv.setServerPort(port);

    qDebug() << QObject::tr("Try connect to [%1:%2]")
                .arg(ip)
                .arg(port)
                .toLocal8Bit().constData();
    if (srv.ctl_connect())
    {
        qDebug() << QObject::tr("Connection was successfully established").toLocal8Bit().constData();
        return true;
    }
    else
    {
        return false;
    }
}


bool disconnectFromServer(MovingCoilsVmeCardPort &srv)
{
    if (srv.isConnected())
    {
        srv.ctl_disconnect();
        qDebug() << QObject::tr("Disconnected").toLocal8Bit().constData();
    }
    return true;
}
/*
bool make_osc_measurement(float t_int, uint sett, uint avgn, uint prTimer, uint startNum, uint plot_flag){

    uint dataTmp;

    //if ADC0 mux is set to REF_L, then Amplifier of CH0 is connected to CH1 Input
    //if ADC1 mux is set to REF_L, then Amplifier of CH1 is connected to - CH1 Input

    //    sett &= (~ADC_MUX_M_MASK);     sett |= ADC_REF_L;

    uint timerTmp = round((t_int)*F_DRDY/OSC_DRDY);// + 10*(1 << avgn);

    cout << dec ;

//    //ADC_GAIN2 + ADC_SIG
//    for(uint ii=0; ii<VSDC_CH_COUNT;++ii)
//    {
//        if(ii == 0)
//            mServer.writeVsDC3Register(ADC_SETT_REG(ii), ADC_GAIN2 + ADC_REF_H);
//        else
//            mServer.writeVsDC3Register(ADC_SETT_REG(ii), ADC_GAIN2 + ADC_SIG);
//    }


    for(uint ii=0; ii<VSDC_CH_COUNT;++ii)
    {
        mServer.writeVsDC3Register(ADC_SETT_REG(ii), sett);

        mServer.writeVsDC3Register(ADC_TIME_REG(ii), timerTmp);
        dataTmp =  mServer.readVsDC3Register( ADC_TIME_REG(ii));
        cout << "Ch["<<ii<<"] Timer: " << dataTmp << "\t";
        mServer.writeVsDC3Register(ADC_PR_TIME_REG(ii), prTimer);
        mServer.writeVsDC3Register(ADC_ADC_AVGN(ii), avgn);
        mServer.writeVsDC3Register(ADC_ADC_WRITE(ii), 0);
    }

    cout << endl;

    mServer.writeVsDC3Register(INT_BUFF_WRITE_POS, 0);
    mServer.writeVsDC3Register(INT_BUFF_READ_POS, 0);

//    mServer.writeVsDC3Register(ADC0_SETT_REG, sett);

//    //    sett &= (~ADC_MUX_M_MASK);     sett |= ADC_SIG;
//    mServer.writeVsDC3Register(ADC1_SETT_REG, sett);

//    uint timerTmp = ceil((t_int)*F_DRDY/OSC_DRDY);// + 10*(1 << avgn);

//    mServer.writeVsDC3Register(ADC0_TIME_REG, timerTmp);
//    dataTmp =  mServer.readVsDC3Register( ADC0_TIME_REG);
//    cout << dec << "Ch0 Timer Setting:\t" << dataTmp << dec << endl;
//    mServer.writeVsDC3Register(ADC1_TIME_REG, timerTmp);
//    dataTmp =  mServer.readVsDC3Register(ADC1_TIME_REG);
//    cout << dec << "Ch1 Timer Setting:\t" << dataTmp << dec << endl;
//    mServer.writeVsDC3Register(ADC0_PR_TIME_REG, prTimer);
//    mServer.writeVsDC3Register(ADC1_PR_TIME_REG, prTimer);
//    mServer.writeVsDC3Register(ADC0_ADC_AVGN, avgn);
//    mServer.writeVsDC3Register(ADC1_ADC_AVGN, avgn);

//    mServer.writeVsDC3Register(INT_BUFF_WRITE_POS, 0);
//    mServer.writeVsDC3Register(INT_BUFF_READ_POS, 0);

//    mServer.writeVsDC3Register(ADC0_ADC_WRITE, 0);
//    mServer.writeVsDC3Register(ADC1_ADC_WRITE, 0);

    Sleep(20);



    for(int jj=0; jj<startNum; ++jj)
    {

        int tmpT = (int)((float)timerTmp*(1000.0/(float)F_DRDY));

        if(0)
        {
            sett &= (~ADC_MUX_M_MASK);     sett |= ADC_GND;
            mServer.writeVsDC3Register(ADC0_SETT_REG, sett);
            mServer.writeVsDC3Register(ADC1_SETT_REG, sett);
            sett &= (~ADC_MUX_M_MASK);     sett |= ADC_REF_H;
            Sleep(10000);
        }

        if(0)
        {
            cout << jj << ":\t";
            int ii;
            cin >> ii;
            if(ii == 99) break;
        }

        if(0) //SET DAC BEFORE START
        {
            float dac_volt=-0.00006;
            mServer.writeVsDC3Register(CAL_DAC_VAL, *(quint32*)(&dac_volt) );
            Sleep(1000);
        }



        mServer.writeVsDC3Register(GCR, PSTART_ALL);
//        mServer.writeVsDC3Register(ADC0_DAC_VOLT, *(quint32*)(&dac_volt) );



        if(0)
        {
            mServer.writeVsDC3Register(ADC0_SETT_REG, sett);
            mServer.writeVsDC3Register(ADC1_SETT_REG, sett);
        }

        Sleep(tmpT+15);
        cout << jj+1 << ":\t";

        if(0)
        {
            cout << "Timeout after start:\t" << tmpT << " ms" << endl;
        }

        for(uint ii=0; ii<VSDC_CH_COUNT;++ii)
        {
            mServer.writeVsDC3Register(ADC_ADC_READ(ii), 0);
        }

        if(plot_flag)
        {
            Sleep(1100);
            get_data(jj, avgn, plot_flag);
        }

//        printVsDCIntBuffviaMem();

        cout << endl;
        for(uint ii=0; ii<VSDC_CH_COUNT;++ii)
        {
            dataTmp =  mServer.readVsDC3Register( ADC_ADC_WRITE(ii));
            cout << dec << "WrAddr["<<ii<<"]: " << dataTmp << dec << "\t";

        }
        cout << endl;

        //        dataTmp =  mServer.readVsDC3Register( ADC0_CSR_REG);
        //        cout << "Ch0 State:\t" << hex << "0x" << dataTmp << dec << endl;
        //        mServer.writeVsDC3Register(ADC0_CSR_REG, dataTmp & (~0xF));
        //        dataTmp =  mServer.readVsDC3Register( ADC0_CSR_REG);
        //        cout << "Ch0 State:\t" << hex << "0x" << dataTmp << dec << endl;
        //        dataTmp =  mServer.readVsDC3Register( ADC1_CSR_REG);
        //        cout << "Ch1 State:\t" << hex << "0x" << dataTmp << dec << endl;
        //        mServer.writeVsDC3Register(ADC1_CSR_REG, dataTmp & (~0xF));
        //        dataTmp =  mServer.readVsDC3Register( ADC1_CSR_REG);
        //        cout << "Ch1 State:\t" << hex << "0x" << dataTmp << dec << endl;

//        dataTmp =  mServer.readVsDC3Register(ADC0_ADC_INT);
//        cout << "Integral0:\t" << *(float *) (&dataTmp) << "Vs"  << endl;//<< "\t Avg voltage: " << (*(float *) (&dataTmp)) / ((float)timerTmp/((float)F_DRDY)) << "\t delta: " << (*(float *) (&dataTmp)) / ((float)timerTmp/((float)F_DRDY)) << endl;
//        dataTmp =  mServer.readVsDC3Register(ADC1_ADC_INT);
//        cout << "Integral1:\t" << *(float *) (&dataTmp) << "Vs" << endl;//"\t Avg voltage: " << (*(float *) (&dataTmp)) / ((float)timerTmp/((float)F_DRDY)) << "\t delta: " << (*(float *) (&dataTmp)) / ((float)timerTmp/((float)F_DRDY)) << endl;

    }
    printVsDCIntegral();
    print_vsdc_range();
    print_osc_read_stat();

}

void print_vsdc_range(void)
{
    uint dataTmp;

    for(uint ch_ii=0; ch_ii<VSDC_CH_COUNT;++ch_ii)
    {
        dataTmp =  mServer.readVsDC3Register( ADC_CSR_REG(ch_ii));
        cout << "\n" << hex << "CSR["<<ch_ii<<"]: " << (dataTmp & 0xFFFF);
        cout << " RANGE: " << RANGES[(dataTmp >> 24) & 0xF].toStdString() << dec << "\t";
    }
}

void print_osc_read_stat(void)
{
    uint dataTmp;

    cout << "\n" << dec << "RdAd:";
    for(uint ch_ii=0; ch_ii<VSDC_CH_COUNT;++ch_ii)
    {
        dataTmp =  mServer.readVsDC3Register( ADC_ADC_READ(ch_ii));
        cout << "\t" << dataTmp;
    }

    cout << "\n" << dec << "WrAd:";
    for(uint ch_ii=0; ch_ii<VSDC_CH_COUNT;++ch_ii)
    {
        dataTmp =  mServer.readVsDC3Register( ADC_ADC_WRITE(ch_ii));
        cout << "\t" << dataTmp;
    }

    cout << "\n" << dec << "qADC:";
    for(uint ch_ii=0; ch_ii<VSDC_CH_COUNT;++ch_ii)
    {
        dataTmp =  mServer.readVsDC3Register( ADC_QUAN(ch_ii));
        cout << "\t" << *(float *) (&dataTmp) * 256.0;
    }

    cout << "\n" << dec << "ofsADC:";
    for(uint ch_ii=0; ch_ii<VSDC_CH_COUNT;++ch_ii)
    {
        dataTmp =  mServer.readVsDC3Register( ADC_OFFS(ch_ii));
        cout << "\t" << *(float *) (&dataTmp);
    }

    cout << "\n" << dec << "offsSW:";
    for(uint ch_ii=0; ch_ii<VSDC_CH_COUNT;++ch_ii)
    {
        dataTmp =  mServer.readVsDC3Register( ADC_SWOFF(ch_ii));
        cout << "\t" << *(float *) (&dataTmp);
    }
    cout << endl;

}

bool  get_ch_data(uint chInd, uint rdNum, float *data_osc)
{
    uint rdPortion = 1000;

    uint nn = rdNum;
    uint rdCount = 0;

//    data_osc = new float[rdNum];
    uint chaddr = 4*VME_CH_OSC_MEM[chInd];
    QVector<quint32> rdData;

    if(data_osc == NULL) return false;

    int kk=0;
    for(;;)
    {
        if(nn > rdPortion)
        {
            rdCount = rdPortion;
        }
        else
        {
            rdCount = nn;
        }

        if(rdCount>0)
        {
            mServer.readVsDC3Registers(chaddr, rdData, rdCount);
            chaddr = rdData[0];
            for(int ii=0; ii<rdCount; ii++)
            {
               data_osc[ii+kk*rdPortion] = *(float*) &rdData[ii+1];
            }
            rdData.clear();
        }

        kk++;
        nn -= rdCount;
        if(nn==0) break;
    }
    return true;
}

bool get_data(uint ind, uint avgn, uint plot_flag)
{

    uint nn_tmp[VSDC_CH_COUNT];
    uint nn;
    float * data_osc[VSDC_CH_COUNT];
    uint chaddr[VSDC_CH_COUNT];
    QVector<quint32> rdData;

    for(uint ii=0; ii<VSDC_CH_COUNT;++ii)
    {
        nn_tmp[ii] = mServer.readVsDC3Register( ADC_ADC_WRITE(ii));
        if(ii==0)
            nn=nn_tmp[0];
        else
            nn = qMin(nn, nn_tmp[ii]);
    }

    for(uint ii=0; ii<VSDC_CH_COUNT;++ii)
    {
        data_osc[ii] = new float[nn];
        chaddr[ii] = 4*VME_CH_OSC_MEM[ii];
    }

    QString filename_t = "g:\\work\\tmpdat\\VsDC3_chopp\\dataNO_%d.txt";
    QString filename;
    filename.sprintf(filename_t.toLocal8Bit().constData(),(ind+1));
    FILE *stream0 = fopen(filename.toLocal8Bit().constData(), "w");

    uint rdPortion = 1000;

    uint rdnum = nn;
    uint rdCount = 0;

    int kk=0;
    for(;;)
    {
        if(nn > rdPortion)
        {
            rdCount = rdPortion;
        }
        else
        {
            rdCount = nn;
        }

        if(rdCount>0)
        {
            for(uint ch_ii=0; ch_ii<VSDC_CH_COUNT;++ch_ii)
            {
                mServer.readVsDC3Registers(chaddr[ch_ii], rdData, rdCount);
                chaddr[ch_ii] = rdData[0];
                for(int ii=0; ii<rdCount; ii++)
                {
                    data_osc[ch_ii][ii+kk*rdPortion] = *(float*) &rdData[ii+1];
                }
                rdData.clear();
            }
            for(int ii=0; ii<rdCount; ii++)
            {
                fprintf( stream0, "%+G\t", ((1.0*(ii+kk*rdPortion))*(OSC_DRDY/((float)F_DRDY))*(1<<avgn) ) );
                for(uint ch_ii=0; ch_ii<VSDC_CH_COUNT;++ch_ii)
                {
                    fprintf(stream0, "%+.7G\t",  data_osc[ch_ii][ii+kk*rdPortion]);
                }
                fprintf(stream0, "\n");
            }
        }

        kk++;
        nn -= rdCount;
        if(nn==0) break;
    }
    fclose(stream0);

    if(plot_flag == 1)
    {
        PyPlot(filename, 0, rdnum, avgn);
//        PyPlot(filename, (128/(1 << avgn)), rdnum - int(128/(1 << avgn))-10, avgn);
//        PyPlot(filename, (1028/(1 << avgn)), rdnum - int(128/(1 << avgn))-10, avgn);
    }

    if(rdnum > 702 && rdnum < 5000000 && (1))
    {
        cout << rdnum-200 << endl;
        for(uint ch_ii=0; ch_ii<VSDC_CH_COUNT; ++ch_ii)
        {
            double mean0 = Mean((float*)data_osc[ch_ii], 500, rdnum-200);
            double rms0 = Rms(mean0, (float*)data_osc[ch_ii], 500, rdnum-200);
            cout << "ADC[" << ch_ii << "]\tavg:\t" << mean0 << "V;\trms:\t" << rms0 << "V;" << endl;
        }
    }

    for(uint ch_ii=0; ch_ii<VSDC_CH_COUNT;++ch_ii)
    {
        delete data_osc[ch_ii];
    }



    return true;
}

*/


//void Plot_NO(QString filename)
//{
//    FILE * gnuplotCmd;
//    QString str;
//    str.append("plot '");
//    str += filename;
//    str.append("' using 1:2 with lines, '");
//    str += filename;
//    str.append("' using 1:3 with lines \n");
//    gnuplotCmd = _popen("pgnuplot.exe -persist", "w");
//    fprintf(gnuplotCmd, str.toLocal8Bit().constData());
//    _pclose(gnuplotCmd);
//}

void PyPlotChan(QString filename, QString fil, int nstart, int nstop, int avgn, int chCount)
{
    FILE * gnuplotCmd;
    QString str;
//    str.append("python.exe g:\\work\\tmpdat\\VsDC3_chopp\\osc_plot_with_Channels.py ");

//    QString filename = "g:\\work\\tmpdat\\VsDC3_chopp\\dataNI.txt";
//    str.append("python.exe g:\\work\\tmpdat\\VsDC3_chopp\\noise_of_integral.py ");
    str.append("python.exe ");
    str += global_file_path;
    str += "\\osc_plot_with_Channels.py ";
    str += filename;
    str += " ";
    str += fil;
    str += " ";
    str.append(QString::number(nstart));
    str += " ";
    str.append(QString::number(nstop));
    str += " ";
    str.append(QString::number(avgn));
    str += " ";
    str.append(QString::number(chCount));
    gnuplotCmd = _popen(str.toLocal8Bit().constData(), "w");
    //    _pclose(gnuplotCmd);
}

void PyPlot(QString filename, int nstart, int nstop, int avgn)
{
    FILE * gnuplotCmd;
    QString str;
    str.append("python.exe g:\\work\\tmpdat\\VsDC3_chopp\\osc_plot.py ");
    str += filename;
    str += " ";
    str.append(QString::number(nstart));
    str += " ";
    str.append(QString::number(nstop));
    str += " ";
    str.append(QString::number(avgn));
    gnuplotCmd = _popen(str.toLocal8Bit().constData(), "w");
    //    _pclose(gnuplotCmd);
}

//void Plot_NB(void)
//{
//    FILE * gnuplotCmd;
//    gnuplotCmd = _popen("pgnuplot.exe -persist", "w");
//    fprintf(gnuplotCmd, "plot 'g:\\work\\tmpdat\\VsDC3_chopp\\dataNI.txt' using 1:2 with lines, 'g:\\work\\tmpdat\\VsDC3_chopp\\dataNI.txt' using 1:3 with lines \n");
//    _pclose(gnuplotCmd);
//}

//void Plot_TM(void)
//{
//    FILE * gnuplotCmd;
//    gnuplotCmd = _popen("pgnuplot.exe -persist", "w");
//    fprintf(gnuplotCmd, "plot 'g:\\work\\tmpdat\\VsDC3_chopp\\dataNI.txt' using 1:4 with lines, 'g:\\work\\tmpdat\\VsDC3_chopp\\dataNI.txt' using 1:5 with lines \n");
//    _pclose(gnuplotCmd);
//}

//void Plot_SWZ(void)
//{
//    FILE * gnuplotCmd;
//    gnuplotCmd = _popen("pgnuplot.exe -persist", "w");
//    fprintf(gnuplotCmd, "plot 'g:\\work\\tmpdat\\VsDC3_chopp\\dataNI.txt' using 1:6 with lines, 'g:\\work\\tmpdat\\VsDC3_chopp\\dataNI.txt' using 1:7 with lines \n");
//    _pclose(gnuplotCmd);
//}

//void Plot_FZ(void)
//{
//    FILE * gnuplotCmd;
//    gnuplotCmd = _popen("pgnuplot.exe -persist", "w");
//    fprintf(gnuplotCmd, "plot 'g:\\work\\tmpdat\\VsDC3_chopp\\dataNI.txt' using 1:8 with lines, 'g:\\work\\tmpdat\\VsDC3_chopp\\dataNI.txt' using 1:9 with lines \n");
//    _pclose(gnuplotCmd);
//}
/*
bool startSynchroBoard(void)
{
    mServer.writeVsDC3Register(TG_CSR, TG_RUN);
    return true;
}

bool stopSynchroBoard(void)
{
    return true;
}


bool config_interleaved_measurement(uint mux0, uint mux1, uint cycleNum, uint avgn)
{

//    mux0 = ADC_GAIN2 + ADC_REF_L + ADC_SET_AUTOZERO;
    mServer.writeVsDC3Register(ADC0_SETT_REG, mux0 | ADC_START_BP | ADC_STOP_BP);
//    mux1 = ADC_GAIN2 + ADC_SIG + ADC_SET_AUTOZERO;
    mServer.writeVsDC3Register(ADC1_SETT_REG, mux1 | ADC_START_BP | ADC_STOP_BP);
    //ADC_AUCAL ADC_SET_AUTOZERO    ADC_SET_GAIN_DIS
    mServer.writeVsDC3Register(TG_SETTINGS,   TG_START_TIMER_SRC |
                               TG_CH0_ENA |
                               TG_CH1_ENA |
                               TG_CH2_ENA |
                               TG_CH3_ENA |
                               (cycleNum << 12)
                               );
    mServer.writeVsDC3Register(TG_TMR_PERIOD, period);
    //    mServer.writeVsDC3Register(TG_CH0_PHASE, 0);
    //    mServer.writeVsDC3Register(TG_CH1_PHASE, phase);
    //    mServer.writeVsDC3Register(TG_CH2_PHASE, phase);
    //    mServer.writeVsDC3Register(TG_CH3_PHASE, 0);

    mServer.writeVsDC3Register(INT_BUFF_WRITE_POS, 0);
    mServer.writeVsDC3Register(INT_BUFF_READ_POS, 0);

    {
        mServer.writeVsDC3Register(ADC0_ADC_AVGN, avgn);
        mServer.writeVsDC3Register(ADC0_ADC_WRITE, 0);
        mServer.writeVsDC3Register(ADC0_ADC_READ, 0);

        mServer.writeVsDC3Register(ADC1_ADC_AVGN, avgn);
        mServer.writeVsDC3Register(ADC1_ADC_WRITE, 0);
        mServer.writeVsDC3Register(ADC1_ADC_READ, 0);
    }
    return true;
}

bool wait_untill_the_end(uint cycleNum)
{
    float timeOut = 500 + (1.001*20.0e-6 * (float)period)*((float)cycleNum);
    const float dTwait = 60000.0;
    cout << "Time out:  " << timeOut / 1000.0 << " seconds." << endl;

    for(;timeOut>=0.0;)
    {
        if(timeOut>dTwait)
        {
            Sleep(dTwait);
        }
        else
        {
            Sleep(timeOut);
            break;
        }
        timeOut -= dTwait;
        cout << timeOut / 60000.0 << " minutes remain" << endl;
    }
    return true;
}

bool read_interleaved_measurements(quint32 sett0, quint32 sett1, quint32 cycleNum, quint32 base)
{
    QString filename;
    QString filename_t = global_file_path + "\\dataNI_%08X.txt";
    filename.sprintf(filename_t.toLocal8Bit().constData(),base);
    FILE *fout;
    //make stop
    mServer.writeVsDC3Register(ADC0_SETT_REG, (sett0 & (~(ADC_STOP_SRC_MUX | ADC_START_SRC_MUX))) | ADC_START_PSTART | ADC_STOP_PSTOP);
    mServer.writeVsDC3Register(ADC1_SETT_REG, (sett1 & (~(ADC_STOP_SRC_MUX | ADC_START_SRC_MUX))) | ADC_START_PSTART | ADC_STOP_PSTOP);

    mServer.writeVsDC3Register(GCR, PSTOP_ALL);
    uint dataTmp;

    uint     * ch0State      = new uint[2*cycleNum];
    float   * ch0SwTime     = new float[2*cycleNum];
    float   * ch0SwZero     = new float[2*cycleNum];
    float   * ch0FullZero   = new float[2*cycleNum];
    float   * ch0Integral   = new float[2*cycleNum];

    uint     * ch1State      = new uint[2*cycleNum];
    float   * ch1SwTime     = new float[2*cycleNum];
    float   * ch1SwZero     = new float[2*cycleNum];
    float   * ch1FullZero   = new float[2*cycleNum];
    float   * ch1Integral   = new float[2*cycleNum];

    float   * fullIntegral = new float[2*cycleNum];

    dataTmp = mServer.readVsDC3Register(INT_BUFF_WRITE_POS);
    cout << dec << "INT_BUFF_WRITE_POS: " << dataTmp << dec << endl;

    const int intBufCellSize = 5;
    const int rdPortion = intBufCellSize * 2 * 100;
    int rdNum = intBufCellSize * 2 * cycleNum;
    int rdLen=0;
    uint rdAddr = (VME_INT_BUFF_MEM * 4);
    QVector<quint32> rdData;

    uint ii=0,kk=0,jj=0,rr=0;

    for(; rdNum>0; rdNum-=rdPortion)
    {
        if(rdNum >= rdPortion)
        {
            rdLen = rdPortion;
        }
        else
        {
            rdLen = rdNum;
        }

        mServer.readVsDC3Registers(rdAddr, rdData, rdLen);
        rdAddr += 4*rdLen;

        rdData.erase(rdData.begin());
        for(jj=0;jj<(rdLen/intBufCellSize);jj++)
        {
            if(rdData[0+intBufCellSize*jj] & INT_BUFF_CHNUM_REG_MASK)
            {
                ch1State[ii]      = rdData[0+intBufCellSize*jj] & INT_BUFF_STATUS_MASK;
                ch1SwTime[ii]     = *(float*) &rdData[1+intBufCellSize*jj];
                ch1SwZero[ii]     = *(float*) &rdData[2+intBufCellSize*jj];
                ch1FullZero[ii]   = *(float*) &rdData[3+intBufCellSize*jj];
                ch1Integral[ii++]   = *(float*) &rdData[4+intBufCellSize*jj];
            }
            else
            {
                ch0State[kk]      = rdData[0+intBufCellSize*jj] & INT_BUFF_STATUS_MASK;
                ch0SwTime[kk]     = *(float*) &rdData[1+intBufCellSize*jj];
                ch0SwZero[kk]     = *(float*) &rdData[2+intBufCellSize*jj];
                ch0FullZero[kk]   = *(float*) &rdData[3+intBufCellSize*jj];
                ch0Integral[kk++]   = *(float*) &rdData[4+intBufCellSize*jj];
            }
            fullIntegral[rr++] = *(float*) &rdData[4+intBufCellSize*jj];
            if(!(rr%10000))
                cout << rr << endl;
        }
        rdData.clear();
    }

    cout << kk << "\t" << ii << endl;
    cout << ch1SwTime[0] << endl;
    //save data
    fout = fopen(filename.toLocal8Bit().constData(), "w");
    if(kk != ii)
        cout << "Reading Error!" << endl;
    if(kk < ii)
        ii = kk;
    {
        for(int jj=0;jj<ii;jj++){
            fprintf(fout, "%+G\t%+.7G\t%+.7G\t%+.7G\t%+.7G\t%+.7G\t%+.7G\t%+.7G\t%+.7G\n", ((1.0*jj)), ch0Integral[jj], ch1Integral[jj], ch0SwTime[jj], ch1SwTime[jj], ch0SwZero[jj], ch1SwZero[jj], ch0FullZero[jj], ch1FullZero[jj]);
        }
    }

    double mean0 = Mean((float*)ch0Integral, 1, kk);
    double rms0 = Rms(mean0, (float*)ch0Integral, 1, kk);
    double mean1 = Mean((float*)ch1Integral, 1, ii);
    double rms1 = Rms(mean1, (float*)ch1Integral, 1, ii);

    cout << "INT0 avg:\t" << mean0 << " V;\tINT0 rms:\t" << rms0 << "V" << endl;
    cout << "INT1 avg:\t" << mean1 << " V;\tINT1 rms:\t" << rms1 << "V" << endl;

    fclose(fout);
    PyPlotInter(filename, cycleNum);

    delete ch0State;
    delete ch0SwTime;
    delete ch0SwZero;
    delete ch0FullZero;
    delete ch0Integral;

    delete ch1State;
    delete ch1SwTime;
    delete ch1SwZero;
    delete ch1FullZero;
    delete ch1Integral;

    delete fullIntegral;


    return true;
}
*/

void PyPlotInter(QString filename, int cycleNum)
{
    FILE * gnuplotCmd;
    QString str;
    str.append("python.exe ");
    str += python_file_path;
    str += " ";
    str += filename;
    str += " ";
    str.append(QString::number(cycleNum));
    gnuplotCmd = _popen(str.toLocal8Bit().constData(), "w");
    //    _pclose(gnuplotCmd);
}

void Plot_NI(QString dirname)
{
    FILE * gnuplotCmd;
    QString str;
//    str.append("python.exe g:\\work\\tmpdat\\VsDC3_chopp\\noise_of_integral.py ");
    str.append("python.exe ");
    str += global_file_path;
    str += "\\noise_of_integral.py ";
    str += dirname;
    gnuplotCmd = _popen(str.toLocal8Bit().constData(), "w");

}

void Plot_NI(QString dirname, QString filename)
{
    FILE * gnuplotCmd;
    QString str;
//    QString filename = global_file_path + "dataNI.txt";
//    str.append("python.exe g:\\work\\tmpdat\\VsDC3_chopp\\noise_of_integral.py ");
    str.append("python.exe ");
    str += global_file_path;
    str += "\\noise_of_integral.py ";
    str += dirname;
    str += " ";
    str += filename;
    gnuplotCmd = _popen(str.toLocal8Bit().constData(), "w");

}

void Plot_TLT(QString dirname)
{
    FILE * gnuplotCmd;
    QString str;
//    str.append("python.exe g:\\work\\tmpdat\\VsDC3_chopp\\error_of_integral.py ");
    str.append("python.exe ");
    str += global_file_path;
    str += "\\error_of_integral.py ";
    str += dirname;
    gnuplotCmd = _popen(str.toLocal8Bit().constData(), "w");
}

void Plot_TLT(QString dirname, QString filename)
{
    FILE * gnuplotCmd;
    QString str;
//    str.append("python.exe g:\\work\\tmpdat\\VsDC3_chopp\\error_of_integral.py ");
    str.append("python.exe ");
    str += global_file_path;
    str += "\\error_of_integral.py ";
    str += dirname;
    str += " ";
    str += filename;
    gnuplotCmd = _popen(str.toLocal8Bit().constData(), "w");

}
