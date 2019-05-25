#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
 
//-------- Customise these values -----------
const char* ssid = "SRK";
const char* password = "rachana22";
TinyGPSPlus gps;  // The TinyGPS++ object
    
SoftwareSerial ss(D4,D5); // The serial connection to the GPS device
    
void PublishData(float la, float ln);
 
#define ORG "mf3koe"
#define DEVICE_TYPE "NodeMCU"
#define DEVICE_ID "9876"
#define TOKEN "987654321"
//-------- Customise the above values --------
 
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/evt/sensordata/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
 
WiFiClient wifiClient;
PubSubClient client(server, 1883,wifiClient);
float latitude , longitude;
int year , month , date, hour , minute , second;

void setup()
{
 Serial.begin(115200);
 Serial.println();
 ss.begin(9600);
 Serial.print("Connecting to ");
 Serial.print(ssid);
 WiFi.begin(ssid, password); //trying to connect to the network
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 } 
 Serial.println("");
 
 Serial.print("WiFi connected, IP address: ");
 Serial.println(WiFi.localIP());
}
 
void loop()
{
while (ss.available() > 0)
        if (gps.encode(ss.read()))
        {
          if (gps.location.isValid())
          {
            latitude = gps.location.lat();
            longitude = gps.location.lng();
            Serial.println(latitude);
            Serial.println(longitude);
          }
    
          if (gps.date.isValid())
          {
            date = gps.date.day();
            month = gps.date.month();
            year = gps.date.year();
            Serial.println(date);
            Serial.println(month);
            Serial.println(year);
          }
        if (gps.time.isValid())
          {
            hour = gps.time.hour();
            minute = gps.time.minute();
            second = gps.time.second();
             minute = (minute + 30);
            if (minute > 59)
            {
              minute = minute - 60;
              hour = hour + 1;
            }
            hour = (hour + 5) ;
            if (hour > 23)
              hour = hour - 24;
            Serial.print(hour);
            Serial.print(":");
            Serial.print(minute);
            Serial.print(":");
            Serial.println(second); 
          }

         PublishData(latitude,longitude,hour,minute);
     
delay(1000);
}

}

void PublishData(float la, float ln , int hr ,int mi)
{
 if (!!!client.connected()) {
 Serial.print("Reconnecting client to ");
 Serial.println(server);
 while (!!!client.connect(clientId, authMethod, token)) {
 Serial.print(".");
 delay(500);
 }
 Serial.println();
 }
  String payload = "{\"d\":{\"latitude\":";
  payload += la;
  payload +="," "\"longitude\":";
  payload += ln;
  payload +="," "\"hour\":";
  payload += hr; 
  payload +="," "\"minute\":";
  payload += mi;
  payload += "}}";
 Serial.print("Sending payload: ");
 Serial.println(payload);
  
 if (client.publish(topic, (char*) payload.c_str())) {
 Serial.println("Publish ok");
 } else {
 Serial.println("Publish failed");
 }
}
