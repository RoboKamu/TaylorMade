#!/usr/bin/python3

'''
This sccripts transforms raw ADC reading from Serial to voltage rating.
This is used to convert the  ADC values from a source to voltage 
and from a sensor to current.

NOTE: this is just a test script, not meant for final project therefore no error handling and may fail
'''


import serial
import time

port = '/dev/ttyACM0'

ser = serial.Serial(port, 115200, timeout=1)
time.sleep(0.1) # wait 100 millis to stabalize

print(f"Listening on {port}. Press Ctrl+C to exit. \n")
time.sleep(1)

while True:
    byte_string = ser.readline()
    real_string = byte_string.decode('utf-8').strip()

    parts = real_string.split(':')
    if len(parts) != 2:
        continue

    ## ----- display all samples ----- 
    try: 
        if parts[0] != 'ch1':
            values = [(3.3*float(x)/4096) for x in parts[1].split(',') if x.strip() != '']
        else: 
            print('Hello world')
            values = [(float(x)) for x in parts[1].split(',') if x.strip() != '']
            print('Hello world 2')
            temp = 0
            temp = sum(values)/len(values)
            #for val  in parts[1]:
            #    mean += val
            #mean /= len(parts[1])
            values = [round((1625*(3.3*(x-temp)/4096)), 3) for x in values]

#            mean = 0
 #           for val in values:
  #              mean += val
   #         mean /= len(values)
    #        values = [(x-mean) for x in values]
    except:
        pass
    if parts[0] == 'ch1':
        try:
            print(parts[0], "[V]: ", ", ".join(str(x) for x in values))
        except NameError:
            print("List is not defined.")
            pass
    else:
        # current sensor has a 100mv = 1A resolution 
        # so values in volts => curent value = (reading value) / 0.1
        print(parts[0], "[A]: ", ", ".join(str(round(x/0.1, 2)) for x in values))
    
    # ----- END ------

    # ---- display mean of sampels ----- 
    # values = [3.3 * (float(x)/4098) for x in parts[1].split(',') if x.strip() != '']
    # mean_value = sum(values)/len(values)
    # if parts[0] == 'ch1':
    #     print(parts[0], ": ", round(mean_value, 2), "V")
    # else:
    #     print(parts[0], ": ", round(mean_value, 2), "V")
    # ---- END -----

    if not ser.in_waiting: 
       print()

   
