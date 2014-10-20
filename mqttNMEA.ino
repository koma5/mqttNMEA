#include <Ethernet.h>
#include <PubSubClient.h>
#include <SPI.h>

char NMEA[100] = "";
int countNMEA = 0;

// !! same as analogMetr
byte mac[]    = {  0x90, 0xA2, 0xAA, 0x33, 0x59, 0x5A };
byte server[] = { 172, 16, 0, 70 };
byte ip[]     = { 172, 16, 0, 133 };

void callback(char* topic, byte* payload, unsigned int length)
{
}
EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
}

void loop() {

  if (client.connected()) {
    client.loop();
    while (Serial.available() && client.connected()) {
      char inChar = (char)Serial.read(); 
      Serial.print(inChar);
      //Serial.println(countNMEA);
      if (inChar == '$' && countNMEA > 1) {
         Serial.println("send...");
         NMEA[countNMEA -1] = '\0';
         client.publish("vw/gps", NMEA);
         //Serial.println(NMEA);
         //for(int x = 0; NMEA[x] != '\0';x++)Serial.print(NMEA[x]);
         countNMEA = 0;
         
      }
      NMEA[countNMEA] = inChar;
      countNMEA++;
  
    }
  }
  else
  {
    client.connect("arduinoNMEA");
    //Serial.println("connecting");
  }
}

