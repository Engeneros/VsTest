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


#include <iomanip>

//#include "vsdc3noiseofintegral.h"
//#include "vsdc3abserror.h"

//#include <QDateTime>
//#include <QtGlobal>
#include <QTime>

#include <direct.h>
#include <fstream>
#include <QDir>

//using namespace std;

enum HWType
{
    TYPE_VSDC3V2,
    TYPE_VSDC4
};

const  int TESTED_HARD_WARE = TYPE_VSDC4;

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
const unsigned long STATE_MASK = 0xF;

quint32 F_DRDY = 5000000;
quint32 OSC_DRDY;

uint period;

QString global_file_path = "D:\\DC\\!W\\Prj\\VSDC3-1\\python\\"; //"G:\\work\\tmpdat\\VsDC3_chopp";
QString globTestPath =     "D:\\DC\\!W\\Tests\\2022_04_VSDC3\\";
QString python_file_path = "D:\\DC\\!W\\Prj\\VSDC3-1\\python\\50Hz_test_plot.py";//"G:\\work\\tmpdat\\VsDC3_chopp\\50Hz_test_plot.py";
std::vector<QString>  devDirs;

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

std::vector<vsdcProductionTable> devList = {
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
//        {0x70000000,    {1.77462,   0.161324},       0x50C,  4}
//        {0x72000000,    {1.774215,   0.1612975},       0x50D,  4},

//    {0x74000000,    {1.7740,   0.161271 },       0x511,  4}
//          {0x76000000,    {1.77442,  0.161301},       0x50D,  4},

   //         {0x78000000,    {1.774245,   0.161282 },       0x50F,  4}
    //    {0x7A000000,    {1.77426,   0.1612865 },       0x510,  4},
    //    {0x7c000000,    {1.77449,   0.161306 },       0x511,  4},
    //    {0x7e000000,    {1.77460,   0.1613275 },       0x512,  4},
     //      {0x80000000,    {1.77412,   0.1613276 },       0x513,  4},
    //   {0x82000000,    {1.77454,   0.161315 },       0x514,  4},


 //  {0x84000000,    {1.77407,   0.161265 },       0x515,  4},

  //  {0x86000000,    {1.77417,   0.161284 },       0x516,  4},
 //{0x88000000,    {1.77438,   0.161299 },       0x517,  4}// bad sinus osc

    //{0x8A000000,    {1.77435,   0.161289},       0x518,  4},
//       {0x8C000000,    {1.774295,   0.161288},       0x519,  4}



     //
    //{0x3E000000,    {1.80000,   0.168100},       0x508,  2}
  // --- {0x3C000000,    {1.80000,   0.168100},       0x508,  2} chan0  ADC
    ////    {0x06000000,   {1.79996,        0.168065},       0x322,  2},
    ////    {0xC0000000,   {1.80007,   0.168071},       0x310,  2},
    //    //DAC dev
    //    {0x22000000,    {1.80007,   0.168071},       0x310,  2},
//        {0x4A000000,    {1.80011,   0.168043},       0x51C,  2},
//        {0x40000000,    {1.80004,   0.168025},       0x51A,  2},
//        {0x46000000,    {1.79993,   0.168042},       0x51B,  2},
//        {0x48000000,    {1.80007,   0.16806},       0x51C,  2}

//   {0x4c000000,    {1.80011,   0.168043},       0x51d,  2},
//   {0x4e000000,    {1.8001,   0.168032},       0x51e,  2},
//   {0x50000000,    {1.800044,   0.1680583},       0x51f,  2},
//   {0x52000000,    {1.80011,   0.168054},       0x520,  2}

 //      {0x42000000,    {1.80004,   0.168071},       0x521,  2},
 //      {0x54000000,    {1.80009,   0.168031},       0x522,  2},
 //      {0x56000000,    {1.89994,   0.168052},       0x523,  2},
 //      {0x58000000,    {1.80003,   0.168085},       0x524,  2}

 //       {0x34000000,    {1.80003,   0.168013},       0x503,  2},
 //       {0x36000000,    {1.80011,   0.168116},       0x504,  2},
  //        {0x3C000000,    {1.80007,   0.168053},       0x507,  2}, //adc ch0 q=8e-8
  //      {0x38000000,    {1.80023,   0.168116},       0x505,  2},
  //      {0x5A000000,    {1.80016,   0.168086},       0x525,  2}

 //      {0x3A000000,    {1.80016,   0.168113},       0x506,  2},
 //       {0x3E000000,    {1.80000,   0.168100},       0x508,  2}
  //  {0x5c000000,    {1.80016,   0.168091},       0x526,  2},
  //  {0x5e000000,    {1.80003,   0.168066},       0x527,  2}
   //   {0xb0000000,    {1.774213,   0.1612859},       0x528,  2} // old series
  //   {0x44000000,    {1.7997,   0.168111},       0x50B,  2}
   //  {0x38000000,    {1.80023,   0.168121},       0x505,  2}
    {0x50000000,    {  1.77369,   0.161235 }, 0x529,  4},// #1 (ref) (0x50) VSDC4 v.0
  //   {0x02000000,    { 1.77318,   0.161191 }, 0x52A,  4},// #2 (0x40) VSDC4 v.0
//     {0x04000000,    { 1.77419,   0.161284 }, 0x52B,  4},// #3 (0x44) VSDC4 v.0
//
    //{0x06000000,    {1.77402,   0.161261 },       0x52C,  4},// #4 (0x48) VSDC4 v.0
    // {0x08000000,    {1.77404,   0.161265 },       0x52D,  4},// #5(0x4C)  VSDC4 v.0
    {0x5c000000,    { 1.80016,   0.168091 }, 0x526,  2}

    };

//std::vector<vsdcProductionTable> devList = {
//    {0x22000000,    {1.80018,   0.168061},       0x501,  2},
//    {0x24000000,    {1.80002,   0.168096},       0x502,  2},
//    //DAC dev
////    {0x22000000,    {1.80018,   0.168061},       0x501,  2},
//};


//std::vector<vsdcProductionTable> devList = {
//    {0x20000000,    {1.774245,   0.161300},       0x601,  4},
    //DAC dev
//    {0x34000000,    {1.80003,   0.168061},       0x502,  2},
//      {0x34000000,    {1.80003,   0.168013},       0x503,  2},
//      {0x36000000,    {1.80011,   0.168116},       0x504,  2},
//      {0x38000000,    {1.80023,   0.168121},       0x505,  2},
//      {0x3A000000,    {1.80016,   0.168117},       0x506,  2}

//    {0x3C000000,    {1.80007,   0.168053},       0x507,  2},
//    {0x3E000000,    {1.80000,   0.168100},       0x508,  2},
//    {0x40000000,    {1.80055,   0.167967},       0x509,  2},
//    {0x42000000,    {1.80001,   0.168073},       0x50A,  2},
//    {0x44000000,    {1.7997,   0.168111},       0x50B,  2}
//      {0x40000000,    {1.80018,   0.168061},       0x507,  2},
//      {0x42000000,    {1.80018,   0.168061},       0x508,  2},
//};

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


enum ADC_Signal_Src
{
    INTERNAL_SRC,
    EXT_GENERATOR
};

std::string makeOscStarts(VsDCOscSettings oscSett, std::vector<VsDC3v2> &vsdcList, ADC_Signal_Src srcADC)
{
    int deviceCount = 0;
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
        std::cout << "...";
    //    while (true)
     //   {

//        }

        for(chan=0; chan<vsdc3Dev.chanCount(); ++chan)
        {
            Sleep(250);
            vsdc3Dev.setTimerSec(oscSett.timerSec, chan);
        }

//        vsdc3Dev.configureAutoZero();


    }

    if(srcADC == INTERNAL_SRC)
    {
        for(auto vsdc3Dev : vsdcList)
            vsdc3Dev.pstartAll();
        Sleep(oscSett.timerSec * 1000 + 100);
    }
    else
    {
        char cxx;
        for(auto vsdc3Dev : vsdcList)
        {
            std::cout << std::endl << "connect generator outputs to inputs device ";
            std::cout << std::hex << (vsdc3Dev.baseAddr >> 24) <<  " and type 'g' to continue" << std::endl;
            std::cout << "or press any key to skip device" << std::endl;
            std::cin >> cxx;
            if((cxx == 'g') || (cxx == 'G'))
            {
               // vsdc3Dev.calibAll();
                    vsdc3Dev.pstartAll();
                    Sleep(oscSett.timerSec * 1000 + 100);

//                        while (true)
//                       {

//                       }
            }
        }
    }
    //
   // Sleep(oscSett.timerSec*1000+100);
   //        Sleep(100);
   //        vsdc3Dev.setDacValue(-1.8);
    std::cout << std::dec << "==" << std::endl;
    return cout.str();
}

