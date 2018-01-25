from builtins import str
from builtins import range
from builtins import object
import logging
import requests

PW_PASS = "12345678"

class Power(object):
    """ *MCS power module* """

    deviceIds = {'MC':0, '0':0, 0:0,
                 'STF':1, '1':1, 1:1,
                 'CISCO':2, '2':2, 2:2,
                 'PC':3, '3':3, 3:3}

    def __init__(self, actor, name,
                 logLevel=logging.INFO,
                 host=None, user=None, password=PW_PASS):
        """ connect to IP power 9858DX """

        self.actor = actor

        self.host = host if host is not None else self.actor.config.get(self,name,
                                                                        'host')
        self.user = user if user is not None else self.actor.config.get(self,name,
                                                                        'user')
        self.password = password
        self.url = 'http://' + user + ':' + password + '@' + host + '/set.cmd?cmd='

    def _deviceId(self, idString):
            # Let failures blow up
            return self.deviceIds[idString.upper] + 1
        
    def set_power(self, device, powerOn):
        """ set the power for a device """

        s = self.deviceIds[device] + '='
        s += '1' if powerOn else '0'
        r = requests.get(self.url + 'setpower&p6' + s)

        return r

    def pulse_power(self, device, duration):
        """ turn off and on the power for a period """

        s = self.deviceIds[device] + '=' + str(duration)
        r = requests.get(self.url + 'setpowercycle&p6' + s)

        return r
    
    def query(self):
        """ query current status """

        r = requests.get(self.url + 'getpower')
        idx = r.text.find('p61') + 4
        state = []
        for n in range(4):
            st = 0 if r.text[idx+n*7]=='0' else 1
            state.append(st)
        return state

    def raw(self, cmdStr):
        """ Send an arbitrary command URL to the controller. """
        
        return requests.get(self.url + cmdStr)
    
    def start(self):
        pass
    def stop(self):
        pass

