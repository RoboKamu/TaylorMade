#!/usr/bin/python3

import serial
import time

port = '/dev/ttyACM0'

ser = serial.Serial(port, 115200, timeout=1)
time.sleep(0.1) # wait 100 millis to stabalize

print(f"Listening on {port}. Press Ctrl+C to exit. \n")
time.sleep(1)

while True:
	byte_string = ser.readline()
	print(f"{byte_string.decode('utf-8').strip()} ", end='\n')
