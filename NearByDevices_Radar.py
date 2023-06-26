import tkinter as tk
import serial
import math
import random
import time

# Serial configuration
SERIAL_PORT = '/dev/cu.usbmodem141301'  # Update with your serial port
BAUD_RATE = 9600

# Create a map visualization using Tkinter
window = tk.Tk()
window.title("Multiple Online Device Malpractice Radar")

canvas = tk.Canvas(window, width=700, height=700, bg='black')
canvas.pack()

device_addresses = set()
device_positions = {}

# Function to update the map visualization
def update_map(device_address, distance):
    x = 300  # x-coordinate of the center of the map
    y = 300  # y-coordinate of the center of the map

    if device_address in device_positions:
        device_number = device_positions[device_address]
    else:
        device_number = len(device_positions) + 1
        device_positions[device_address] = device_number

    # Calculate the position on the map based on the distance
    radius = min(distance, 250)
    angle = device_number * 5

    # Convert angle to radians
    angle_rad = angle * (math.pi / 180.0)
    randx = random.uniform(2, 3)
    randy = random.uniform(2, 3)

    # Calculate the position on the canvas
    pos_x = x + radius * math.cos(angle_rad) + randx
    pos_y = y + radius * math.sin(angle_rad) + randy

    if device_number > 0:
        # Check if the distance is within the malpractice zone
        if distance < 150:
            color = "red"
        else:
            color = "green"

    if device_number in device_positions:
        # Update existing device position
        device_positions[device_number]["distance"] = distance
        device_positions[device_number]["position"] = (pos_x, pos_y)
        canvas.coords(device_positions[device_number]["dot"], pos_x - 5, pos_y - 5, pos_x + 5, pos_y + 5)
        canvas.itemconfig(device_positions[device_number]["text"], text=f"Device {device_number}\n{distance:.1f} cm", fill=color)
    else:
        # Create a new device representation
        dot = canvas.create_oval(pos_x - 5, pos_y - 5, pos_x + 5, pos_y + 5, fill=color)
        text = canvas.create_text(pos_x, pos_y + 10, text=f"Device {device_number}\n{distance:.1f} cm", fill=color)
        device_positions[device_number] = {"distance": distance, "position": (pos_x, pos_y), "dot": dot, "text": text}

# Open the serial connection
ser = serial.Serial(SERIAL_PORT, BAUD_RATE)
# Draw the malpractice zone circle
x = 300  # x-coordinate of the center of the map
y = 300  # y-coordinate of the center of the map
canvas.create_oval(x - 150, y - 150, x + 150, y + 150, outline="white")

# Read the serial data and update the map
start_time = time.time()
while True:
    if ser.in_waiting > 0:
        line = ser.readline().decode().strip()

        if line:
            address, distance = line.split("-")
            if address not in device_addresses:
                device_addresses.add(address)
            update_map(address, float(distance))

    window.update_idletasks()
    window.update()

    # Check if 45 seconds have passed
    elapsed_time = time.time() - start_time
    if elapsed_time >= 45:
        break

window.mainloop()
