> [!NOTE]
> Main prototype complete but a revision of the web software as well as the hardware is in progress.

### Firmware

Currently the firmware development is complete. One update that may occur is that the amount of periods/cycles being sampled increases, so going from sampling just one period (20 samples) to maybe 2 periods (40 samples) for more accurate RMS calculations and edge cases. 

Firmware overview:
 - A timer is used to trigger an TRGO interrupt every ms 
 - This triggers the ADC to start conversion of 5 channels 
 - The ADC starts then converting all channels once with 20 us / channel (read more in ADC/ folder)
 - ADC stops once 5 channels converted and waits for the next timer interrupt flag
 - Every conversion is stored in a DMA buffer
 - Once DMA buffer has reached 100 samples (20 per channel) an interrupts trigger
 - The DMA interrupt handler saves the buffer values into arrays for each channel and notes USB buffer as ready
 - Lastly the superloop (main.c) sees the USB buffer as ready and sends readings over Serial port 

### Software

The website is still being developed. Getting the flask server running, controlling raspberry pi GPIO, and data analysis works but some frontend features like "history" and WSGI server is in development. 

While it is in development, some small scrips have been created to just read the Serial port readings

**NOTE: I have developed the firmware and python scripts in a GNU/Linux enviorment so if new firmware is desired then the Makefile has to be swapped and the python script port variable has to be changed**

Changing the port variable to read Serial port based on OS:

    port = '' # change this to serial port location variable
    port = '/dev/ttyACM0'           # for linux (debian)
    # port = /dev/tty.usbmodem1101  # ex. for MACOS - found via: ls /dev/tty.usb*
    # port = 'COM3'                 # ex. Windows - found via Device Manager -> Ports (COM & LPT) -> find something like "USB Serial Device (COM3)" (?)

In `app.py`, the application uses pythons `threading.Thread` to read serial monitor in parallel with flask web server. While Python (3) does not have *true* parallell multi threading because of GIL, Gloabl Interpreter Lock, this design is not affected because both threads are primarly I/O bound. The Flask server spends most of its time idle, waiting for HTTP requests, so GIL gets released and another thread can continue execution. As a result, the two threads cooperate efficiently, providing responsive real-time behavior and clean data handling. 

### Hardware
(this part might need to be updated) ACS770xCB 

A PCB has been designed and printed for the power electronics.

components include:

 - 4 current sensors ( ACS770xCB ) 
 - 2 opAmps -> current to 0-3.3 V for ADC (conv. to ± 16 Amps via software)
 - 4 Relays
 - some resistors for voltage division of input source ex. AC mains (0-3.3V to ADC)
 - 1 Microcontroller Unit (MCU) - GIGADEVICE GD32VF103
 - 1 Single board computer - Raspberry pi 4 with 2 gb RAM ( tobe replaced )

The MCU is placed on a small prototype board, connected with pin headers. Each input pin A1-A5 has a pull down resistor to a ground buss. 
