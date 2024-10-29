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
        sw_time0 = []
        sw_time1 = []


        for row in rows:
            time.append(float(row[0]))
            ch0.append(float(row[1]))
            ch1.append(float(row[2]))
            sw_time0.append(float(row[3]))
            sw_time1.append(float(row[4]))



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

    with open('out_'+filename, 'w', newline='') as csv_file:
        csv_writer = csv.writer(csv_file, dialect='mydialect')
        for ind in range(len(tfl)):
            csv_writer.writerow([ind, tfl[ind], flux[ind]])

    return [filename, time, ch0, ch1, sw_time0, sw_time1, tfl, flux]

NAME    = 0
TIME    = 1
CH0     = 2
CH1     = 3
SW0_T   = 4
SW1_T   = 5
TFULL   = 6
FLUX    = 7

def print_data(data):
    for dt in data:
        plt.figure(1)

        # plt.subplot(311)
        # plt.plot(dt[TIME][1:], dt[CH0][1:])
        # plt.plot(dt[TIME][1:], dt[CH1][1:])
        # plt.grid(True)
        # plt.ylabel(r'$delta flux, Vs$')
        # plt.xlabel(r'$time, s$')
        # plt.title(data[NAME], fontsize=20)
        # plt.subplot(312)
        # plt.plot(dt[TIME][1:], dt[SW0_T][1:])
        # plt.plot(dt[TIME][1:], dt[SW1_T][1:])
        # plt.grid(True)
        # plt.ylabel(r'$Integration Time, s$')
        # plt.xlabel(r'$time, s$')
        #
        #
        # plt.subplot(313)
        # plt.plot(dt[TFULL][1:], dt[FLUX][1:])
        plt.plot(dt[FLUX][1:])
        plt.grid(True)
        plt.ylabel(r'$Full flux, Vs$')
        plt.xlabel(r'$time, s$')

    plt.show()

if __name__ == '__main__':
    import sys

    files_down = [
        #Zero Field, Coil is connected, synchroniztion to Step Motor
        ['coil_zero_field_Down_0.txt'],
        #Short curcuit with shielded cable in field 1.1T, synchroniztion to Step Motor
        ['cable_zero_Down_0.txt'],
        #Short curcuit with twisted pair in field 1.1T, synchroniztion to Step Motor
        ['zero_Down_0.txt'],
        ['zero_Down_1.txt'],
        ['zero_Down_2.txt'],
        #Field 1.1T, signal from coil, synchroniztion to Step Motor
        ['flux_vs_pos_Down_0.txt'],
        ['flux_vs_pos_Down_1.txt'],
        ['flux_vs_pos_Down_2.txt'],
        ['flux_vs_pos_Down_3.txt']
    ]
    files_up = [
        # Zero Field, Coil is connected, synchroniztion to Step Motor
        ['coil_zero_field_Up_0.txt'],
        # Short curcuit with shielded cable in field 1.1T, synchroniztion to Step Motor
        ['cable_zero_Up_0.txt'],
        # Short curcuit with twisted pair in field 1.1T, synchroniztion to Step Motor
        ['zero_Up_0.txt'],
        ['zero_Up_1.txt'],
        ['zero_Up_2.txt'],
        # Field 1.1T, signal from coil, synchroniztion to Step Motor
        ['flux_vs_pos_Up_0.txt'],
        ['flux_vs_pos_Up_1.txt'],
        ['flux_vs_pos_Up_2.txt'],
        ['flux_vs_pos_Up_3.txt']
    ]

    if(len(sys.argv) > 1):
        print(sys.argv)
        process_50Hz_data(sys.argv[1], sys.argv[2])
    else:
        res = []
        for file in files_down:
            res.append(process_50Hz_data(file[0], 200))


        print_data(res)


            # plot delta Flux

            # plt.figure(1)
            #
            # plt.subplot(311)
            # plt.plot(time[1:], ch0[1:])
            # plt.plot(time[1:], ch1[1:])
            # plt.grid(True)
            # plt.ylabel(r'$delta flux, Vs$')
            # plt.xlabel(r'$time, s$')
            # plt.title(filename, fontsize=20)
            # plt.subplot(312)
            # plt.plot(time[1:], sw_time0[1:])
            # plt.plot(time[1:], sw_time1[1:])
            # plt.grid(True)
            # plt.ylabel(r'$Integration Time, s$')
            # plt.xlabel(r'$time, s$')


            # plt.subplot(313)
            # plt.plot(tfl[0:], flux[0:])
            # plt.grid(True)
            # plt.ylabel(r'$Full flux, Vs$')
            # plt.xlabel(r'$time, s$')
            #
            # plt.show()