#!/usr/bin/env python3

from flask import Flask
from waitress import serve
from server import app
import os

import pigpio
import spidev

import time
from collections import deque
from classes.MonitorClass import PowerMonitor

from multiprocessing import Process, Queue

def find_sync(raw):
    for i in range(len(raw) - 1):
        if raw[i] == 0x01 and raw[i+1] == 0xA4:
            return i
    return -1

def add_channel_data(ch1, ch2, ch3, ch4, ch5, val):
    if val == 0x01A4:
        return

    match (val >> 12):
        case 1:
            ch1.append(val & 0x0FFF)
        case 2:
            ch2.append(val & 0x0FFF)
        case 3:
            ch3.append(val & 0x0FFF)
        case 4:
            ch4.append(val & 0x0FFF)
        case 5:
            ch5.append(val & 0x0FFF)
        case _:
            print("DBG --> HERE WEIRD NIBBLE: {}, val: {} = {}\n".format(val>>12, hex(val), val))
            time.sleep(0.0001)
            return -1

def calc_process(queue, monitor):
    while True:
        if queue.empty():
            time.sleep(0.000010) # wait 10 us 
            continue

        data = queue.get()

        # reconstruct to 16 bits integers
        result = [data[i] << 8 | data[i+1] for i in range(0, len(data), 2)]
        #print(bytesToHex(result))
        if len(result) < 501:
            print("SOMEHTING DROPPED") 
            time.sleep(1)
        
        ch1 = []
        ch2 = []
        ch3 = []
        ch4 = []
        ch5 = []

        for val in result:
            err = add_channel_data(ch1, ch2, ch3, ch4, ch5, val)
            if err:
                continue

        ch_arr = { "ch1": ch1, "ch2": ch2, "ch3": ch3, "ch4": ch4, "ch5": ch5 }
        
        for ch, value in ch_arr.items():
            monitor.run(ch, value)

def cbf_gpio(gpio, level, tick):
    time.sleep(0.000010)  # 10 us delay
    #print("GPIO {} is {}".format(gpio, level))
    
    # duplex read SPI (8 bit words)
    raw_bytes = Spi.xfer([0x00] * 1002)    

    # shift to start bit 
    sync_idx = find_sync(raw_bytes)
    if (sync_idx == -1):
        print("sync not found, discard... " )
        return
    uint8_data = deque(raw_bytes)
    uint8_data.rotate(-sync_idx)
    queue.put(uint8_data)

def setup_gpio(GPIO: int):
    if not Pi.connected:
       exit()

    # enable pull down on the GPIO
    Pi.set_pull_up_down(GPIO, pigpio.PUD_DOWN)
    # define a toggle interrupt on the GPIO 
    Pi.callback(GPIO, pigpio.EITHER_EDGE, cbf_gpio) 

def spi_init_params(Spi):
    Spi.max_speed_hz = 5_120_000
    Spi.mode = 0b11
#    os.nice() # prioritize SPI over server

def bytesToHex(data):
    return ''.join(["0x%04X " % x for x in data]).strip()


if __name__ == "__main__":
    monitor = PowerMonitor()
    queue = Queue()
    p1 = Process(target=calc_process, args=(queue,monitor,))
    p1.start()

    try:
        Pi = pigpio.pi()
        Spi = spidev.SpiDev()

        Spi.open(0, 0)
        spi_init_params(Spi)
        Spi.xfer([0x00] * 1002) # garbage first pull
        setup_gpio(4)
        
        print("Server is live at: ")
        os.system('hostname -I')
        serve(app, listen='*:8080', threads=1)
        print("...server closed\n")

    finally:
        print("closing...\n")
        Spi.close()
        Pi.stop()

