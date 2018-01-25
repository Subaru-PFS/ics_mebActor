from __future__ import print_function
from builtins import range
from builtins import object
import telnetlib
import socket

# These are the unused parts of MEB, and should be broken out loke the power controller.

ADAM_HOST = "10.1.120.92"
ADAM_PORT = 502
TM_HOST = "10.1.120.93"

class Adam6015(object):
    """ *MCS ADAM 6015 module* """

    def __init__(self, host=ADAM_HOST, port=ADAM_PORT):
        """ set IP for Adam 6015 modules """
        self.host = host
        self.port = port

    def query(self):
        """ Read data from Adam 6015 modules """

        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((self.host, self.port))
        s.sendall('\x00\xef\x00\x00\x00\x06\x01\x04\x00\x00\x00\x07')
        data = s.recv(23, socket.MSG_WAITALL)
        s.close()

        if data[:9] != '\x00\xef\x00\x00\x00\x11\x01\x04\x0e':
            print("Receiving invalid data")
            exit()

        temp = [0.0] * 7
        j = 9
        for i in range(7):
            temp[i] = (ord(data[j]) * 256 + ord(data[j + 1])) / 65535.0 * 200.0 - 50.0
            j += 2
        return temp


class Telemetry(object):
    """ *MCS EBOX telemetry module* """

    def __init__(self, host=TM_HOST):
        """ set IP for arduino board """
        self.host = host

    def query(self):
        """ Read data from Telemetry sensors """

        tn = telnetlib.Telnet(self.host)
        tn.write("Q\r")
        res = tn.read_until(":", TIME_OUT).split()
        tn.close()
        return {
            'Temperature': float(res[2]),
            'Humidity': float(res[6]),
            'DewPoint': float(res[10]),
            'FlowMeter': float(res[14]),
            'Leakage': int(res[18][:1]),
            'LeakageDisconnection': int(res[20])
        }
