#!/usr/bin/env python

from builtins import object
import opscore.protocols.keys as keys

class FlowCmd(object):

    def __init__(self, actor):
        # This lets us access the rest of the actor.
        self.actor = actor

        # Declare the commands we implement. When the actor is started
        # these are registered with the parser, which will call the
        # associated methods when matched. The callbacks will be
        # passed a single argument, the parsed and typed command.
        #
        self.vocab = [
            ('flow', 'status', self.status),
            ('shutter', 'status', self.shutterStatus),
        ]

        # Define typed command arguments for the above commands.
        self.keys = keys.KeysDictionary("meb_flow", (1, 2),
                                        )
        self.kFactor = float(self.actor.config.get('flow', 'kFactor'))

    @property
    def flowDev(self):
        return self.actor.controllers['flow']

    def status(self, cmd, doFinish=True):
        """Report flow meter status."""

        status = self.flowDev.query()

        # convert from Hz to Gal/min
        speed = status['FlowMeter'] / self.kFactor * 60

        # You need to format this as keywords...
        flow = '%0.2f,%0.1f' % (speed, status['FlowMeter'])
        cmd.inform('flow=%s' % (flow))

        if doFinish:
            cmd.finish()

    def shutterStatus(self, cmd, doFinish=True):
        """Report flow meter status."""

        status = self.flowDev.shutterQuery()
        shutter = '%d,%d,%d' % (status['Shutter'], status['LastOpen'], status['LastClose'])
        cmd.inform('shutter=%s' % (shutter))

        if doFinish:
            cmd.finish()
