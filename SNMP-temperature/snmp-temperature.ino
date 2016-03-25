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
 //#include <Flash.h>  
 //  
 //  
 #define DEBUG 

 #define DHTTYPE DHT22 
 #define DHTPIN 2 
 DHT dht(DHTPIN, DHTTYPE);
  
 //  
 static byte mac[] = { 0xAA, 0xAA, 0xBB, 0xBB, 0x00, 0x01 };  
 static byte ip[] = { 10, 10, 10, 177 };  
 static byte gateway[] = { 10, 10, 10, 1 };  
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
 // .iso.org.dod.internet.private.enterprises.arduino.RAW (.1.3.6.1.4.1.36582.0.1)  
 const static char RAWa1[] PROGMEM   = "1.3.6.1.4.1.36582.0.1"; // read-only (Integer)  
 // .iso.org.dod.internet.private.enterprises.arduino.RAW (.1.3.6.1.4.1.36582.0.2)  
 const static char RAWa2[] PROGMEM   = "1.3.6.1.4.1.36582.0.2"; // read-only (Integer)  
 // .iso.org.dod.internet.private.enterprises.arduino.RAW (.1.3.6.1.4.1.36582.0.3)  
 const static char RAWa3[] PROGMEM   = "1.3.6.1.4.1.36582.0.3"; // read-only (Integer)  
 // .iso.org.dod.internet.private.enterprises.arduino.RAW (.1.3.6.1.4.1.36582.0.4)  
 //const static char RAWa4[] PROGMEM   = "1.3.6.1.4.1.36582.0.4"; // read-only (Integer)  
 // .iso.org.dod.internet.private.enterprises.arduino.RAW (.1.3.6.1.4.1.36582.0.5)  
 const static char RAWa5[] PROGMEM   = "1.3.6.1.4.1.36582.0.5"; // read-only (Integer)  
 // .iso.org.dod.internet.private.enterprises.arduino.RAW (.1.3.6.1.4.1.36582.1.0)  
 //const static char RAWd0[] PROGMEM   = "1.3.6.1.4.1.36582.1.0"; // read-only (Integer)  
 // .iso.org.dod.internet.private.enterprises.arduino.RAW (.1.3.6.1.4.1.36582.1.1)  
 //const static char RAWd1[] PROGMEM   = "1.3.6.1.4.1.36582.1.1"; // read-only (Integer)  
 // .iso.org.dod.internet.private.enterprises.arduino.RAW (.1.3.6.1.4.1.36582.1.2)  
 const static char RAWd2[] PROGMEM   = "1.3.6.1.4.1.36582.1.2"; // read-only (Integer)  
 // .iso.org.dod.internet.private.enterprises.arduino.RAW (.1.3.6.1.4.1.36582.1.3)  
 const static char RAWd3[] PROGMEM   = "1.3.6.1.4.1.36582.1.3"; // read-only (Integer)  
 // .iso.org.dod.internet.private.enterprises.arduino.RAW (.1.3.6.1.4.1.36582.1.4)  
 const static char RAWd4[] PROGMEM   = "1.3.6.1.4.1.36582.1.4"; // read-only (Integer)  
 // .iso.org.dod.internet.private.enterprises.arduino.RAW (.1.3.6.1.4.1.36582.1.5)  
 const static char RAWd5[] PROGMEM   = "1.3.6.1.4.1.36582.1.5"; // read-only (Integer)  
 // .iso.org.dod.internet.private.enterprises.arduino.RAW (.1.3.6.1.4.1.36582.1.6)  
 const static char RAWd6[] PROGMEM   = "1.3.6.1.4.1.36582.1.6"; // read-only (Integer)  
 // .iso.org.dod.internet.private.enterprises.arduino.RAW (.1.3.6.1.4.1.36582.1.7)  
 const static char RAWd7[] PROGMEM   = "1.3.6.1.4.1.36582.1.7"; // read-only (Integer)  
 // .iso.org.dod.internet.private.enterprises.arduino.RAW (.1.3.6.1.4.1.36582.1.8)  
 const static char RAWd8[] PROGMEM   = "1.3.6.1.4.1.36582.1.8"; // read-only (Integer)  
 // .iso.org.dod.internet.private.enterprises.arduino.RAW (.1.3.6.1.4.1.36582.1.9)  
 const static char RAWd9[] PROGMEM   = "1.3.6.1.4.1.36582.1.9"; // read-only (Integer)  
 // .iso.org.dod.internet.private.enterprises.arduino.RAW (.1.3.6.1.4.1.36582.1.10)  
 //const static char RAWd10[] PROGMEM   = "1.3.6.1.4.1.36582.1.10"; // read-only (Integer)  
 // .iso.org.dod.internet.private.enterprises.arduino.RAW (.1.3.6.1.4.1.36582.1.11)  
 //const static char RAWd11[] PROGMEM   = "1.3.6.1.4.1.36582.1.11"; // read-only (Integer)  
 // .iso.org.dod.internet.private.enterprises.arduino.RAW (.1.3.6.1.4.1.36582.1.12)  
 //const static char RAWd12[] PROGMEM   = "1.3.6.1.4.1.36582.1.12"; // read-only (Integer)  
 // .iso.org.dod.internet.private.enterprises.arduino.RAW (.1.3.6.1.4.1.36582.1.13)  
 //const static char RAWd13[] PROGMEM   = "1.3.6.1.4.1.36582.1.13"; // read-only (Integer)  
 //  
 //  
 // RFC1213 local values  
 static char locDescr[]       = "Agentuino, a light-weight SNMP Agent."; // read-only (static)  
 static char locObjectID[]      = "1.3.6.1.3.2009.0";            // read-only (static)  
 static uint32_t locUpTime      = 0;                    // read-only (static)  
 static char locContact[20]     = "";              // should be stored/read from EEPROM - read/write (not done for simplicity)  
 static char locName[20]       = "Sensor";    // should be stored/read from EEPROM - read/write (not done for simplicity)  
 static char locLocation[20]     = "";            // should be stored/read from EEPROM - read/write (not done for simplicity)  
 //static int32_t locServices     = 7;                    // read-only (static)  
 uint32_t prevMillis = millis();  
 char oid[SNMP_MAX_OID_LEN];  
 SNMP_API_STAT_CODES api_status;  
 SNMP_ERR_CODES status;  
 void pduReceived()  
 {
    
  float h = dht.readHumidity();
  // float t = dht.readTemperature();
  static int32_t locServices =  dht.readTemperature();
  
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
      } else if ( strcmp_P(oid, RAWa0) == 0 ) {  
    int rawReading = 0;  
    // handle RAW (set/get) requests  
    if ( pdu.type == SNMP_PDU_SET ) {  
     // response packet from set-request - object is read-only  
     pdu.type = SNMP_PDU_RESPONSE;  
     pdu.error = SNMP_ERR_READ_ONLY;  
    } else {  
     // response packet from get-request - RAW  
            rawReading = analogRead(A0);  // Range : 0..1024  
            status = pdu.VALUE.encode(SNMP_SYNTAX_INT, rawReading);  
            pdu.type = SNMP_PDU_RESPONSE;  
     pdu.error = status;  
    }  
    //  
    #ifdef DEBUG  
     Serial << F("RAWa0...") << rawReading << F(" ") << pdu.VALUE.size << endl;  
    #endif  
    } else if ( strcmp_P(oid, RAWa1) == 0 ) {  
    int rawReading = 0;  
    // handle RAW (set/get) requests  
    if ( pdu.type == SNMP_PDU_SET ) {  
     // response packet from set-request - object is read-only  
     pdu.type = SNMP_PDU_RESPONSE;  
     pdu.error = SNMP_ERR_READ_ONLY;  
    } else {  
     // response packet from get-request - RAW  
            rawReading = analogRead(A1);  // Range : 0..1024  
            status = pdu.VALUE.encode(SNMP_SYNTAX_INT, rawReading);  
            pdu.type = SNMP_PDU_RESPONSE;  
     pdu.error = status;  
    }  
    //  
    #ifdef DEBUG  
     Serial << F("RAW...") << rawReading << F(" ") << pdu.VALUE.size << endl;  
    #endif  
    } else if ( strcmp_P(oid, RAWa2) == 0 ) {  
    int rawReading = 0;  
    // handle RAW (set/get) requests  
    if ( pdu.type == SNMP_PDU_SET ) {  
     // response packet from set-request - object is read-only  
     pdu.type = SNMP_PDU_RESPONSE;  
     pdu.error = SNMP_ERR_READ_ONLY;  
    } else {  
     // response packet from get-request - RAW  
            rawReading = analogRead(A2);  // Range : 0..1024  
            status = pdu.VALUE.encode(SNMP_SYNTAX_INT, rawReading);  
            pdu.type = SNMP_PDU_RESPONSE;  
     pdu.error = status;  
    }  
    //  
    #ifdef DEBUG  
     Serial << F("RAWa2...") << rawReading << F(" ") << pdu.VALUE.size << endl;  
    #endif  
      } else if ( strcmp_P(oid, RAWa3) == 0 ) {  
    int rawReading = 0;  
    // handle RAW (set/get) requests  
    if ( pdu.type == SNMP_PDU_SET ) {  
     // response packet from set-request - object is read-only  
     pdu.type = SNMP_PDU_RESPONSE;  
     pdu.error = SNMP_ERR_READ_ONLY;  
    } else {  
     // response packet from get-request - RAW  
            rawReading = analogRead(A3);  // Range : 0..1024  
            status = pdu.VALUE.encode(SNMP_SYNTAX_INT, rawReading);  
            pdu.type = SNMP_PDU_RESPONSE;  
     pdu.error = status;  
    }  
    //  
    #ifdef DEBUG  
     Serial << F("RAWa3...") << rawReading << F(" ") << pdu.VALUE.size << endl;  
    #endif  
 //     } else if ( strcmp_P(oid, RAWa4) == 0 ) {  
 //   int rawReading = 0;  
 //   // handle RAW (set/get) requests  
 //   if ( pdu.type == SNMP_PDU_SET ) {  
 //    // response packet from set-request - object is read-only  
 //    pdu.type = SNMP_PDU_RESPONSE;  
 //    pdu.error = SNMP_ERR_READ_ONLY;  
 //   } else {  
 //    // response packet from get-request - RAW  
 //           rawReading = analogRead(A4);  // Range : 0..1024  
 //           status = pdu.VALUE.encode(SNMP_SYNTAX_INT, rawReading);  
 //           pdu.type = SNMP_PDU_RESPONSE;  
 //    pdu.error = status;  
 //   }  
 //   //  
 //   #ifdef DEBUG  
 //    Serial << F("RAWa4...") << rawReading << F(" ") << pdu.VALUE.size << endl;  
 //   #endif  
      } else if ( strcmp_P(oid, RAWa5) == 0 ) {  
    int rawReading = 0;  
    // handle RAW (set/get) requests  
    if ( pdu.type == SNMP_PDU_SET ) {  
     // response packet from set-request - object is read-only  
     pdu.type = SNMP_PDU_RESPONSE;  
     pdu.error = SNMP_ERR_READ_ONLY;  
    } else {  
     // response packet from get-request - RAW  
            rawReading = analogRead(A5);  // Range : 0..1024  
            status = pdu.VALUE.encode(SNMP_SYNTAX_INT, rawReading);  
            pdu.type = SNMP_PDU_RESPONSE;  
     pdu.error = status;  
    }  
    //  
    #ifdef DEBUG  
     Serial << F("RAWa5...") << rawReading << F(" ") << pdu.VALUE.size << endl;  
    #endif  
 //      } else if ( strcmp_P(oid, RAWd0) == 0 ) {  
 //   int rawReading = 0;  
 //   // handle RAW (set/get) requests  
 //   if ( pdu.type == SNMP_PDU_SET ) {  
 //    // response packet from set-request - object is read-only  
 //    pdu.type = SNMP_PDU_RESPONSE;  
 //    pdu.error = SNMP_ERR_READ_ONLY;  
 //   } else {  
 //    // response packet from get-request - RAW  
 //           if (digitalRead(0) == LOW) {  
 //            rawReading = 1; // Range : 0..1  
 //           }  
 //           status = pdu.VALUE.encode(SNMP_SYNTAX_INT, rawReading);  
 //           pdu.type = SNMP_PDU_RESPONSE;  
 //    pdu.error = status;  
 //   }  
 //   //  
 //   #ifdef DEBUG  
 //    Serial << F("RAWd0...") << rawReading << F(" ") << pdu.VALUE.size << endl;  
 //   #endif  
 //     } else if ( strcmp_P(oid, RAWd1) == 0 ) {  
 //   int rawReading = 0;  
 //   // handle RAW (set/get) requests  
 //   if ( pdu.type == SNMP_PDU_SET ) {  
 //    // response packet from set-request - object is read-only  
 //    pdu.type = SNMP_PDU_RESPONSE;  
 //    pdu.error = SNMP_ERR_READ_ONLY;  
 //   } else {  
 //    // response packet from get-request - RAW  
 //           if (digitalRead(1) == LOW) {  
 //            rawReading = 1; // Range : 0..1  
 //           }           status = pdu.VALUE.encode(SNMP_SYNTAX_INT, rawReading);  
 //           pdu.type = SNMP_PDU_RESPONSE;  
 //    pdu.error = status;  
 //   }  
 //   //  
 //   #ifdef DEBUG  
 //    Serial << F("RAWd1...") << rawReading << F(" ") << pdu.VALUE.size << endl;  
 //   #endif  
      } else if ( strcmp_P(oid, RAWd2) == 0 ) {  
    int rawReading = 0;  
    // handle RAW (set/get) requests  
    if ( pdu.type == SNMP_PDU_SET ) {  
     // response packet from set-request - object is read-only  
     pdu.type = SNMP_PDU_RESPONSE;  
     pdu.error = SNMP_ERR_READ_ONLY;  
    } else {  
     // response packet from get-request - RAW  
            if (digitalRead(2) == LOW) {  
             rawReading = 1; // Range : 0..1  
            }  
            status = pdu.VALUE.encode(SNMP_SYNTAX_INT, rawReading);  
            pdu.type = SNMP_PDU_RESPONSE;  
     pdu.error = status;  
    }  
    //  
    #ifdef DEBUG  
     Serial << F("RAWd2...") << rawReading << F(" ") << pdu.VALUE.size << endl;  
    #endif  
      } else if ( strcmp_P(oid, RAWd3) == 0 ) {  
    int rawReading = 0;  
    // handle RAW (set/get) requests  
    if ( pdu.type == SNMP_PDU_SET ) {  
     // response packet from set-request - object is read-only  
     pdu.type = SNMP_PDU_RESPONSE;  
     pdu.error = SNMP_ERR_READ_ONLY;  
    } else {  
     // response packet from get-request - RAW  
            if (digitalRead(3) == LOW) {  
             rawReading = 1; // Range : 0..1  
            }  
            status = pdu.VALUE.encode(SNMP_SYNTAX_INT, rawReading);  
            pdu.type = SNMP_PDU_RESPONSE;  
     pdu.error = status;  
    }  
    //  
    #ifdef DEBUG  
     Serial << F("RAWd3...") << rawReading << F(" ") << pdu.VALUE.size << endl;  
    #endif  
      } else if ( strcmp_P(oid, RAWd4) == 0 ) {  
    int rawReading = 0;  
    // handle RAW (set/get) requests  
    if ( pdu.type == SNMP_PDU_SET ) {  
     // response packet from set-request - object is read-only  
     pdu.type = SNMP_PDU_RESPONSE;  
     pdu.error = SNMP_ERR_READ_ONLY;  
    } else {  
     // response packet from get-request - RAW  
            if (digitalRead(4) == LOW) {  
             rawReading = 1; // Range : 0..1  
            }  
            status = pdu.VALUE.encode(SNMP_SYNTAX_INT, rawReading);  
            pdu.type = SNMP_PDU_RESPONSE;  
     pdu.error = status;  
    }  
    //  
    #ifdef DEBUG  
     Serial << F("RAWd4...") << rawReading << F(" ") << pdu.VALUE.size << endl;  
    #endif  
      } else if ( strcmp_P(oid, RAWd5) == 0 ) {  
    int rawReading = 0;  
    // handle RAW (set/get) requests  
    if ( pdu.type == SNMP_PDU_SET ) {  
     // response packet from set-request - object is read-only  
     pdu.type = SNMP_PDU_RESPONSE;  
     pdu.error = SNMP_ERR_READ_ONLY;  
    } else {  
     // response packet from get-request - RAW  
            if (digitalRead(5) == LOW) {  
             rawReading = 1; // Range : 0..1  
            }  
            status = pdu.VALUE.encode(SNMP_SYNTAX_INT, rawReading);  
            pdu.type = SNMP_PDU_RESPONSE;  
     pdu.error = status;  
    }  
    //  
    #ifdef DEBUG  
     Serial << F("RAWd5...") << rawReading << F(" ") << pdu.VALUE.size << endl;  
    #endif  
      } else if ( strcmp_P(oid, RAWd6) == 0 ) {  
    int rawReading = 0;  
    // handle RAW (set/get) requests  
    if ( pdu.type == SNMP_PDU_SET ) {  
     // response packet from set-request - object is read-only  
     pdu.type = SNMP_PDU_RESPONSE;  
     pdu.error = SNMP_ERR_READ_ONLY;  
    } else {  
     // response packet from get-request - RAW  
            if (digitalRead(6) == LOW) {  
             rawReading = 1; // Range : 0..1  
            }  
            status = pdu.VALUE.encode(SNMP_SYNTAX_INT, rawReading);  
            pdu.type = SNMP_PDU_RESPONSE;  
     pdu.error = status;  
    }  
    //  
    #ifdef DEBUG  
     Serial << F("RAWd6...") << rawReading << F(" ") << pdu.VALUE.size << endl;  
    #endif  
      } else if ( strcmp_P(oid, RAWd7) == 0 ) {  
    int rawReading = 0;  
    // handle RAW (set/get) requests  
    if ( pdu.type == SNMP_PDU_SET ) {  
     // response packet from set-request - object is read-only  
     pdu.type = SNMP_PDU_RESPONSE;  
     pdu.error = SNMP_ERR_READ_ONLY;  
    } else {  
     // response packet from get-request - RAW  
            if (digitalRead(7) == LOW) {  
             rawReading = 1; // Range : 0..1  
            }  
            status = pdu.VALUE.encode(SNMP_SYNTAX_INT, rawReading);  
            pdu.type = SNMP_PDU_RESPONSE;  
     pdu.error = status;  
    }  
    //  
    #ifdef DEBUG  
     Serial << F("RAWd7...") << rawReading << F(" ") << pdu.VALUE.size << endl;  
    #endif  
      } else if ( strcmp_P(oid, RAWd8) == 0 ) {  
    int rawReading = 0;  
    // handle RAW (set/get) requests  
    if ( pdu.type == SNMP_PDU_SET ) {  
     // response packet from set-request - object is read-only  
     pdu.type = SNMP_PDU_RESPONSE;  
     pdu.error = SNMP_ERR_READ_ONLY;  
    } else {  
     // response packet from get-request - RAW  
            if (digitalRead(8) == LOW) {  
             rawReading = 1; // Range : 0..1  
            }  
            status = pdu.VALUE.encode(SNMP_SYNTAX_INT, rawReading);  
            pdu.type = SNMP_PDU_RESPONSE;  
     pdu.error = status;  
    }  
    //  
    #ifdef DEBUG  
     Serial << F("RAWd8...") << rawReading << F(" ") << pdu.VALUE.size << endl;  
    #endif  
      } else if ( strcmp_P(oid, RAWd9) == 0 ) {  
    int rawReading = 0;  
    // handle RAW (set/get) requests  
    if ( pdu.type == SNMP_PDU_SET ) {  
     // response packet from set-request - object is read-only  
     pdu.type = SNMP_PDU_RESPONSE;  
     pdu.error = SNMP_ERR_READ_ONLY;  
    } else {  
     // response packet from get-request - RAW  
            if (digitalRead(9) == LOW) {  
             rawReading = 1; // Range : 0..1  
            }  
            status = pdu.VALUE.encode(SNMP_SYNTAX_INT, rawReading);  
            pdu.type = SNMP_PDU_RESPONSE;  
     pdu.error = status;  
    }  
    //  
    #ifdef DEBUG  
     Serial << F("RAWd9...") << rawReading << F(" ") << pdu.VALUE.size << endl;  
    #endif  
 //     } else if ( strcmp_P(oid, RAWd10) == 0 ) {  
 //   int rawReading = 0;  
 //   // handle RAW (set/get) requests  
 //   if ( pdu.type == SNMP_PDU_SET ) {  
 //    // response packet from set-request - object is read-only  
 //    pdu.type = SNMP_PDU_RESPONSE;  
 //    pdu.error = SNMP_ERR_READ_ONLY;  
 //   } else {  
 //    // response packet from get-request - RAW  
 //           if (digitalRead(10) == LOW) {  
 //            rawReading = 1; // Range : 0..1  
 //           }  
 //           status = pdu.VALUE.encode(SNMP_SYNTAX_INT, rawReading);  
 //           pdu.type = SNMP_PDU_RESPONSE;  
 //    pdu.error = status;  
 //   }  
 //   //  
 //   #ifdef DEBUG  
 //    Serial << F("RAWd10...") << rawReading << F(" ") << pdu.VALUE.size << endl;  
 //   #endif  
 //     } else if ( strcmp_P(oid, RAWd11) == 0 ) {  
 //   int rawReading = 0;  
 //   // handle RAW (set/get) requests  
 //   if ( pdu.type == SNMP_PDU_SET ) {  
 //    // response packet from set-request - object is read-only  
 //    pdu.type = SNMP_PDU_RESPONSE;  
 //    pdu.error = SNMP_ERR_READ_ONLY;  
 //   } else {  
 //    // response packet from get-request - RAW  
 //           if (digitalRead(11) == LOW) {  
 //            rawReading = 1; // Range : 0..1  
 //           }  
 //           status = pdu.VALUE.encode(SNMP_SYNTAX_INT, rawReading);  
 //           pdu.type = SNMP_PDU_RESPONSE;  
 //    pdu.error = status;  
 //   }  
 //   //  
 //   #ifdef DEBUG  
 //    Serial << F("RAWd11...") << rawReading << F(" ") << pdu.VALUE.size << endl;  
 //   #endif  
 //     } else if ( strcmp_P(oid, RAWd12) == 0 ) {  
 //   int rawReading = 0;  
 //   // handle RAW (set/get) requests  
 //   if ( pdu.type == SNMP_PDU_SET ) {  
 //    // response packet from set-request - object is read-only  
 //    pdu.type = SNMP_PDU_RESPONSE;  
 //    pdu.error = SNMP_ERR_READ_ONLY;  
 //   } else {  
 //    // response packet from get-request - RAW  
 //           if (digitalRead(12) == LOW) {  
 //            rawReading = 1; // Range : 0..1  
 //           }  
 //           status = pdu.VALUE.encode(SNMP_SYNTAX_INT, rawReading);  
 //           pdu.type = SNMP_PDU_RESPONSE;  
 //    pdu.error = status;  
 //   }  
 //   //  
 //   #ifdef DEBUG  
 //    Serial << F("RAWd12...") << rawReading << F(" ") << pdu.VALUE.size << endl;  
 //   #endif  
 //     } else if ( strcmp_P(oid, RAWd13) == 0 ) {  
 //   int rawReading = 0;  
 //   // handle RAW (set/get) requests  
 //   if ( pdu.type == SNMP_PDU_SET ) {  
 //    // response packet from set-request - object is read-only  
 //    pdu.type = SNMP_PDU_RESPONSE;  
 //    pdu.error = SNMP_ERR_READ_ONLY;  
 //   } else {  
 //    // response packet from get-request - RAW  
 //           if (digitalRead(13) == LOW) {  
 //            rawReading = 1; // Range : 0..1  
 //           }  
 //           status = pdu.VALUE.encode(SNMP_SYNTAX_INT, rawReading);  
 //           pdu.type = SNMP_PDU_RESPONSE;  
 //    pdu.error = status;  
 //   }  
 //   //  
 //   #ifdef DEBUG  
 //    Serial << F("RAWd13...") << rawReading << F(" ") << pdu.VALUE.size << endl;  
 //   #endif  
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
  Ethernet.begin(mac, ip);  
  //  
  //  
  pinMode(A1, INPUT);  
  pinMode(A0, INPUT);  
  pinMode(A2, INPUT);  
  pinMode(A3, INPUT);  
 // pinMode(A4, INPUT);  
  pinMode(A5, INPUT);  
 // pinMode(0, INPUT);  
 // pinMode(1, INPUT);  
  pinMode(2, INPUT);  
  pinMode(3, INPUT);  
  pinMode(4, INPUT);  
  pinMode(5, INPUT);  
  pinMode(6, INPUT);  
  pinMode(7, INPUT);  
  pinMode(8, INPUT);  
  pinMode(9, INPUT);  
 // pinMode(10, INPUT);  
 // pinMode(11, INPUT);  
 // pinMode(12, INPUT);  
 // pinMode(13, INPUT);  
 //digitalWrite(0, HIGH);  
 //digitalWrite(1, HIGH);  
 digitalWrite(2, HIGH);  
 digitalWrite(3, HIGH);  
 digitalWrite(4, HIGH);  
 digitalWrite(5, HIGH);  
 digitalWrite(6, HIGH);  
 digitalWrite(7, HIGH);  
 digitalWrite(8, HIGH);  
 digitalWrite(9, HIGH);  
 //digitalWrite(10, HIGH);  
 //digitalWrite(11, HIGH);  
 //digitalWrite(12, HIGH);  
 //digitalWrite(13, HIGH);  
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
 }  

