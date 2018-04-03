/**
* Agentuino SNMP Agent Library Prototyping...
*
* Copyright 2010 Eric C. Gionet <lavco_eg@hotmail.com>
*
* Update snmpGetNext by Petr Domorazek <petr@domorazek.cz>
*/

#define BOOTLOADER //Comment this line if you are not using bootloader
//#define DEBUG   //Uncomment this line for debug output
#ifdef DEBUG    //Macros are usually in all capital letters.
  #define DPRINT(...)    Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //DPRINTLN is a macro, debug print with new line
#else
  #define DPRINT(...)     //now defines a blank line
  #define DPRINTLN(...)   //now defines a blank line
#endif

// telemetry sensor board
// by Enos and ChihYi 2017/08/23

#include <Streaming.h>         // Include the Streaming library
#include <Ethernet.h>          // Include the Ethernet library
#include <SPI.h>
#include <MemoryFree.h>
#include <Agentuino.h>
#include <Flash.h>
#include <avr/wdt.h>

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x87, 0x07 };
IPAddress ip(10, 1, 164, 210);

// telnet defaults to port 23

EthernetServer g_server(23);
EthernetClient g_client;
boolean connected = false;
String g_strcmd = "";
unsigned long last_active;
#define TELNET_TIMEOUT 10000

int flowPin = 2;
unsigned long duration;
#define SAMPLES 3

//
// tkmib - linux mib browser
//
// .iso (.1)
// .iso.org (.1.3)
// .iso.org.dod (.1.3.6)
// .iso.org.dod.internet (.1.3.6.1)
// .iso.org.dod.internet.private (.1.3.6.1.4)
// .iso.org.dod.internet.private.enterprises (.1.3.6.1.4.1)
//
// ASIAA defined OIDs
//
// .iso.org.dod.internet.private.enterprises.asiaa (.1.3.6.1.4.1.50399)
// .iso.org.dod.internet.private.enterprises.asiaa.sysDescr (.1.3.6.1.4.1.50399.1)
const static char sysDescr[] PROGMEM      = "1.3.6.1.4.1.50399.1.0";  // read-only  (DisplayString)
// .iso.org.dod.internet.private.enterprises.asiaa.sysObjectID (.1.3.6.1.4.1.50399.2)
const static char sysObjectID[] PROGMEM   = "1.3.6.1.4.1.50399.2.0";  // read-only  (ObjectIdentifier)
// .iso.org.dod.internet.private.enterprises.asiaa.sysUpTime (.1.3.6.1.4.1.50399.3)
const static char sysUpTime[] PROGMEM     = "1.3.6.1.4.1.50399.3.0";  // read-only  (TimeTicks)
// .iso.org.dod.internet.private.enterprises.asiaa.sysContact (.1.3.6.1.4.1.50399.4)
const static char sysContact[] PROGMEM    = "1.3.6.1.4.1.50399.4.0";  // read-write (DisplayString)
// .iso.org.dod.internet.private.enterprises.asiaa.sysName (.1.3.6.1.4.1.50399.5)
const static char sysName[] PROGMEM       = "1.3.6.1.4.1.50399.5.0";  // read-write (DisplayString)
// .iso.org.dod.internet.private.enterprises.asiaa.sysLocation (.1.3.6.1.4.1.50399.6)
const static char sysLocation[] PROGMEM   = "1.3.6.1.4.1.50399.6.0";  // read-write (DisplayString)
// .iso.org.dod.internet.private.enterprises.asiaa.sysFlow (.1.3.6.1.4.1.50399.10)
const static char sysFlow[] PROGMEM       = "1.3.6.1.4.1.50399.7.0";  // read-only  (Integer)
// .iso.org.dod.internet.private.enterprises.asiaa.sysServices (.1.3.6.1.4.1.50399.13)
const static char sysServices[] PROGMEM    = "1.3.6.1.4.1.50399.8.0";  // read-only  (Integer)
//
// RFC1213 local values
static char locDescr[]              = "Subaru PFI telemmetry sensors";  // read-only (static)
static char locObjectID[]           = "1.3.6.1.4.1.50399";        // read-only (static)
static uint32_t locUpTime           = 0;                                // read-only (static)
static char locContact[20]          = "ChihYi Wen";                     // should be stored/read from EEPROM - read/write (not done for simplicity)
static char locName[20]             = "Telemetry sensors";              // should be stored/read from EEPROM - read/write (not done for simplicity)
static char locLocation[20]         = "Subaru";                         // should be stored/read from EEPROM - read/write (not done for simplicity)
//static uint32_t lastUpdateTime      = 0;
static int32_t locServices          = 7;

