import csv

csv.register_dialect(
    'mydialect',
    delimiter = '\t',
    quotechar = '"',
    doublequote = True,
    skipinitialspace = True,
    lineterminator = '\n',
    quoting = csv.QUOTE_MINIMAL)

with open('dataNO.txt','r') as vsdc3osc:
    thedata = csv.reader(vsdc3osc, dialect='mydialect')
    for row in thedata:
        print(row[0])