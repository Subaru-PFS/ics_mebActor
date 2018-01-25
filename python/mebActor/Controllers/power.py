from builtins import str
from builtins import range
from builtins import object
import logging
import requests

PW_PASS = "12345678"

class power(object):
    """ *MCS power module* """

    deviceIds = {'MC':0, '0':0, 0:0,
                 'STF':1, '1':1, 1:1,
                 'CISCO':2, '2':2, 2:2,
                 'PC':3, '3':3, 3:3}

    def __init__(self, actor, name,
                 logLevel=logging.INFO,
                 host=None, user=None, password=PW_PASS):
        """ connect to IP power 9858DX """

        self.name = name
        self.actor = actor
        self.logger = logging.getLogger('power')

        if host is None:
            host = self.actor.config.get(self.name, 'host')
        if user is None:
            user = self.actor.config.get(self.name, 'user')

        self.password = password
        self.url = 'http://' + user + ':' + password + '@' + host + '/set.cmd?cmd='
        
    def _deviceId(self, idString):
            # Let failures blow up
            return self.deviceIds[idString.upper()] + 1

    def _sendReq(self, reqStr):
        """ Actually send the request. """
        
        req = self.url + reqStr
        self.logger.info('sent: %s', req)
        r = requests.get(req)
        self.logger.debug('recv: %s', r)

        return r
        
    def set_power(self, device, powerOn):
        """ set the power for a device """

        s = self.deviceIds[device] + '='
        s += '1' if powerOn else '0'
        r = self._sendReq('setpower&p6' + s)

        return r

    def pulse_power(self, device, duration):
        """ turn off and on the power for a period """

        s = self.deviceIds[device] + '=' + str(duration)
        r = self._send('setpowercycle&p6' + s)

        return r
    
    def query(self):
        """ query current status """

        r = self._sendReq('getpower')
        idx = r.text.find('p61') + 4
        state = []
        for n in range(4):
            st = 0 if r.text[idx+n*7]=='0' else 1
            state.append(st)
        return state

    def raw(self, cmdStr):
        """ Send an arbitrary command URL to the controller. """

        return self.sendReq(cmdStr)

    def start(self):
        pass
    def stop(self):
        pass

