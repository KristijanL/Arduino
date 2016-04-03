/**
 * SNMP Sensor Input Sketch v.1
 *
 * Adapted from Justin Bennett http://justin-bennett-msjc.blogspot.com/
 *
 *
 * Adapted from the Agentuino SNMP Example Sketch:
 *    Agentuino SNMP Agent Library Prototyping...
 *
 *    Copyright 2016 Kristijan Leèk <janez3k@gmail.com>
 *
 */
 #include <Streaming.h>     // Include the Streaming library
 #include <Ethernet.h>     // Include the Ethernet library
 #include <SPI.h>
 #include <MemoryFree.h>
 #include <Agentuino.h>
 #include <DHT.h>
 
 #define DEBUG

 #define DHTTYPE DHT22
 #define DHTPIN 2
 DHT dht(DHTPIN, DHTTYPE);

 //
 static byte mac[] = { 0xAA, 0xAA, 0xBB, 0xBB, 0x00, 0x01 };
 static byte ip[] = { 192, 168, 1, 188 };
 static byte gateway[] = { 192, 168, 1, 1 };
 static byte subnet[] = { 255, 255, 255, 0 };
 //
 //
 // tkmib - linux mib browser
 //
 // RFC1213-MIB OIDs
 // .iso (.1)
 // .iso.org (.1.3)
 // .iso.org.dod (.1.3.6)
 // .iso.org.dod.internet (.1.3.6.1)
 // .iso.org.dod.internet.mgmt (.1.3.6.1.2)
 // .iso.org.dod.internet.mgmt.mib-2 (.1.3.6.1.2.1)
 // .iso.org.dod.internet.mgmt.mib-2.system (.1.3.6.1.2.1.1)
 // .iso.org.dod.internet.mgmt.mib-2.system.sysDescr (.1.3.6.1.2.1.1.1)
 const static char sysDescr[] PROGMEM   = "1.3.6.1.2.1.1.1.0"; // read-only (DisplayString)
 // .iso.org.dod.internet.mgmt.mib-2.system.sysObjectID (.1.3.6.1.2.1.1.2)
 const static char sysObjectID[] PROGMEM  = "1.3.6.1.2.1.1.2.0"; // read-only (ObjectIdentifier)
 // .iso.org.dod.internet.mgmt.mib-2.system.sysUpTime (.1.3.6.1.2.1.1.3)
 const static char sysUpTime[] PROGMEM   = "1.3.6.1.2.1.1.3.0"; // read-only (TimeTicks)
 // .iso.org.dod.internet.mgmt.mib-2.system.sysContact (.1.3.6.1.2.1.1.4)
 const static char sysContact[] PROGMEM  = "1.3.6.1.2.1.1.4.0"; // read-write (DisplayString)
 // .iso.org.dod.internet.mgmt.mib-2.system.sysName (.1.3.6.1.2.1.1.5)
 const static char sysName[] PROGMEM    = "1.3.6.1.2.1.1.5.0"; // read-write (DisplayString)
 // .iso.org.dod.internet.mgmt.mib-2.system.sysLocation (.1.3.6.1.2.1.1.6)
 const static char sysLocation[] PROGMEM  = "1.3.6.1.2.1.1.6.0"; // read-write (DisplayString)
 // .iso.org.dod.internet.mgmt.mib-2.system.sysServices (.1.3.6.1.2.1.1.7)
 const static char sysServices[] PROGMEM  = "1.3.6.1.2.1.1.7.0"; // read-only (Integer)
 //
 // Arduino defined OIDs
 // .iso.org.dod.internet.private (.1.3.6.1.4)
 // .iso.org.dod.internet.private.enterprises (.1.3.6.1.4.1)
 // .iso.org.dod.internet.private.enterprises.arduino (.1.3.6.1.4.1.36582)
 // .iso.org.dod.internet.private.enterprises.arduino.RAW (.1.3.6.1.4.1.36582.0.0)
 const static char RAWa0[] PROGMEM   = "1.3.6.1.4.1.36582.0.0"; // read-only (Integer)
