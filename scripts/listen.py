import serial
import time

def listen_to_serial(port='/dev/ttyACM0', baudrate=115200):
    try:
        with serial.Serial(port, baudrate, timeout=1) as ser:
            print(f"Listening on {port} @ {baudrate} baud. Press Ctrl+C to exit.\n")
            time.sleep(1)  # Give some time for the connection to stabilize

            while True:
                if ser.in_waiting:
                    line = ser.readline().decode('utf-8', errors='ignore').strip()
                    if line:
                        print(line)
    except serial.SerialException as e:
        print(f"[Error] Could not open serial port {port}: {e}")
        print("Trying next port\n")
        if port != '/dev/ttyACM1' : 
            listen_to_serial(port='/dev/ttyACM1', baudrate=115200)
        else : 
            print("Run make debug to reset back to ACM0\n")
    except KeyboardInterrupt:
        print("\n[Stopped] Serial listening stopped by user.")

if __name__ == '__main__':
    listen_to_serial()

