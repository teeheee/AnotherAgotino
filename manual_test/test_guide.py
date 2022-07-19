import serial
import time
from astropy import units as u
from astropy.coordinates import SkyCoord
ser = serial.Serial('COM9',9600)  # open serial port

c_model = SkyCoord(ra=0*u.degree, dec=90*u.degree, frame='icrs')

def init():
    print(ser.read_until(expected=b"ready"))
    time.sleep(1)
    ser.reset_input_buffer()

def write_and_read(ser, text, until=b"#"): 
    print("send: ",text)
    ser.reset_input_buffer()
    ser.write(text)
    if until != None:
        x = (ser.read_until(until)) 
        print("recv: ",x)
        return x
    else:
        return None

def status():
    x = write_and_read(ser,b":Z#",until=b"###")
    print(x.decode("utf-8"))

init()
for i in range(10):
    for i in [b"n0500",b"s0500",b"w0500",b"e0500"]:
        print("============MOVE %s============="%i)
        write_and_read(ser, b":M"+i+b"#", until=None)
        status()
        status()
        status()
        status()
        time.sleep(4)
ser.close()  