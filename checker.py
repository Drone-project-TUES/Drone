import serial
import time
import getch

# Define the serial port (replace '/dev/ttyUSB0' with your Arduino's port)
ser = serial.Serial('/dev/ttyACM0', 115200)  


# Wait for Arduino to initialize
time.sleep(2)  

try:
    while True:
        
        # Read data from Arduino
        response = ser.readline().decode('utf-8').strip()
        print("Response from Arduino:", response)
        
        # Add a small delay if needed

except KeyboardInterrupt:
    # Close the serial port on keyboard interrupt
    ser.close()
    print("\nSerial port closed")