uint32_t prevMillis = millis();
char oid[SNMP_MAX_OID_LEN];
SNMP_API_STAT_CODES api_status;
SNMP_ERR_CODES status;

void pduReceived()
{
  SNMP_PDU pdu;
  api_status = Agentuino.requestPdu(&pdu);
  //
  if ((pdu.type == SNMP_PDU_GET || pdu.type == SNMP_PDU_GET_NEXT || pdu.type == SNMP_PDU_SET)
    && pdu.error == SNMP_ERR_NO_ERROR && api_status == SNMP_API_STAT_SUCCESS ) {
    //
    pdu.OID.toString(oid);
    // Implementation SNMP GET NEXT
    if ( pdu.type == SNMP_PDU_GET_NEXT ) {
      char tmpOIDfs[SNMP_MAX_OID_LEN];
      if ( strcmp_P(oid, sysDescr ) == 0 ) {
        strcpy_P ( oid, sysObjectID );
        strcpy_P ( tmpOIDfs, sysObjectID );
        pdu.OID.fromString(tmpOIDfs);
      } else if ( strcmp_P(oid, sysObjectID ) == 0 ) {
        strcpy_P ( oid, sysUpTime );
        strcpy_P ( tmpOIDfs, sysUpTime );
        pdu.OID.fromString(tmpOIDfs);
      } else if ( strcmp_P(oid, sysUpTime ) == 0 ) {
        strcpy_P ( oid, sysContact );
        strcpy_P ( tmpOIDfs, sysContact );
        pdu.OID.fromString(tmpOIDfs);
      } else if ( strcmp_P(oid, sysContact ) == 0 ) {
        strcpy_P ( oid, sysName );
        strcpy_P ( tmpOIDfs, sysName );
        pdu.OID.fromString(tmpOIDfs);
      } else if ( strcmp_P(oid, sysName ) == 0 ) {
        strcpy_P ( oid, sysLocation );
        strcpy_P ( tmpOIDfs, sysLocation );
        pdu.OID.fromString(tmpOIDfs);
      } else if ( strcmp_P(oid, sysLocation ) == 0 ) {
        strcpy_P ( oid, sysFlow );
        strcpy_P ( tmpOIDfs, sysFlow );
        pdu.OID.fromString(tmpOIDfs);
      } else if ( strcmp_P(oid, sysFlow ) == 0 ) {
        strcpy_P ( oid, sysServices );
        strcpy_P ( tmpOIDfs, sysServices );
        pdu.OID.fromString(tmpOIDfs);
      } else if ( strcmp_P(oid, sysServices ) == 0 ) {
        strcpy_P ( oid, "1.0" );
      } else {
        int ilen = strlen(oid);
        if ( strncmp_P(oid, sysDescr, ilen ) == 0 ) {
          strcpy_P ( oid, sysDescr );
          strcpy_P ( tmpOIDfs, sysDescr );
          pdu.OID.fromString(tmpOIDfs);
        } else if ( strncmp_P(oid, sysObjectID, ilen ) == 0 ) {
          strcpy_P ( oid, sysObjectID );
          strcpy_P ( tmpOIDfs, sysObjectID );
          pdu.OID.fromString(tmpOIDfs);
        } else if ( strncmp_P(oid, sysUpTime, ilen ) == 0 ) {
          strcpy_P ( oid, sysUpTime );
          strcpy_P ( tmpOIDfs, sysUpTime );
          pdu.OID.fromString(tmpOIDfs);
        } else if ( strncmp_P(oid, sysContact, ilen ) == 0 ) {
          strcpy_P ( oid, sysContact );
          strcpy_P ( tmpOIDfs, sysContact );
          pdu.OID.fromString(tmpOIDfs);
        } else if ( strncmp_P(oid, sysName, ilen ) == 0 ) {
          strcpy_P ( oid, sysName );
          strcpy_P ( tmpOIDfs, sysName );
          pdu.OID.fromString(tmpOIDfs);
        } else if ( strncmp_P(oid, sysLocation, ilen ) == 0 ) {
          strcpy_P ( oid, sysLocation );
          strcpy_P ( tmpOIDfs, sysLocation );
          pdu.OID.fromString(tmpOIDfs);
        } else if ( strncmp_P(oid, sysFlow, ilen ) == 0 ) {
          strcpy_P ( oid, sysFlow );
          strcpy_P ( tmpOIDfs, sysFlow );
          pdu.OID.fromString(tmpOIDfs);
        } else if ( strncmp_P(oid, sysServices, ilen ) == 0 ) {
          strcpy_P ( oid, sysServices );
          strcpy_P ( tmpOIDfs, sysServices );
          pdu.OID.fromString(tmpOIDfs);
        }
      }
    }
    // End of implementation SNMP GET NEXT / WALK

    if ( strcmp_P(oid, sysDescr ) == 0 ) {
      // handle sysDescr (set/get) requests
      if ( pdu.type == SNMP_PDU_SET ) {
        // response packet from set-request - object is read-only
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = SNMP_ERR_READ_ONLY;
      } else {
        // response packet from get-request - locDescr
        status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locDescr);
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      }
      //
    } else if ( strcmp_P(oid, sysObjectID ) == 0 ) {
      // handle sysDescr (set/get) requests
      if ( pdu.type == SNMP_PDU_SET ) {
        // response packet from set-request - object is read-only
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = SNMP_ERR_READ_ONLY;
      } else {
        // response packet from get-request - locDescr
        status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locObjectID);
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      }
      //
    } else if ( strcmp_P(oid, sysUpTime ) == 0 ) {
      // handle sysName (set/get) requests
      if ( pdu.type == SNMP_PDU_SET ) {
        // response packet from set-request - object is read-only
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = SNMP_ERR_READ_ONLY;
      } else {
        // response packet from get-request - locUpTime
        status = pdu.VALUE.encode(SNMP_SYNTAX_TIME_TICKS, locUpTime);
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      }
      //
    } else if ( strcmp_P(oid, sysName ) == 0 ) {
      // handle sysName (set/get) requests
      if ( pdu.type == SNMP_PDU_SET ) {
        // response packet from set-request - object is read/write
        status = pdu.VALUE.decode(locName, strlen(locName));
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      } else {
        // response packet from get-request - locName
        status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locName);
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      }
      //
    } else if ( strcmp_P(oid, sysContact ) == 0 ) {
      // handle sysContact (set/get) requests
      if ( pdu.type == SNMP_PDU_SET ) {
        // response packet from set-request - object is read/write
        status = pdu.VALUE.decode(locContact, strlen(locContact));
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      } else {
        // response packet from get-request - locContact
        status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locContact);
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      }
      //
    } else if ( strcmp_P(oid, sysLocation ) == 0 ) {
      // handle sysLocation (set/get) requests
      if ( pdu.type == SNMP_PDU_SET ) {
        // response packet from set-request - object is read/write
        status = pdu.VALUE.decode(locLocation, strlen(locLocation));
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      } else {
        // response packet from get-request - locLocation
        status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locLocation);
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      }
      //
    } else if ( strcmp_P(oid, sysFlow ) == 0 ) {
      // handle sysFlow (set/get) requests
      if ( pdu.type == SNMP_PDU_SET ) {
        // response packet from set-request - object is read-only
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = SNMP_ERR_READ_ONLY;
      } else {
        // response packet from get-request
        unsigned long t_hi, t_low, t_in;
        double hz;
        int i, hz100;
        t_hi = 0;
        t_low = 0;
        for (i=0; i<SAMPLES; i++) {
          t_in = pulseIn(flowPin, HIGH);
          if (t_in == 0) break;
          t_hi += t_in;
          t_in = pulseIn(flowPin, LOW);
          if (t_in == 0) break;
          t_low += t_in;
        }
        if (i < SAMPLES) {
          hz = 0.0;
        } else {
          hz = 1000000.0 * SAMPLES / (t_hi + t_low);
        }
        hz100 = (int)(100 * hz);
        status = pdu.VALUE.encode(SNMP_SYNTAX_INT, hz100);
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      }
      //
    } else if ( strcmp_P(oid, sysServices) == 0 ) {
      // handle sysServices (set/get) requests
      if ( pdu.type == SNMP_PDU_SET ) {
        // response packet from set-request - object is read-only
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = SNMP_ERR_READ_ONLY;
      } else {
        // response packet from get-request - locServices
        status = pdu.VALUE.encode(SNMP_SYNTAX_INT, locServices);
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      }    
    } else {
      // oid does not exist
      // response packet - object not found
      pdu.type = SNMP_PDU_RESPONSE;
      pdu.error = SNMP_ERR_NO_SUCH_NAME;
    }
    //
    Agentuino.responsePdu(&pdu);
  }
  //
  Agentuino.freePdu(&pdu);
  //
}

