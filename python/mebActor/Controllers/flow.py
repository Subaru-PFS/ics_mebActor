from builtins import range
from builtins import object
import logging
import telnetlib

TIME_OUT = 15

class flow(object):
    """ MCS E-box flow meter """

    def __init__(self, actor, name,
                 logLevel=logging.INFO,
                 host=None):
        """ connect to Arduino board """

        self.name = name
        self.actor = actor
        self.logger = logging.getLogger('flow')

        if host is None:
            host = self.actor.config.get(self.name, 'host')
        self.host = host
        self.logger.warn('host: %s', self.host)

    def query(self):
        """ Read data from Arduino board """

        tn = telnetlib.Telnet(self.host)
        req = b'Q\r'
        self.logger.info('sent: %s', req)
        tn.write(req)
        data = tn.read_until(b':', TIME_OUT)
        tn.close()
        self.logger.info('recv: %s', data)
        res = data.decode('latin-1').split()
        if len(res) != 5:
            raise RuntimeError('Receiving invalid response')
        return {
            'FlowMeter': float(res[2]),
        }

    def shutterQuery(self):
        """ Read data from Arduino board """

        tn = telnetlib.Telnet(self.host)
        req = b'S\r'
        self.logger.info('sent: %s', req)
        tn.write(req)
        data = tn.read_until(b':', TIME_OUT)
        tn.close()
        self.logger.info('recv: %s', data)
        res = data.decode('latin-1').split()
        if len(res) != 12:
            raise RuntimeError('Receiving invalid response')
        return {
            'Shutter': int(res[2]),
            'LastOpen': int(res[7]),
            'LastClose': int(res[8]),
        }

    def raw(self, cmdStr):
        """ Send an arbitrary command URL to the controller. """

        raise NotImplementedError('no raw command')

    def start(self, cmd=None):
        pass

    def stop(self, cmd=None):
        pass
