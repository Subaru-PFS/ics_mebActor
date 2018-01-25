#!/usr/bin/env python

import actorcore.ICC

class OurActor(actorcore.Actor.ICC):
    def __init__(self, name,
                 productName=None, configFile=None,
                 modelNames=(),
                 debugLevel=30):

        """ Setup an Actor instance. See help for actorcore.ICC for details. """
        
        # This sets up the connections to/from the hub, the logger, and the twisted reactor.
        #
        actorcore.ICC.ICC.__init__(self, name, 
                                   productName=productName, 
                                   configFile=configFile,
                                   modelNames=modelNames)
#
# To work
def main():
    theActor = OurActor('meb', productName='mebActor')
    theActor.run()

if __name__ == '__main__':
    main()
