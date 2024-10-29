import pyvisa as visa
from pyvisa import errors


gen_addr = "TCPIP0::192.168.1.111::inst0::INSTR"

def main():
    rm = visa.ResourceManager("@py")
    inst = None
    print("Hello Python")
    try:
        inst = rm.open_resource(gen_addr)
    except errors.VisaIOError as e:
        print(e)
        print("Probabpy Multimeter is not connected or turned on")
        exit(1)

    # print(inst.write("*RST"))
    inst.write("VOLTage:RANGe:AUTO ON")
    inst.write("VOLTage:NPLC 10")
    inst.query("READ?")
    inst.query("READ?")
    # inst.query("READ?")
    # inst.query("READ?")
    # inst.query("READ?")
    # inst.query("READ?")
    # inst.query("READ?")
    # inst.query("READ?")
    msmt = inst.query("READ?")

    rm.close()

    #with open('D:/DC/!W/Prj/VSDC3-1/python/Tests/hpVoltage', 'wb') as f:
    with open('D:/DC/!W/Tests/2022_04_VSDC3/hpVoltage', 'wb') as f:
   # "D:\\DC\\!W\\Tests\\2022_04_VSDC3\\";
        f.write(msmt.encode('ascii'))

    return msmt

if __name__ == '__main__':
    print(main())