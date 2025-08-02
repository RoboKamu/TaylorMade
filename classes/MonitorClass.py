import numpy as np
from datetime import datetime
import csv
import os
import time     # to display time taken on calculations

class PowerMonitor:
    def __init__(self):
        self.ROUND_NUM = 3
        self.CYCLES = 50  # MCU sends data every 20ms, 50 cycles = 1 second
        self.cycle_counter = 0
        self.u_samples = np.array([])

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

    def calc_rms(self, samples, ch):
        # flatten 2d array to 1d in order to perform DFT
        flattened = [value for row in samples for value in row]
        y = np.asarray(flattened)
        X = np.fft.fft(y) # compute fft 
        N = len(X)
        fs = 1/N
        freq_axis = np.fft.fftfreq(N, d=1/fs)
        # remove bins under threshold
        threshold = 0.03 if ch != "ch1" else 5.0
        X_filtered = X.copy()
        X_filtered[np.abs(X_filtered) < threshold * (N//2+1)] = 0
        # band pass filter
        low_cutoff = 1 # Hz
        high_cutoff = 455 # Hz
        X_filtered[(np.abs(freq_axis) < low_cutoff) | (np.abs(freq_axis) > high_cutoff)] = 0
        y_recon = np.fft.ifft(X_filtered).real  # Reconstructed signal (same as input with noise)

        return np.sqrt(np.mean(np.square(y_recon)))

    def calc_active_power(self, u, i):
        # P = u*i in real time, then the mean is calculated after enough samples gathered
        u = np.asarray(u)
        i = np.asarray(i)
        return np.mean(np.multiply(u, i))

    def parse_values(self, ch, values):
        try:
            if ch == 'ch1':
                data = np.asarray([ (3.3*float(x)/4095) for x in values ])
                data = np.divide(data, 0.1)
            else:
                data = np.asarray(values)
                tmp = np.mean(data)
                data = 1617*(3.3*(data-tmp)/4095)
        except ValueError:
            return np.asarray([])

        return data

    def calc_power(self, ch):
        if ch == "ch1":
            Urms_val = self.calc_rms(self.channel_data_raw[ch]["Urms"], ch)
            self.channel_data_result[ch]["Urms"] = round(Urms_val, 1)
            return

        Irms_array = np.asarray(self.channel_data_raw[ch]["Irms"])
        P_array = np.asarray(self.channel_data_raw[ch]["P"])
        Urms_val = self.channel_data_result["ch1"]["Urms"]

        Irms_m = self.calc_rms(Irms_array, ch)
        if Irms_m == 0.0:
            self.channel_data_result[ch].update({
                "Irms": 0.0,
                "P": 0.0,
                "S": 0.0,
                "Q": 0.0,
                "PF": 0
            })  
        else:  
            P_m = np.sum(P_array) / len(P_array)
            S = Urms_val * Irms_m
            Q_mag = np.sqrt(abs(S**2 - P_m**2))
            PF = P_m / S if S != 0 else 0.0

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

    def run(self, ch, values):
        start_timer = time.time()
        try:
            values = self.parse_values(ch, values)

            if ch == 'ch1':
                if len(values) != 100: 
                    return
                self.u_samples = values
                self.channel_data_raw[ch]["Urms"].append(values)
                return
            
            if len(values) != 100 or len(self.u_samples) != 100:
                return    # skip if either set incomplete

            self.channel_data_raw[ch]["Irms"].append(values)
            self.channel_data_raw[ch]["P"].append(self.calc_active_power(self.u_samples, values*(-1)))

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
