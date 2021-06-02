#!/usr/bin/python3
import os, sys

fd = os.open("/dev/scdev", os.O_RDWR)

try:
    os.lseek(fd, 1, os.SEEK_SET)
    os.lseek(fd, 2, os.SEEK_SET)
    os.lseek(fd, 3, os.SEEK_SET)
    os.lseek(fd, 4, os.SEEK_SET)

    print("read: {}".format(os.read(fd, 10)))
    print("read: {}".format(os.read(fd, 10)))

    b = str.encode("test")
    os.write(fd, b)
    
    print("read: {}".format(os.read(fd, 10)))

finally:
    os.close(fd)
