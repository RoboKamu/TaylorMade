#!/usr/bin/python3

'''
NOTE: This is a test file for the class int the power_monitor file

This scripts handles:
 - Reading ADC values over Serial port
 - Calculating RMS values 
 - Calulating Power 
 - Logging the calculations

 Idea:
    Since wainting until a second has passed then handling the data would be very inefficient this approach is discontinued.
    A thought is to do real-time data analysis every 20ms, quick calculations on a peiod, save it and wait till the next one.
    Once a second has passed (50 cycles) then the mean value of all relevant real-time calculations is performed.
    example:
     - every 20ms calculate P (1 cycle)
     - after half second (25 cycles) calculate mean(P)
     - Now log this to the daily csv file 
    
     ideal final output:
     [date], U_rms, {Irms1: x, P1: x, Q1: x, S1: x, pf1: x}, {Irms2: y, P1: y, .., pf: y}, ..., {Irms4: z, P4: z, Q4: z, S4:z, pf4: z}
     (this could also be separated into one csv file per port, easier to parse and not excessive long lines)
'''

# This example is for using power_monitor class, logs to folder aswell as prints to terminal

import time
from threading import Thread
from classes.power_monitor import PowerMonitor

port = '/dev/ttyACM0'

print(f"Listening on {port}. Press Ctrl+C to exit. \n")
# time.sleep(0.9)

monitor = PowerMonitor()

if __name__ == '__main__':
    t = Thread(target=monitor.run, daemon=True) 
    t.start()
    while True:
        time.sleep(1)
