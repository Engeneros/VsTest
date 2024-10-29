import csv
import matplotlib.pyplot as plt
import os
import fnmatch
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
        chNum = int((N_col - 2)/2)

        print(chNum)
        time = []
        theor = []
        ch = []
        chStd = []
        res = []
        rel = []

        for ch_ind in range(0, chNum):
            ch.append([])
            res.append([])
            rel.append([])
            chStd.append([])

        for row in rows:
            time.append(float(row[0]))
            theor.append( float(row[1]))
            for ch_ind in range(0, chNum):
                ch[ch_ind].append(float(row[2 + 2 * ch_ind]))
                chStd[ch_ind].append(float(row[3 + 2 * ch_ind]))
                rel[ch_ind].append(abs(float(row[2 + 2 * ch_ind]) - float(row[1])) / float(row[0]))
                res[ch_ind].append(abs(float(row[2 + 2 * ch_ind]) - float(row[1])))


    ns = 0

    path, folder = os.path.split(filename)

    plt.figure()


    plt.subplot(3, 1, 1)
    for ch_ind in range(0, chNum):
        plt.plot(time[ns:], res[ch_ind][ns:])
    plt.yscale('log')
    plt.xscale('log')
    plt.grid(True)
    plt.ylabel(r'Error of Integral, Vs')
    plt.xlabel(r'$time, s')

    plt.subplot(3, 1, 2)
    for ch_ind in range(0, chNum):
        plt.plot(time[ns:], rel[ch_ind][ns:])
    plt.yscale('log')
    plt.xscale('log')
    plt.grid(True)
    plt.ylabel(r'Error of Integral/Time, V')
    plt.xlabel(r'$time, s')

    plt.subplot(3, 1, 3)
    for ch_ind in range(0, chNum):
        plt.plot(time[ns:], chStd[ch_ind][ns:], label='Ch[{}]'.format(ch_ind))
    plt.yscale('log')
    plt.xscale('log')
    plt.grid(True)
    plt.ylabel(r'Integral RMS, Vs')
    plt.xlabel(r'$time, s')
    plt.legend(bbox_to_anchor=(0.05, 0.85, 0.5, .102), loc=0,
               ncol=2, mode="expand", borderaxespad=0.)
    plt.suptitle(folder, fontsize=16)

    # plt.show()

if __name__ == '__main__':
    import sys

    if len(sys.argv) < 3:
        if len(sys.argv) == 1:
            fil_lst = fnmatch.filter(os.listdir('.'), '*_TLT.txt')
            print(fil_lst)
            for fil in fil_lst:
                process_osc_data(fil)
        else:
            fil_lst = fnmatch.filter(os.listdir(sys.argv[1]), '*_TLT.txt')
            print(fil_lst)
            for fil in fil_lst:
                process_osc_data(sys.argv[1]+fil)

        # process_osc_data('G:\\work\\tmpdat\\VsDC3_chopp\\vsdc_0x10000000_0V2_TLT.txt')
        plt.show()
    else:
        process_osc_data(sys.argv[2]+sys.argv[1])
        plt.show()