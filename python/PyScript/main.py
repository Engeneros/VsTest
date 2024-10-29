import csv
import matplotlib.pyplot as plt
# from numpy.fft import fft
from scipy.fftpack import fft
import numpy as np


def process_osc_data(filename, ns = 0, ne = -1, navg = 0):

    # print(sys.argv)

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
        N_col = (len(rows[0]))-1
        time = []
        ch = []

        for cols in range(1, N_col):
            ch.append([])

        for row in rows:
            time.append(float(row[0]))
            for cols in range(0,N_col-1):
                ch[cols].append(float(row[cols+1]))

    if ne == 0:
        ne = N
    elif ne < 0:
        ne = N+ne


    plt.figure(1)
    lines = []
    for cols in range(0, N_col-1):
        lin_tmp, =plt.plot(time[ns:ne], ch[cols][ns:ne], label='Ch[{}]'.format(cols))
        lines.append(lin_tmp)
    plt.grid(True)
    plt.xlabel(r'$time, s$')
    plt.ylabel(r'$voltage, V$')
    # plt.legend(handles=lines)
    plt.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc=3,
               ncol=4, mode="expand", borderaxespad=0.)

    if (ns == 0) & ((ne-ns+1) > (1152/(2**navg)+10)):
        ns = (int(1028/(2**navg)))
        ne = N - int(128/(2**navg))-10

    L = ne - ns
    chCount = N_col-1
    osc_fft = np.zeros([chCount, L])

    for ind in range(0, chCount):
        osc = np.array(ch[ind][ns:ne])
        osc_tmp = fft(osc)
        osc_fft[ind] = 2*np.absolute(osc_tmp)/L




    Ts = time[1]-time[0]
    # print("Sampling:\t", 1/Ts, 'SPS')

    freq = [x/L/Ts for x in range(L)]

    plt.figure(2)
    fft_plot_ind = int(L/2)
    for cols in range(0, N_col-1):
        # plt.plot(freq[:fft_plot_ind], ch_fft[N_col-2-cols][:fft_plot_ind])
        plt.plot(freq[:fft_plot_ind], osc_fft[cols][:fft_plot_ind], label='Ch[{}]'.format(cols))
    plt.yscale('log')
    plt.xscale('log')
    plt.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc=3,
               ncol=4, mode="expand", borderaxespad=0.)
    plt.grid(True)

#    plt.figure(3)
#    # plt.plot(time, ch0)
#    # plt.plot(time, ch1)
#    for cols in range(0, N_col-1):
#        plt.plot(time[ns:ne], ch[N_col-2-cols][ns:ne] - np.mean(ch[N_col-2-cols][ns:ne]))
#    plt.grid(True)
#    plt.xlabel(r'$time, s$')
#    plt.ylabel(r'$voltage, V$')
#
#    plt.figure(4)
#    # plt.plot(time, ch0)
#    # plt.plot(time, ch1)
#    for cols in range(0, 2, N_col - 1):
#        plt.plot(time[ns:ne], np.array(ch[cols+1][ns:ne]) - np.array(ch[cols][ns:ne]))
#    plt.grid(True)
#    plt.xlabel(r'$time, s$')
#    plt.ylabel(r'$voltage, V$')

    plt.show()

if __name__ == '__main__':
    import sys
    if len(sys.argv) == 5:
        process_osc_data(sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4]))
    else:
        process_osc_data('dataNO_1.txt', 0, 0, 0)
