"""
-----------------------------------------------------------------------------------------
Made by Filip Stehlik
-backend
lol
PROTO DOCUMENTATION 
Simple script,
after handshake, one way arduino communication, pulling data from arduino
code is pretty shit, i know
NOTES:
    https://stackoverflow.com/questions/19908167/reading-serial-data-in-realtime-in-python
    Speeding up readline if needed

    Period choice after handshake, move loopstartflag after setting it - Ready to implement

TO DO:
Try to optimize the animation. DONE
rewrite ongoing to event

Bug testing
Clean up code

Space for thoughts:
FIX garbage data bug
SOLVED-need to optimize (serial buff overflow)
SOLVED BY OPTIMAZING PLOTTING

-----------------------------------------------------------------------------------------
"""
import serial
import threading
import time
import os
from sys import exit
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import random
import csv
import datetime
import tkinter as tk
import numpy as np

#shared_vars-----------------------------------------------------------------------------
class SharedState:
    def __init__(self):
        self.arr1 = []
        self.arr2 = []
        self.graphvar1 = 0
        self.graphvar2 = 0
        self.ongoing1 = False
        self.recording = False
        self.baud = 500000
        self.mode = 0
        self.serialname1 = 0
        self.serialname2 = 0
        self.HowmuchData = 0

        # values for first graph
        self.x_vals = []
        self.y_vals = []
        # values for second graph
        self.y_vals2 = []

        self.startmes = (b'5\x0A')
        self.settingsmes = (b'2\x0A')
        self.handshakemes = (b'8\x0A')
        self.stopmes = (b'4\x0A')
        self.datames = (b'6\x0A')
        self.plotreqmes = (b'3\x0A')

#arduino thread--------------------------------------------------------------------------
def serial_read(serialname, data, threadnumber):
    data.ongoing1 = True
    ser1 = serial.Serial(serialname, data.baud, timeout = 5, bytesize=8, parity='N', stopbits=1)
    time.sleep(4)

    ser1.write(data.handshakemes)
    r1 = ser1.readline().decode('ascii').strip()
    if r1 == "C":
        print(serialname, ": Handshake success")
    else:
        print(serialname, ": Handshake failed, try restarting arduino or check ReadMe")
        exit()

    ser1.write(data.startmes)
    r1 = ser1.readline().decode('ascii').strip()
    if r1 == "S":
        print(serialname, ": Start success")
    else:
        print(serialname, ": Start failed, try restarting arduino or check ReadMe")
        exit()
    i = 0
    # Main loop
    while(data.ongoing1):
        try:
            serdata = ser1.readline().decode('ascii').strip()
            response1, arduinoMillies = serdata.split(',')
        except Exception as e:
            print(serialname, "Data Parse Error:", e)
            print("Data in question:", serdata)
            continue
        """
        i += 1
        if i%100 == 0:
            print(serialname,": ", ser1.inWaiting())
            pass
        """
        if threadnumber == 0:   
            data.graphvar1 = response1
            data.arr1.append([arduinoMillies, response1]) if data.recording else None
        else:
            data.graphvar2 = response1
            data.arr2.append([arduinoMillies, response1]) if data.recording else None

            
    
    ser1.write(data.stopmes)
    time.sleep(1)
    ser1.flush()
    ser1.close()
    print(serialname, "Serial Read Thread END")

#csvwriter--------------------------------------------------------------------------------
def csvwriter(mode, data):
    # Set up folder  
    folder_name = "data_folder"
    # Create the new folder
    os.makedirs(folder_name, exist_ok=True)
    # Set up CSV file
    now = datetime.datetime.now()
    # format date and time for use in file name
    date_time = now.strftime("%m_%d_%Y_%H_%M_%S")
    # create file name
    file_name = "data_" + "mode_" + str(mode)+"_date_" + date_time + ".csv"

    with open(os.path.join(folder_name, file_name), 'w', newline='') as csvfile:
        if data.mode == 1:
            fieldnames = ['Elapsed time', 'EKG']
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
            writer.writeheader()
            for i in range(len(data.arr1)):
                timevar = int(data.arr1[i][0]) - int(data.arr1[0][0])
                writer.writerow({'Elapsed time': getcustomtime(timevar), 'EKG': data.arr1[i][1]})
        elif data.mode == 2:
            fieldnames = ['Elapsed time', 'EMG']
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
            writer.writeheader()
            for i in range(len(data.arr2)):
                timevar = int(data.arr2[i][0]) - int(data.arr2[0][0])
                writer.writerow({'Elapsed time': getcustomtime(timevar), 'EMG': data.arr2[i][1]})
        elif data.mode == 3:
            fieldnames = ['Elapsed time EKG', 'EKG','Elapsed time EMG', 'EMG',]
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
            writer.writeheader()
            for i in range(len(data.arr1)):
                timevar1 = int(data.arr1[i][0]) - int(data.arr1[0][0])
                timevar2 = int(data.arr2[i][0]) - int(data.arr2[0][0])
                writer.writerow({'Elapsed time EKG': getcustomtime(timevar1), 'EKG': data.arr1[i][1],
                'Elapsed time EMG': getcustomtime(timevar2), 'EMG': data.arr2[i][1]})
        else:
            print("CSV writer failed")
            return
    print("End of CSV writer")

