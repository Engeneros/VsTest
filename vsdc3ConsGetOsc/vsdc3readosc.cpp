//#include "vsdc3noiseofintegral.h"

//#include "vsdc3RegDefs.h"
//#include "processData.h"
//#include <iostream>
//#include <QCoreApplication>
//#include <qt_windows.h>

//using namespace std;

//bool make_osc_measurement(MovingCoilsServer * pServer, float t_int, uint sett, uint avgn, uint prTimer, uint startNum);
//bool get_data(MovingCoilsServer * pServer, uint ind, uint avgn);

//void GetOsc(MovingCoilsServer * pServer)
//{

//}

//bool make_osc_measurement(MovingCoilsServer * pServer, float t_int, uint sett, uint avgn, uint prTimer, uint startNum){
//    uint dataTmp;

//    //ADC_SET_AUTOZERO ADC_SET_GAIN_DIS ADC_REF_L ADC_AUCAL


////    mServer.writeVsDC3Register(ADC0_SETT_REG, ADC_GAIN02 + ADC_REF_H);
////    mServer.writeVsDC3Register(ADC1_SETT_REG, ADC_GAIN02 + ADC_REF_H);
////    Sleep(30);

//        mServer.writeVsDC3Register(ADC0_SETT_REG, sett);
//        mServer.writeVsDC3Register(ADC1_SETT_REG, sett);

//    uint timerTmp = (t_int)*F_DRDY + 10*(1 << avgn);
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
///*
//    mServer.writeVsDC3Register(ADC0_ADC_WRITE, 0);
//    mServer.writeVsDC3Register(ADC1_ADC_WRITE, 0);
//*/
////    mServer.writeVsDC3Register(ADC0_SETT_REG, sett);
////    mServer.writeVsDC3Register(ADC1_SETT_REG, sett);

//    float dac_volt = 0.0;
////    mServer.writeVsDC3Register(ADC0_DAC_VOLT, *(quint32*)(&dac_volt) );
//     *(quint32*)(&dac_volt) = mServer.readVsDC3Register( ADC0_DAC_VOLT);
//    cout << "DAC Settings: " << dac_volt << endl;

//    Sleep(300);

//    for(int jj=0; jj<startNum; ++jj)
//    {
//        int tmpT = (int)((float)timerTmp*(1000.0/(float)F_DRDY));

////        mServer.writeVsDC3Register(GCR, PSTART_ALL);
////        mServer.writeVsDC3Register(ADC0_SETT_REG, sett);
////        mServer.writeVsDC3Register(ADC1_SETT_REG, sett);

//        Sleep(tmpT+1000);
//        cout << "\n\n\n" << jj << endl;
//        cout << "Timeout after start:\t" << tmpT << " ms" << endl;

//        mServer.writeVsDC3Register(ADC0_ADC_READ, 0);
//        mServer.writeVsDC3Register(ADC1_ADC_READ, 0);

//        if(1)
//        {
//            get_data(pServer, jj, avgn);
//        }

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

//        dataTmp =  mServer.readVsDC3Register( ADC0_ADC_READ);
//        cout << dec << "read address 0: " << dataTmp << dec << endl;
//        dataTmp =  mServer.readVsDC3Register( ADC1_ADC_READ);
//        cout << dec << "read address 1: " << dataTmp << dec << endl;
//        dataTmp =  mServer.readVsDC3Register( ADC0_ADC_WRITE);
//        cout << dec << "write address 0: " << dataTmp << dec << endl;
//        dataTmp =  mServer.readVsDC3Register( ADC1_ADC_WRITE);
//        cout << dec << "write address 1: " << dataTmp << dec << endl;

//        dataTmp =  mServer.readVsDC3Register(ADC0_ADC_INT);
//        cout << "ADC0 integral:" << *(float *) (&dataTmp) << "\t Avg voltage: " << (*(float *) (&dataTmp)) / ((float)timerTmp/((float)F_DRDY)) << "\t delta: " << (*(float *) (&dataTmp)) / ((float)timerTmp/((float)F_DRDY)) << endl;
//        dataTmp =  mServer.readVsDC3Register(ADC1_ADC_INT);
//        cout << "ADC1 integral:" << *(float *) (&dataTmp) << "\t Avg voltage: " << (*(float *) (&dataTmp)) / ((float)timerTmp/((float)F_DRDY)) << "\t delta: " << (*(float *) (&dataTmp)) / ((float)timerTmp/((float)F_DRDY)) << endl;
//        dataTmp =  mServer.readVsDC3Register(INT_BUFF_SW_TIME);
//        cout << "Sw on Time:" << *(float *) (&dataTmp) << endl;

