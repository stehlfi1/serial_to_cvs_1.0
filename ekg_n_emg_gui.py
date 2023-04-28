"""
-----------------------------------------------------------------------------------------
Made by Filip Stehlik
Frontend
lol 
To do:
Make the fronend prettier
-color buttons
-layout
-----------------------------------------------------------------------------------------
"""

import tkinter as tk
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import ekg_n_emg_be as be
import numpy as np
import random #tobe deleted

class GUI:

    def __init__(self, root):
        self.root = root
        self.root.geometry("1300x800")
        self.root.title("EKG and EMG to CSV")
        self.create_widgets()
        
    def create_widgets(self):
        # Create the graph window
        self.canvas = FigureCanvasTkAgg(plt.gcf(), master=self.root)
        self.canvas.get_tk_widget().pack(side=tk.BOTTOM, fill=tk.BOTH, expand=True)

        # Create the subplots
        #self.ax1, self.ax2 = plt.gcf().subplots(1, 2)

        # Create labels for the text input fields
        self.label1 = tk.Label(self.root, text="MODE(1.EKG 2:EMG 3:BOTH):")
        self.label2 = tk.Label(self.root, text="EKG PORT:")
        self.label3 = tk.Label(self.root, text="EMG PORT:")
        self.label5 = tk.Label(self.root, text="For more info, checkout ReadMe")
        self.output_label = tk.Label(self.root, text="Waiting for Params")

        # Create the text input fields
        self.entry1 = tk.Entry(self.root)
        self.entry2 = tk.Entry(self.root)
        self.entry3 = tk.Entry(self.root)
        self.entry4 = tk.Entry(self.root)
        self.entry1.insert(0, "1")
        self.entry2.insert(0, "COM4")
        self.entry3.insert(0, "COM5")
        self.entry4.insert(0, "10000")

        # Add the labels and text input fields to the window
        self.label1.pack(padx=0, anchor="s",side="left", pady=10)
        self.entry1.pack(padx=10, anchor="s",side="left", pady=10)
        
        self.label2.pack(padx=0, anchor="s",side="left", pady=10)
        self.entry2.pack(padx=10, anchor="s",side="left", pady=10)
        
        self.label3.pack(padx=0, anchor="s",side="left", pady=10)
        self.entry3.pack(padx=10, anchor="s",side="left", pady=10)

        # Create a label to output the values
        #self.output_label.pack(padx=10, anchor="w")

        # Create the buttons
        self.set_params_button = tk.Button(self.root, text="Set Params", command=self.set_params, activebackground='#00ff00')
        self.confirm_button = tk.Button(self.root, text="Startup", command=self.button_click, activebackground='#0000ff')
        self.start_button = tk.Button(self.root, text="RecordStart", command=self.startrecording, activebackground='#00ff00')
        self.exit_button = tk.Button(self.root, text="EXIT", command=self.exitprogram, activebackground='#ff0000')
        self.end_button = tk.Button(self.root, text="RecordStop", command=self.endrecording, activebackground='#ff0000')

        # Add the buttons to the window
        self.set_params_button.pack(side="left",padx=5, pady=10)
        self.confirm_button.pack(side="left",padx=5, pady=10)
        self.start_button.pack(side="left",padx=5, pady=10)
        self.end_button.pack(side="left",padx=5, pady=10)
        self.exit_button.pack(side="left",padx=5, pady=10)
        self.label5.pack(padx=20, anchor="w",side=tk.RIGHT, pady=10)

    def set_params(self):
        # Function to set the parameters
        # Get the values of the four text input fields
        text1 = self.entry1.get()
        text2 = self.entry2.get()
        text3 = self.entry3.get()
        text4 = self.entry4.get()
        # Print the values to the console
        self.output_label.config(text=f"{text1}, {text2}, {text3}, {text4}")
        # Set up serial port connections
        data.mode = int(text1)
        data.serialname1 = text2
        data.serialname2 = text3
        data.HowmuchData = int(text4)
        print("parametres set")
    def button_click(self):
        # Function to set the parameters
        be.button_click(data)
    def startrecording(self):
        # Function to set the parameters
        be.startrecording(data)
    def endrecording(self):
        # Function to set the parameters
        be.endrecording(data)
    def exitprogram(self):
        be.exitprogram(data)
        #self.root.destroy()
        #exit()
    
def init():
    line1.set_data([], [])
    line2.set_data([], [])
    return line1, line2 ,
def animate(frame, data): 
    y1 = int(data.graphvar1)
    y2 = int(data.graphvar2)
    ydata1[frame % len(ydata1)] = y1
    ydata2[frame % len(ydata2)] = y2
    line1.set_data([xdata], [ydata1])
    line2.set_data([xdata], [ydata2])
    ax1.autoscale_view(True,True,True)
    ax2.autoscale_view(True,True,True)
    # Check if the animation has run its course
    if (frame % len(xdata)) == len(xdata) - 1:
        # Reset ydata arrays to clear the lines on the plot
        ydata1.fill(np.nan)
        ydata2.fill(np.nan)
    return line1, line2 ,

#main
root = tk.Tk()
data = be.SharedState()
GUI(root)
# Create the subplots
ax1, ax2 = plt.gcf().subplots(1, 2)

ax1.set_ylim([-50, 1000])
ax2.set_ylim([-50, 1000])
ax1.set_xlim([0,5])
ax2.set_xlim([0,5])

xdata = np.linspace(0, 5, 200)
ydata1 = np.empty(len(xdata))
ydata1.fill(np.nan)
ydata2 = np.empty(len(xdata))
ydata2.fill(np.nan)
line1, = ax1.plot([], [], color="red", lw=2)
line2, = ax2.plot([], [], color="blue", lw=2)

ani = animation.FuncAnimation(plt.gcf(), animate, fargs=(data,),init_func=init ,interval=5, blit=True)
root.mainloop()