//
 // RFC1213 local values
 static char locDescr[]       = "Agentuino, a light-weight SNMP Agent."; // read-only (static)
 static char locObjectID[]      = "1.3.6.1.3.2009.0";            // read-only (static)
 static uint32_t locUpTime      = 0;                    // read-only (static)
 static char locContact[20]     = "";              // should be stored/read from EEPROM - read/write (not done for simplicity)
 static char locName[20]       = "Sensor";    // should be stored/read from EEPROM - read/write (not done for simplicity)
 static char locLocation[20]     = "";            // should be stored/read from EEPROM - read/write (not done for simplicity)
 static int32_t locServices     = 7;                    // read-only (static)
 uint32_t prevMillis = millis();
 char oid[SNMP_MAX_OID_LEN];
 SNMP_API_STAT_CODES api_status;
 SNMP_ERR_CODES status;


 float temp_temperature = 0.0;
 float temp_humidity = 0.0;

 const static char TemperatureOID[] PROGMEM = "1.3.6.1.4.1.36582.0.1";
 const static char HumidityOID[] PROGMEM = "1.3.6.1.4.1.36582.0.2";

 void pduReceived()
 {
  SNMP_PDU pdu;
  //
  #ifdef DEBUG
   Serial << F("UDP Packet Received Start..") << F(" RAM:") << freeMemory() << endl;
  #endif
  //
  api_status = Agentuino.requestPdu(&pdu);
  //
  if ( pdu.type == SNMP_PDU_GET || pdu.type == SNMP_PDU_GET_NEXT || pdu.type == SNMP_PDU_SET
   && pdu.error == SNMP_ERR_NO_ERROR && api_status == SNMP_API_STAT_SUCCESS ) {
   //
   pdu.OID.toString(oid);
   //
   //Serial << "OID: " << oid << endl;
   //
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
    #ifdef DEBUG
     Serial << F("sysDescr...") << locDescr << F(" ") << pdu.VALUE.size << endl;
    #endif
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
    #ifdef DEBUG
     Serial << F("sysUpTime...") << locUpTime << F(" ") << pdu.VALUE.size << endl;
    #endif
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
    #ifdef DEBUG
     Serial << F("sysName...") << locName << F(" ") << pdu.VALUE.size << endl;
    #endif
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
    #ifdef DEBUG
     Serial << F("sysContact...") << locContact << F(" ") << pdu.VALUE.size << endl;
    #endif
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
    #ifdef DEBUG
     Serial << F("sysLocation...") << locLocation << F(" ") << pdu.VALUE.size << endl;
    #endif
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
    //
    #ifdef DEBUG
     Serial << F("locServices...") << locServices << F(" ") << pdu.VALUE.size << endl;
    #endif
    } else if ( strcmp_P(oid, TemperatureOID) == 0 ) {
      int32_t temperature = (int32_t) (temp_temperature *100);
      if ( pdu.type == SNMP_PDU_SET ) {
       // response packet from set-request - object is read-only
       pdu.type = SNMP_PDU_RESPONSE;
       pdu.error = SNMP_ERR_READ_ONLY;
      } else {
       // response packet from get-request - locServices
       status = pdu.VALUE.encode(SNMP_SYNTAX_INT, temperature);
       pdu.type = SNMP_PDU_RESPONSE;
       pdu.error = status;
      }
    #ifdef DEBUG
     Serial << F("getTemperature...") << temperature << F(" ") << pdu.VALUE.size << endl;
    #endif
    } else if ( strcmp_P(oid, HumidityOID) == 0 ) {
      int32_t humidity = (int32_t) (temp_humidity * 100);
      if ( pdu.type == SNMP_PDU_SET ) {
       // response packet from set-request - object is read-only
       pdu.type = SNMP_PDU_RESPONSE;
       pdu.error = SNMP_ERR_READ_ONLY;
      } else {
       // response packet from get-request - locServices
       status = pdu.VALUE.encode(SNMP_SYNTAX_INT, humidity);
       pdu.type = SNMP_PDU_RESPONSE;
       pdu.error = status;
      }
    #ifdef DEBUG
     Serial << F("getHumidity...") << humidity << F(" ") << pdu.VALUE.size << endl;
    #endif
    } else {
    // oid does not exist
    //
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
  //Serial << "UDP Packet Received End.." << " RAM:" << freeMemory() << endl;
 }
 void setup()
 {
  Serial.begin(9600);
  Ethernet.begin(mac);  // DHCP
  // Ethernet.begin(mac, ip); // Static
  //
  //pinMode(2, INPUT);
  dht.begin();

  api_status = Agentuino.begin();
  if ( api_status == SNMP_API_STAT_SUCCESS ) {
   //
   Agentuino.onPduReceive(pduReceived);
   //
   delay(10);
   //
   Serial << F("SNMP Agent Initalized...") << endl;
   //
   return;
  }
  //
  delay(10);
  //
  Serial << F("SNMP Agent Initalization Problem...") << status << endl;
 }

static int doDHTRead = 0;
 
 void loop()
 {
  int currentMillis = millis();
  // listen/handle for incoming SNMP requests
  if (currentMillis - doDHTRead > 2001) {
    doDHTRead = currentMillis;
    temp_temperature = dht.readTemperature();
    temp_humidity = dht.readHumidity();
  }

  Agentuino.listen();
  //
  // sysUpTime - The time (in hundredths of a second) since
  // the network management portion of the system was last
  // re-initialized.
  if (currentMillis - prevMillis > 1000 ) {
   // increment previous milliseconds
   prevMillis += 1000;
   //
   // increment up-time counter
   locUpTime += 100;
  }
 }