std::string makeOscRead(VsDCOscSettings oscSett, std::vector<VsDC3v2> &vsdcList, QString preamble = "")
{
    std::stringstream cout;
//    QString dir_path = globTestPath;//global_file_path;// "D:\\DC\\!W\\Prj\\VSDC3-1\\python\\;"//"g:\\work\\tmpdat\\VsDC3_chopp\\";
    QString filename;
//    QString dirName;

    QString oscDir[vsdcList.size()];

//    if(!dir2osc.exists())
//        dir2osc.mkpath(".");
//    oscDir += "//";
    bool do_SAVE = true;
    int devCount = 0;
    for(auto vsdc3Dev : vsdcList)
    {
        oscDir[devCount] = devDirs[devCount] + QString("Osc_") + preamble;
        QDir  dir (oscDir[devCount]);
        if(!dir.exists())
            dir.mkpath(".");
        oscDir[devCount] += "\\";
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
                filename = oscDir[devCount] + preamble + makeOscChanFilName (makeOscDevFilName(vsdc3Dev.baseAddr, oscSett.sett, oscSett.avgN, vsdc3Dev.getState(chan)), chan);
                FILE *stream0 = fopen( filename.toLocal8Bit().constData(), "w");
                for(uint ii=0; ii<osc_data.size(); ++ii)
                    fprintf( stream0, "%+.7G\t%+.7G\n",  1.0*ii*vsdc3Dev.adcSamplingTime*(1<<oscSett.avgN), osc_data[ii] );
                fclose(stream0);
            }
        }
        cout << std::endl;
        ++devCount;
    }
    QString plot_name;
    for(auto i=0; i < vsdcList.size(); ++i)
    {
        plot_name = preamble + makeOscDevFilName(vsdcList[i].baseAddr, oscSett.sett, oscSett.avgN, vsdcList[i].getState(0));
        PyPlotChan(oscDir[i], plot_name,  oscSett.plot_start, oscSett.plot_stop, oscSett.avgN, vsdcList[i].chanCount()); //(dir_path, fil_name, ns = 0, ne = -1, navg = 0, ch_count = 2)
        std::cout << "*" << i << "#";
        Sleep(500);
    }

    return cout.str();
}

std::string makeOscTest(VsDCOscSettings oscSett, std::vector<VsDC3v2> &vsdcList, QString preamble)
{
    (preamble == "SIN_")? makeOscStarts(oscSett, vsdcList, EXT_GENERATOR) : makeOscStarts(oscSett, vsdcList, INTERNAL_SRC);

    return makeOscRead(oscSett, vsdcList, preamble);
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


void DirCreate(const std::vector <QString> &parents, QString name, std::vector <QString> &childs, std::vector<QString> &wSlash)
{
   // std::vector <QString> vsecDir;
    for(int i = 0; i < parents.size(); ++i)
    {
        childs.push_back( parents[i] + name);
        wSlash.push_back(childs[i] + QString("\\") );
        QDir dir(childs[i]);
        if(!dir.exists())
            dir.mkpath(".");

    }
}


uint32_t vsdc3IqrVectorFactory( uint32_t _addr, uint32_t _chanCount, uint32_t _chanNum)
{
    uint32_t vector = 0xFE & (_addr >> 24) + _chanNum;
    return vector;
}


char SetRefVoltage(double voltage, int devType, VsDC3v2 &dev)
{
   char tmp = 'g';
   if(devType == TYPE_VSDC3V2)
      dev.setDacValue(voltage) ;
   else if (devType == TYPE_VSDC4)
   {
      std::cout << ( (voltage > 1.5)?  "\n connect to HIGH REF \n" : "\n connect to LOW REF\n");
      std:: cout << std::endl << "and press G to continue, X to exit" << std::endl;
      std::cin >> tmp;
      std::cout << std::endl;
   }
   return tmp;
}


double Get34465Voltage()
{
    QProcess p;
    std::ifstream hpVlt;
    QString hpVFName =  globTestPath + "hpVoltage";
    QStringList arguments {global_file_path + "HP34410A.py"};
    p.start("python", arguments);
    p.waitForFinished();

    hpVlt.open(hpVFName.toStdString());
    double retV;
    hpVlt >> retV;
    hpVlt.close();
    remove (hpVFName.toStdString().c_str());
    return retV;
}

const int ALL_CHAN = -1;

void SetRange(VsDC3v2 &dev,  unsigned long rng, int chan)
{
    VsDCChSettings conf = VsDCChSettings((rng | ADC_GND), 1000, 0, 0);
    if (chan == ALL_CHAN)
    {
      dev.configureChannel(conf, 0);
      dev.configureChannel(conf, 1);
    }
    else
        dev.configureChannel(conf, chan);
}



void PrintDeviceInfo(std::vector<VsDC3v2> vsdcList)
{
    std::ofstream infoFile;
    infoFile.exceptions(std::ofstream::badbit | std::ofstream::failbit);
    QString fName;
    for(auto ind=0ul; ind < devList.size(); ++ind)
    {
        std::cout << vsdcList[ind].printDevInfo();
        std::cout << std::endl;
        fName = devDirs[ind]  + QString("0x%1_info.txt").arg(vsdcList[ind].baseAddr >> 24, 0, 16);
        std::cout << fName.toStdString() << std::endl;
        try
        {
            infoFile.open(fName.toStdString().c_str());
            if(!infoFile.is_open())
               std::cout << "not open"; //throw std::string("file" + fName.toStdString() + "not open");

            SetRange(vsdcList[ind], ADC_GAIN02, ALL_CHAN);
            infoFile << vsdcList[ind].printDevInfo();

            SetRange(vsdcList[ind], ADC_GAIN2, ALL_CHAN);
            infoFile << vsdcList[ind].printDevInfo();

            infoFile.close();
        }
       catch(std::ofstream::failure &ex)
        {
           std::cout << ex.what();
        }
    }
}


void CalibrateDevices(std::vector<VsDC3v2> vsdcList)
{

   for(auto vsdc3Dev : vsdcList)
        vsdc3Dev.calibAll();
    Sleep(1000);
}

char WriteCalibrationConst(std::vector<VsDC3v2> &vsdcList, int devT)
{
//    std::cout << "Do you want calibrate DAC? Y, N or X to exit the program" << std::endl;
      char ret;
//    std::cin >> ret;
//    if((ret == 'Y') || (ret == 'y') )
//    {
        double vltg;
        for(auto ind=0ul; ind < devList.size(); ++ind)
        {
            int baseAddr = vsdcList[ind].baseAddr >> 24;
            if (TESTED_HARD_WARE == TYPE_VSDC4)
                std::cout << "\n device " << std::hex << baseAddr << ":" << std::endl;
            else if (TESTED_HARD_WARE == TYPE_VSDC3V2)
                std::cout << "\n connect reference or DAC output device " << std::hex << baseAddr <<  " to voltmeter and press anykey to continue, X to exit \n";
            else
            {
                ret = 'x';
                break;
            }
            std:: cin >> ret;
            ret = SetRefVoltage(1.8, TESTED_HARD_WARE, vsdcList[ind]);
            //vsdcList[ind].setDacValue(1.8);
            Sleep(2000);
            vltg = Get34465Voltage();
            std::cout <<  "High reference (1.8V) is: " <<  vltg  << "V "  << std::endl;
            if((vltg < 0.1) || (ret == 'x') || (ret == 'X'))
                break;
            devList[ind].refVoltages[0]=vltg;//.push_back(vltg);
            ret = SetRefVoltage(0.168, TESTED_HARD_WARE, vsdcList[ind]);
            vltg = Get34465Voltage();
 //           vsdcList[ind].setDacValue(0.168);
            Sleep(2000);
            std::cout << "Low reference (0.168V) is: " <<  vltg  << "V "  << std::endl;
            if((vltg < 0.1) || (ret == 'x') || (ret == 'X'))
                break;
            devList[ind].refVoltages[1]=vltg;//.push_back(vltg);
        }
        std::cout << "Upload Constants? Y or N" << std::endl;
        char yOrN;
        std::cin >> yOrN;
        if((yOrN == 'Y') || (yOrN == 'y') )
        {
            for(auto ind=0ul; ind < devList.size(); ++ind)
            {
                vsdcList[ind].uploadRefVoltage(devList[ind].refVoltages);
                std::cout << "upload " << std::hex << (vsdcList[ind].baseAddr >> 24) << std::endl;
            }
        }
       CalibrateDevices(vsdcList);
       PrintDeviceInfo(vsdcList);
//    }
    return ret;
}





char NoiseTest(std::vector<VsDC3v2> vsdcList)
{
//    char yOrN;
//    std::cout << std::endl <<  "-----------NOISE TESTS------------" << std::endl;
//    std::cout << "Do you want launch Noise test? Y, N or X to exit" << std::endl;
//    std::cin >> yOrN;//range_in_mV;
//    if((yOrN == 'Y') || (yOrN == 'y') )
//    {
        int range = 2000;
        unsigned short regRng;
        while(range != 0) //for (short range_in_mV = 200; range_in_mV != 0;; )
        {
            if(TESTED_HARD_WARE == TYPE_VSDC3V2)
            {
                std::cout << "Enter 2000 for 2V input range Noise test" << std:: endl;
                std::cout << "Enter 200 for  0.2V input range Noise test" << std:: endl;
                std::cout << "Enter 0 to exit Noise tests" << std:: endl;
                std::cin >> range;
            }
            if((range == 200) || (range == 2000))
            {
                std::cout << "Noise test " << (double) range << "mV run, wait" << std::endl;
                regRng = (range == 2000)? 0 : 0x0100;
                VsDCChSettings conf = VsDCChSettings((regRng | ADC_GND), 0, 0, 0);                
                for(auto vsdc3Dev : vsdcList)
                    for(int chan=0; chan<vsdc3Dev.chanCount(); ++chan)
                        vsdc3Dev.configureChannel(conf, chan);
                VsdcIntNoiseTest noiseTest;
                VsDCTestConfig test_conf;
                test_conf.configureMsmts(40, 0, 6, 3);//(100, 0, 7, 4);//(80, 0, 6, 4);//(30, 0, 7, 3);
                for(auto vsdc3Dev : vsdcList)
                   vsdc3Dev.calibAll();
                Sleep(1500);
                std::vector <QString> dirs;
                std::vector <QString> dirAndSlash;
                DirCreate(devDirs, QString("Noise"), dirs, dirAndSlash);

                std::cout << "---0----";
                noiseTest.doTest(test_conf, vsdcList);
                std::cout << "---1----";
                noiseTest.saveDataToFile(dirAndSlash);////devDirs);
                std::cout << "---2----";
                noiseTest.saveRawDataToFile(dirAndSlash);////devDirs);
                std::cout << "---3----";

                for(int i = 0; i < devList.size(); ++i)
                {
                    Sleep(1500);
                    Plot_NI(dirAndSlash[i]); //devDirs[ind]);//QString::fromUtf8(filName.c_str()));
                }
                for (int i = 0; i < 3; ++i)
                {
                    Sleep(1000);
                    std::cout << "wait" << std::endl;
                }
                Sleep(1000);
                std::cout << "Noise test " << std::dec << range << "mV done" << std::endl;
            }
            if(TESTED_HARD_WARE == TYPE_VSDC4)
                range = 0;
        }
//   }
   std::cout << "==================THE END OF NOISE TESTS==============" << std::endl << std::endl;
   return 't';
}
const int VXS_SWITSH_BAGS_FIND = 0;
//--
char VoltXSecondTest(std::vector<VsDC3v2> &vsdcs)
{
//    char yOrN;
//    std::cout <<  std::endl << "----------VOLT * SECONDS TESTS-------------" << std::endl;
//    std::cout << "Do you want launch Volt*Second test?  Y, N or X to exit" << std::endl;
//    std::cin >> yOrN;//range_in_mV;
    //if((yOrN == 'Y') || (yOrN == 'y') )
    //{
        int range = 2000;
        unsigned short regRng;
        while(range != 0) //for (short range_in_mV = 200; range_in_mV != 0;; )
        {
            if(TESTED_HARD_WARE == TYPE_VSDC3V2)
            {
                std::cout << "Enter 2000 for 2V input range Volt*Second test" << std:: endl;
                std::cout << "Enter 200 for  0.2V input range Volt*Second test" << std:: endl;
                std::cout << "Enter 0 to exit Volt*Second tests" << std:: endl;
                std::cin >> range;
            }
            if((range == 200) || (range == 2000))
            {
                std::cout << "Volt * Second test " << (double) range << "mV run, wait" << std::endl;
                regRng = (range == 2000)? 0 : 0x0100;
                VsDCChSettings conf = VsDCChSettings((regRng | ADC_REF_H), 0, 0, 0);
                for(auto vsdc3Dev : vsdcs)
                    for(int chan=0; chan<vsdc3Dev.chanCount(); ++chan)
                        vsdc3Dev.configureChannel(conf, chan);
                VsdcIntErrorTest errorTest;
                VsDCTestConfig test_conf;
                test_conf.configureMsmts(40, 1, 7, 3);//(30, 1, 8, 3);
                for(auto vsdc3Dev : vsdcs)
                    vsdc3Dev.calibAll();
                Sleep(1500);
                std::vector <QString> dirs;
                std::vector <QString> dirAndSlash;
                DirCreate(devDirs, QString("VxS"), dirs, dirAndSlash);

//                for(auto ind=0ul; ind < devList.size(); ++ind)
//                {
//                    vsecDir.push_back( devDirs[ind] + QString("Volt_x_Sec"));
//                    QDir dir(vsecDir[ind]);
//                    if(!dir.exists())
//                        dir.mkpath(".");
//                    vsecDir[ind] += "\\";
//                }
                if(VXS_SWITSH_BAGS_FIND)
                {
                    char temp = 'a';
                    do {


                        for (int i = 0; i < 20; ++i)
                        {
                            errorTest.doTest(test_conf, vsdcs);
                            Sleep(200);
                        }
                        errorTest.saveDataToFile(dirAndSlash);
                        errorTest.saveRawDataToFile(dirAndSlash);
                        Sleep(1500);
                        std::cout << "press any key, x to exit" << std::endl;
                        std::cin >> temp;
                    } while ((temp != 'x') && (temp != 'X'));
                }
                errorTest.doTest(test_conf, vsdcs);
                errorTest.saveDataToFile(dirAndSlash);
                errorTest.saveRawDataToFile(dirAndSlash);
                for(int i=0; i < devList.size(); ++i)
                    Plot_TLT(dirAndSlash[i]);//QString::fromUtf8(filName.c_str()));

                for (int i = 0; i < 3; ++i)
                {
                    Sleep(1000);
                    std::cout << "wait" << std::endl;
                }
                Sleep(1000);
                std::cout << "Volt*Second test" << std::dec << range << "mV done" << std::endl;
            }
            if(TESTED_HARD_WARE == TYPE_VSDC4)
                range = 0;
        }
   // }
    std::cout << "======= THE END OF VOLT * SECOND TESTS ===============" << std::endl << std::endl;
    return '0';
}
//--
void OscRefsTests(std::vector<VsDC3v2> &vsdcs)
{
    char yOrN = 'Y';
//    std::cout << std::endl << "---------- THE REF + GND + FREE INPUTS OSC TESTS -------------" << std::endl;
//    std::cout << "If you want launch osc tests with reference, ground and decoupled inputs, set inputs resistaces to 120 Ohm" << std::endl;
//    std::cout << "and press 'Y' " << std::endl;
//    std::cout << "Press any key to continue and do other tests " << std::endl;
//    std::cin >> yOrN;
    if((yOrN == 'Y') || (yOrN == 'y') )
    {
        std::cout << "ok, we make oscRefGndTests, wait.." << std::endl;
        std::vector<VsDCOscSettings> oscSettList;
        std::vector <QString> dsc;
        uint32_t avgN = 6;

                        dsc.push_back(QString("2V_Ref..\n"));
                        oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_H,   10, avgN,  1+31/(1<<avgN), -127/(1<<avgN)-1, 100, 128));

  //                      dsc.push_back(QString("0V2_Ref..\n"));
 //                       oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_REF_L,   20, avgN,  1+31/(1<<avgN), -127/(1<<avgN)-1, 100, 128));

avgN = 0;
                        dsc.push_back(QString("2V_Sig..\n"));
                        oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_L,   10, avgN,  1+31/(1<<avgN), -127/(1<<avgN)-1, 100, 128));
                        dsc.push_back(QString("2V_GND..\n"));
                        oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_GND,   0.01, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 100, 128));
