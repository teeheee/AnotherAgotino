import serial
import time
ser = serial.Serial('COM9',9600)  # open serial port

def write_and_read(ser, text):
    print("send ", text)
    ser.reset_input_buffer()
    ser.write(text)
    x = (ser.read_until(b"#")) 
    print(x)
    return x  

print(ser.read_until(expected=b"ready"))
time.sleep(2)
start = time.time()
while time.time()-start < 60:
     x = write_and_read(ser,b':GR#') 
     time.sleep(1)

ser.close()  