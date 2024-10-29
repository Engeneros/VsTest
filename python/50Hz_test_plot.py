import csv
import matplotlib.pyplot as plt
from numpy.fft import fft,ifft
import numpy as np


def process_50Hz_data(filename, cyc_num):
    csv.register_dialect(
        'mydialect',
        delimiter='\t',
        quotechar='"',
        doublequote=True,
        skipinitialspace=True,
        lineterminator='\n',
        quoting=csv.QUOTE_MINIMAL)

    with open(filename, 'r') as vsdc3osc:
        thedata = csv.reader(vsdc3osc, dialect='mydialect')
        rows = list(thedata)
        N = len(rows)
        N_col = (len(rows[0]))
        time = []
        ch0 = []
        ch1 = []
        vol0 = []
        vol1 = []
        sw_time0 = []
        sw_time1 = []


        for row in rows:
            time.append(float(row[0]))
            ch0.append(float(row[1]))
            ch1.append(float(row[2]))
            vol0.append( float(row[1]) / float(row[3]))
            vol1.append( float(row[2]) / float(row[4]))
            sw_time0.append(float(row[3]))
            sw_time1.append(float(row[4]))

    #plot delta Flux

    L = len(ch0)
    dec_rate = 10
    N = int(L/dec_rate)

    print(range(N))

    v_ch = []


    for idx in range(N):
        S0 = sum(ch0[idx * dec_rate: (idx + 1) * dec_rate])
        S1 = sum(ch1[idx * dec_rate: (idx + 1) * dec_rate])
        T0 = sum(sw_time0[idx * dec_rate: (idx + 1) * dec_rate])
        T1 = sum(sw_time1[idx * dec_rate: (idx + 1) * dec_rate])
        v_ch.append((S0+S1)/(T0+T1))
        # print(S0, S1, T0, T1)
        # v_ch.append(sum(ch0[idx*dec_rate : (idx+1)*dec_rate]) + sum(ch1[idx*dec_rate : (idx+1)*dec_rate])) / (sum(sw_time0[idx*dec_rate : (idx+1)*dec_rate]) + sum(sw_time1[idx*dec_rate : (idx+1)*dec_rate]))

    osc_tmp = fft(v_ch)
    osc_fft = 2 * np.absolute(osc_tmp) / len(osc_tmp)

    plt.figure(1)
    plt.subplot(511)
    plt.plot(time[1:], ch0[1:])
    plt.plot(time[1:], ch1[1:])
    plt.ylabel(r'$delta flux, Vs$')
    plt.xlabel(r'$time, s$')
    plt.title(r'Osc', fontsize=20)
    plt.subplot(512)
    plt.plot(time[1:], sw_time0[1:])
    plt.plot(time[1:], sw_time1[1:])
    plt.ylabel(r'$Integration Time, s$')
    plt.xlabel(r'$time, s$')

    plt.subplot(514)
    plt.plot(v_ch[1:])
    # plt.plot(time[1:], vol0[1:])
    # plt.plot(time[1:], vol1[1:])
    plt.ylabel(r'$Average Voltage, V$')
    plt.xlabel(r'$time, s$')

    plt.subplot(515)
    plt.plot(osc_fft[1:int(len(osc_fft)/2)])
    # plt.plot(time[1:], vol0[1:])
    # plt.plot(time[1:], vol1[1:])
    plt.yscale('log')
    plt.xscale('log')
    plt.ylabel(r'$Average Voltage, V$')
    plt.xlabel(r'$time, s$')

    #calc Flux
    ch0_offs = 0.0
    ch1_offs = 0.0

    Nstart = 1
    Nstop = int(len(time))

    for ind in range(Nstart, Nstop):
        ch0_offs += (ch0[ind] / sw_time0[ind])
        ch1_offs += (ch1[ind] / sw_time1[ind])

    ch0_offs /= (Nstop - Nstart)
    ch1_offs /= (Nstop - Nstart)

    print(ch0_offs, ch1_offs)

    val = 0.0
    time_t = 0.0

    tfl = []
    flux = []


    for ind in range(1,len(time)):
        val += (ch0[ind] - (ch0_offs*sw_time0[ind]))
        time_t += sw_time0[ind]
        tfl.append(time_t)
        flux.append(val)
        val += (ch1[ind] - (ch1_offs * sw_time1[ind]))
        time_t += sw_time1[ind]
        flux.append(val)
        tfl.append(time_t)

    plt.subplot(513)
    plt.plot(tfl[0:], flux[0:])

    plt.ylabel(r'$Full flux, Vs$')
    plt.xlabel(r'$time, s$')

    plt.show()


if __name__ == '__main__':
    import sys
    if(len(sys.argv) > 1):
        print(sys.argv)
        process_50Hz_data(sys.argv[1], sys.argv[2])
    else:
        process_50Hz_data('g:/work/tmpdat/VsDC3_chopp/dataNI_06000000.txt', 10)

