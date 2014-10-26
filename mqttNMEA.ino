#include <Ethernet.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <TinyGPS++.h>
#include <EEPROM.h>
#include "EEPROMAnything.h"

TinyGPSPlus gps;

char buffer[15] = "";
int len = 0;

char NMEA[100] = "";
int countNMEA = 0;

struct config_arduinoNMEA
{
    boolean publishNMEA;
    boolean publishParsed;
} configArduinoNMEA;

// !! same as analogMetr
byte mac[]    = {  0x90, 0xA2, 0xAA, 0x33, 0x59, 0x5A };
byte server[] = { 172, 16, 0, 70 };
byte ip[]     = { 172, 16, 0, 133 };

void callback(char* topic, byte* payload, unsigned int length)
{
  payload[length] = '\0';
  String message = (char *) payload;
  
  if(message == "all") {
    configArduinoNMEA.publishNMEA = true;
    configArduinoNMEA.publishParsed = true;
  }
  else if (message == "nmea") {
    configArduinoNMEA.publishNMEA = !configArduinoNMEA.publishNMEA;
    EEPROM_writeAnything(0, configArduinoNMEA);
  }
  else if (message == "parsed") {
    configArduinoNMEA.publishParsed = !configArduinoNMEA.publishParsed;
    EEPROM_writeAnything(0, configArduinoNMEA);  
  }
  else if (message == "off") {
    configArduinoNMEA.publishNMEA = false;
    configArduinoNMEA.publishParsed = false;
    EEPROM_writeAnything(0, configArduinoNMEA);
  }
}
EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

void setup() {
  EEPROM_readAnything(0, configArduinoNMEA);
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
}

void loop() {

  if (client.connected()) {
    client.loop();
    while (Serial.available() && client.connected()) {
      
      char inChar = (char)Serial.read();
      gps.encode(inChar);
      //Serial.print(inChar);
      //Serial.println(countNMEA);
      
      if(configArduinoNMEA.publishParsed && gps.altitude.isUpdated() && gps.altitude.isValid())
      {
        len = gps.altitude.meters() < 1000 ? 6 : 7;
        dtostrf((double)gps.altitude.meters(), len, 2, buffer);
        buffer[7] = '\0';
        
        client.publish("vw/gps/altitude", buffer);
      }
      
      if (configArduinoNMEA.publishNMEA) {
        if (inChar == '$' && countNMEA > 1) {
           //Serial.println("send...");
           NMEA[countNMEA -1] = '\0';
           client.publish("vw/gps/nmea", NMEA);
           //Serial.println(NMEA);
           //for(int x = 0; NMEA[x] != '\0';x++)Serial.print(NMEA[x]);
           countNMEA = 0;
           
        }
        NMEA[countNMEA] = inChar;
        countNMEA++;
      }
      
    } // end while loop
  }
  else
  {
    client.connect("arduinoNMEA");
    client.subscribe("vw/gps/command");
    //Serial.println("connecting");
  }
}

