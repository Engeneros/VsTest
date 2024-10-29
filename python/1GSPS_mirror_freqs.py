import csv
import matplotlib.pyplot as plt
from numpy.fft import fft,ifft
import numpy as np


def process_osc_data(filename, F_test, Nstop=-1):

    csv.register_dialect(
        'mydialect',
        delimiter='\t',
        quotechar='"',
        doublequote=True,
        skipinitialspace=True,
        lineterminator='\n',
        quoting=csv.QUOTE_MINIMAL)

    Ts = (1.0e-9)
    # print(Ts)
    with open(filename, 'r') as vsdc3osc:
        thedata = csv.reader(vsdc3osc, dialect='mydialect')
        rows = list(thedata)
        N = len(rows)
        N_col = (len(rows[0]))
        adc0 = []
        time = []
        ind=0

        adc_quant = 1

        for row in rows[6:]:
            adc0.append(adc_quant*float(row[1]))
            time.append(float(ind*Ts))
            ind += 1

    if Nstop == -1:
        L = ind
    else:
        L = Nstop

    # plt.figure(1)
    # plt.plot(time[:L], adc0[:L])
    #
    # plt.grid(True)
    # plt.xlabel(r'$time, s$')
    # plt.ylabel(r'$voltage, V$')


    adc0_fft_src = fft(adc0[:L])


    adc0_fft = 2*np.absolute((adc0_fft_src))/L

    freq = [x/L/Ts for x in range(L)]

    f_ind = int(F_test * (Ts * L))
    freq = [x / L / Ts for x in range(L)]


    print("adc0 ampl at :\t", freq[f_ind], "\t ampl:\t", adc0_fft[f_ind], "\tangle:\t", np.angle(adc0_fft_src[f_ind]), "\n")


    fft_plot_ind = int(L/2)
    # print("adc_num:\tmean\trms\tp2p")
    # print("adc0:\t", np.mean(adc0), "\t", np.std(adc0), "\t", np.max(adc0) - np.min(adc0))
    # print("adc1:\t", np.mean(adc1), "\t", np.std(adc1), "\t", np.max(adc1) - np.min(adc1))
    # print("adc2:\t", np.mean(adc2), "\t", np.std(adc2), "\t", np.max(adc2) - np.min(adc2))



    plt.figure(2)

    plt.plot(freq[:fft_plot_ind], adc0_fft[:fft_plot_ind])
    plt.yscale('log')
    # plt.xscale('log')
    plt.grid(True)
    plt.show()



if __name__ == '__main__':
    import sys
    process_osc_data('D:\Single\Projects\Work\ADC4X250\metrology\data1.txt', 90e6, 10000)