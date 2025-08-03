# TaylorMade - General purpose 4 load power monitor 

![Taylormade-prototype-4-sockets](readme-files/taylormade-prototype.png)
TaylorMade prototype at rest 

3D model made by collaborator: Adam Younes

### Firmware

Current update now samples 5 times as many samples with a total of 100 samples per period compared to the old 20 samples per period. 

Firmware overview:
 - A timer is used to trigger an TRGO interrupt every 200 us 
 - This triggers the ADC to start conversion of 5 channels 
 - The ADC starts then converting all channels once with 20 us / channel (read more in ADC/ folder)
 - ADC stops once 5 channels converted and waits for the next timer interrupt flag
 - Every conversion is stored in a DMA buffer
 - Once DMA buffer has reached 500 samples (100 per channel) an interrupt triggers
 - The DMA ISR converts DMA buffer to byte sized array with a channel identification nibble and sets GPIO ON 
 - RPi has a change-of-state interrupt on the GPIO and sends SPI request
 - Each request triggers an interrupt on the MCU for SPI and the ISR transfers one byte to the RPi
 - For lack of FIFO on SPI slave, shift array to start flag on the RPi
 - Lastly the RPi splits into respective channel buffer based on most significant nibble on reconstructed 16-bit array and initiates calculations 

### Software

Website complete with the exception of the "history" tab. Now uses Waitress, WSGI, instead of running flask debug server. 

**NOTE: I have developed the firmware and python scripts in a GNU/Linux enviorment so if new firmware is desired then the Makefile has to be swapped**

The software not uses multiprocessing instead of multithreading becuase of more demanding calculations, as well as blocking SPI reads. One improvement that can be done is to generate graphs from its own process, a nasty work around is to continue updating data but only update graph once refreshed. 

![dft-on-ADC](readme-files/dft-showcase.png)

Example of the results/output of the filtering process of the ADC values. This is with a load. 

### Hardware
![hardwareschematic](readme-files/hardware.png)

Made by collaborator: Skuttispower

The MCU and RPi is placed on a small prototype board, connected with pin headers. Each input pin on ADC has a pull down resistor to a ground buss. 

# Showcase

https://github.com/user-attachments/assets/e7613a2c-53ce-4c46-9b37-75e01b1198a9

This is a compressed demo. First, the user turn on 4 device. Then, the user analyzes the graphs showing voltage, active/reactive/apparent power as well as power factor on the site. Lastly, the user turn off all devices and show all of the power graphs going towards zero. Left side of monitor show a debug terminal displaying the measured power values in clear numbers.  
