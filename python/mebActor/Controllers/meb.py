from __future__ import print_function
from builtins import object
import telnetlib

# These are the unused parts of MEB, and should be broken out loke the power controller.

ADAM_HOST = "10.1.120.92"
ADAM_PORT = 502
TM_HOST = "10.1.120.93"

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
