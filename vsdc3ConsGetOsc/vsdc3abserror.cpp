#include "vsdc3abserror.h"
#include <iostream>
#include <QCoreApplication>
#include <qt_windows.h>

#include "vsdc3RegDefs.h"
#include "processData.h"

//vsdc3AbsError::vsdc3AbsError()
//{

//}

using namespace std;

quint32 XOSC_DRDY = 1;
float   PREC_TIMER_FREQ = 1e8;
float   PR_TIMER_TO_ADC_TIMER;

void Plot_TLT(void);

void ErrorOfTntegral(MovingCoilsServer * pServer)
{
    quint32 tltN = 20;
    quint32 tltM = 0; //steps per decade

    quint32 tltDeltaT = 0;

    float vRefH = 0;
    quint32 data_tmp = 0;
    int ii = 0, jj = 0;
    FILE * stream0, *stream1;

    quint32 sett = ADC_GAIN2 + ADC_START_PSTART + ADC_STOP_TIMER + ADC_REF_H;
//    quint32 sett = ADC_GAIN02 + ADC_START_PSTART + ADC_STOP_TIMER + ADC_REF_L;

    //TODO:
    //Select REF source depend on VsDC type and Range Selected


    float timeQuant;
    *(quint32*)(&timeQuant) = pServer->readVsDC3Register(TIME_QUANT);//.2e-6;
    cout << "ADC Time Quant:\t" << timeQuant << endl;

    PR_TIMER_TO_ADC_TIMER = 1.0/(PREC_TIMER_FREQ * timeQuant);

M0:
    cout << "Enter number of points per step:" << endl;
    cin >> tltN;
    if(tltN<=0){
        cout<< "Not bad! but i catch you!" << endl;
        goto M0;
    }

    quint32 tltStart;

M2:
    cout << "start decade:" << endl;
    cin >> tltStart;
    if(tltStart<1){
        cout<< "Should be multiply of 1ns!" << endl;
        goto M2;
    }
    //M3:
    cout << "decade num:" << endl;
    cin >> tltDeltaT;

    quint32 tlNumDec;

    cout << "steps per decade: " << endl;
    cin >> tlNumDec;

    int kk=0;
    quint32 * tltBFTimer  = new quint32[(tltDeltaT)*(tlNumDec+1)];
    quint32 * tltADCTimer = new quint32[(tltDeltaT)*(tlNumDec+1)];






    quint32 dN = pow(10, tltStart-1);
    quint32 Dstep;
    kk = 0;
    quint32 nTmp=0;
    //    for(ii=tltStart-1;ii<(tltStart+tltDeltaT)-1;ii++){
    //      //cout << "dN: " << dN << " dN/step " << Dstep << endl;
    //      nTmp = dN;
    //      dN *= 10;
    //      Dstep = ceil(1.0*dN/tlNumDec);
    //      while(nTmp < dN){
    //        tltBFTimer[kk]  = nTmp;
    //        tltADCTimer[kk] = tltBFTimer[kk]/320 + 1;
    //        nTmp += Dstep;
    //        kk++;
    //      }
    //    }

    for(ii=tltStart-1;ii<(tltStart+tltDeltaT)-1;ii++){
        //cout << "dN: " << dN << " dN/step " << Dstep << endl;
        nTmp = dN;
        dN *= 10;
        if(tlNumDec == 2)
        {
            tltBFTimer[kk]  = 1*nTmp;
            tltADCTimer[kk] = int((float)tltBFTimer[kk]*PR_TIMER_TO_ADC_TIMER) + 1;
            kk++;
            tltBFTimer[kk] = 5*nTmp;
            tltADCTimer[kk] = int((float)tltBFTimer[kk]*PR_TIMER_TO_ADC_TIMER) + 1;
            kk++;
        }
        else if(tlNumDec == 3)
        {

            tltBFTimer[kk] = 1*nTmp;
            tltADCTimer[kk] = int((float)tltBFTimer[kk]*PR_TIMER_TO_ADC_TIMER) + 1;
            kk++;
            tltBFTimer[kk] = 2*nTmp;
            tltADCTimer[kk] = int((float)tltBFTimer[kk]*PR_TIMER_TO_ADC_TIMER) + 1;
            kk++;
            tltBFTimer[kk] = 5*nTmp;
            tltADCTimer[kk] = int((float)tltBFTimer[kk]*PR_TIMER_TO_ADC_TIMER) + 1;
            kk++;

        }
        else if(tlNumDec == 4)
        {

            tltBFTimer[kk] = 1*nTmp;
            tltADCTimer[kk] = int((float)tltBFTimer[kk]*PR_TIMER_TO_ADC_TIMER) + 1;
            kk++;
            tltBFTimer[kk] = 3*nTmp;
            tltADCTimer[kk] = int((float)tltBFTimer[kk]*PR_TIMER_TO_ADC_TIMER) + 1;
            kk++;
            tltBFTimer[kk] = 5*nTmp;
            tltADCTimer[kk] = int((float)tltBFTimer[kk]*PR_TIMER_TO_ADC_TIMER) + 1;
            kk++;
            tltBFTimer[kk] = 7*nTmp;
            tltADCTimer[kk] = int((float)tltBFTimer[kk]*PR_TIMER_TO_ADC_TIMER) + 1;
            kk++;

        }
        else if(tlNumDec == 5)
        {
            tltBFTimer[kk] = 1*nTmp;
            tltADCTimer[kk] = int((float)tltBFTimer[kk]*PR_TIMER_TO_ADC_TIMER) + 1;
            kk++;
            tltBFTimer[kk] = 2*nTmp;
            tltADCTimer[kk] = int((float)tltBFTimer[kk]*PR_TIMER_TO_ADC_TIMER) + 1;
            kk++;
            tltBFTimer[kk] = 4*nTmp;
            tltADCTimer[kk] = int((float)tltBFTimer[kk]*PR_TIMER_TO_ADC_TIMER) + 1;
            kk++;
            tltBFTimer[kk] = 6*nTmp;
            tltADCTimer[kk] = int((float)tltBFTimer[kk]*PR_TIMER_TO_ADC_TIMER) + 1;
            kk++;
            tltBFTimer[kk] = 8*nTmp;
            tltADCTimer[kk] = int((float)tltBFTimer[kk]*PR_TIMER_TO_ADC_TIMER) + 1;
            kk++;
        }
        else
        {
            Dstep = ceil(1.0*dN/tlNumDec);
            while(nTmp < dN){
                tltBFTimer[kk] = nTmp;
                tltADCTimer[kk] = int((float)tltBFTimer[kk]*PR_TIMER_TO_ADC_TIMER) + 1;
                nTmp += Dstep;
                kk++;
            }
        }
    }

    tltM = kk;

    cout << tltM << endl;
    for(ii=0;ii<kk;ii++){
        cout << "Pr timer: " << (float)tltBFTimer[ii]/PREC_TIMER_FREQ << "\t\tADC timer: " << tltADCTimer[ii]*timeQuant << "\t\tPause: " << (int)((float)tltADCTimer[ii]*timeQuant*1000.0 + 30.0) << endl;
    }

    double IntMean[VSDC_CH_COUNT];// new double[tltM];
    double IntRms[VSDC_CH_COUNT];// new double[tltM];
    double TheorInt;// new double[tltM];
     float * Int_tmp[VSDC_CH_COUNT];

    for(uint ch_ind=0; ch_ind<VSDC_CH_COUNT; ++ch_ind)
    {
        pServer->writeVsDC3Register(ADC_SETT_REG(ch_ind), sett);//источник стопа АЦП0 - по таймеру
        Int_tmp[ch_ind] = new float[tltN];
    }

    //TODO: Select Ref Value depend on VsDC Type and Range selected
    if(sett & ADC_GAIN02)
        *(uint*)(&vRefH) = pServer->readVsDC3Register(REFL);
    else
        *(uint*)(&vRefH) = pServer->readVsDC3Register(REFH);




    stream0 = fopen("g:\\work\\tmpdat\\VsDC3_chopp\\dataTLT_tmp.txt", "w");
    stream1 = fopen("g:\\work\\tmpdat\\VsDC3_chopp\\dataTLT.txt", "w");


    pServer->writeVsDC3Register(GCR, CALIB_ALL);//WriteMaskVsdcReg(GCSR, CALIB, CALIB, vsdcRegInUse);
    Sleep(1000);
    kk=1;

//    for(ii=0;ii<tltM;ii++)
    for(ii=tltM-1;ii>=0;--ii)
    {
        for(uint ch_ind=0; ch_ind<VSDC_CH_COUNT; ++ch_ind)
        {
            pServer->writeVsDC3Register(ADC_TIME_REG(ch_ind), tltADCTimer[ii]);
            pServer->writeVsDC3Register(ADC_PR_TIME_REG(ch_ind), tltBFTimer[ii]);
        }

        TheorInt = vRefH*(float)tltBFTimer[ii]/PREC_TIMER_FREQ;

        for(jj=0;jj<tltN;jj++)
        {
            pServer->writeVsDC3Register(GCR, PSTART_ALL);
            Sleep((int)((float)tltADCTimer[ii]*timeQuant*1000.0 + 10.0));
            fprintf(stream0, "%+G\t%+G\t+.8G", 10.0e-9*tltBFTimer[ii], 1.0*jj, TheorInt);
            for(uint ch_ind=0; ch_ind<VSDC_CH_COUNT; ++ch_ind)
            {
                data_tmp = pServer->readVsDC3Register(ADC_ADC_INT(ch_ind));
                Int_tmp[ch_ind][jj] = * ((float*) & data_tmp);
                fprintf(stream0, "\t%+.8G", Int_tmp[ch_ind][jj]);
            }
            fprintf(stream0, "\n");
        }


        cout << "Theor: " << TheorInt;
        fprintf(stream1, "%+G\t%+.8+G", (float)tltBFTimer[ii]/PREC_TIMER_FREQ, TheorInt);
        for(uint ch_ind=0; ch_ind<VSDC_CH_COUNT; ++ch_ind)
        {
            IntMean[ch_ind] = Mean(Int_tmp[ch_ind], 0, tltN);
            IntRms[ch_ind] = Rms(IntMean[ch_ind], Int_tmp[ch_ind], 0, tltN);
            cout << "\t" << IntMean[ch_ind];
            fprintf(stream1, "\t%+.8G", IntMean[ch_ind]);
        }
        cout << endl;
        fprintf(stream1, "\n");

        Sleep(2000);
    }

    fclose(stream0);
    fclose(stream1);

    for(uint ch_ind=0; ch_ind<VSDC_CH_COUNT; ++ch_ind)
    {
        pServer->writeVsDC3Register(ADC_TIME_REG(ch_ind), 0);
        pServer->writeVsDC3Register(ADC_PR_TIME_REG(ch_ind), 0);
    }


    Plot_TLT();
}

void Plot_TLT(void){

    FILE * gnuplotCmd;
    QString str;
    QString filename = "g:\\work\\tmpdat\\VsDC3_chopp\\dataTLT.txt";
    str.append("python.exe g:\\work\\tmpdat\\VsDC3_chopp\\error_of_integral.py ");
    str += filename;
    gnuplotCmd = _popen(str.toLocal8Bit().constData(), "w");

}