//                        dsc.push_back(QString("200mV_GND..\n"));
//                        oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_GND,   0.01, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 100, 128));
                        dsc.push_back(QString("2V_Sig..\n"));
                        oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_SIG,   0.01, avgN,  1+31/(1<<avgN), -127/(1<<avgN)-1, 100, 128));
//                        dsc.push_back(QString("200mV_Sig..\n"));
//                        oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_SIG,   0.01, avgN,  1+31/(1<<avgN), -127/(1<<avgN)-1, 100, 128));

                        dsc.push_back(QString("2V_refH 0.01 avg = 0..\n"));
                        oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_H, 0.01, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 100, 128));
//                        dsc.push_back(QString("2V_refL 0.01 avg = 0..\n"));
//                        oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_L, 0.01, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 100, 128));
//                        dsc.push_back(QString("200mV_refL 0.01 avg = 0..\n"));
//                        oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_REF_L, 0.01, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 100, 128));

//        dsc.push_back(QString("2V_refH front 0.01 avg = 0..\n"));
//        oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_H, 0.01, avgN, 0, 0, 100, 128));


//        dsc.push_back(QString("2V_refH 10s avg = 10..\n"));
//        avgN = 10;
//        oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_H, 10, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 100, 128));
//        dsc.push_back(QString("2V_refH 10s avg = 0..\n"));
//        avgN = 0;
//        oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_H, 10, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 100, 128));
//        avgN = 0;
//        avgN = 0;
//        dsc.push_back(QString("2V_refH 1s avg=0..\n"));
 //       oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_H, 1, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 100, 128));


//        dsc.push_back(QString("2V_refL front 0.01 avg = 0..\n"));
//        oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_L, 0.01, avgN, 0, 0, 100, 128));

//        dsc.push_back(QString("2V_refL 0.01 avg = 0..\n"));
//        oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_L, 0.01, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 100, 128));
//        dsc.push_back(QString("2V_refL 10s avg = 10..\n"));
//        avgN = 10;
//        oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_L, 10, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 100, 128));
//        dsc.push_back(QString("2V_refL 10s avg = ..\n"));
//        avgN = 0;
//        oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_L, 10, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 100, 128));
//        avgN = 0;
//        dsc.push_back(QString("2V_refL 1s avg=0..\n"));
//        oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_L, 1, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 100, 128));


//        dsc.push_back(QString("2V_GND..\n"));
//        oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_GND,   0.015, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 100, 128));


//        avgN = 8;

//        dsc.push_back(QString("-avg8-2V_refH..\n"));
//        oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_H, 10.1, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 1+99/(1<<avgN), 127/(1<<avgN)+1));
//        dsc.push_back(QString("-avg8-2V_refL..\n"));
//        oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_L, 10.1, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 1+99/(1<<avgN), 127/(1<<avgN)+1));
//        dsc.push_back(QString("2V_GND..\n"));
//        oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_GND,   0.015, 0, 0, 0, 100, 128));

