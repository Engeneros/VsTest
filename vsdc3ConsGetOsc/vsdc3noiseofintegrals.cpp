#include "vsdc3noiseofintegral.h"

#include "vsdc3RegDefs.h"
#include "processData.h"
#include <iostream>
#include <QCoreApplication>
#include <qt_windows.h>

using namespace std;

#define C_SIGNAL 0
#define C_GROUND 1
#define C_VREFHIGH 2
#define C_VREFLOW 3

void Plot_NI(void);



void NoiseInt(MovingCoilsServer * pServer){
    quint32 s2nM = 0; //number of points
    quint32 s2nNstart = 0;
    quint32 s2nNstop = 0;
    quint32 s2nN = 0; //enter number of integrals
    quint32 s2nDeltaN = 0;
    int kk=0, ii=0, jj=0;
    quint32 sett = ADC_START_PSTART + ADC_STOP_TIMER;

    float timeQuant;
    *(quint32*)(&timeQuant) = pServer->readVsDC3Register(TIME_QUANT);//.2e-6;
    cout << "ADC Time Quant:\t" << timeQuant << endl;

M0:
    cout << "Source of signal:\n0 - input\n1 - gnd\n2 - VRefHigh\n3 - VRefLow\n99 - exit " << endl;
    cin >> kk;
    switch(kk){
    case C_SIGNAL:
        sett += ADC_SIG;//WriteMaskVsdcReg(GCSR, MUX_M_SIG, MUX_M_MASK, vsdcRegInUse);
        break;
    case C_GROUND:
        sett += ADC_GND;//WriteMaskVsdcReg(GCSR, MUX_M_GND, MUX_M_MASK, vsdcRegInUse);
        break;
    case C_VREFHIGH:
        sett += ADC_REF_H;//WriteMaskVsdcReg(GCSR, MUX_M_REFH, MUX_M_MASK, vsdcRegInUse);
        break;
    case C_VREFLOW:
        sett += ADC_REF_L;//WriteMaskVsdcReg(GCSR, MUX_M_REFL, MUX_M_MASK, vsdcRegInUse);
        break;
    default:
        cout << "Wrong Source" << endl;
        goto M0;
    }

M1:
    cout << "0 - range 2V, 1 - range 200mV;" << endl;
    cin >> kk;
    if(kk==1) {
        sett += ADC_GAIN02;//WriteMaskVsdcReg(GCSR, AUTOCAL, AUTOCAL, vsdcRegInUse);
    }
    else if(kk==0){
        sett += ADC_GAIN2;//WriteMaskVsdcReg(GCSR, !AUTOCAL, AUTOCAL, vsdcRegInUse);
    }
    else {
        cout << "It's a wrong answer!" << endl;
        goto M1;
    }

    cout << "Enter number of integrals per every point:" << endl;
    cin >> s2nN;

    cout << "start decade:" << endl;
    cin >> s2nNstart;

    cout << "decade num:" << endl;
    cin >> s2nNstop;

    cout << "Steps per decade:" << endl;
    cin >> s2nDeltaN;

    float * s2nAvgTmp[VSDC_CH_COUNT];
    for(uint ch_ind=0;ch_ind<VSDC_CH_COUNT;++ch_ind)
    {
        s2nAvgTmp[ch_ind] = new float[s2nN];
    }

    s2nM = s2nNstop * (s2nDeltaN);
    quint32 *  s2nTimer = new quint32[s2nM];

    quint32 dN = pow(10, s2nNstart-1);
    quint32 Dstep;
    kk = 0;
    quint32 nTmp=0;
    for(ii=s2nNstart-1;ii<(s2nNstart+s2nNstop)-1;ii++){
        //cout << "dN: " << dN << " dN/step " << Dstep << endl;
        nTmp = dN;
        dN *= 10;
        if(s2nDeltaN == 2)
        {
            s2nTimer[kk] = 1*nTmp;
            kk++;
            s2nTimer[kk] = 5*nTmp;
            kk++;
        }
        else if(s2nDeltaN == 3)
        {

            s2nTimer[kk] = 1*nTmp;
            kk++;
            s2nTimer[kk] = 2*nTmp;
            kk++;
            s2nTimer[kk] = 5*nTmp;
            kk++;

        }
        else if(s2nDeltaN == 4)
        {

            s2nTimer[kk] = 1*nTmp;
            kk++;
            s2nTimer[kk] = 3*nTmp;
            kk++;
            s2nTimer[kk] = 5*nTmp;
            kk++;
            s2nTimer[kk] = 7*nTmp;
            kk++;

        }
        else if(s2nDeltaN == 5)
        {
            s2nTimer[kk] = 1*nTmp;
            kk++;
            s2nTimer[kk] = 2*nTmp;
            kk++;
            s2nTimer[kk] = 4*nTmp;
            kk++;
            s2nTimer[kk] = 6*nTmp;
            kk++;
            s2nTimer[kk] = 8*nTmp;
            kk++;
        }
        else
        {
            Dstep = ceil(1.0*dN/s2nDeltaN);
            while(nTmp < dN){
                s2nTimer[kk] = nTmp;
                nTmp += Dstep;
                kk++;
            }
        }
    }

    s2nM = kk;

    double * s2nAvg[VSDC_CH_COUNT];
    double * s2nSigma[VSDC_CH_COUNT];

    for(uint ch_ind=0;ch_ind<VSDC_CH_COUNT;++ch_ind)
    {
        s2nAvg[ch_ind] = new double[s2nM];
        s2nSigma[ch_ind] = new double[s2nM];
    }

    cout << s2nM << endl;
    for(ii=0;ii<kk;ii++){
      cout << s2nTimer[ii] << "\t\t\tTime: " << s2nTimer[ii]*timeQuant << endl;
    }
//    return;

    FILE *stream0 = fopen("g:\\work\\tmpdat\\VsDC3_chopp\\dataNI.txt", "w");
    FILE *stream1 = fopen("g:\\work\\tmpdat\\VsDC3_chopp\\dataNI_tmp.txt", "w");
    fclose(stream1);

    pServer->writeVsDC3Register(GCR, CALIB_ALL);//WriteMaskVsdcReg(GCSR, CALIB, CALIB, vsdcRegInUse);
    Sleep(1000);
    for(uint ch_ind=0; ch_ind<VSDC_CH_COUNT;++ch_ind)
    {
        pServer->writeVsDC3Register(ADC_SETT_REG(ch_ind), sett);
        pServer->writeVsDC3Register(ADC_PR_TIME_REG(ch_ind), 0);
    }

    Sleep(350);

    for(ii=0;ii<s2nM;ii++)
    {
        for(uint ch_ind=0;ch_ind<VSDC_CH_COUNT;++ch_ind)
        {
            s2nAvg[ch_ind][ii] = 0;
            s2nSigma[ch_ind][ii] = 0;
        }


        //   WriteMaskVsdcReg(GCSR, CALIB, CALIB, vsdcRegInUse);
        Sleep(20);
        stream1 = fopen("g:\\work\\tmpdat\\VsDC3_chopp\\dataNI_tmp.txt", "a");
        for(jj=0;jj<s2nN;jj++)
        {
            for(uint ch_ind=0; ch_ind<VSDC_CH_COUNT;++ch_ind)
            {
                pServer->writeVsDC3Register(ADC_TIME_REG(ch_ind), s2nTimer[ii]);//число измерений АЦП0
            }

            pServer->writeVsDC3Register(GCR, PSTART_ALL);//WriteMaskVsdcReg(GCSR, PSTART_ALL, PSTART_ALL, vsdcRegInUse);

            Sleep(s2nTimer[ii]*(1010.0*timeQuant)+100);

            fprintf(stream1, "%+G\t%+G\t", timeQuant*(float)jj, timeQuant*s2nTimer[ii]);

            for(uint ch_ind=0; ch_ind<VSDC_CH_COUNT;++ch_ind)
            {
                *(quint32*)(&s2nAvgTmp[ch_ind][jj]) = pServer->readVsDC3Register(ADC_ADC_INT(ch_ind));
                fprintf(stream1, "%+G\t", s2nAvgTmp[ch_ind][jj]);
            }

            fprintf(stream1, "\n");
        }
        fclose(stream1);

        for(uint ch_ind=0; ch_ind<VSDC_CH_COUNT;++ch_ind)
        {
            s2nAvg[ch_ind][ii] =  Mean(s2nAvgTmp[ch_ind], 1, s2nN);
            s2nSigma[ch_ind][ii] = Rms(s2nAvg[ch_ind][ii], s2nAvgTmp[ch_ind], 1, s2nN);
        }


        fprintf(stream0, "%+G", timeQuant*s2nTimer[ii]);//, s2nAvg0[ii], s2nAvg1[ii], s2nSigma0[ii], s2nSigma1[ii]);

        cout << "Point N" << ii << "\t" << s2nTimer[ii]*timeQuant << " s" << ":" << endl;
        cout << "AVG:";
        for(uint ch_ind=0; ch_ind<VSDC_CH_COUNT;++ch_ind)
        {
            cout << "\t" << s2nAvg[ch_ind][ii];
            fprintf(stream0, "\t%+G\t%+G", s2nAvg[ch_ind][ii], s2nSigma[ch_ind][ii]);
        }
        fprintf(stream0, "\n");
        cout << "\nRMS:";
        for(uint ch_ind=0; ch_ind<VSDC_CH_COUNT;++ch_ind)
        {
            cout << "\t" << s2nSigma[ch_ind][ii];
        }
        cout << endl;

        //s2nNstart *= s2nDeltaN;
        Sleep(350);
    }
    fclose(stream0);
    // fclose(stream1);
    Plot_NI();

    for(uint ch_ind=0; ch_ind<VSDC_CH_COUNT;++ch_ind)
    {
        pServer->writeVsDC3Register(ADC_TIME_REG(ch_ind), 0);//число измерений АЦП0
        delete s2nAvg[ch_ind];
        delete s2nAvgTmp[ch_ind];
        delete s2nSigma[ch_ind];
    }

//    ClrVsdcReg(ADC0_CSR, STOP_MASK|START_MASK, vsdcRegInUse);
//    ClrVsdcReg(ADC1_CSR, STOP_MASK|START_MASK, vsdcRegInUse);
//    ClrVsdcReg(GCSR, AUTOCAL|MUX_M_MASK, vsdcRegInUse);


}

