import csv
import matplotlib.pyplot as plt
from numpy.fft import fft,ifft
import numpy as np


def process_osc_data(filename):

    csv.register_dialect(
        'mydialect',
        delimiter='\t',
        quotechar='"',
        doublequote=True,
        skipinitialspace=True,
        lineterminator='\n',
        quoting=csv.QUOTE_MINIMAL)

    # Ts = (3.2e-6) * (1 << navg)
    # print(Ts)
    with open(filename, 'r') as vsdc3osc:
        thedata = csv.reader(vsdc3osc, dialect='mydialect')
        rows = list(thedata)

        N = len(rows)
        N_col = (len(rows[0]))
        chNum = int((N_col - 1) / 2)
        print(chNum)
        time = []
        ch = []

        for ch_ind in range(0, chNum):
            ch.append([])

        for row in rows:
            time.append(float(row[0]))
            for ch_ind in range(0, chNum):
                ch[ch_ind].append(float(row[2 + 2 * ch_ind]))

        tI = float(rows[0][1])
        print(tI)
        time = [tI]
        ch0 = [[]]
        ch1 = [[]]
        int0 = []
        int1 = []
        tt_int = []
        ch0_mn = []
        ch1_mn = []

        for row in rows:
            int0.append(float(row[2]))
            int1.append(float(row[3]))
            tt_int.append(float(row[1]))
            if (float(row[1]) != tI):
                time.append(float(row[1]))
                ch0_mn.append(np.mean(ch0[-1])/tI)
                ch1_mn.append(np.mean(ch1[-1])/tI)
                ch0.append([float(row[2])])
                ch1.append([float(row[3])])
                tI = float(row[1])
            else:
                ch0[-1].append(float(row[2]))
                ch1[-1].append(float(row[3]))

    # np.array(int0)
    ch0_mean = np.mean(ch0[-1])
    ch1_mean = np.mean(ch1[-1])

    print(ch0_mn)

    print(tt_int[-1], ch0_mean, ch1_mean)

    ch0_int = []
    ch1_int = []

    for ii in range(len(int0)):
        ch0_int.append(int0[ii] - ch0_mean * tt_int[ii])
        ch1_int.append(int1[ii] - ch1_mean * tt_int[ii])

    plt.figure(1)

    ax1 = plt.gca()
    ax2 = ax1.twinx()

    ax1.plot(ch0_int)
    ax1.plot(ch1_int)
    ax2.plot(tt_int, color='r')
    ax2.set_yscale('log')
    # plt.yscale('log')
    # plt.xscale('log')
    plt.grid(True)
    plt.ylabel(r'Integral, Vs$')
    plt.xlabel(r'$time, s$')
    plt.title(r'Osc', fontsize=20)
    #
    plt.show()

if __name__ == '__main__':
    import sys
    if len(sys.argv) == 1:
        npath = 'G:\\work\\tmpdat\\VsDC3_chopp\\dataNI_tmp.txt'
        process_osc_data(npath)
    else:
        process_osc_data(sys.argv[1])