void doFlow() {
  char str[30], hzstr[16];
  unsigned long t_hi, t_low, t_in;
  double hz;
  int i;

  t_hi = 0;
  t_low = 0;
  for (i=0; i<SAMPLES; i++) {
    t_in = pulseIn(flowPin, HIGH);
    if (t_in == 0) break;
    t_hi += t_in;
    t_in = pulseIn(flowPin, LOW);
    if (t_in == 0) break;
    t_low += t_in;
  }
  if (i < SAMPLES) {
    sprintf(str, "Flow = 0 Hz\n");
    g_client.write(str);
  } else {
    hz = 1000000.0 * SAMPLES / (t_hi + t_low);
    dtostrf(hz, 6, 1, hzstr);
    sprintf(str, "Flow = %s Hz\n", hzstr);
    g_client.write(str);
    sprintf(str, "High = %lu us, Low = %lu us\n", t_hi/3, t_low/3);
    g_client.write(str);
  }
}

void parsing()
{
  char str[30];
  if(g_strcmd == "Q") {
    doFlow();
  } else if (g_strcmd == "RST") {
    // command to test reset function
    delay(5000);
  } else {
    sprintf(str, "unknown\n");
    g_client.write("unknown\n");
  }
  return;
}

void setup()
{
#ifndef BOOTLOADER
  // Clear the reset bit
  MCUSR &= ~_BV(WDRF);
  // Disable the WDT
  WDTCSR |= _BV(WDCE) | _BV(WDE);
  WDTCSR = 0;
#endif

#ifdef DEBUG
  Serial.begin(9600);
  // while the serial stream is not open, do nothing:
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
#endif

  // start the Ethernet connection
  Ethernet.begin(mac, ip);
  // print your local IP address
  DPRINT("My IP address: ");
  DPRINTLN(Ethernet.localIP());
  //
  g_server.begin();
  //
  pinMode(flowPin, INPUT);
  //
  api_status = Agentuino.begin();
  //
  if ( api_status == SNMP_API_STAT_SUCCESS ) {
    Agentuino.onPduReceive(pduReceived);
    delay(10);
    wdt_enable(WDTO_4S);
  } else {
    DPRINTLN("Failed to start SNMP server");
  }
}

