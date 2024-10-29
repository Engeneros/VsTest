import csv
import matplotlib.pyplot as plt

csv.register_dialect(
        'mydialect',
        delimiter='\t',
        quotechar='"',
        doublequote=True,
        skipinitialspace=True,
        lineterminator='\n',
        quoting=csv.QUOTE_MINIMAL)

fil_name = 'data_osc_HighLow'

with open(fil_name, 'r') as vsdc3osc:
    thedata = csv.reader(vsdc3osc, dialect='mydialect')
    rows = list(thedata)
    N = len(rows)
    N_col = (len(rows[0])) - 1

    ch_count = int(N_col/3)
    time = []
    ch = []

    for idx in range(ch_count):
        time.append([])
        ch.append([])

    for row in rows:
        for idx in range(ch_count):
            time[idx].append(float(row[0+3*idx]))
            ch[idx].append(float(row[1+3*idx]))

    ne = 0
    ns = 0
    plt.figure(1)
    lines = []

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
    plt.show()