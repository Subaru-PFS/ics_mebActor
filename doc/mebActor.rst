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
    Send a raw command to the power controller 
meb power on|off|bounce mc|stf|cisco|pc
    Set or bounce power to a device 
meb power status
    Report camera status and actor version. 

ADAM 6015(controller=temps)
---------------------------

meb temps status
    Report camera status and actor version. 

Flow meter(controller=flow)
---------------------------

meb flow status
    Report status 

