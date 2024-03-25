import serial
import time
import getch

# Define the serial port (replace '/dev/ttyUSB0' with your Arduino's port)
ser = serial.Serial('/dev/ttyACM0', 115200)  


# Wait for Arduino to initialize
time.sleep(2)  

try:
    while True:
        # Read a single character from the keyboard
        char = getch.getch()
        
        # Send the character to Arduino
        ser.write(char.encode())
        print("Sent:", char)
        

except KeyboardInterrupt:
    # Close the serial port on keyboard interrupt
    ser.close()
    print("\nSerial port closed")