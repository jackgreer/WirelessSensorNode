# Filename: wsnDesktop.py
# Author: Srinivas Rambha, Jack Greer
# First accessed by me on: 15 Nov 2021
# Date last modified: 16 Nov 2021

# This software is largely based off an example on the www.daniweb.com programming forum written
# by user "srinivas.rambha", linked below:
# https://www.daniweb.com/programming/software-development/threads/457871/how-to-display-received-data-on-the-window
# The most significant changes I made were retaining data on the screen (rather than deleting it
# after 100 ms like the original program), and implementing a date/time display; however the
# most important structure comes from this tutorial, and I want to give the appropriate credit
# where it is due.

# Since Milestone 4, I have added a feature that displays a graphic on the screen related to the
# temperature of the thermistor.

import serial
import threading
import queue
import tkinter as tk
from PIL import ImageTk, Image
from enum import Enum
from datetime import datetime
import struct

global coldTempImage
global hotTempImage

com_port = 'COM6'
baudrate = 9600

# This is just for switching between the hot temperature image and the cold temperature image
class TemperatureState(Enum):
    HOT = 1
    COLD = 2

# This method instantiates a thread for the program to read incoming serial data (in my case,
# from the Bluetooth module connected to the Arduino). The multithreading is required because
# every program loop, we need to check for incoming data while also potentially refreshing the
# GUI; this is not possible using Tkinter's default main loop without the use of multithreading.
class SerialThread(threading.Thread):
    def __init__(self, _queue):
        threading.Thread.__init__(self)
        self._queue = _queue
    def run(self):
        arduinoSerial = serial.Serial(com_port,baudrate)
        while True:
            # If there is any data waiting in the serial port, turn it into a string and send it
            # to the queue.
            if arduinoSerial.inWaiting():
                string_temp_fahr = arduinoSerial.readline(arduinoSerial.inWaiting())
                self._queue.put(string_temp_fahr)

class App(tk.Tk):
    def __init__(self):
        tk.Tk.__init__(self)

        self.title('ECE 2804 - Wireless Sensor Node Desktop App')
        self.geometry("1200x600") # Blessed Resolution

        self.frame_description = tk.Frame(self, padx=2, pady=2)
        self.frame_description.grid(row=0, column=0)
        self.text_description = tk.Text(self.frame_description, wrap='word', font='TimesNewRoman 12',
                                        bg=self.cget('bg'), relief='flat')
        self.text_description.insert('end', 'Temperature (F): \n')
        self.text_description.insert(2.0, 'Data Last Received: ')
        tk.Label(self.frame_description, padx=5, pady=5).pack() # Attach description frame to main window
        self.text_description.pack()

        # Graphics which change in response to temperature
        self.cold_image = ImageTk.PhotoImage(Image.open("ice400.png"))
        self.hot_image = ImageTk.PhotoImage(Image.open("fire400.png"))
        self.temperatureState = TemperatureState.COLD
        self.label = tk.Label(self, image=self.cold_image)
        self.label.grid(row=0, column=1)


        # Initialize a queue for threading; such that the program can both refresh the GUI
        # and also check for serial input in COM8.
        self._queue = queue.Queue()
        thread = SerialThread(self._queue)
        thread.start()

        self.process_serial()

    def process_serial(self):
        # This method checks to see if there is anything waiting in the serial port; if so,
        # print the incoming data (in this case, the temperature in Fahrenheit) and refresh
        # the date displayed.
        while self._queue.qsize():
            try:
                newTemperature = self._queue.get()
                self.text_description.delete(1.17, 2.0)
                self.text_description.insert(1.17, newTemperature)

                self.text_description.delete(2.19, 'end')
                self.text_description.insert(2.19, datetime.now().strftime("%m/%d/%Y, %H:%M:%S"))
                # Cut off the /r/n at the end of the transmitted message, then convert to bytes
                # from string; then convert to int from bytes
                newTemp_int = int(bytes.decode(newTemperature.strip()))
                #formerly = bytes.decode(sneed)
                #chuck = int(formerly)
                #print(sneed)
                #print(chuck)

                if(newTemp_int <= 58 and self.temperatureState == TemperatureState.HOT):
                    self.temperatureState = TemperatureState.COLD
                    self.label['image'] = self.cold_image
                elif(newTemp_int > 58 and self.temperatureState == TemperatureState.COLD):
                    self.temperatureState = TemperatureState.HOT
                    self.label['image'] = self.hot_image

            except queue.Empty:
                pass
        
        # Wait to check for incoming data for 100ms. This can probably be refined, since right 
        # now, the Arduino only sends data every 30 seconds.
        self.after(100, self.process_serial)

    def open_graphic(self):
        coldTempImage = "ice400.png"
        img = ImageTk.PhotoImage(file = coldTempImage)
        label = tk.Label(self.frame_description, image=img)
        label.pack()

if __name__ == '__main__':
    app = App()
    app.mainloop()