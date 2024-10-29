import csv
import matplotlib.pyplot as plt
from numpy.fft import fft
# from scipy.fftpack import fft
import numpy as np


def process_osc_data(dir_path, fil_name, ns = 0, ne = -1, navg = 0, ch_count = 2):

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

    time = []
    ch = []

    for ch_ind in range(ch_count):
        filename = dir_path + '\\' + fil_name + '{}.oscdat'.format(ch_ind)
        time.append([])
        ch.append([])
        with open(filename, 'r') as vsdc3osc:
            thedata = csv.reader(vsdc3osc, dialect='mydialect')
            rows = list(thedata)
            N = len(rows)
            N_col = (len(rows[0]))-1
            # print(N, N_col)
            for row in rows:
                time[ch_ind].append(float(row[0]))
                ch[ch_ind].append(float(row[1]))

    plt.figure(1)
    lines = []
    # tl = ['CAL0_SW_GND', 'CAL0_GND', 'CAL0_REF', 'CAL1_SW_GND', 'CAL1_GND', 'CAL1_REF']
    # info_str = []
    # for ch_ind in range(ch_count):
    #     for jdx in range(6):
    #         info_str.append("{}\t".format(ch_ind) + tl[jdx] + "\t{0:0.2f}".format(np.mean(ch[ch_ind][(jdx*80000+40000):(jdx*80000+80000)])))
    #     info_str.append("{}\t".format(ch_ind) + '2V_GND\t' + "{0:0.2f}".format(np.mean(ch[ch_ind][480000+10:(480000 + 9990)])))
    #     info_str.append("{}\t".format(ch_ind) + '2V_SW_GND\t' + "{0:0.2f}".format(np.mean(ch[ch_ind][490000+10:(490000 + 790)])))
    #     info_str.append("{}\t".format(ch_ind) + '0V2_GND\t' + "{0:0.2f}".format(np.mean(ch[ch_ind][(490800+10):(490800 + 9990)])))
    #     info_str.append("{}\t".format(ch_ind) + '0V2_SW_GND\t' + "{0:0.2f}".format(np.mean(ch[ch_ind][(500800+10):(500800 + 790)])))
    #
    # for lns in info_str:
    #     print(lns)
    #
    # with open(dir_path + '\\' + fil_name + 'cal.txt', 'w') as f:
    #     for item in info_str:
    #         f.write("%s\n" % item)

    for ch_ind in range(ch_count):
        if ne == 0:
            ne = len(time[ch_ind])
        elif ne < 0:
            ne = len(time[ch_ind])+ne

        lin_tmp, =plt.plot(time[ch_ind][ns:ne], ch[ch_ind][ns:ne], label='Ch[{}]'.format(ch_ind))
        lines.append(lin_tmp)

    plt.grid(True)
    plt.xlabel(r'$time, s$')
    plt.ylabel(r'$voltage, V$')
    # plt.legend(handles=lines)
    plt.legend(bbox_to_anchor=(0., 0., 1., .102), loc=0,
               ncol=4, mode="expand", borderaxespad=0.)

    plt.suptitle(fil_name, fontsize=16)



#=================================================================
    plt.figure(2)
    L = ne - ns

    osc_fft = np.zeros([ch_count, L])

    for ind in range(0, ch_count):
        osc = np.array(ch[ind][ns:ne])
        osc_tmp = fft(osc)
        osc_fft[ind] = 2 * np.absolute(osc_tmp) / L

    Ts = time[0][1] - time[0][0]
    freq = [x / L / Ts for x in range(L)]
    fft_plot_ind = int(L / 2)
    for cols in range(0, ch_count):
        plt.plot(freq[:fft_plot_ind], osc_fft[cols][:fft_plot_ind], label='Ch[{}]'.format(cols))

    plt.yscale('log')
    plt.xscale('log')
    plt.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc=3,
               ncol=4, mode="expand", borderaxespad=0.)
    plt.grid(True)
    plt.suptitle(fil_name, fontsize=16)


    plt.show()

if __name__ == '__main__':
    import sys
    if len(sys.argv) == 7:
        process_osc_data(sys.argv[1], sys.argv[2], int(sys.argv[3]), int(sys.argv[4]), int(sys.argv[5]), int(sys.argv[6]))
    else:
        process_osc_data('G:\\work\\tmpdat\\VsDC3_chopp', 'vsdc_0x6000000_SIG_avg0_0V2_ch_', 10, 0, 0, 2)