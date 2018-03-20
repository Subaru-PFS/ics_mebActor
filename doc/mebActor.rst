ics_mebActor
============

MCS Electronic Box Actor

Top commands
------------

meb connect controller=\"SSS\" [name=\"SSS\"]
    Reload controller objects.
meb disconnect controller=\"SSS\"
    Disconnect the given, or all, controller objects.
meb exitexit
    Brutal exit when all else has failed.
meb help [full] [cmds=???] [pageWidth=N] [html]
    Return a summary of all commands, or the complete help string for the specified commands.
meb ipdb
    Try to start some debugger.
meb ipython
    Try to start a subshell.
meb monitor controllers=??? period=N
    Enable/disable/adjust period controller monitors.
meb ping
    Query the actor for liveness/happiness.
meb reload [cmds=???]
    If cmds argument is defined, reload the listed command sets, otherwise reload all command sets.
meb reloadConfiguration
    Reload the configuration.
meb status
    Report camera status and actor version.
meb version
    Return a version keyword.


Power module(controller=power)
------------------------------

meb power @raw
    Send a raw command to the power controller.
meb power on|off|bounce mc|stf|cisco|pc
    Set or bounce power to a device.
meb power status
    Report current power status.

ADAM 6015(controller=temps)
---------------------------

meb temps status
    Report temperature from RTD sensors.

Flow meter(controller=flow)
---------------------------

meb flow status
    Report flow meter status.


Actor keywords
--------------

Defined in ics_actorkeys/python/actorkeys/meb.py

::

  KeysDictionary('meb', (1, 6),
                 Key("text", String(help="text for humans")),
                 Key("version", String(help="EUPS/git version")),
                 Key('temps', Float()*7,
                     help='MCS Ebox temperatures'),
                 Key('power', Int()*4,
                     help='MCS power status'),
                 Key('flow', Float(),
                     help='Flow meter for MCS'),
  )


Examples
--------

Query temps controller(ADAM 6015)

::

  meb temps status

Connect power controller

::

  meb connect controller=power

Turn on power for Metrology camera

::

  meb power on mc

Turn on power for MCS computer

::

  meb power on pc

Turn on power for shutter, ADAM 6015 and flow meter

::

  meb power on stf

Query power controller

::

  meb power status

Update status every 5s for temps and flow controllers

::

  meb monitor controllers=temps,flow period=5

Turn off power for Metrology camera

::

  meb power off mc
