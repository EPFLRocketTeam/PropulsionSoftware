# This Python file uses the following encoding: utf-8

import serial
import serial.tools.list_ports

BAUDRATE = 115200

class msv2:
    def __init__(self):
        self.ser = serial.Serial()


    def connect(self, port):
        self.ser.baudrate = BAUDRATE
        self.ser.port = port
        self.ser.connect()