//         dataTmp =  mServer.readVsDC3Register(ADC0_QUAN);
//         cout << "ADC0 quant:" << *(float *) (&dataTmp) * 256.0 << "\t";
//         dataTmp =  mServer.readVsDC3Register(ADC1_QUAN);
//         cout << "ADC1 quant:" << *(float *) (&dataTmp) * 256.0 << endl;

//        //   cin >> ii;
//        //   if(ii == 99) break;

//    }
//}

//bool get_data(uint ind, uint avgn)
//{
//    uint nn0 =  mServer.readVsDC3Register( ADC0_ADC_WRITE);
//    cout << nn0 << " ch0 Points will be readed" << endl;

//    uint nn1 =  mServer.readVsDC3Register( ADC1_ADC_WRITE);
//    cout << nn1 << " ch1 Points will be readed" << endl;

//    uint nn = qMin(nn0, nn1);

//    float * data0 = new float[nn];
//    float * data1 = new float[nn];
//    QString filename_t = "g:\\work\\tmpdat\\VsDC3_chopp\\dataNO_%d.txt";
//    QString filename;
//    filename.sprintf(filename_t.toLocal8Bit().constData(),(ind+1));
//    FILE *stream0 = fopen(filename.toLocal8Bit().constData(), "w");

//    uint rdPortion = 1000;
//    uint ch0addr = (VME_CH0_OSC_MEM * 4);
//    uint ch1addr = (VME_CH1_OSC_MEM * 4);
//    QVector<quint32> rdData0;
//    QVector<quint32> rdData1;


//    uint rdnum = nn;

//    int kk=0;
//    for(;;)
//    {
//        if(nn > rdPortion)
//        {
//            mServer.readVsDC3Registers(ch0addr, rdData0, rdPortion);
//            mServer.readVsDC3Registers(ch1addr, rdData1, rdPortion);
//            ch0addr = rdData0[0];
//            ch1addr = rdData1[0];
//            for(int ii=0; ii<rdPortion; ii++)
//            {
//                data0[ii+kk*rdPortion] = *(float*) &rdData0[ii+1];
//                data1[ii+kk*rdPortion] = *(float*) &rdData1[ii+1];
//                fprintf(stream0, "%+G\t%+.7G\t%+.7G\n", ((1.0*(ii+kk*rdPortion))  *(1/((float)F_DRDY))*(1<<avgn) )/* ((1.0*ii+jj*nn)*(3.2e-6)*(1<<avgn))*/, data0[ii+kk*rdPortion], data1[ii+kk*rdPortion]);
//            }
//        }
//        else
//        {
//            if(nn>0)
//            {
//                mServer.readVsDC3Registers(ch0addr, rdData0, nn);
//                mServer.readVsDC3Registers(ch1addr, rdData1, nn);
//                for(int ii=0; ii<nn; ii++)
//                {
//                    data0[ii+kk*rdPortion] = *(float*) &rdData0[ii+1];
//                    data1[ii+kk*rdPortion] = *(float*) &rdData1[ii+1];
//                    fprintf(stream0, "%+G\t%+.7G\t%+.7G\n", (1.0*(ii+kk*rdPortion)  *(1/((float)F_DRDY))*(1<<avgn) )/* ((1.0*ii+jj*nn)*(3.2e-6)*(1<<avgn))*/, data0[ii+kk*rdPortion], data1[ii+kk*rdPortion]);
//                }
//            }
//            break;
//        }
//        kk++;
//        nn -= rdPortion;
//        rdData0.clear();
//        rdData1.clear();
//    }
//    fclose(stream0);

//    PyPlot(filename, 100/*(128/(2^avgn))*/, rdnum-128/*-int(128/(2^avgn))-10*/, avgn);

//    if(rdnum > 102 && rdnum < 1000000)
//    {
//        double mean0 = Mean((float*)data0, 100, rdnum-128);
//        double rms0 = Rms(mean0, (float*)data0, 100, rdnum-128);
//        double mean1 = Mean((float*)data1, 100, rdnum-128);
//        double rms1 = Rms(mean1, (float*)data1, 100, rdnum-128);
//        /*
//                              double mean0 = Mean(data0, 0, 200);
//                              double rms0 = Rms(mean0, data0, 0, 200);
//                              double mean1 = Mean(data1, 0, 200);
//                              double rms1 = Rms(mean1, data1, 0, 200);
//                        */
//        cout << "ADC0 avg: " << mean0 << " V; ADC0 rms: " << rms0 << "V; dU: " << mean0 << "V" << endl;
//        cout << "ADC1 avg: " << mean1 << " V; ADC1 rms: " << rms1 << "V; dU: " << mean1 << "V" << endl;

//    }

//    delete data0;
//    delete data1;



//    return true;
//}

