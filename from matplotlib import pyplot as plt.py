from matplotlib import pyplot as plt 
import csv
import numpy as np


csv_path = 'data_mode_1_date_04_05_2023_16_47_49.csv'
import matplotlib.pyplot as plt
import csv

# Set the path to your CSV file


# Initialize empty lists for the x and y data
x_data = []
y_data = []

# Load the data from the CSV file into the x and y lists
with open(csv_path) as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=',')
    for row in csv_reader:
        x_data.append(float(row[0]))
        y_data.append(float(row[1]))

# Plot the data as a line graph
plt.plot(x_data, y_data)

# Add labels and a title to the graph
plt.xlabel('X-axis label')
plt.ylabel('Y-axis label')
plt.title('Title of graph')

# Display the graph
plt.show()