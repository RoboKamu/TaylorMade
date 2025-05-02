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



from flask import Flask, request, jsonify, render_template, send_from_directory, redirect, url_for
import csv
import os
import atexit
from datetime import datetime
#import RPi.GPIO as GPIO

app = Flask(__name__)

# Set up GPIO
GPIO.setmode(GPIO.BCM)
output_pins = {
    1: 17,
    2: 18,
    3: 27,
    4: 22,
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
    return render_template("today.html")

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
    return f"Port {port} turned ON"

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

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)