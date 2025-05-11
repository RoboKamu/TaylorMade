import numpy as np
from datetime import datetime
import csv
import os
import serial
import time     # to display time taken on calculations

class PowerMonitor:
    def __init__(self):
        self.ROUND_NUM = 6
        self.CYCLES = 50  # MCU sends data every 20ms, 50 cycles = 1 second
        self.cycle_counter = 0
        self.u_samples = np.array([])
        self.i = 0
        self.port = '/dev/ttyACM0'
        self.ser = serial.Serial(self.port, 115200, timeout=1)

        # P = u*i in real time, then the mean is calculated after enough samples gathered
        # Other power calculations only need to be performed once
        # Separate raw (accumulating lists) from results (final values)
        self.channel_data_raw = {
            "ch1": {"Urms": []},
            "ch2": {"Irms": [], "P": []},
            "ch3": {"Irms": [], "P": []},
            "ch4": {"Irms": [], "P": []},
            "ch5": {"Irms": [], "P": []}
        }

        self.channel_data_result = {
            "ch1": {"Urms": 0},
            "ch2": {"Irms": 0, "P": 0, "Q": 0, "S": 0, "PF": 0},
            "ch3": {"Irms": 0, "P": 0, "Q": 0, "S": 0, "PF": 0},
            "ch4": {"Irms": 0, "P": 0, "Q": 0, "S": 0, "PF": 0},
            "ch5": {"Irms": 0, "P": 0, "Q": 0, "S": 0, "PF": 0}
        }

        os.makedirs("logs", exist_ok=True)

    def calc_rms(self, samples):
        return np.sqrt(np.mean(np.square(samples)))

    def calc_active_power(self, u, i):
        u = np.asarray(u)
        i = np.asarray(i)
        return np.mean(np.multiply(u, i))

    def parse_line(self, line):
        parts = line.split(':')
        if len(parts) != 2:
            return None, np.array([])
        try:
            if parts[0] != 'ch1':
                data = np.array([ (3.3*float(x)/4095) for x in parts[1].split(',') if x.strip() != ''])
                # convert voltage reading from current sensor: 100mv = 1A
                data = np.divide(data, 0.1)
            else:
                data = np.array([[(float(x)) for x in parts[1].split(',') if x.strip() != '']])
                tmp = np.mean(data)
                data = [1625*(3.3*(x-tmp)/4096) for x in data]
        except ValueError:
            return None, np.array([])
        
        return parts[0], data

    def calc_power(self, ch):
        if ch == "ch1":
            self.channel_data_result[ch]["Urms"] = np.sum(self.channel_data_raw[ch]["Urms"]) / len(self.channel_data_raw[ch]["Urms"])
            self.channel_data_result[ch]["Urms"] = round(self.channel_data_result, self.ROUND_NUM)
            return

        Irms_array = np.asarray(self.channel_data_raw[ch]["Irms"])
        P_array = np.asarray(self.channel_data_raw[ch]["P"])
        Urms_val = self.channel_data_result["ch1"]["Urms"]

        Irms_m = np.sum(Irms_array) / len(Irms_array)
        P_m = np.sum(P_array) / len(P_array)
        S = Urms_val * Irms_m
        Q_mag = np.sqrt(abs(S**2 - P_m**2))
        PF = P_m / S if S != 0 else None

        self.channel_data_result[ch].update({
            "Irms": round(Irms_m, self.ROUND_NUM),
            "P": round(P_m, self.ROUND_NUM),
            "S": round(S, self.ROUND_NUM),
            "Q": round(Q_mag, self.ROUND_NUM),
            "PF": round(PF, self.ROUND_NUM)
        })

    def log_to_csv(self):
        timestamp = datetime.now().replace(microsecond=0)
        date_path = timestamp.strftime("logs/%Y/%m/%d")
        os.makedirs(date_path, exist_ok=True)

        for ch, vals in self.channel_data_result.items():
            filename = os.path.join(date_path, f"{ch}.csv")
            file_exists = os.path.isfile(filename)
            with open(filename, 'a', newline='') as f:
                writer = csv.writer(f)
                if not file_exists:
                    writer.writerow(["timestamp"] + list(vals.keys()))
                writer.writerow([timestamp] + [vals.get(k, "") for k in vals])

    def clear_raw(self):
        for ch in self.channel_data_raw:
            for key in self.channel_data_raw[ch]:
                self.channel_data_raw[ch][key].clear()
        for ch in self.channel_data_result:
            for key in self.channel_data_result[ch]:
                self.channel_data_result[ch][key] = 0

    def display_monitor(self):
        for ch, vals in self.channel_data_result.items():
            print(f"{ch}: {vals}")

    def run(self):
        start_timer = time.time()
        while True:
            try:
                byte_string = self.ser.readline()
                try:
                    line = byte_string.decode('utf-8').strip()
                except UnicodeDecodeError:
                    continue

                ch, values = self.parse_line(line)
                if ch is None or ch not in self.channel_data_raw:
                    continue

                if ch == 'ch1':
                    if len(values) != 20: continue
                    self.u_samples = values
                    self.channel_data_raw[ch]["Urms"].append(self.calc_rms(values))
                    continue
                
                if len(values) != 20 or len(self.u_samples) != 20:
                    continue    # skip if either set incomplete

                self.channel_data_raw[ch]["Irms"].append(self.calc_rms(values))
                self.channel_data_raw[ch]["P"].append(self.calc_active_power(self.u_samples, values))

                if ch == 'ch5':
                    self.cycle_counter += 1
                    if self.cycle_counter == self.CYCLES:
                        self.cycle_counter = 0
                        for ch_key in self.channel_data_raw:
                            self.calc_power(ch_key)
                        
                        self.log_to_csv()
                        self.display_monitor() # debug to terminal
                        print(time.time()-start_timer)
                        start_timer = time.time()
                        self.clear_raw()

            except KeyboardInterrupt:
                print("KEY_ITRPT..Exiting..\n")
                self.ser.close()
                break;