//        dsc.push_back(QString("-avg8-G2V_GND..\n"));
//        oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_GND, 10.1, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 1+99/(1<<avgN), 127/(1<<avgN)+1));
//        avgN = 0;
//        dsc.push_back(QString("200mV_GND..\n"));
//        oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_GND,   0.01, avgN, 0/*32*/, 0/*-128*/, 100, 800));
//        dsc.push_back(QString("200mV_Sig..\n"));
//        oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_SIG,   0.01, avgN, 0, 0, 100, 128));
//        dsc.push_back(QString("200mV_refH..\n"));
//        oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_REF_H, 0.01, avgN, 0/*32*/, 0/*-128*/, 100, 800));
//        avgN = 8;
//        dsc.push_back(QString("-avg8-200mV_refH..\n"));
//        oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_REF_H, 10.1, avgN, /*0*/1+31/(1<<avgN), /*0*/-127/(1<<avgN)-1, 1+99/(1<<avgN), 127/(1<<avgN)+1));
//        dsc.push_back(QString("-avg8-200mV_GND..\n"));
//        oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_GND,   10.1, avgN, 0/*1+31/(1<<avgN)*/, 0/*-127/(1<<avgN)-1*/, 1+99/(1<<avgN), 127/(1<<avgN)+1));

        QString tempStr;
        for(auto i = 0; i<vsdcs.size(); ++i)
            tempStr += QString("0x%1_").arg(vsdcs[i].baseAddr >> 24, 0, 16);
        std::string dir = globTestPath.toStdString() + tempStr.toStdString() +  "osc_summery.txt";

        FILE *stream0 = fopen(dir.c_str(), "w");
        int cnt = 0;
        char trash;
        for(auto setti : oscSettList)
        {
            auto resStr = makeOscTest(setti, vsdcs, "REF_");
            Sleep(1500);
            fprintf(stream0, "%s", resStr.c_str());
            std::cout << dsc[cnt++].toStdString() << " ..done. Press anyu key to continue" << std::endl;
            std::cin >> trash;
        }
        fclose(stream0);
        std::cout << "congratulations!, oscRefGndTests done." << std::endl;
    }
    std::cout << "======= THE END OF REF + GND + FREE INPUTS OSC TESTS ===============" << std::endl;
}

void OscInputSinusTests(std::vector<VsDC3v2> &vsdcs)
{
    int range = 2000;
    unsigned short regRng;
    while(range != 0 )
    {
        VsDCChSettings conf = VsDCChSettings((ADC_GAIN2 | ADC_GND), 0, 0, 0);
        for(auto vsdc3Dev : vsdcs)
            for(auto chan=0; chan<vsdc3Dev.chanCount(); ++chan)
                vsdc3Dev.configureChannel(conf, chan);

        if(TESTED_HARD_WARE == TYPE_VSDC3V2)
        {
            std::cout << std::endl << "---------INPUT SINUS OSC TEST-------------" << std::endl;
           // std::cout << "If you want launch sinus signal osc test, set input resistance to 120 Ohm" << std::endl;
            std::cout << "Enter input range in mV" << std::endl << "2000 for 2V input range or 200 for 200mV input range " << std::endl;
            std::cout << "Press 0 to skip and do other tests " << std::endl << std::endl;
            std::cin >> range;
        }
        else if(TESTED_HARD_WARE == TYPE_VSDC4)
        {
            std::cout << "Enter G to continue or X to exit" << std::endl;
            char temp ;
            std::cin >>  temp;
            if((temp == 'x') || (temp == 'X'))
                range = 0;
        }
        CalibrateDevices(vsdcs);
        if(range == 200)
            regRng = ADC_GAIN02;
        else if (range == 2000)
            regRng = ADC_GAIN2;
        else
            continue;

        std::vector<VsDCOscSettings> oscSettList;
        uint32_t avgN = 0;

        QString tempStr;
        for(auto i = 0; i < vsdcs.size(); ++i)
            tempStr += QString("0x%1_").arg(vsdcs[i].baseAddr >> 24, 0, 16);

        std::string dir = globTestPath.toStdString() + tempStr.toStdString() +  "osc_Sinus_summery.txt";

        FILE *stream0 = fopen(dir.c_str(), "w");
        int cnt = 0;

 //       void OscStart(VsDCOscSettings oscSett, std::vector<VsDC3v2> &vsdcList, ADC_Signal_Src srcADC);
        auto resStr = makeOscTest(VsDCOscSettings(regRng | ADC_SIG,  500, avgN, 0, 0, 100, 128), vsdcs, "SIN_");
        Sleep(1500);
        std::cout << resStr;
        fprintf(stream0, "%s", resStr.c_str());
        fclose(stream0);
   }
   std::cout << "======= THE END OF INPUT SINUS OSC TESTS ===============" << std::endl;
}

char OscTests(std::vector<VsDC3v2> &vsdcs)
{
    char yOrN = 'r';
    do
    {
        std::cout << std::endl << "----------OSC TESTS-------------" << std::endl;
//        std::cout << "Do you want launch oscilloscope tests?" << std::endl;
        std::cout << "Press 'S' to make oscilloscope sinus input tests" << std::endl;
        std::cout << "Press 'R' to make oscilloscope reference & Gnd input tests" << std::endl << std::endl;
        std::cout << "Press 'T' to skip oscilloscope add do other tests" << std::endl << std::endl;
        std::cout << "Press 'X' to exit" << std::endl << std::endl;
        std::cin >> yOrN;
        Sleep(2000);
        std::cout<< 'R' << std::endl;
        if((yOrN == 'S') || (yOrN == 's') )
             OscInputSinusTests(vsdcs);
        else if ((yOrN == 'R') || (yOrN == 'r') )
            OscRefsTests(vsdcs);
    }
    while ((yOrN != 'X') && (yOrN != 'x') && (yOrN != 'T') && (yOrN != 't'));
    std::cout << "======= THE END OF OSC TESTS ===============" << std::endl;
    return yOrN;
}

//

void FPSyncStartStopTest(VsDC3v2 device, int sync)
{
    char cxx, syncName = 'A' + sync - 1;
    unsigned long tmp;
    std::cout << "Connect pulse generator to sync " << syncName << " input dev: " << std::hex << (device.baseAddr >> 24) << " and press any key to continue" << std::endl;
    std:: cout << "OR PRESS 'x' TO EXIT " << std::endl;
    std::cin >> cxx;
    VsDCChSettings conf = {VsDCChSettings(ADC_GAIN2 | ADC_REF_H | sync | ADC_STOP_PSTOP, 1000000, 0, 0)};
    if((cxx != 'X') && (cxx != 'x'))
    {
        for(unsigned int chan = 0; chan < device.chanCount(); ++chan)
        {
            vsdc3Dev.configureChannel(conf, chan);
            do
                tmp =  device.getState(chan) & STATE_MASK;
            while (tmp  != PSTART);
            std::cout << std::hex << (device.baseAddr >> 24) << ".ch:" << chan << "sync " << syncName<<    " START done. dev.State = " << std::hex << tmp <<  std::endl;
            device.pstopAll();
            do
                tmp =  device.getState(chan) & STATE_MASK;
            while (tmp  != PSTOP);
            std::cout << std::hex << (device.baseAddr >> 24) << ".ch:" << chan << "the process is stopped by the program. dev.State = " << std::hex << tmp  << std::endl;
        }
    }
    std::cout << std::dec << "---------------" << std::endl;
}

void ProgStartStopTest(VsDC3v2 device)
{
    unsigned long tmp;
    VsDCChSettings conf = {VsDCChSettings(ADC_GAIN2 | ADC_REF_H | ADC_START_PSTART | ADC_STOP_PSTOP, 1000000, 0, 0)};
    for(unsigned int chan=0; chan<device.chanCount(); ++chan)
    {
        device.configureChannel(conf, chan);
        device.pstartAll();
        std::cout << std::hex << (device.baseAddr >> 24) << ":" << chan << " Programm START.." << std::endl;
        do
        {
            tmp =  device.getState(chan) & STATE_MASK;
        } while (tmp  != PSTART);
        std::cout << "--done-- "  <<  (device.baseAddr >> 24) << ":" << chan << " state" << tmp <<  std::endl;
        device.pstopAll();
        std::cout <<  (device.baseAddr >> 24) << ":" << chan << " Programm STOP.." << std::endl;
        do
        {
            tmp =  device.getState(chan) & STATE_MASK;
        } while (tmp  != PSTOP);
        std::cout << "--done-- " << (device.baseAddr >> 24) << ":" << chan << " state"  << tmp <<  std::dec << std::endl << std::endl;
    }
}


void BPStartStopTest(VsDC3v2 device, int ttlLine)
{

    VsDCChSettings conf = {VsDCChSettings(ADC_GAIN2 | ADC_REF_H | ADC_START_BP | ADC_STOP_BP, 1000000, 0, 0)};
    for(unsigned int chan=0; chan<device.chanCount(); ++chan)
    {
        device.configureChannel(conf, chan);
        //vsdc3Dev.setBpTTLMux(ttlLine, ttlLine, 0);
        device.setBpStartMux(ttlLine, chan);
        unsigned long tmp;
        std::cout << std::hex << (device.baseAddr >> 24) << ":" << chan << " BackPanel START.." << std::endl;
        do
        {
            tmp =  device.getState(chan);
            std:: cout << "state = " << std::hex << tmp << ", ";
        } while ((tmp & STATE_MASK)  != PSTART);
        std::cout << "--done-- "  <<  (device.baseAddr >> 24) << ":" << chan << " state" << tmp <<  std::endl;

        device.setBpStopMux(ttlLine, chan);
        std::cout <<  (device.baseAddr >> 24) << ":" << chan << "  BackPanel STOP.." << std::endl;
        do
        {
            tmp =  device.getState(chan) & STATE_MASK;
        } while (tmp  != PSTOP);
        std::cout << "--done-- " << (device.baseAddr >> 24) << ":" << chan << " state"  << tmp <<  std::dec << std::endl << std::endl;
    }
}


