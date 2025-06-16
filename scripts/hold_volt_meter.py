import serial
import time

port = '/dev/ttyACM0'

ser = serial.Serial(port, 115200, timeout=1)
time.sleep(0.1) # wait 100 millis to stabalize

print(f"Listening on {port}. Press Ctrl+C to exit. \n")
time.sleep(1)

list = []

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
            values = [(float(x)) for x in parts[1].split(',') if x.strip() != '']
            temp = 0
            temp = sum(values)/len(values)
            #for val  in parts[1]:
            #    mean += val
            #mean /= len(parts[1])
            values = [round((1625*(3.3*(x-temp)/4096)), 3) for x in values]

    except:
        pass
    #if parts[0] == 'ch1':
    #    try:
    #        #print(parts[0], "[V]: ", ", ".join(str(x) for x in values))
    #    except NameError:
    #        print("List is not defined.")
    #        pass
    #else:
        # current sensor has a 100mv = 1A resolution 
        # so values in volts => curent value = (reading value) / 0.1
        #print(parts[0], "[A]: ", ", ".join(str(round(x/0.1, 2)) for x in values))
    if parts[0] == "ch4":
        list.append([round(x/0.1, 3) for x in values])
        if len(list) == 50:
            print(list)
            list = []
    # ----- END ------

    # ---- display mean of sampels ----- 
    # values = [3.3 * (float(x)/4098) for x in parts[1].split(',') if x.strip() != '']
    # mean_value = sum(values)/len(values)
    # if parts[0] == 'ch1':
    #     print(parts[0], ": ", round(mean_value, 2), "V")
    # else:
    #     print(parts[0], ": ", round(mean_value, 2), "V")
    # ---- END -----

    #if not ser.in_waiting: 
    #   print()


