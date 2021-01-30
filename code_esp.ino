//======================================================
//  Include headers & define constants  
//======================================================
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Servo.h>
#include <IFTTTMaker.h>
#include <WiFiClientSecure.h>
#include "mainpage.h"  //including the webpage
#include "dht.h"
#define dhtpin D4  // i/p pin for dht11 sensor 
#define IFTTT_KEY "YOUR_UNIQUE_IFTTT_KEY"	//your unique IFTTT key
#define IFTTT_FIRE_EVENT "fire_notify"		// the event name you want to trigger
#define IFTTT_PUMP_EVENT "pump_state"		
//======================================================
//  Declare & assign variables   
//======================================================
Servo servo;
dht DHT;
WiFiClientSecure client;
IFTTTMaker flame(IFTTT_KEY, client);
IFTTTMaker tank(IFTTT_KEY, client);
const int Light=D7;    // o/p pin for light
const int tnklvl=D0;   // i/p pin for tank level
const int pump=D6;	   // o/p pin for pump motor
const int fire=D5;	   // i/p read for flame
const int fire_buzz=D2;	   // o/p buzzer for flame
const int fanpin=D8;   // o/p pin for fan motor
const int door=D1;     //IR sensor pin
const int door_oc=D3;  //Action pin on door open/close 
int isFlame = HIGH;    // HIGH MEANS NO FLAME
String htm_dht;
float temp,hum;
int pmp_state=0;	   // pump state holding value
int pos=0;			   // angle for servo motor
int tank_C=0,flame_C=0;		//flag for IFTTT trigger
String led_sw,pump_sw,htm_tnk;
String htm_on="<span style=\"color:white;text-shadow:0 0 3px white;\">ON</span>";
String htm_off="<span style=\"color:red;\">OFF</span>";
//======================================================
//  Set up variables for static ip setup  
//======================================================

//IPAddress staticip(192, 168, 0, 23); 	// IP of my ESP module when using Wifi
IPAddress staticip(192, 168, 43, 23); // IP of my ESP when using hotspot
//IPAddress gateway(192, 168, 0, 1);   	// set gateway of your Wifi network 
IPAddress gateway(192, 168, 43, 1);   // set gateway of hotspot
IPAddress subnet(255, 255, 255, 0);  	// set subnet mask to match your network
//IPAddress subnet(255, 255, 248, 0);    // set subnet mask to match your network
// Set these to your desired credentials. 
const char *ssid = "YOUR_SSID";
const char *password = "YOUR_PASS";

//Declare a global object variable from the ESP8266WebServer class.
ESP8266WebServer server(80);//Server on port 80