void StartStopTest (std::vector<VsDC3v2> &vsdcList)
{
//    std::cout << std::endl << "---------START STOP TESTS-------------" << std::endl;
//    std::cout << "Do you want launch Start Stop tests?" << std::endl;
//    std::cout << "Press 'Y' to make Start Stop tests" << std::endl;
//    std::cout << "Press any key to continue and do other tests" << std::endl;
//    std::cin >> yOrN;
   for(auto vsdc3Dev : vsdcList)
    {
      for (int sInp = ADC_START_SYNCA; sInp <= ADC_START_SYNCD; ++sInp )
          FPSyncStartStopTest (vsdc3Dev, sInp);
      ProgStartStopTest (vsdc3Dev);
//          BPStartStopTest(vsdc3Dev, inCreatedevNum);
   }
    std::cout << "========== THE END OF START STOP TESTS==============" << std::endl << std::endl;
}


enum WhatDoYouWont
{
  NEXT_CHANEL,
  NEXT_DEVICE,
  NEXT_TEST,
  FINISH
};



void DotsToComma(QString fName)
{
    std::ifstream fSrc;
    std::vector <char> buff;

    fSrc.open(fName.toStdString());
    char symb;
    char outSmb;
    while(fSrc.get(symb))
    {
        if(symb == 9)
        {
            std::cout << "\t";
            buff.push_back('\t');
        }
        else if(symb == 13)
        {
            fSrc.get(symb);
            std::cout << "\n";
            buff.push_back('\n');
        }
        else
        {
            outSmb = (symb == '.')? ',' : symb;
            std::cout << outSmb;
            buff.push_back(outSmb);
        }
    }
    fSrc.close();
    std::ofstream fDst;
    fDst.open(fName.toStdString());
    for (int i = 0; i < buff.size(); ++i)
        fDst << buff[i];
    fDst.close();
}


void LinTester(std::vector<VsDC3v2> &vsdcList, int devId, int chNum, int dacId, unsigned long range)
{
    std::cout << "LinTester===" << range << "++++====----========" << std::endl;
    auto dev = vsdcList[devId];
    auto dac = vsdcList[dacId];
    int devBase = dev.baseAddr >> 24;
    int dacBase = dac.baseAddr >> 24;

    float dac_volt = 0.0;
    const  int AVGN = 6;
    auto msmt_time = 0.5;//seconds
    QVector<double> chVoltAvg;
    QVector<double> chVoltInt;
    QVector<double> hp3458AVolt;
    QVector<double> dacSetVolt;
    QString linDir = devDirs[devId] + QString("NoLin_x%1").arg(devBase, 0, 16);
    mkdir(linDir.toStdString().c_str());
    QString outFName = linDir + QString("//dataLin0x%1_ch%2_").arg(devBase, 0, 16).arg( chNum );//devDirs[devId] + QString( "dataLin0x%1_ch%2_").arg(devBase, 0, 16).arg( chNum );
    outFName += (range == ADC_GAIN02)? QString("_200mV.txt") : QString("_2V.txt");
    QString DACFName = linDir + QString("//dac0x%1").arg(dacBase, 0, 16); //devDirs[dacId] + QString( "dac0x%1").arg(dacBase, 0, 16);
    DACFName += (range == ADC_GAIN02)? QString("_200mV.txt") : QString("_2V.txt");
    std::ofstream dst;
    std::ofstream DACdst;
    dst.open(outFName.toStdString());//tempFName.toStdString());
    DACdst.open(DACFName.toStdString());//DACtempFName.toStdString() );

    dst << "\t\t\t LinTest\n";
    dst << "DAC:\t" << std::hex << dacBase <<"\n";
    dst << "dev:\t" << devBase << "ch\t" << chNum << "\t\n";
    dst << "rng:\t" << ((range == ADC_GAIN02)? "200mV\n" : "2V\n\n");
    dst << "Index\tDAC\tHP\t";
    dst << "CH[" << chNum << "]Avg\tCH[" << chNum << "]Int\t\n" ;
    Sleep(50);
    DACdst << "\t\t\t LinTestDac\n";
    DACdst << "DAC:\t" << std::hex << dacBase <<"\n";
    DACdst << "rng:\t" << ((range == ADC_GAIN02)? "200mV\n" : "2V\n\n");
    DACdst << "Index\tDAC\tHP\t";

    std::cout << " files created... " << std::endl;
    VsDCChSettings conf = VsDCChSettings((range | ADC_SIG), (int)(msmt_time*1000000), AVGN, 0);
    dev.configureChannel(conf, chNum);

    Sleep(2000);
    dev.calibAll();
    Sleep(2000);
    bool first_step = true;
    float val;

    const double DAC_MAX = (range == ADC_GAIN02)? 0.202 : 2.02;
    const double DAC_MIN = (range == ADC_GAIN02)? -0.2 : -2.0;
    const double DAC_STEP = (range == ADC_GAIN02)? 0.01 : 0.1;
    for(dac_volt = DAC_MIN; dac_volt < DAC_MAX; dac_volt += DAC_STEP)
    {
        dac.setDacValue(dac_volt);
        auto fvolt = dac.getDacValue();
        dacSetVolt.append(fvolt);
        if(first_step)
        {
            first_step = false;
            Sleep(2000);
        }
        Sleep(2000);
        dev.pstartAll();
        Sleep((int)(msmt_time*1000)+100);
        hp3458AVolt.append(Get34465Voltage());
        chVoltInt.append(dev.readIntegral(chNum)/msmt_time);
        std::cout << std::endl << "HP: " << hp3458AVolt.last() << "\tVolt["<<chNum<<"]: " << chVoltInt.last() << "\t";
            //READ MEMORY
        auto osc_data = dev.getOscilloscope(chNum);
        auto mean0 = VsDC3v2::avg(osc_data, 500, 200);
        auto std0 = VsDC3v2::std(mean0, osc_data, 500, 200);
        chVoltAvg.append(mean0);
        std::cout << "ADC[" << chNum << "]\tavg:\t" << mean0 << "V;\trms:\t" << std0 << "V;" << std::endl;
        chVoltAvg.append(chVoltInt.last());
        std::cout << std::endl;
        //SAVE DATA
        dst << dacSetVolt.size() <<  "\t" << std::setw(8) << std::showpos <<  dacSetVolt.last() <<  "\t" << hp3458AVolt.last() << "\t";
        dst << std::setw(8) << chVoltAvg.last() << "\t" << std::setw(8) <<  chVoltInt.last() << "\t\n";
        Sleep(50);
        DACdst << dacSetVolt.size() <<  "\t" << std::setw(8) << std::showpos <<  dacSetVolt.last() <<  "\t" << hp3458AVolt.last() << "\t\n";
     }
    dst.close();
    DACdst.close();
    DotsToComma(outFName);
    DotsToComma(DACFName);
}


int  LinChanTests(std::vector<VsDC3v2> &vsdcList, int devId, int chNum, int dacId)
{
    auto dev = vsdcList[devId];
    auto dac = vsdcList[dacId];
    int devBase = dev.baseAddr >> 24;
    int dacBase = dac.baseAddr >> 24;
    char yOrN;
    std::cout << std::endl << "---------INL TESTS-------------" << std::endl;

    std::cout << "Connect DAC outpun of the device " << dacBase << " to KEYSIGHT 344656A voltmeter and to chanel " << chNum << " of the device  " << devBase << std::endl;
    std::cout << " and press G" << std::endl << std::endl;
    if (chNum == 0)
        std::cout << "To skip and go to the next channel press C" << std::endl;
    std::cout << "To skip and go to the next device press D" << std::endl;
    std::cout << "To skip and go to the next test press T" << std::endl;
    std::cout << "To exit the program, press X" << std::endl;
    std::cin >> yOrN;
    int ret = ((yOrN == 'D') || (yOrN == 'd') )? NEXT_DEVICE :((yOrN == 'T') || (yOrN == 't') )? NEXT_TEST : ((yOrN == 'X') || (yOrN == 'x') ) ? FINISH : NEXT_CHANEL;
    if ((yOrN == 'G') || (yOrN == 'g') )
    {
        if(TESTED_HARD_WARE == TYPE_VSDC3V2)
        {
           std::cout << "200mV Lin test Dev " << std::hex << devId << " ch " << chNum << " dac " << dacId << "in process"  << std::endl;
           LinTester(vsdcList, devId, chNum, dacId, ADC_GAIN02);
        }
        std::cout << "2V Lin test Dev " << std::hex << devId << " ch " << chNum << " dac " << dacId << "in process"  << std::endl;
        LinTester(vsdcList, devId, chNum, dacId, ADC_GAIN2);
    }
    return ret;
}

int LinearityTests(std::vector<VsDC3v2> &vsdcList)
{
    int ret;
    for(auto devId = 0; (devId < vsdcList.size()) && (ret < NEXT_TEST);  ++devId)
    {
        ret = NEXT_CHANEL;
        std::cout << "id "  << devId << "sz " << vsdcList.size() << std::endl;
        for(int chN = 0; chN < vsdcList[devId].chanCount() && (ret == NEXT_CHANEL) ; ++chN )
            ret = LinChanTests(vsdcList, devId, chN, (devId == (vsdcList.size() - 1))?  0 :( devId + 1 ) );
    }
    std::cout << "the End ofLin:"  << ret << std::endl;
    return (ret < NEXT_TEST)?  NEXT_TEST : ret;
}

bool WaitAnyKey()
{
    std::cout << "\n Press any key to continue \n";
    char temp;
    std::cin >> temp;
}

bool CompareAndPrint(unsigned long x, unsigned long y)
{
    if(x == y)
         std::cout <<  "write: " << std::hex << x << "== read: " << std::hex << y << "  -OK-";
    else
    {
        std::cout << std::endl << std::endl << "----------ACHTUNG BUS DATA ERROR!--------" << std::endl;
        std::cout << "write: " << std::hex << x << " read: " << std::hex << y << " OOPS.. We've got a problem" << std::endl;
        WaitAnyKey();
    }
    std::cout << std::dec << std::endl;
}

