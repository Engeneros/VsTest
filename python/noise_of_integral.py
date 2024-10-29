import csv
import matplotlib.pyplot as plt
import os
import fnmatch
from numpy.fft import fft, ifft
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

    path, folder = os.path.split(filename)

    plt.figure()
    for ch_ind in range(0, chNum):
        plt.plot(time, ch[ch_ind], label='Ch[{}]'.format(ch_ind))
    plt.yscale('log')
    plt.xscale('log')
    plt.grid(True)
    plt.ylabel(r'Integral, Vs$')
    plt.xlabel(r'$time, s$')
    plt.legend(bbox_to_anchor=(0.01, 0.85, 0.5, .102), loc=0,
               ncol=2, mode="expand", borderaxespad=0.)
    plt.suptitle(folder, fontsize=16)
    # plt.show()


if __name__ == '__main__':
    import sys

    if len(sys.argv) < 3:
        if len(sys.argv) == 1:
            fil_lst = fnmatch.filter(os.listdir('.'), '*_NI.txt')
            print(fil_lst)
            for fil in fil_lst:
                process_osc_data(fil)
        else:
            fil_lst = fnmatch.filter(os.listdir(sys.argv[1]), '*_NI.txt')
            print(fil_lst)
            for fil in fil_lst:
                process_osc_data(sys.argv[1]+fil)
        # npath = 'D:/_work/alt_hw/VsDC3\VsDC3_rev2/TestResults/vsdc3v2/_vsdc3v2_board01_results/dataNI_2V_30times_ZeroPostInt.txt'
        # npath = 'G:\\work\\tmpdat\\VsDC3_chopp\\vsdc_0x10000000_0V2_NI.txt'
        # process_osc_data(npath)
        plt.show()
    else:
        process_osc_data(sys.argv[2] + sys.argv[1])
        plt.show()
