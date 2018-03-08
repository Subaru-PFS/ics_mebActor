from builtins import str
from builtins import range
from builtins import object
import logging
import requests

PW_PASS = "12345678"

class power(object):
    """ *MCS power module* """

    deviceIds = {'mc':0, '0':0, 0:0,
                 'stf':1, '1':1, 1:1,
                 'cisco':2, '2':2, 2:2,
                 'pc':3, '3':3, 3:3}

    def __init__(self, actor, name,
                 logLevel=logging.INFO,
                 host=None, user=None, password=None):
        """ connect to IP power 9858DX """

        self.name = name
        self.actor = actor
        self.logger = logging.getLogger('power')

        if host is None:
            host = self.actor.config.get(self.name, 'host')
        if user is None:
            user = self.actor.config.get(self.name, 'user')
        if password is None:
            password = self.actor.config.get(self.name, 'password')

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

        s = str(self.deviceIds[device] + 1) + '='
        s += '1' if powerOn else '0'
        r = self._sendReq('setpower&p6' + s)

        return r

    def bounce_power(self, device):
        """ turn off and on the power for a period """

        s = str(self.deviceIds[device] + 1) + '=0'
        r = self._sendReq('setpowercycle&p6' + s)

        return r

    def query(self):
        """ query current status """

        r = self._sendReq('getpower')
        idx = r.text.find('p61') + 4
        state = []
        for n in range(4):
            st = False if r.text[idx+n*7]=='0' else True
            state.append(st)
        return state

    def raw(self, cmdStr):
        """ Send an arbitrary command URL to the controller. """

        return self._sendReq(cmdStr).text

    def start(self):
        pass

    def stop(self):
        pass