void BusTest(std::vector<VsDC3v2> &vsdcList)
{
    for(auto vsdc3Dev : vsdcList)
     {
       unsigned long rData;
       int bitNum = 0;
       for (unsigned long wData = 1; wData > 0; wData <<= 1)
       {
           std::cout << "\n bit: " << bitNum++ << "  : ";
           vsdc3Dev.setTimerReg(wData, 0);
           Sleep(2);
           rData = vsdc3Dev.getTimerReg(0);
           CompareAndPrint(wData, rData);
           vsdc3Dev.setTimerReg(~wData, 0);
           Sleep(2);
           rData = vsdc3Dev.getTimerReg(0);
           CompareAndPrint(~wData, rData);
       }
    }
}



char PrintMenu()
{
    std::cout << "Enter X to exit or select test:" << std::endl;
    std::cout << "B - VME bus test" << std::endl;
    std::cout << " D - calibrate DAC" << std::endl;
    std::cout << "   P - print device info" << std::endl;
    std::cout << "     O - oscilloscope test" << std::endl;
    std::cout << "       S - StartStop test" << std::endl;
    std::cout << "     V - Volt*Sec test" << std::endl;
    std::cout << "   C - Calibrate devices" << std::endl;
    std::cout << " N - Noise test" << std::endl;
    std::cout << "L - Non Linearity test" << std::endl;

    char ret;
    std::cin >> ret;
    return ret;
}


