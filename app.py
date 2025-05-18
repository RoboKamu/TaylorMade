# ----- ONLY WHEN NOT RUNNING ON PI -----
try:
    import RPi.GPIO as GPIO 
except (ImportError, RuntimeError):
    # Mock GPIO for testing without Raspberry Pi
    class GPIO:
        BCM = "BCM"
        OUT = "OUT"
        HIGH = "HIGH"
        LOW = "LOW"

        @staticmethod
        def setmode(mode):
            print(f"GPIO setmode({mode})")

        @staticmethod
        def setup(pin, mode):
            print(f"GPIO setup(pin={pin}, mode={mode})")

        @staticmethod
        def output(pin, state):
            print(f"GPIO output(pin={pin}, state={state})")

        @staticmethod
        def cleanup():
            print("GPIO cleanup()")
# ---------- END ----------

import base64
from datetime import datetime
from io import BytesIO
import os
import pandas as pd
from flask import Flask, request, jsonify, render_template, redirect, url_for
import atexit

from matplotlib.figure import Figure
import matplotlib.dates as mdates
#import RPi.GPIO as GPIO

app = Flask(__name__)

# Set up GPIO
GPIO.setmode(GPIO.BCM)
output_pins = {
    1: 27,
    2: 22,
    3: 23,
    4: 24,
}

# Set up all pins as outputs
for pin in output_pins.values():
    GPIO.setup(pin, GPIO.OUT)
    GPIO.output(pin, GPIO.LOW)

# Global button states
button_states = {
    1: False,
    2: False,
    3: False,
    4: False,
}

@app.route('/')
def home():
    return render_template('index.html', button_states=button_states, output_pins=output_pins)

@app.route("/today.html")
def today_view():
    timestamp = datetime.now().replace(microsecond=0)
    date_path = timestamp.strftime("logs/%Y/%m/%d")

    measurements = ["Irms", "P", "Q", "S", "PF"]
    chart_images = {}
    

    try:
        channel_files = [f for f in os.listdir(date_path) if f.endswith(".csv")]

        # read CSV data
        # Structure: measurements -> {channel: (x, y)}
        data_by_measurement = {m : {} for m in measurements}
        for file in channel_files:
            ch_name = os.path.splitext(file)[0] # ex. "ch2"
            filepath = os.path.join(date_path, file)
            df = pd.read_csv(filepath, parse_dates=["timestamp"])
            # remove the date from timestamp    
            # df["timestamp"] = pd.to_datetime(df["timestamp"])
            # df["timestamp"] = df["timestamp"].dt.strftime("%H:%M:%S")

            for m in measurements:
                if m in df.columns:
                    data_by_measurement[m][ch_name] = (df["timestamp"], df[m])
        
        for m, ch_data in data_by_measurement.items():
            # create figure without pyplot
            fig = Figure()
            ax = fig.subplots()

            for ch, (x, y) in ch_data.items():
                ax.plot(x, y, label=ch)
            
            # dynamic x axis 
            # locator = mdates.AutoDateLocator()
            # formatter = mdates.AutoDateFormatter(locator)

            # ax.xaxis.set_major_locator(locator)
            ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M:%S'))
            fig.autofmt_xdate()

            ax.set_title(f"Chart {m}")
            ax.legend()
            fig.tight_layout()

            # convert plot to PNG image as base64
            buf = BytesIO()
            fig.savefig(buf, format="png")
            buf.seek(0)
            chart_data = base64.b64encode(buf.getvalue()).decode("ascii")
            chart_images[m] = chart_data

    except Exception as e:
        print(f"Error: {e}")
        # failed to parse csv 
        chart_images = {}

    return render_template("today.html", charts=chart_images)

@app.route("/history.html")
def history_view():
    return render_template("history.html")


@app.route('/toggle/<int:button_id>')
def toggle(button_id):
    if button_id in button_states:
        button_states[button_id] = not button_states[button_id]
        GPIO.output(output_pins[button_id], GPIO.HIGH if button_states[button_id] else GPIO.LOW)
    return redirect(url_for('home'))

@app.route("/ON")
def turn_on():
    port = int(request.args.get("port"))
    button_states[port] = True
    GPIO.output(output_pins[port], GPIO.HIGH)
#    return f"Port {port} turned ON"
    return '', 204

@app.route("/OFF")
def turn_off():
    port = int(request.args.get("port"))
    button_states[port] = False
    GPIO.output(output_pins[port], GPIO.LOW)
    return f"Port {port} turned OFF"

@app.route("/portstatus")
def port_status():
    return jsonify(button_states)


@atexit.register
def cleanup():
    GPIO.cleanup()

# configure Thread for power moonitor
from classes.power_monitor import PowerMonitor
from threading import Thread

monitor = PowerMonitor()

if __name__ == "__main__":
    t = Thread(target=monitor.run, daemon=True)
    t.start()
    app.run(host="0.0.0.0", port=5000)  # change host to router IP
