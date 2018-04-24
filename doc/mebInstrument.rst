==================
MCS Electronic BOX
==================

Hardware
--------

**ADAM-6015 (x1) with RTD sensors (1x7)**
  - http://www.omega.com/pptst/RTD-830.html
  - http://buy.advantech.com/Remote-I-O-Modules/Ethernet-I-O-Modules-Analog-IO-Modules/model-ADAM-6015-BE.htm?country=USA&token=636372260221909922&f=ATW&f=AUS
**Liquid Flowmeter**
  - http://www.omega.com/pptst/FPR301_302_303_304.html
**Arduino ethernet board**
  This board is used to read the liquid flowmeter
**Aviosys IP Power 9858DX**
  - http://www.aviosys.com/downloads/manuals/power/9858DX-V1.0B.pdf
**USB microphone with USB sound card**
  - http://www.panasonic.com/in/consumer/tv-audio-video/accessories/microphones/rp-vc201.html
  - http://www.galileo.com.tw/USB51A.html
**Cisco Catalyst 2960CG switch**
  - https://www.cisco.com/c/en/us/support/switches/catalyst-2960cg-8tc-l-compact-switch/model.html

Aviosys IP Power 9858DX
-----------------------

IP Power 9858DX is a new generation of the Power Distribution Unit (PDU) & Remote Power Control (RPC) system. It's able
to connect to WiFi network with WPS function for easy & quick set up a device to user’s wireless home/office network
and control in the network.

With embedded web server and HTTPS protection , 9858DX supports higher grade security as working on Internet. User can
control power easily and more safely through the web browser on Windows PC or on Smartphone like Internet Explorer (IE)
, Firefox , Google Chrome , Safari ( iOS ) and Android system.

9858DX allows user to remote control power up to 4 separate devices on/off via network . As support SSL & SNMP, user
can use public email like Gmail / Hotmail / Yahoo Mail to get the email as the ON/OFF status change . User can also
control by e-mail without doing port forwarding / port mapping and search the other IP Device in webpage directly.

:IP: 10.1.164.209
:MAC: 00:98:58:00:14:f6
:user: admin
:password: 12345678

+---------+------------------------------+---------+
| Output  | Devices                      | Default |
+=========+==============================+=========+
| POWER 1 | Metrology camera             | Off     |
+---------+------------------------------+---------+
| POWER 2 | Shutter, Adam6015, Flowmeter | Off     |
+---------+------------------------------+---------+
| POWER 3 | Cisco 2960CG                 | On      |
+---------+------------------------------+---------+
| POWER 4 | PC, Cooling fans             | Off     |
+---------+------------------------------+---------+

**To get the status of power on/ off: getpower**

::

  http://admin:12345678@10.1.164.209/set.cmd?cmd=getpower

System return:

::

  CGI Command : Data follows
  p61=0, p62=0, p63=0, p64=0

P61 to P64 means : POWER1 to POWER4

**To set the power on / off: setpower&p6x=0 or 1**
  p6x=0 means off, p6x=1 means ON, x can be 1 to 4 ( power1 to power4)

Example : Turn on POWER1 and POWER2 and turn off POWER3

::

  http://admin:12345678@10.1.164.209/set.cmd?cmd=setpower&p61=1&p62=1&p63=0

System return:

::

  CGI Command : Data follows
  p61=1,p62=1,p63=0

**Setup power to reboot as RESET: setpowercycle&p6x=delayTime**
  X can be 1 to 4 (power1 to power4), dealyTime mean the time(second) waiting for reset

For example :

::

  http://admin:12345678@10.1.164.209/set.cmd?cmd=setpowercycle&p61=5&p62=2&p63=4

System return:

::

  CGI Command : Data follows
  p61 cycle ok,p62 cycle ok,p63 cycle ok


Arduino ethernet board
----------------------

This board collect the data from flow meter. We can program it to use DHCP or static IP.

:IP: 10.1.164.210
:MAC: 90:a2:da:0f:87:07

There are two ways to read the data:

**Telnet protocol**
  Only support ‘Q’ command for query.

::

  > telnet 10.1.164.210
  :Q
  Flow = 0 Hz
  :X
  unknown

**SNMP protocol**
  You can also use SNMP command to query.