//=======================================================================
// This routine handles main page on static ip
//=======================================================================
void handleRoot() {
  String s = MAIN_page;
  server.send(200, "text/html", s);    
}
//=======================================================================
void Humdity_Temp() {	
//Write temperature sensing code here**************
  int chk = DHT.read11(dhtpin) ;
  temp = DHT.temperature;
  hum = DHT.humidity;
 
  htm_dht="Temp:<span style=\"color:rgb(255,201,14);\">"+String(temp,1)+"<sup>C</sup></span>"+" "+"Humidity:<span style=\"color:rgb(95,197,222);\">"+String(hum,1)+"<sup>%</sup></span>";
  
  server.send(200, "text/plane", htm_dht); //Send ADC temperature humidity value only to client ajax request
}
//=======================================================================
void handlefan() {
	//Write motor driver code here************
	String fanspd = server.arg("speedval");
	int i=fanspd.toInt(),val,showval;
	showval=map(i,0,50,0,5);
	val=map(i,0,50,0,1023);
	analogWrite(fanpin,val);
	server.send(200, "text/plane", String(showval));
}
//=======================================================================
void handleled(){
	String swled = server.arg("ledval");
	//Change Light State as per request*****connected between gnd & D8
  if(swled=="ON")
  {
    led_sw=htm_on;    
    digitalWrite(Light, HIGH);       //Load1 Turned on
  }
 
  if(swled=="OFF")
  {
    led_sw=htm_off;    
    digitalWrite(Light, LOW);      //Load1 Turned off  
  }
  server.send(200, "text/plane",led_sw);
}
//=======================================================================
void handlepump(){
	String swpmp = server.arg("pmpval");
 //On/off pump motor as per request*****connected between vcc(3.3v) & D2
  if(swpmp=="ON")
  {
    pump_sw=htm_on;
    digitalWrite(pump, HIGH);       //pump Turned on
    pmp_state=1;
  }
  
  if(swpmp=="OFF")
  {
    pump_sw=htm_off;    
    digitalWrite(pump, LOW);      //pump Turned off  
    pmp_state=0;
  }
  server.send(200, "text/plane",pump_sw);
}
//=======================================================================
void Tankstat(){
 if(digitalRead(tnklvl)== HIGH){//tank full
    digitalWrite(pump, LOW);      // Turn off pump
    pump_sw=htm_off; 
    pmp_state=0;
    htm_tnk="<span style=\"color:green;\">Full</span>@"+pump_sw;
	if(tank_C==0)
		tank.triggerEvent(IFTTT_PUMP_EVENT);
	tank_C=1;
    }
  else {
	  tank_C=0;
   if(pmp_state==1)
		htm_tnk="<span style=\"color:yellow;\">Filling...</span>@"+pump_sw;
   if(pmp_state==0)
    htm_tnk="<span style=\"color:red;\">Empty</span>@"+pump_sw;
  }
  server.send(200, "text/plane", htm_tnk);
}
//=======================================================================
void Firestat(){
	String alert,htm_alrt;
  isFlame = digitalRead(fire);
  if (isFlame== LOW)
  {
	  alert="FIRE ALERT!";
   // Serial.println(alert);
	htm_alrt="<span style=\"color:red;\">"+alert+"</span>";
	if(flame_C==0)
		flame.triggerEvent(IFTTT_FIRE_EVENT);
	flame_C=1;
  }
  else
  {
	flame_C=0;
	alert="Safe";
   // Serial.println(alert);
	htm_alrt="<span style=\"color:green;\">"+alert+"</span>";
  }
  server.send(200, "text/plane", htm_alrt);
}
//=======================================================================
void DoorOC(){
	int val=0;
	String dstat;
	val = digitalRead(door); // read IR sensor value 
   if (val == HIGH)
   { // check if the input is HIGH
      pos=0; // close door 
	    dstat="<span style=\"color:silver;\">Closed</span>";
   } 
   else 
   { 
	  dstat="<span style=\"color:yellow;\">Opened</span>";
      pos=90; // open door
   } 
  server.send(200, "text/plane", dstat);
}
//=======================================================================
//                    Power on setup
//=======================================================================
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting Sketch...");
/*
//ESP32 As access point
  WiFi.mode(WIFI_AP); //Access Point mode
  WiFi.softAP(ssid, password);
*/
//ESP32 connects to your wifi -----------------------------------
  WiFi.config(staticip, subnet, gateway);
  WiFi.mode(WIFI_STA); //Connect to your wifi
  WiFi.begin(ssid, password);
  
  Serial.println("Connecting to ");
  Serial.print(ssid);
 
  //Wait for WiFi to connect
  while(WiFi.waitForConnectResult() != WL_CONNECTED){      
      Serial.print(".");
    }   
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.println("IP address: ");
  Serial.println(staticip);
//----------------------------------------------------------------
 
  server.on("/", handleRoot);        //This is display page
  server.on("/fireAlert", Firestat); //To alert on fire detect
  server.on("/doorstate", DoorOC);   //To set update of door status
  server.on("/tankstate", Tankstat); //To set update of tank status
  server.on("/readTemphu", Humdity_Temp);//To get update of temperature Value only
  server.on("/altspd", handlefan);   //To get change speed of fan
  server.on("/togled", handleled);   //Toggle led 
  server.on("/togpmp", handlepump);   //Toggle motor
  server.begin();					 //Start server                 
  Serial.println("HTTP server started");
//---------------------------------------------------------------- 
  pinMode(tnklvl, INPUT_PULLDOWN_16);
  pinMode(door, INPUT_PULLUP);pinMode(fire, INPUT_PULLUP);pinMode(dhtpin, INPUT);
  pinMode(pump, OUTPUT);pinMode(Light, OUTPUT);pinMode(fanpin, OUTPUT);pinMode(fire_buzz, OUTPUT);  
  servo.attach(door_oc);  
}
//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop() {
  server.handleClient();
  servo.write(pos);
  if(isFlame==LOW)
	  digitalWrite(fire_buzz,HIGH);
  else
	  digitalWrite(fire_buzz,LOW);
}
