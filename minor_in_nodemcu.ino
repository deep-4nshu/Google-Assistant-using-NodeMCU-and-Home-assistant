#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<body>
  <style>
body {background-color: rgb(0, 250, 253);}\

</style>
<h2>Minor Project F5 </h2>
<h3> Wi-Fi Login</h3>
 
<form action="/action_page">
  User name:<br>
  <input type="text" name="firstname" value=" ">
  <br>
  Password:<br>
  <input type="text" name="lastname" value=" ">
  <br><br>
  <input type="submit" value="Submit">
</form> 
 
</body>
</html>
)=====";

/************************* Variables *********************************/
char ch;
int y,lasty,flag=0;
volatile boolean l1,l2,n=0;
uint8_t GPIO_Pin = D2;
String page = "";
const char* ssid = "MyEsp8266";
int power;
#define WLAN_SSID       "Deep"
#define WLAN_PASS       "frhyre"
String firstName,lastName;
char uname[300],passw[300];
volatile long unsigned int debounce_time=0;
volatile int debounce_check=1000;
/************************* Adafruit.io Setup *********************************/
//#define AIO_SERVER      "192.168.88.105"        //IP address of RPi
//#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_SERVER      "io.adafruit.com"        //IP address of RPi
#define AIO_SERVERPORT  1883                   // use 8883 for SSL

#define AIO_USERNAME    "inghanshu312"
#define AIO_KEY         "621e9f75697044cea88f7e31521b17a4"

/************ Global State (you don't need to change this!) ******************/
// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
ESP8266WebServer server(8080);   //instantiate server at port 80 (http port)

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY );

/****************************** Feeds ***************************************/
Adafruit_MQTT_Publish Light = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Fan");
Adafruit_MQTT_Subscribe Light1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Fan");

/*************************** Sketch Code ************************************/
void MQTT_connect();

/////////////////////////////////////Setup/////////////////////////////
void setup() 
{
pinMode(D2,INPUT_PULLUP);   // put your setup code here, to run once:
pinMode(D6,OUTPUT);         // put your setup code here, to run once:
pinMode(D3,OUTPUT);         // put your setup code here, to run once:
pinMode(D5,OUTPUT);         // put your setup code here, to run once:
digitalWrite(D6,LOW);
digitalWrite(D3,HIGH);
digitalWrite(D5,HIGH);

Serial.begin(9600);
attachInterrupt(digitalPinToInterrupt(GPIO_Pin), IntCallback, RISING);

WiFi.mode(WIFI_AP);
WiFi.softAP(ssid);

}

/////////////////////////////////////LOOP/////////////////////////////
void loop() 
{
  attachInterrupt(digitalPinToInterrupt(GPIO_Pin), IntCallback, RISING);
//drawFontFaceDemo();

  if (WiFi.status() != WL_CONNECTED) 
  {
    delay(250);
    create_AP();
  } 
  if (WiFi.status() == WL_CONNECTED)
  {
    digitalWrite(D5,LOW);
    
  mqtt.subscribe(&Light1);
  MQTT_connect();
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(20000))) {

/////////////////////////////////////Subscription light/////////////////////////////


    if (subscription == &Light1) {
      char *Light1_State = ((char *)Light1.lastread);
      if (strcmp(Light1_State,"ON")==0)
    { ch='N';
     Serial.println(ch);
    }else
     {ch='F';
     Serial.println(ch);
     }
if (ch== 'N')
{
l1=1;
flag=1;
n=1;
}
else if (ch== 'F')
{
l1=0;
flag=1;
n=0;
}
   if (flag==1)
{
  digitalWrite(D6,l1);
  //digitalWrite(D5,l1);
flag=0;
}
 //drawFontFaceDemo();
}
 }
}}

/////////////////////////////////////Mqtt connect/////////////////////////////
void MQTT_connect() {

  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      //while (1);
     return;
     }
  }
  
//digitalWrite(D7,HIGH);
  //  delay(1000);
    //digitalWrite(D7,LOW);
  
  Serial.println("MQTT Connected!");
}

/////////////////////////////////////Physical Button/////////////////////////////
void publish_fanstatus()
{ 
  
  if (millis()-debounce_time>debounce_check)
 {
  if (l1==0)
 
{
 l1=1;
 digitalWrite(D6,l1);

 //digitalWrite(D5,l1);
 Light.publish("ON");
n=1;
}
 else if (l1==1)
{
  l1=0;
 digitalWrite(D6,l1);
 //digitalWrite(D5,l1);
 Light.publish("OFF");
 
 n=0;}
 }
 debounce_time=millis();
//drawFontFaceDemo();

}
/////////////////////////////creating access point///////////////////////////////////////////// 
void create_AP()
{
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP()); 
  attachInterrupt(digitalPinToInterrupt(GPIO_Pin), IntCallback, RISING);
  server.on("/", handleRoot);      //Which routine to handle at root location
 server.on("/action_page", handleForm); //form action is handled here
 if (strcmp(uname,"")==0)
 {
  Serial.println("Web server started!");}
  else
  {
    WiFi.mode(WIFI_STA);
   WiFi.begin("Deepanshu","A3E2E0E2");
  //WiFi.begin("Autobots","abhiabhi" );

  Serial.println(uname);
  Serial.println(passw);
     while (WiFi.status() != WL_CONNECTED)
 {
      Serial.print(".");
      delay(250);
     }
}
  server.begin();
  server.handleClient();
  delay(100);
   }
/////////////////////////////////////INTERRUPT ROUTINE/////////////////////////////
void IntCallback()
{
publish_fanstatus();
}
/////////////////////////////////////Webpage/////////////////////////////
void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}
void handleForm() {
  firstName = server.arg("firstname"); 
  lastName = server.arg("lastname"); 
 strcpy(uname, firstName.c_str());
 strcpy(passw, lastName.c_str());
 Serial.print("Username: ");
 Serial.println(firstName);
 Serial.print("Ppassword: ");
 Serial.println(lastName);
 String s = "<a href='/'> Go Back </a>";
 server.send(200, "text/html", s); //Send web page
}
