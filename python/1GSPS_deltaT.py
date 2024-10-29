import csv
import matplotlib.pyplot as plt
from numpy.fft import fft,ifft
import numpy as np


def process_osc_data(filename, Ts, F_test, Nstop):

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
        adc0 = []
        adc1 = []
        adc2 = []
        adc3 = []
        time = []
        ind=0

        adc_quant = 1

        for row in rows:
            adc0.append(adc_quant*float(row[1]))
            adc1.append(adc_quant*float(row[2]))
            adc2.append(adc_quant*float(row[3]))
            adc3.append(adc_quant * float(row[4]))
            time.append(float(ind*Ts))
            ind += 1

    L = Nstop

    plt.figure(1)
    plt.plot(time[:Nstop], adc0[:Nstop])
    plt.plot(time[:Nstop], adc1[:Nstop])
    plt.plot(time[:Nstop], adc2[:Nstop])
    plt.plot(time[:Nstop], adc3[:Nstop])
    plt.grid(True)
    plt.xlabel(r'$time, s$')
    plt.ylabel(r'$voltage, V$')


    # adc0_fft = 2*np.absolute(fft(adc0))/L*np.sqrt(Ts*L)
    # adc1_fft = 2*np.absolute(fft(adc1))/L*np.sqrt(Ts*L)
    # adc2_fft = 2*np.absolute(fft(adc2))/L*np.sqrt(Ts*L)

    adc0_fft_src = fft(adc0[:Nstop])
    adc1_fft_src = fft(adc1[:Nstop])
    adc2_fft_src = fft(adc2[:Nstop])
    adc3_fft_src = fft(adc3[:Nstop])

    adc0_fft = 2*np.absolute((adc0_fft_src))/L
    adc1_fft = 2*np.absolute((adc1_fft_src))/L
    adc2_fft = 2*np.absolute((adc2_fft_src))/L
    adc3_fft = 2 * np.absolute((adc3_fft_src)) / L

    f_ind = int(F_test*(Ts*L))
    freq = [x/L/Ts for x in range(L)]

    dt1 = (np.angle(adc1_fft_src[f_ind]) - np.angle(adc0_fft_src[f_ind]))/2/np.pi/F_test - 1.0e-9
    dt2 = (np.angle(adc2_fft_src[f_ind]) - np.angle(adc0_fft_src[f_ind]))/2/np.pi/F_test - 2.0e-9
    dt3 = (np.angle(adc3_fft_src[f_ind]) - np.angle(adc0_fft_src[f_ind])) / 2 / np.pi / F_test - 3.0e-9

    print("adc0 ampl at :\t", freq[f_ind], "\t ampl:\t", adc0_fft[f_ind], "\tangle:\t", np.angle(adc0_fft_src[f_ind]), "\n")
    print("adc1 ampl at :\t", freq[f_ind], "\t ampl:\t", adc1_fft[f_ind], "\tangle:\t", np.angle(adc1_fft_src[f_ind]), "\t", dt1, "\n")
    print("adc2 ampl at :\t", freq[f_ind], "\t ampl:\t", adc2_fft[f_ind], "\tangle:\t", np.angle(adc2_fft_src[f_ind]), "\t", dt2, "\n")
    print("adc3 ampl at :\t", freq[f_ind], "\t ampl:\t", adc3_fft[f_ind], "\tangle:\t", np.angle(adc3_fft_src[f_ind]), "\t", dt3, "\n")

    fft_plot_ind = int(L/2)
    # print("adc_num:\tmean\trms\tp2p")
    # print("adc0:\t", np.mean(adc0), "\t", np.std(adc0), "\t", np.max(adc0) - np.min(adc0))
    # print("adc1:\t", np.mean(adc1), "\t", np.std(adc1), "\t", np.max(adc1) - np.min(adc1))
    # print("adc2:\t", np.mean(adc2), "\t", np.std(adc2), "\t", np.max(adc2) - np.min(adc2))



    plt.figure(2)

    plt.plot(freq[:fft_plot_ind], adc0_fft[:fft_plot_ind])
    plt.yscale('log')
    plt.xscale('log')
    plt.plot(freq[:fft_plot_ind], adc1_fft[:fft_plot_ind])
    plt.plot(freq[:fft_plot_ind], adc2_fft[:fft_plot_ind])
    plt.grid(True)
    plt.show()



if __name__ == '__main__':
    import sys
    process_osc_data('D:\Single\Projects\Work\ADC4X250\metrology\data.txt', 4.0e-9, 31250000, 1000)