#!/usr/bin/python3

'''
This script listens to the Serial port and prints raw ADC data

NOTE: this is just a test script, not meant for final project therefore no error handling
'''

import serial
import time

port = '/dev/ttyACM0'

ser = serial.Serial(port, 115200, timeout=1)
time.sleep(0.1) # wait 100 millis to stabalize

print(f"Listening on {port}. Press Ctrl+C to exit. \n")
time.sleep(1)

while True:
	# ser.reset_input_buffer()
	byte_string = ser.read_all()
	print(f"{byte_string.decode('utf-8').strip()} ", end='\n')
	time.sleep(0.1)