void loop()
{
  // listen/handle for incoming SNMP requests
  Agentuino.listen();
  //
  // sysUpTime - The time (in hundredths of a second) since
  // the network management portion of the system was last
  // re-initialized.
  if ( millis() - prevMillis > 1000 ) {
    // increment previous milliseconds
    prevMillis += 1000;
    //
    // increment up-time counter
    locUpTime += 100;
  }
  //
  char c = 0;
  char t = ':';
  char r = 0x0d;
  EthernetClient g_client_new;

  // Allow only one connection
  if(connected) {
    if(!g_client.connected()) {
      g_client.stop();
      connected = false;
      DPRINTLN("Close connection");
    } else if ((millis() - last_active) > TELNET_TIMEOUT) {
      g_client.write("Bye\n");
      g_client.stop();
      connected = false;
      DPRINTLN("Connection timeouted");
    }
  }
  if(!connected && g_server.available()) {
    connected = true;
    g_client = g_server.available();
    last_active = millis();
    DPRINTLN("New connection");
  }
  if(connected) {
    while (g_client.available()) {
      c = g_client.read();
      if (c == r) {
        parsing(); // parsing command
        g_strcmd = ""; // empty cmd buffer
        g_client.write(t);
      } else if ((c >= 0x41 && c <= 0x5A) || (c >= 0x30 && c <= 0x39) || c == 0x2E) {
        g_strcmd += c;
      }
      last_active = millis();
    }
  }

  //
  wdt_reset();
}

