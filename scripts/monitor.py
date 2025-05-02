#!/usr/bin/python3

'''
NOTE: WIP

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
     - after 1 second (50 cycles) calculate mean(P)
     - Now log this to the daily csv file 
    
     ideal final output:
     [date], U_rms, {Irms1: x, P1: x, Q1: x, S1: x, pf1: x}, {Irms2: y, P1: y, .., pf: y}, ..., {Irms4: z, P4: z, Q4: z, S4:z, pf4: z}
'''

import serial
import time

port = '/dev/ttyACM0'

ser = serial.Serial(port, 115200, timeout=1)
time.sleep(0.1) # wait 100 millis to stabalize

print(f"Listening on {port}. Press Ctrl+C to exit. \n")
time.sleep(1)

# reads every 20 ms, 50 cycles gives 1 miniute, average and log/print
cycles = 0
U_rms = 0


def calc_rms(samples):
    N = len(samples)
    return (sum(x**2 for x in samples) / N) ** 0.5

while True:
    try:
        byte_string = ser.readline()
        real_string = byte_string.decode('utf-8').strip()

        parts = real_string.split(':')
        if len(parts) != 2:
            continue

        ## ----- display all samples 
        values = [round(3.3*(float(x)/4096), 2) for x in parts[1].split(',') if x.strip() != '']
        # print(parts[0], ": ", ", ".join(str(x) for x in values))
        P = Q = S = 0
        if parts[0] == 'ch1':
            U_rms = calc_rms(values)
        else:
            I_rms = calc_rms(values)
        
        #if U_rms == 0:
            

        
        



        # # ------ display mean of sampels 
        # values = [3.3 * (float(x)/4098) for x in parts[1].split(',') if x.strip() != '']
        # mean_value = sum(values)/len(values)
        # if parts[0] == 'ch1':
        #     print(parts[0], ": ", round(mean_value, 2), "V")
        # else:
        #     print(parts[0], ": ", round(mean_value, 2), "A")
            
        if not ser.in_waiting: 
            print()
    
    except KeyboardInterrupt:
        print("Exiting...\n")
        ser.close()
        break
    
