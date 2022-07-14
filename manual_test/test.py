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
    x = (ser.read_until(until)) 
    print("recv: ",x)
    return x

def status():
    x = write_and_read(ser,b":Z#",until=b"###")
    print(x.decode("utf-8"))

def read_coordinates():
    ra_str = write_and_read(ser,b':GR#') 
    dec_str = write_and_read(ser,b':GD#')
    ra_dig = ra_str.strip(b"#").split(b":")
    dec_dig_tmp = dec_str.strip(b"#").split(b"*")
    dec_dig = [dec_dig_tmp[0]] + dec_dig_tmp[1].split(b":")
    ra_h = float(ra_dig[0])
    ra_m = float(ra_dig[1])
    ra_s = float(ra_dig[2])
    dec_deg = float(dec_dig[0])
    dec_m = float(dec_dig[1])
    dec_s = float(dec_dig[2])
    ra_deg = ra_h*360.0/24.0+ra_m/60+ra_s/3600
    dec_deg = dec_deg+dec_m/60+dec_s/3600
    c_recv = SkyCoord(ra=ra_deg*u.degree, dec=dec_deg*u.degree, frame='icrs')
    return c_recv

def verify_coordinates():
    c_recv = read_coordinates()
    if(str(c_recv.separation(c_model)) != "0d00m00s"):
        print("error: ",str(c_recv.separation(c_model)) )
        print("recv", c_recv)
        print("model", c_model)
        return

def wait_until_match():
    old_diff = None
    while True:
        c_recv = read_coordinates()
        diff = c_recv.separation(c_model)
        print(c_recv)
        print(c_model)
        if diff == old_diff:
            verify_coordinates()
            return 
        old_diff = diff
        status()
        time.sleep(10)

            


def slewto(ra_deg, dec_deg):
    global c_model
    c_model = SkyCoord(ra=ra_deg*u.degree, dec=dec_deg*u.degree, frame='icrs')
    hh = int(ra_deg)
    mm = int((ra_deg-hh)*60)
    ss = int((ra_deg-hh-mm/60)*3600)
    ra_str = b":Sr%02d:%02d:%02d#"%(
        hh,
        mm,
        ss
        )
    if dec_deg > 0:
        sign = b"+"
    else:
        sign = b"-"    
    hh = int(dec_deg)
    mm = int((dec_deg-hh)*60)
    ss = int((dec_deg-hh-mm/60)*3600)
    dec_str = b":Sd%s%02d*%02d:%02d#"%(
        sign,
        hh,
        mm,
        ss
        )
    write_and_read(ser,ra_str,until=b"1")
    write_and_read(ser,dec_str,until=b"1")
    write_and_read(ser,b":MS#",until=b"0")

init()
verify_coordinates()
status()
slewto(45,45)
wait_until_match()
verify_coordinates()
ser.close()  