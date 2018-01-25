from builtins import range
from builtins import object
import logging
import socket

class temps(object):
    """ MCS E-box temperatures """

    def __init__(self, actor, name,
                 logLevel=logging.INFO,
                 host=None, port=None):
        """ connect to Adam 6015 box """

        self.name = name
        self.actor = actor
        self.logger = logging.getLogger('temps')

        if host is None:
            host = self.actor.config.get(self.name, 'host')
        if port is None:
            port = self.actor.config.get(self.name, 'port')
        self.host = host
        self.port = port
        
    def query(self):
        """ Read data from Adam 6015 modules """

        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((self.host, self.port))
        req = b'\x00\xef\x00\x00\x00\x06\x01\x04\x00\x00\x00\x07'
        self.logger.info('send: %r', req)
        s.sendall(req)
        data = s.recv(23, socket.MSG_WAITALL)
        self.logger.debug('recv: %r', data)
        s.close()

        if data[:9] != b'\x00\xef\x00\x00\x00\x11\x01\x04\x0e':
            print("Receiving invalid data")
            exit()

        temp = [0.0] * 7
        j = 9
        for i in range(7):
            temp[i] = (ord(data[j]) * 256 + ord(data[j + 1])) / 65535.0 * 200.0 - 50.0
            j += 2
        return temp

    def raw(self, cmdStr):
        """ Send an arbitrary command URL to the controller. """

        raise NotImplementedError('no raw command')

    def start(self):
        pass
    def stop(self):
        pass