void Plot_NI(void){
//    FILE * gnuplotCmd;
//    gnuplotCmd = fopen ("g:\\work\\tmpdat\\VsDC3_chopp\\cmdFile.gp","w");
//    //  fprintf(gnuplotCmd, "plot 'c:\\tmp\\data0.txt' with lines title \"\"\n");
//    fprintf(gnuplotCmd, "plot 'g:\\work\\tmpdat\\VsDC3_chopp\\dataNI.txt' using 1:2 w l 1, 'g:\\work\\tmpdat\\VsDC3_chopp\\dataNI.txt' using 1:3 w l 2 \n");
//    fprintf(gnuplotCmd, "pause 100000\n");
//    //  fprintf(gnuplotCmd, "set terminal png size 2048,1536 enhanced font arial 32\n");
//    //  fprintf(gnuplotCmd, "set output 'c:\\tmp\\plot.png'\n");
//    //  fprintf(gnuplotCmd, "plot 'c:\\tmp\\data.txt' with lines title \"\"\n");
//    fclose(gnuplotCmd);
//    system("start wgnuplot g:\\work\\tmpdat\\VsDC3_chopp\\cmdFile.gp");

//    FILE * gnuplotCmd;
//    gnuplotCmd = _popen("pgnuplot.exe -persist", "w");
//    fprintf(gnuplotCmd, "plot 'g:\\work\\tmpdat\\VsDC3_chopp\\dataNI.txt' using 1:2 with lines, 'g:\\work\\tmpdat\\VsDC3_chopp\\dataNI.txt' using 1:3 with lines \n");
//    _pclose(gnuplotCmd);

    FILE * gnuplotCmd;
    QString str;
    QString filename = "g:\\work\\tmpdat\\VsDC3_chopp\\dataNI.txt";
    str.append("python.exe g:\\work\\tmpdat\\VsDC3_chopp\\noise_of_integral.py ");
    str += filename;
//    str += " ";
//    str.append(QString::number(nstart));
//    str += " ";
//    str.append(QString::number(nstop));
//    str += " ";
//    str.append(QString::number(avgn));
    gnuplotCmd = _popen(str.toLocal8Bit().constData(), "w");

}
