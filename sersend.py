#!/usr/bin/python
import serial
import time
import math
import sys
import struct
import pyglet
import threading
import random

def hsv2rgb(h, s, v):
    h = float(h)
    s = float(s)
    v = float(v)
    h60 = h / 60.0
    h60f = math.floor(h60)
    hi = int(h60f) % 6
    f = h60 - h60f
    p = v * (1 - s)
    q = v * (1 - f * s)
    t = v * (1 - (1 - f) * s)
    r, g, b = 0, 0, 0
    if hi == 0: r, g, b = v, t, p
    elif hi == 1: r, g, b = q, v, p
    elif hi == 2: r, g, b = p, v, t
    elif hi == 3: r, g, b = p, q, v
    elif hi == 4: r, g, b = t, p, v
    elif hi == 5: r, g, b = v, p, q
    r, g, b = int(r * 255), int(g * 255), int(b * 255)
    return g, r, b

def writeLeds(ser,loc,data):
    dataLength = data.__len__() + 1

    out = bytearray();
    for b in struct.pack("!H",dataLength):
        out.append(b)
    out.append(loc)
    for c in data:
        out.append(c)
    ser.write(out);

def main():
    port = "/dev/ttyACM0"
    baudrate = 115200

    if (sys.argv.__len__() >= 2): port = sys.argv[1]
    if (sys.argv.__len__() >= 3): baudrate = sys.argv[2]

    ser = serial.Serial(
        port=port,
        baudrate=baudrate
    )

    #writeLeds(ser,0,[10,0,0,0,10,0,0,0,10]);
    h = 0;
    while(True):
        h+=1
        if (h >= 360): h= 0;
        r,g,b = hsv2rgb(h,1,.1);
        writeLeds(ser,0,[r,g,b]*125);

        while(True):
            c = ser.read(1)
            if (ord(c) == 0): break;
            sys.stdout.write(c)
            sys.stdout.flush()
    
#    try:
#        while(True):
#            writeLeds(ser,0,leds);
#            ser.read(1)
#    except:
#        ser.close()
#        raise

main()
