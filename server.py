#!/usr/bin/python3

from flask import Flask, request, jsonify, send_file
import csv
import os
from datetime import datetime


app = Flask(__name__)

CSV_FILE = "log.csv"

# Define ports – adjust to your GPIO numbers
def init_csv():
    if not os.path.exists(CSV_FILE):
        with open(CSV_FILE, "w", newline="") as f:
            writer = csv.writer(f)
            writer.writerow(["timestamp", "voltage", "current", "power"])

init_csv()

# Simulated status for outlets
PORTS = {
    "1": False,
    "2": False,
    "3": False,
    "4": False,
}

@app.route("/")
def home():
    return send_file("index.html")

@app.route("/today.html")
def today_view():
    return send_file("today.html")

@app.route("/history.html")
def history_view():
    return send_file("history.html")


@app.route("/ON")
def turn_on():
    port = request.args.get("port")
    if port in PORTS:
        PORTS[port] = True
        return f"Port {port} ON"
    return "Invalid port", 400

@app.route("/OFF")
def turn_off():
    port = request.args.get("port")
    if port in PORTS:
        PORTS[port] = False
        return f"Port {port} OFF"
    return "Invalid port", 400

@app.route("/portstatus")
def port_status():
    return jsonify(PORTS)

@app.route("/log", methods=["POST"])
def log_data():
    data = request.get_json()
    with open(CSV_FILE, "a", newline="") as f:
        writer = csv.writer(f)
        writer.writerow([
            datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            data.get("voltage"),
            data.get("current"),
            data.get("power")
        ])
    return "Measurement data saved!", 200

@app.route("/data", methods=["GET"])
def get_data():
    with open(CSV_FILE, newline="") as f:
        reader = csv.DictReader(f)
        return jsonify(list(reader))

if __name__ == '__main__':
    app.run(host="0.0.0.0", port=5000)
 