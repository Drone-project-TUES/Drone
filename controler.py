import serial
import time
import getch
import pygame
import sys

# Define the serial port (replace '/dev/ttyUSB0' with your Arduino's port)
ser = serial.Serial('/dev/ttyACM1', 115200)  


# Wait for Arduino to initialize
time.sleep(2)  

pygame.init()
display = pygame.display.set_mode((300, 300))
try:
    while True:
        events = pygame.event.get()
        for event in events:
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
        #print("here")
        time.sleep(0.05)
        keys = pygame.key.get_pressed()
        if(keys[pygame.K_w]):
            ser.write("w".encode())
            print("Sent:", "w")
        if(keys[pygame.K_a]):
            ser.write("a".encode())
            print("Sent:", "a")
        if(keys[pygame.K_s]):
            ser.write("s".encode())
            print("Sent:", "s")
        if(keys[pygame.K_d]):
            ser.write("d".encode())
            print("Sent:", "d")
        '''
        for event in events:
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            if event.type == pygame.KEYDOWN:
                print("SJKE")
                if event.key == pygame.K_d:
                    char = "d"
                    ser.write(char.encode())
                    print("Sent:", char)
                if event.key == pygame.K_a:
                    char = "a"
                    ser.write(char.encode())
                    print("Sent:", char)
                if event.key == pygame.K_s:
                    char = "s"
                    ser.write(char.encode())
                    print("Sent:", char)
                if event.key == pygame.K_w:
                    char = "w"
                    ser.write(char.encode())
                    print("Sent:", char)
        '''
        # Read a single character from the keyboard
        #char = getch.getch()
        
        # Send the character to Arduino
        #ser.write(char.encode())
        #print("Sent:", char)
        

except KeyboardInterrupt:
    # Close the serial port on keyboard interrupt
    ser.close()
    print("\nSerial port closed")