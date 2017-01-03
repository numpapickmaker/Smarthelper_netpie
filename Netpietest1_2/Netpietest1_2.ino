#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
//#include <easyConfig.h>
#include <MicroGear.h>

//IMU MPU6050
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

#define APPID   "AppTestNP"
#define KEY     "xQuLgo2I4Haa0g1"
#define SECRET  "2YN2uWURLoT7ISJwzrL2EEsDg"
#define ALIAS   "testled"
#define ALIAS2  "smartSensor/Poten"
#define ALIAS3  "smartSensor/statusDetection"
#define ALIAS4  "smartSensor/led"
#define ALIAS5  "smartSensor/NameSurname"
#define ALIAS6  "smartSensor/WeightHeightAge"
#define NameSurname "Kitravee Siwatkittisuk"
#define WeightHeightAge "180/80/21"
#define analogPin  A0
#define LED2PIN    D4
#define LED1PIN    D0
WiFiClient client;

unsigned long timer = 0;
unsigned long timerAnalog = 0;
MicroGear microgear(client);
float  val;
#define SW_RESTROE 0
String RealTime;

MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;
int valx , valy , valz;

//ESP8266WebServer server(80);
//easyConfig conf(server);

// Initialize Wifi connection to the router
char ssid[] = "chainarin";              // your network SSID (name)
char password[] = "860790128";                              // your network key

/* If a new message arrives, do this */
void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
  Serial.print("Incoming message --> ");
  msg[msglen] = '\0';
  String mStr = String((char *)msg);
  Serial.println(mStr);
  
  if (mStr == "LED1ON") {
    digitalWrite(LED1PIN, HIGH);
  } else if (mStr == "LED1OFF") {
    digitalWrite(LED1PIN, LOW);
  } else if (mStr == "LED2ON") {
    digitalWrite(LED2PIN, HIGH);
    microgear.chat(ALIAS4, mStr);
  } else if (mStr == "LED2OFF") {
    digitalWrite(LED2PIN, LOW);
    microgear.chat(ALIAS4, mStr);
  } else if ( mStr.indexOf("#Time")>-1) {
    //setRealtime
    RealTime = mStr;

  } 

  //    if (mStr == "CHECK") {
  //      //microgear.chat(ALIAS, (String)(digitalRead(LED1PIN) ? "ON" : "OFF") + "," + (String)(digitalRead(LED2PIN) ? "ON" : "OFF"));
  //      microgear.chat(ALIAS3, "Normal");
  //    }
}

/* When a microgear is connected, do this */
void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  /* Set the alias of this microgear ALIAS */
  microgear.setName(ALIAS);
}

void BatteryADC() {
  if (timerAnalog >= 1000) {
    val = (analogRead(analogPin) * 100) / 1023; //convert to %
    microgear.chat(ALIAS2, (String)val);
    Serial.println(val);
    timerAnalog = 0;
    microgear.chat(ALIAS3, "Normal");
  }
  timerAnalog++;
}
void setInformation() {
  if (timerAnalog >= 1000) {
    microgear.chat(ALIAS5, NameSurname);
    microgear.chat(ALIAS6, WeightHeightAge);
  }
}

void IMU() {
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  valx = map(ax, -17000, 17000, 0, 179);
  valy = map(ay, -17000, 17000, 0, 179);
  valz = map(az, -17000, 17000, 0, 179);
  Serial.print("axis x = ") ;
  Serial.print(valx) ;
  Serial.print(" axis y = ") ;
  Serial.print(valy) ;
  Serial.print(" axis z = ") ;
  Serial.println(valz) ;
}

void setup() {
  //  Serial.begin(115200);
  //  Serial.println("Starting...");

  //  conf.restoreButton(SW_RESTROE, true);
  //  conf.begin(true);
  Serial.begin(115200);
  Wire.begin();
  Serial.println("Initialize MPU");
  mpu.initialize();
  Serial.println(mpu.testConnection() ? "Connected" : "Connection failed");


  pinMode(LED2PIN, OUTPUT);
  /* Add Event listeners */

  delay(10);

  // attempt to connect to Wifi network:
  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  /* Call onMsghandler() when new message arraives */
  microgear.on(MESSAGE, onMsghandler);

  /* Call onConnected() when NETPIE connection is established */
  microgear.on(CONNECTED, onConnected);

  /* Initial with KEY, SECRET and also set the ALIAS here */
  microgear.init(KEY, SECRET, ALIAS);

  /* connect to NETPIE to a specific APPID */
  microgear.connect(APPID);
}

void loop() {
  //  conf.run();

  //  if (conf.isConnected()) {
  if (microgear.connected()) {
    microgear.loop();
    setInformation();
    //BatteryADC();
  } else {
    if (millis() - timer >= 5000) {
      Serial.println("connection lost, reconnect...");
      timer = millis();
      microgear.connect(APPID);
    }


  }

}
//}