int main(int argc, char *argv[])
{

    QString dev_ip      = "192.168.1.6";
    quint16 dev_port    = 6000;
    float time_q;
    int devInd = 0;
    quint32 base_addr   = devList[devInd].base_addr;
    std::vector<VsDC3v2> vsdcList;
    bool res = connectToServer(mServer, dev_ip, dev_port);
    if (!res) exit(1);
    std::cout << "connect!" << std::endl;
    try
    {
        //Initialize list
    //    for(auto const& vsdc : vsdcList)
        mkdir(globTestPath.toStdString().c_str());
        QString tempStr;
        for(auto ind=0ul; ind<devList.size(); ++ind)
        {
            vsdcList.push_back(VsDCFactory(devList[ind], ind));
            std::cout << std::hex << (devList[ind].base_addr >> 24) << std::endl;

            vsdcList[ind].assignBaseAddr(devList[ind].base_addr, devList[ind].chanNum, ind);

            tempStr = globTestPath + QString("0x%1_").arg(vsdcList[ind].baseAddr >> 24, 0, 16);
            mkdir(tempStr.toStdString().c_str());
            tempStr += "\\";
            devDirs.push_back(tempStr);
            std::cout << tempStr.toStdString() << std::endl;
        }
 //       CalibrateDevices(vsdcList);
        char whatDoWeDo = 0;
        while((whatDoWeDo != 'x') && (whatDoWeDo != 'X') )
        {
            whatDoWeDo = PrintMenu();
            std::cout << "recieve: " << whatDoWeDo << std::endl;
            switch (whatDoWeDo)
            {

                case 'B' :  BusTest(vsdcList);
                   break;
                case 'b' :  BusTest(vsdcList);
                   break;
                case 'C' :  CalibrateDevices(vsdcList);;
                    break;
                case 'c' :  CalibrateDevices(vsdcList);
                    break;
                case 'D' :  whatDoWeDo = WriteCalibrationConst(vsdcList, TESTED_HARD_WARE);
                    break;
                case 'd' :  whatDoWeDo = WriteCalibrationConst(vsdcList, TESTED_HARD_WARE);
                      break;
                case 'P' :  PrintDeviceInfo(vsdcList);
                    break;
                case 'O' :  whatDoWeDo = OscTests(vsdcList);
                    break;
                case 'o' :  whatDoWeDo = OscTests(vsdcList);
                    break;
                case 'S' :  StartStopTest(vsdcList);
                    break;
                case 's' :  StartStopTest(vsdcList);
                    break;
                case 'V' :  whatDoWeDo = VoltXSecondTest(vsdcList);
                   break;
                case 'v' :  whatDoWeDo = VoltXSecondTest(vsdcList);
                   break;
                case 'N' :  whatDoWeDo = NoiseTest(vsdcList);
                  break;
                case 'L' :  whatDoWeDo = LinearityTests( vsdcList );
                  break;
               case 'n' :   whatDoWeDo = NoiseTest(vsdcList);
                  break;
               case 'l' :   whatDoWeDo = LinearityTests( vsdcList );
                  break;
                default: break;
            }
        }



//                    WriteCalibrationConst(vsdcList, TESTED_HARD_WARE);
//                    CalibrateDevices(vsdcList);
//             //       char tmp = '0';
//             //       while(tmp != 'x')
//             //       {
//                    PrintDeviceInfo(vsdcList);
//            //            std::cout << "press any key, x to exit" << std::endl;
//             //           std::cin >> tmp;
//             //       }




//                    OscTests(vsdcList);


//                    StartStopTest(vsdcList);
//                    VoltXSecondTest(vsdcList);

//                    std::cout << std::endl << "====" << LinearityTests( vsdcList ) << std::endl;



//                    NoiseTest(vsdcList);

        throw std::string("Calib Voltages");

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

  //////////////////////////

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
    str += "error_of_integral.py ";
    str += dirname;
    std::cout << str.toStdString() << std::endl;
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





////////////////////////////////////////////////////////////

//WRITE CALIB CONSTS

//                            if(DO_WRITE_CALIB)
//                            {
//                                if(1)
//                                {
//                                    int cxx;
//                                    for(auto vsdc3Dev : vsdcList)
//                                    {
//                                        vsdc3Dev.setDacValue(0.168);
//                                    }
//                                     std::cout << "dac = 0.168 press any key5" << std::endl;
//                                    std::cin >> cxx;
//                                    for(auto vsdc3Dev : vsdcList)
//                                    {
//                                        vsdc3Dev.setDacValue(1.8);
//                                    }
//                                    std::cout << "dac = 1.8 press any key6" << std::endl;
//                                    std::cin >> cxx;
//                                }
//                                for(auto ind=0ul; ind < devList.size(); ++ind)
//                                {
//                                    vsdcList[ind].uploadRefVoltage(devList[ind].refVoltages);
//                                }
//                                throw std::string("Calib Voltages");
//                            }


//CALIB DEVICE
//if(1) {
//    for(auto vsdc3Dev : vsdcList) {
//        vsdc3Dev.calibAll();
//    }
//    Sleep(1000);
//}

//        //GET DEV INFO
//        if(1)
//        {
//    //        for(int devInd=0; devInd<devList.size(); ++devInd)
//            for(auto vsdc3Dev : vsdcList)
//            {
//                std::cout << vsdc3Dev.printDevInfo();
//            }

//        }


//        Range 2V
        //Make Noise Of Int 2V
//            if(0)
//            {
//                VsDCChSettings conf = VsDCChSettings((ADC_GAIN2 | ADC_GND), 0, 0, 0);
//                for(auto vsdc3Dev : vsdcList)
//                {
//                    for(int chan=0; chan<vsdc3Dev.chanCount(); ++chan)
//                    {
//                        vsdc3Dev.configureChannel(conf, chan);
//                    }
//                }
//                VsdcIntNoiseTest noiseTest;
//                VsDCTestConfig test_conf;
//                test_conf.configureMsmts(40, 0, 6, 3);//(100, 0, 7, 4);//(80, 0, 6, 4);//(30, 0, 7, 3);
//                if(1)
//                {
//                    for(auto vsdc3Dev : vsdcList)
//                        vsdc3Dev.calibAll();
//                    Sleep(1500);
//                }
//                noiseTest.doTest(test_conf, vsdcList);
//                std::string filName = global_file_path.toStdString();
//                noiseTest.saveDataToFile(filName);
//                noiseTest.saveRawDataToFile(filName);
//                Sleep(1500);
//                Plot_NI(QString::fromUtf8(filName.c_str()));
//            }


//        Range 0.2V
        //Make Noise Of Int 0.2V
//        if(0)
//        {
//            VsDCChSettings conf = VsDCChSettings((ADC_GAIN02 | ADC_GND), 0, 0, 0);
//            for(auto vsdc3Dev : vsdcList)
//            {
//                for(int chan=0; chan<vsdc3Dev.chanCount(); ++chan)
//                {
//                    vsdc3Dev.configureChannel(conf, chan);
//                }
//            }
//            VsdcIntNoiseTest noiseTest;
//            VsDCTestConfig test_conf;
//            test_conf.configureMsmts(40, 0, 6, 3);//(100, 0, 7, 4);//(80, 0, 6, 4);//(30, 0, 7, 3);
//            if(1)
//            {
//                for(auto vsdc3Dev : vsdcList)
//                    vsdc3Dev.calibAll();
//                Sleep(1500);
//            }
//            noiseTest.doTest(test_conf, vsdcList);
//            std::string filName = global_file_path.toStdString();
//            noiseTest.saveDataToFile(filName);
//            noiseTest.saveRawDataToFile(filName);
//            Sleep(1500);
//            Plot_NI(QString::fromUtf8(filName.c_str()));
//        }



//Test Front Panel START/STOP
//if(0)
//{
//    int cxx;
//    int chan;
//    VsDCChSettings conf[] = {VsDCChSettings(ADC_GAIN2 | ADC_REF_H | ADC_START_SYNCA | ADC_STOP_TIMER, 10000, 0, 0)};

//    for(auto vsdc3Dev : vsdcList)
//    {
//        conf[0] = {VsDCChSettings(ADC_GAIN2 | ADC_REF_H | ADC_START_SYNCA | ADC_STOP_TIMER, 10000, 0, 0)};
//        for(chan=0; chan<vsdc3Dev.chanCount(); ++chan)
//        {
//            vsdc3Dev.configureChannel(conf[0], chan);
//        }
//        std::cout << std::hex << vsdc3Dev.baseAddr << std::dec <<  "\tSYNC_A\n";
//        std::cout << "press any key0" << std::endl;
//        std::cin >> cxx;

//        conf[0] = {VsDCChSettings(ADC_GAIN2 | ADC_REF_H | ADC_START_SYNCB | ADC_STOP_TIMER, 10000, 0, 0)};
//        for(chan=0; chan<vsdc3Dev.chanCount(); ++chan)
//        {
//            vsdc3Dev.configureChannel(conf[0], chan);
//        }
//        std::cout << std::hex << vsdc3Dev.baseAddr << std::dec <<  "\tSYNC_B\n";
//         std::cout << "press any key1" << std::endl;
//        std::cin >> cxx;

//        conf[0] = {VsDCChSettings(ADC_GAIN2 | ADC_REF_H | ADC_START_SYNCC | ADC_STOP_TIMER, 10000, 0, 0)};
//        for(chan=0; chan<vsdc3Dev.chanCount(); ++chan)
//        {
//            vsdc3Dev.configureChannel(conf[0], chan);
//        }
//        std::cout << std::hex << vsdc3Dev.baseAddr << std::dec <<  "\tSYNC_C\n";
//         std::cout << "press any key2" << std::endl;
//        std::cin >> cxx;

//        conf[0] = {VsDCChSettings(ADC_GAIN2 | ADC_REF_H | ADC_START_SYNCD | ADC_STOP_TIMER, 10000, 0, 0)};
//        for(chan=0; chan<vsdc3Dev.chanCount(); ++chan)
//        {
//            vsdc3Dev.configureChannel(conf[0], chan);
//        }
//        std::cout << std::hex << vsdc3Dev.baseAddr << std::dec <<  "\tSYNC_D\n";
//         std::cout << "press any key3" << std::endl;
//        std::cin >> cxx;

//        conf[0] = {VsDCChSettings(ADC_GAIN2 | ADC_REF_H | ADC_START_PSTART | ADC_STOP_TIMER, 10000, 0, 0)};
//        for(chan=0; chan<vsdc3Dev.chanCount(); ++chan)
//        {
//            vsdc3Dev.configureChannel(conf[0], chan);
//        }

//    }
//}
//Volt*Second Linearity Error Test
//2V
//if(0)
//{
//    VsDCChSettings conf = VsDCChSettings((ADC_GAIN2 | ADC_REF_H), 0, 0, 0);
//    for(auto vsdc3Dev : vsdcList)
//    {
//        for(int chan=0; chan<vsdc3Dev.chanCount(); ++chan)
//        {
//            vsdc3Dev.configureChannel(conf, chan);
//        }
//    }

//    VsdcIntErrorTest errorTest;
//    VsDCTestConfig test_conf;
//    test_conf.configureMsmts(40, 1, 7, 3);//(30, 1, 8, 3);
//    if(1)
//    {
//        for(auto vsdc3Dev : vsdcList)
//            vsdc3Dev.calibAll();
//        Sleep(1500);
//    }
//    errorTest.doTest(test_conf, vsdcList);
//    std::string filName = global_file_path.toStdString();
//    errorTest.saveDataToFile(filName);
//    errorTest.saveRawDataToFile(filName);
//    Sleep(1500);
//    Plot_TLT(QString::fromUtf8(filName.c_str()));
//}

////Volt*Second Linearity Error Test
////0.2V
//if(0)
//{
//    VsDCChSettings conf = VsDCChSettings((ADC_GAIN02 | ADC_REF_H), 0, 0, 0);
//    for(auto vsdc3Dev : vsdcList)
//    {
//        for(int chan=0; chan<vsdc3Dev.chanCount(); ++chan)
//        {
//            vsdc3Dev.configureChannel(conf, chan);
//        }
//    }

//    VsdcIntErrorTest errorTest;
//    VsDCTestConfig test_conf;
//    test_conf.configureMsmts(40, 1, 7, 3);//(30, 1, 8, 3);
//    if(1)
//    {
//        for(auto vsdc3Dev : vsdcList)
//            vsdc3Dev.calibAll();
//        Sleep(1500);
//    }
//    errorTest.doTest(test_conf, vsdcList);
//    std::string filName = global_file_path.toStdString();
//    errorTest.saveDataToFile(filName);
//    errorTest.saveRawDataToFile(filName);
//    Sleep(1500);
//    Plot_TLT(QString::fromUtf8(filName.c_str()));
//}

//if(1)
//{
//    std::vector<VsDCOscSettings> oscSettList;
////            quint32 sett    = ADC_GAIN2
////                            | ADC_REF_H
////    //                        | ADC_START_BP
////    //                    | ADC_STOP_BP
////    //                        | ADC_IRQ_EN
////    //                        | ADC_SINGLE
////                    | ADC_SET_AUTOZERO
////                    ;
////            float timerSec = 0.001;
//    uint32_t avgN = 0;
////            uint32_t    plot_start=32;
////            int32_t     plot_stop=-128;
////            uint32_t    stat_srart=100, stat_stop=128;
//    if(1)
//    {
////(uint32_t _sett, float _timerSec, uint32_t _avgN, uint32_t _plot_start=32, int32_t _plot_stop=-128, uint32_t _stat_srart=100, uint32_t _stat_stop=128)

//       oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_GND,   0.01, 0, 0, 0, 100, 128));
//       oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_SIG,   0.01, 0, 0, 0, 100, 128));
//       oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_H, 0.01, 0, 0, 0, 100, 128));


////                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_L, 0.01, 0, 0, 0, 100, 128));
////                  avgN=0;  oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_GND,   0.01, 0, 100, -128, 100, 128));
////                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_SIG,   0.01, 0, 100, -128, 100, 128));
////                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_H, 0.01, 0, 100, -128, 100, 128));
////                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_L, 0.01, 0, 100, -128, 100, 128));
////                avgN = 8;  oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_GND,   1.1, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 1+99/(1<<avgN), 127/(1<<avgN)+1));
////                avgN = 8;  oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_SIG,   10.1, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 1+99/(1<<avgN), 127/(1<<avgN)+1));

//        avgN = 8;
//        oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_H, 10.1, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 1+99/(1<<avgN), 127/(1<<avgN)+1));
//        avgN = 8;
//        oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_GND, 10.1, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 1+99/(1<<avgN), 127/(1<<avgN)+1));
//    }
//    if(1)
//    {
//       oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_GND,   0.01, 0, 0/*32*/, 0/*-128*/, 100, 800));
//       oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_SIG,   0.01, 0, 0, 0, 100, 128));
//       oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_REF_H, 0.01, 0, 0/*32*/, 0/*-128*/, 100, 800));


////                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_GND,   0.01, 0, 0, 0, 100, 128));
////                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_SIG,   0.01, 0, /*0*/32, /*0*/-128, 100, 128));
////                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_GND,   0.01, 0, 0, 0, 100, 128));
////                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_REF_L, 0.01, 0, 0, 0, 100, 128));
////                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_GND,   0.01, 0, 32, -128, 100, 128));
////                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_REF_H,   0.1, 0, 32, -128, 100, 128));
////                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_GND,   0.01, 0, 32, -128, 100, 128));
////                avgN = 8;  oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_SIG,    1.1, avgN, 0/*1+31/(1<<avgN)*/, 0/*-127/(1<<avgN)-1*/, 1+99/(1<<avgN), 127/(1<<avgN)+1));

//        avgN = 8;  oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_REF_H, 10.1, avgN, /*0*/1+31/(1<<avgN), /*0*/-127/(1<<avgN)-1, 1+99/(1<<avgN), 127/(1<<avgN)+1));
//        avgN = 8;  oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_GND,   10.1, avgN, 0/*1+31/(1<<avgN)*/, 0/*-127/(1<<avgN)-1*/, 1+99/(1<<avgN), 127/(1<<avgN)+1));
//    }
////            avgN = 10;  oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_GND,   2.1, avgN, 0/*1+31/(1<<avgN)*/, 0/*-127/(1<<avgN)-1*/, 1+99/(1<<avgN), 127/(1<<avgN)+1));

//    std::string dir = global_file_path.toStdString() + "osc_summery.txt";
//    FILE *stream0 = fopen(dir.c_str(), "w");

//    for(auto setti : oscSettList)
//    {
////                auto resStr = makeOscStarts(setti, vsdcList);
//        auto resStr = makeOscTest(setti, vsdcList);

//        Sleep(1500);
//        std::cout << resStr;
//        fprintf(stream0, "%s", resStr.c_str());
//    }

//            {
//                auto resStr = makeOscRead(oscSettList[0], vsdcList);
//                std::cout << resStr;
//            }

//    fclose(stream0);
//}

//                //Make Oscilloscope Testing
//                //if(1)
//                //{
//                    std::vector<VsDCOscSettings> oscSettList;
//                //            quint32 sett    = ADC_GAIN2
//                //                            | ADC_REF_H
//                //    //                        | ADC_START_BP
//                //    //                    | ADC_STOP_BP
//                //    //                        | ADC_IRQ_EN
//                //    //                        | ADC_SINGLE
//                //                    | ADC_SET_AUTOZERO
//                //                    ;
//                //            float timerSec = 0.001;
//                    uint32_t avgN = 0;
//                //            uint32_t    plot_start=32;
//                //            int32_t     plot_stop=-128;
//                //            uint32_t    stat_srart=100, stat_stop=128;
//                //            if(0)
//                //            {
//                //        //(uint32_t _sett, float _timerSec, uint32_t _avgN, uint32_t _plot_start=32, int32_t _plot_stop=-128, uint32_t _stat_srart=100, uint32_t _stat_stop=128)

//                //                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_GND,   0.01, 0, 0, 0, 100, 128));
//                //                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_SIG,   0.01, 0, 0, 0, 100, 128));
//                //                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_H, 0.01, 0, 0, 0, 100, 128));


//                //        //                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_L, 0.01, 0, 0, 0, 100, 128));
//                //        //                  avgN=0;  oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_GND,   0.01, 0, 100, -128, 100, 128));
//                //        //                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_SIG,   0.01, 0, 100, -128, 100, 128));
//                //        //                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_H, 0.01, 0, 100, -128, 100, 128));
//                //        //                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_L, 0.01, 0, 100, -128, 100, 128));
//                //        //                avgN = 8;  oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_GND,   1.1, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 1+99/(1<<avgN), 127/(1<<avgN)+1));
//                //        //                avgN = 8;  oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_SIG,   10.1, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 1+99/(1<<avgN), 127/(1<<avgN)+1));

//                //                avgN = 8;
//                //                oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_REF_H, 10.1, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 1+99/(1<<avgN), 127/(1<<avgN)+1));
//                //                avgN = 8;
//                //                oscSettList.push_back(VsDCOscSettings(ADC_GAIN2 | ADC_GND, 10.1, avgN, 1+31/(1<<avgN), -127/(1<<avgN)-1, 1+99/(1<<avgN), 127/(1<<avgN)+1));

//                //            }
//                //            if(0)
//                //            {
//                //                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_GND,   0.01, 0, 0/*32*/, 0/*-128*/, 100, 800));
//                //                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_SIG,   0.01, 0, 0, 0, 100, 128));
//                //                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_REF_H, 0.01, 0, 0/*32*/, 0/*-128*/, 100, 800));


//                //        //                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_GND,   0.01, 0, 0, 0, 100, 128));
//                //        //                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_SIG,   0.01, 0, /*0*/32, /*0*/-128, 100, 128));
//                //        //                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_GND,   0.01, 0, 0, 0, 100, 128));
//                //        //                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_REF_L, 0.01, 0, 0, 0, 100, 128));
//                //        //                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_GND,   0.01, 0, 32, -128, 100, 128));
//                //        //                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_REF_H,   0.1, 0, 32, -128, 100, 128));
//                //        //                           oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_GND,   0.01, 0, 32, -128, 100, 128));
//                //        //                avgN = 8;  oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_SIG,    1.1, avgN, 0/*1+31/(1<<avgN)*/, 0/*-127/(1<<avgN)-1*/, 1+99/(1<<avgN), 127/(1<<avgN)+1));

//                //                avgN = 8;  oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_REF_H, 10.1, avgN, /*0*/1+31/(1<<avgN), /*0*/-127/(1<<avgN)-1, 1+99/(1<<avgN), 127/(1<<avgN)+1));
//                //                avgN = 8;  oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_GND,   10.1, avgN, 0/*1+31/(1<<avgN)*/, 0/*-127/(1<<avgN)-1*/, 1+99/(1<<avgN), 127/(1<<avgN)+1));
//                //            }
//                //        //            avgN = 10;  oscSettList.push_back(VsDCOscSettings(ADC_GAIN02 | ADC_GND,   2.1, avgN, 0/*1+31/(1<<avgN)*/, 0/*-127/(1<<avgN)-1*/, 1+99/(1<<avgN), 127/(1<<avgN)+1));

//                //            std::string dir = global_file_path.toStdString() + "osc_summery.txt";
//                //            FILE *stream0 = fopen(dir.c_str(), "w");

//                //            for(auto setti : oscSettList)
//                //            {
//                //        //                auto resStr = makeOscStarts(setti, vsdcList);
//                //                auto resStr = makeOscTest(setti, vsdcList);

//                //                Sleep(1500);
//                //                std::cout << resStr;
//                //                fprintf(stream0, "%s", resStr.c_str());
//                //            }

//                //        //            {
//                //        //                auto resStr = makeOscRead(oscSettList[0], vsdcList);
//                //        //                std::cout << resStr;
//                //        //            }

//                //            fclose(stream0);
//                //        }

//            void OscRead(VsDCOscSettings oscSett, VsDC3v2 vsdc, int deviceIndex)
//            {
//                std::stringstream cout;
//            //    QString dir_path = globTestPath;//global_file_path;// "D:\\DC\\!W\\Prj\\VSDC3-1\\python\\;"//"g:\\work\\tmpdat\\VsDC3_chopp\\";
//                QString filename;
//                QString dirName;
//                bool do_SAVE = true;

//                dirName = devDirs[deviceIndex];
//                cout << std::hex << "0x" << vsdc3Dev.baseAddr << std::dec << " AvgN(" << oscSett.avgN << ")\t" << vsdc3Dev.muxName(oscSett.sett) << " ";
//                for(auto chan=0; chan<vsdc3Dev.chanCount(); ++chan)
//                {
//                    auto osc_data = vsdc3Dev.getOscilloscope(chan);
//                    //print some info
//                    auto mean = VsDC3v2::avg(osc_data, oscSett.stat_srart, oscSett.stat_stop);
//                    auto std = VsDC3v2::std(mean, osc_data, oscSett.stat_srart, oscSett.stat_stop);
//                    auto valVs = vsdc3Dev.readIntegral(chan);
//                    cout << "\nosc Len:\t" << vsdc3Dev.oscLen[chan] << "\n";
//                    cout << "\nCH" << chan << ":\t" << mean << "V\t" << std << "V\tINT:" << valVs << "Vs\t" << valVs/oscSett.timerSec;
//                    //do something with osc data
//                    if(do_SAVE)
//                    {
//            //                        filename.sprintf(filename_t.toLocal8Bit().constData(), vsdc3Dev.baseAddr, chan);
//                                               //(uint32_t baseAddr, uint32_t sett, uint32_t avgn, uint32_t rangeCode, uint32_t ch_num)
//                        std::cout << "==========================------DIR-------============" << std::endl << dirName.toStdString() << std::endl;
//                        std::cout << "==========================-------------============" << std::endl  << std::endl;
//                        filename = dirName + preamble + makeOscChanFilName (makeOscDevFilName(vsdc3Dev.baseAddr, oscSett.sett, oscSett.avgN, vsdc3Dev.getState(chan)), chan);
//                        std::cout << "==========================-----FILE------============" << std::endl << filename.toStdString() << std::endl;
//                        std::cout << "==========================-------------============" << std::endl  << std::endl;
//                        FILE *stream0 = fopen( filename.toLocal8Bit().constData(), "w");

//            //                FILE *stream0 = fopen((globTestPath + filename).toLocal8Bit().constData(), "w");
//                        for(uint ii=0; ii<osc_data.size(); ++ii)
//                        {
//                            fprintf( stream0, "%+.7G\t%+.7G\n",  1.0*ii*vsdc3Dev.adcSamplingTime*(1<<oscSett.avgN), osc_data[ii] );
//                        }
//                        fclose(stream0);
//                    }
//                }
//                cout << std::endl;

//                //plot
//                if(1)
//                {
//                    QString plot_name;
//             //       for(auto vsdc3Dev : vsdcList)
//                    for(auto i=0; i < vsdcList.size(); ++i)
//                    {
//                        //plot_name = makeOscDevFilName(vsdc3Dev.baseAddr, oscSett.sett, oscSett.avgN, vsdc3Dev.getState(0));
//            //            PyPlotChan(global_file_path, plot_name,  oscSett.plot_start, oscSett.plot_stop, oscSett.avgN, vsdc3Dev.chanCount()); //(dir_path, fil_name, ns = 0, ne = -1, navg = 0, ch_count = 2)
//                        plot_name = preamble + makeOscDevFilName(vsdcList[i].baseAddr, oscSett.sett, oscSett.avgN, vsdcList[i].getState(0));
//                        PyPlotChan(devDirs[i], plot_name,  oscSett.plot_start, oscSett.plot_stop, oscSett.avgN, vsdcList[i].chanCount()); //(dir_path, fil_name, ns = 0, ne = -1, navg = 0, ch_count = 2)
//                    }
//                }
//                return cout.str();
//            }



//            void OscStart(VsDCOscSettings oscSett, std::vector<VsDC3v2> &vsdcList)
//            {
//                int deviceCount = 0;
//                std::stringstream cout;
//                VsDCChSettings conf = VsDCChSettings(oscSett.sett, 0, oscSett.avgN, 0);
//                for(auto vsdc3Dev : vsdcList)
//                {
//                    auto chan=0;
//                    for(chan=0; chan<vsdc3Dev.chanCount(); ++chan)
//                        vsdc3Dev.configureChannel(conf, chan);
//                    for(chan=0; chan<vsdc3Dev.chanCount(); ++chan)
//                    {
//                        Sleep(250);
//                        vsdc3Dev.setTimerSec(oscSett.timerSec, chan);
//                    }
//                }
//                int devN;
//                while(devN < 4)
//                {
//                    std::cout << std::endl << "connect generator outputs to inputs device" << devN << "and type 'g' to continue" << std::endl;
//                    std::cout << "or press 4 to exit" << std::endl;
//                    std::cin >> devN;
//                    vsdcList[devN].pstart(0);
//                    Sleep(oscSett.timerSec * 1000 + 100);
//                    OscRead()
//                }
//            }