#time formatting Note: add choice to choose if you want ms-------------------------------
def getcustomtime(micros):                                        
    minutes, micros = divmod(micros, 60000000)
    seconds, micros = divmod(micros, 1000000)
    ms, micros = divmod(micros, 1000)
    time_passed = "{:02d}:{:02d}:{:03d}:{:03d}".format(minutes, seconds, ms, micros)
    return time_passed

#Sort of a debug, delete later-------------------------------------------------------------
def controlprint(arr1, HowmuchData):
    print(arr1[0])
    print(arr1[HowmuchData-1])
    print(0)
    a = int(arr1[HowmuchData-1][0])
    b = int(arr1[0][0])
    print(a - b)

#main function---------------------------------------------------------------------------
def exitprogram(data):
    if(data.ongoing1):
        data.ongoing1 = False
        time.sleep(2)
    exit()
    print("exiting the program")

def set_params(data):
    # Get the values of the four text input fields
    text1 = entry1.get()
    text2 = entry2.get()
    text3 = entry3.get()
    text4 = entry4.get()
    # Print the values to the console
    output_label.config(text=f"{text1}, {text2}, {text3}, {text4}")
    # Set up serial port connections
    data.mode = int(text1)
    data.serialname1 = text2
    data.serialname2 = text3
    data.HowmuchData = int(text4)
    print("parametres set")

def startrecording(data):
    data.recording = True
    print("recording start")

def endrecording(data):
    data.recording = False
    data.ongoing1 = False
    print("recording stop")

def confirm(data):
    if(data.mode == 0 and data.serialname1 == 0 and data.serialname2 == 0 and data.HowmuchData == 0):#exit program if missing or incorrect params TO DO
        text1 = "inputs not set"
        output_label.config(text=f"{text1}")
        print(text1)
        return
    
    print("Starting")
    # Configure threading
    t1 = threading.Thread(target = serial_read, args=[data.serialname1, data, 0])
    t2 = threading.Thread(target = serial_read, args=[data.serialname2, data, 1])

    if data.mode == 1:
        t1.start()
        t1.join()
        try:
            controlprint(data.arr1, len(data.arr1))
        except:
            ("uh oh")
        csvwriter(1, data)

    if data.mode == 2:
        t2.start()
        t2.join()
        controlprint(arr2, len(arr2))
        csvwriter(2, data)
        
    if data.mode == 3:
        t1.start()
        t2.start()
        t1.join()
        t2.join()
        csvwriter(3, data)

    print("Script done")
    exit() #for some reason only exits threads

def button_click(data):
    thread = threading.Thread(target=confirm, args=[data])
    thread.start()

#not used anymore, can be deleted - mod3 was too cpu heavy - serialcom overflow
def animate(i, data):
    # Generate values
    data.y_vals.append(int(data.graphvar1))
    data.y_vals2.append(int(data.graphvar2))
    data.y_vals = data.y_vals[-20:]
    data.y_vals2 = data.y_vals2[-20:]
    # Get all axes of figure
    ax1, ax2 = plt.gcf().get_axes()
    # Clear current data
    ax1.cla()
    ax2.cla()
    # Plot new data
    ax1.plot(data.y_vals)
    ax2.plot(data.y_vals2)

def main():
    data = SharedState()
    data.mode = int(input("Input mode:"))
    data.serialname1 = input("Input ser1:")
    data.serialname2 = input("Input ser2:")
    data.HowmuchData = 10000
    button_click(data)
    l = input("press any key to start recording")
    startrecording(data)
    l = input("press any key to stop recording")
    endrecording(data)
    
if __name__ == "__main__":
    main()
    #STANDALONE BACKEND USABILITY
    
