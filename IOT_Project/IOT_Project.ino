/*Weather monitoring system with Thingspeak.*/

/*Library files are included*/
#include <SFE_BMP180.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <DHT.h>

#define ALTITUDE 216.0 // Altitude in meters
#define DHTTYPE DHT11
/*objects and variables are created for these libraries*/

#define dht_dpin 0
DHT dht(dht_dpin, DHTTYPE);
SFE_BMP180 bmp;
double T, P;
char status;
WiFiClient client;

/*include the apiKey and WI-FI connection details.*/

String apiKey = "ZYSP2CKX539FWPJ6";
const char *ssid =  "Galaxy S23";
const char *pass =  "1234567890";
const char* server = "api.thingspeak.com";

/*In the setup function*/

void setup() {
  Serial.begin(9600);
  delay(10);
  bmp.begin();
  Wire.begin();
  dht.begin();
  WiFi.begin(ssid, pass);


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

/*The loop function includes all the main functions*/

void loop() {
  
  //BMP180 sensor
  char status;
  double T,P,p0,a;

  status =  bmp.startTemperature();
  if (status != 0) {
    delay(status);
    status = bmp.getTemperature(T);

    status = bmp.startPressure(3);// 0 to 3
    if (status != 0) {
      delay(status);
      status = bmp.getPressure(P, T);
      if (status != 0) {
       
   p0 = bmp.sealevel(P,ALTITUDE); // we're at 1655 meters (Boulder, CO)
          Serial.print("relative (sea-level) pressure: ");
          Serial.print(p0,2);
          Serial.print(" mb, ");
          Serial.print(p0*0.0295333727,2);
          Serial.println(" inHg");
      }
    }
  }

  //DHT11 sensor
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Serial.print("Current humidity = ");
    Serial.print(h);
    Serial.print("%  ");
    Serial.print("temperature = ");
    Serial.print(t); 
    Serial.println("C  ");

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

 


  if (client.connect(server, 80)) {
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(t);
    postStr += "&field2=";
    postStr += String(h);
    postStr += "&field3=";
    postStr += String(P, 2);
    postStr += "\r\n\r\n\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n\n\n");
    client.print(postStr);

    Serial.print("Temperature: ");
    Serial.println(t);
    Serial.print("Humidity: ");
    Serial.println(h);
    Serial.print("absolute pressure: ");
    Serial.print(P, 2);
    Serial.println("mb");
    

  }
  client.stop();
  delay(1000);
}