::

  > snmpwalk -c public -v 1 10.1.164.210 1.3.6.1.4.1.50399
  SNMPv2-SMI::enterprises.50399.1.0 = STRING: "Subaru MCS telemmetry sensors"
  SNMPv2-SMI::enterprises.50399.2.0 = STRING: "1.3.6.1.4.1.50399"
  SNMPv2-SMI::enterprises.50399.3.0 = Timeticks: (6500) 0:01:05.00
  SNMPv2-SMI::enterprises.50399.4.0 = STRING: "ChihYi Wen"
  SNMPv2-SMI::enterprises.50399.5.0 = STRING: "Telemetry sensors"
  SNMPv2-SMI::enterprises.50399.6.0 = STRING: “Subaru"
  # flow meter (x100, Hz)
  SNMPv2-SMI::enterprises.50399.7.0 = INTEGER: 0
  # number of services
  SNMPv2-SMI::enterprises.50399.8.0 = INTEGER: 7
  End of MIB

  > snmpget -c public -v 1 10.1.164.210 1.3.6.1.4.1.50399.1.0
  SNMPv2-SMI::enterprises.50399.1.0 = STRING: “Subaru MCS telemmetry sensors"

  > snmpgetnext -c public -v 1 10.1.164.210 1.3.6.1.4.1.50399.6.0
  SNMPv2-SMI::enterprises.50399.7.0 = INTEGER: 0

Adam 6015
---------

The ADAM-6015 is a 16-bit, 7-channel RTD input module that provides programmable input ranges
on all channels. It accepts various RTD inputs (PT100, PT1000, Balco 500 & Ni) and provides data
to the host computer in engineering units (°C). In order to satisfy various temperature requirements
in one module, each analog channel is allowed to configure an individual range for several applications.

There is only one ADAM modules inside EBox and total 7 RTD sensors. This module supports Modbus/TCP Protocol
and following is the function to read RTD sensors. A python module has been built to get the temperature readings.
It doesn’t support DHCP and SNMP protocols.

:IP: 10.1.164.211
:MAC: 00:d0:c9:f4:2a:5f

+-------+-------------------+
| RTD-1 | Top Plate         |
+-------+-------------------+
| RTD-2 | Carbon fiber tube |
+-------+-------------------+
| RTD-3 | Primary mirror    |
+-------+-------------------+
| RTD-4 | Cover panel       |
+-------+-------------------+
| RTD-5 | Coolant water in  |
+-------+-------------------+
| RTD-6 | Coolant water out |
+-------+-------------------+
| RTD-7 | Electronic rack   |
+-------+-------------------+

Function Code 03/04
  The function code 03 or 04 is used to read the binary contents of input registers

  Request message format for function code 03 or 04:

  +-----------------+---------------+-------------------------+------------------------+----------------------------------------+---------------------------------------+
  | Station Address | Function Code | Start Address High Byte | Start Address Low Byte | Requested Number of Register High Byte | Requested Number of Register Low Byte |
  +-----------------+---------------+-------------------------+------------------------+----------------------------------------+---------------------------------------+

  Example: Read Analog inputs #1 and #2 in addresses 40001 to 40002 as floating point value from ADAM-6017 module

  ::

    01 04 00 01 00 02

  Response message format for function code 03 or 04:

  +-----------------+---------------+------------+------+------+-----+
  | Station Address | Function Code | Byte Count | Data | Data | ... |
  +-----------------+---------------+------------+------+------+-----+

  Example: Analog input #1 and #2 as floating point values where AI#1=100.0 and AI#2=55.32

  ::

    01 04 08 42 C8 00 00 47 AE 42 5D

USB microphone
--------------

This device is supported in Ubuntu 14.04. In the following we demonstrate how to use ALSA utility to record sound.

::

  > lsusb
  Bus 008 Device 004: ID 0d8c:0139 C-Media Electronics, Inc. Multimedia Headset [Gigaware by Ignition L.P.]

  > cat /proc/bus/input/devices
  I: Bus=0003 Vendor=0d8c Product=0139 Version=0100
  N: Name="C-Media Electronics Inc.       USB PnP Sound Device"
  P: Phys=usb-0000:03:00.0-2.1/input3
  S: Sysfs=/devices/pci0000:00/0000:00:01.0/0000:01:00.0/0000:02:01.0/0000:03:00.0/usb8/8-2/8-2.1/8-2.1:1.3/0003:0D8C:0139.0004/input/input8
  U: Uniq=
  H: Handlers=kbd event5
  B: PROP=0
  B: EV=13
  B: KEY=1 0 0 e000000000000 0
  B: MSC=10

  > arecord —list-devices
  **** List of CAPTURE Hardware Devices ****
  card 1: Device [USB PnP Sound Device], device 0: USB Audio [USB Audio]
    Subdevices: 1/1
    Subdevice #0: subdevice #0

  # record sound for 20s
  > arecord -f cd -D hw:1,0 -c 1 -d 20 test.wav

Cisco Catalyst 2960CG switch
----------------------------

:IP: 10.1.164.208
:MAC: a0:55:4f:a8:b1:40
:password: (Subaru Computer Devision takes control now)
