#!/usr/bin/env python

from builtins import object
import opscore.protocols.keys as keys
import opscore.protocols.types as types
from opscore.utility.qstr import qstr
from twisted.internet import reactor

class PowerCmd(object):

    def __init__(self, actor):
        # This lets us access the rest of the actor.
        self.actor = actor

        # Declare the commands we implement. When the actor is started
        # these are registered with the parser, which will call the
        # associated methods when matched. The callbacks will be
        # passed a single argument, the parsed and typed command.
        #
        self.vocab = [
            ('power', '@raw', self.raw),
            ('power', '@(on|off|bounce) @(mc|stf|cisco|pc)', self.setPower),
            ('power', 'status', self.status),
        ]

        # Define typed command arguments for the above commands.
        self.keys = keys.KeysDictionary("meb_power", (1, 1),
                                        )
    @property
    def powerDev(self):
        return self.actor.controllers['power']

    def raw(self, cmd):
        """ Send a raw command to the power controller."""

        cmdTxt = cmd.cmd.keywords['raw'].values[0]
        ret = self.powerDev.raw(cmdTxt)
        cmd.finish('text="raw return: %s"' % (ret))

    def status(self, cmd, doFinish=True):
        """Report current power status."""

        status = self.powerDev.query()

        # You need to format this as keywords...
        cmd.inform('power=%s' % ','.join(["%d" % s for s in status]))

        if doFinish:
            cmd.finish()

    def setPower(self, cmd):
        """ Set or bounce power to a device."""

        cmdKeys = cmd.cmd.keywords
        for name in 'mc', 'stf', 'cisco', 'pc':
            if name in cmdKeys:
                deviceName = name
                break

        if 'bounce' in cmdKeys:
            self.powerDev.bounce_power(deviceName)
            reactor.callLater(3.0, self.status, cmd)
        else:
            powerOn = 'on' in cmdKeys
            self.powerDev.set_power(deviceName, powerOn)
            self.status(cmd)
