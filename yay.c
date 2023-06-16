#include <MotorDriver.h>
#include <Servo.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <ThingSpeak.h>
#include <ArduinoJson.h>

MotorDriver m;
bool on = false;
#define TRIG_PIN A2
#define ECHO_PIN A3
#define TRIG_PIN_L A0
#define ECHO_PIN_L A1
#define TRIG_PIN_R A4
#define ECHO_PIN_R A5
#define MAX_SPEED 200 // sets speed of DC  ms
#define MAX_SPEED_OFFSET 20
#define ROUNDTRIP_CM 57      //  Microseconds (uS) it takes sound to travel round-trip 1cm (2cm total), uses integer to save compiled code space.

static const int servoPin = 10;
Servo myservo;

bool goesForward = false;
int distance = 100;
int speedSet = 0;
float angle = 0;
float x = 0;
float y = 0;
float pi = 3.14;

char ssid[] = "vivo V23 5G";             //  your network SSID (name) between the " "
char pass[] = "vivo V23";                // your network password between the " "
const int Field_Number_1 = 1;
const int Field_Number_2 = 2;
String value = "";

//mqtt connection
const char* serve = "mqtt3.thingspeak.com";
const char * myWriteAPIKey = "8UGCGIC0BG4MAMW4";
const char* mqttUserName = "OiseEQULMygNJQ4FIhUHDxE";
const char* mqttPass = "U/xK0JhdmFc8qyUdp53IgdYz";
const char* clientID = "OiseEQULMygNJQ4FIhUHDxE";

int port=1883;

long writeChannelID =  1764921;

int keyIndex = 0;                 // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;      //connection status
WiFiServer server(80);            //server socket

char cse_ip[] = "esw-onem2m.iiit.ac.in";
int cse_port = 443;
WiFiClient wifi;
String cse_server = "http://"+String(cse_ip)+":"+String(cse_port)+"/~/in-cse/in-name/";

HttpClient Hclient = HttpClient(wifi, cse_ip, cse_port);
WiFiClient client = server.available();

void setup() 
{
  Serial.begin(9600);
  ThingSpeak.begin(client);
  enable_WiFi();
  connect_WiFi();
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN_R, INPUT);
  pinMode(TRIG_PIN_R, OUTPUT);
  pinMode(ECHO_PIN_L, INPUT);
  pinMode(TRIG_PIN_L, OUTPUT);

  server.begin();
  printWifiStatus();
  
  myservo.attach(10);
  myservo.write(130);
  delay(100);
  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);
}

void loop() 
{
  int distance, distance_l, distance_r;
  int duration, duration_l, duration_r;
  
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(15);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;
  Serial.print("Distance: ");
  Serial.println(distance);

  //delay(10);
  
  // left sensor
  digitalWrite(TRIG_PIN_L, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN_L, HIGH);
  delayMicroseconds(15);
  digitalWrite(TRIG_PIN_L, LOW);

  duration_l = pulseIn(ECHO_PIN_L, HIGH);
  distance_l = duration_l * 0.034 / 2;
  Serial.print("Distance left = ");
  Serial.println(distance_l);

  //delay(10);

  // right sensor
  digitalWrite(TRIG_PIN_R, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN_R, HIGH);
  delayMicroseconds(15);
  digitalWrite(TRIG_PIN_R, LOW);

  duration_r = pulseIn(ECHO_PIN_R, HIGH);
  distance_r = duration_r * 0.034 / 2;
  Serial.print("Distance right = ");
  Serial.println(distance_r);
  //delay(10);

//  if(distance <= 40 && distance_r <= 20 && distance_l <=20)
//  {
//    moveBackward();
//    delay(1000);
//  }
//  if (distance <= 40 && distance >= 0)
//  { 
//    moveStop();
//   delay(100);
//    moveBackward();
//   
//    // moveStop();
//    // delay(10);
//    if(distance_r < 20)
//    {
//      turnLeft();
//       delay(100);
//      //moveStop();
//    }
//   
//    if (distance_l < 20)
//    {
//      turnRight();
//       delay(100);
//    }
//
//    String val = String(distance); 
//    createCI(val);
//
//    // int left = lookLeft();
//    // delay(200);
//    // int right = lookRight();
//    // delay(200);
//    // if (left >= right)
//    // {
//    //   turnLeft();
//    //   moveStop();
//    // }
//    // else
//    // {
//    //   turnLeft();
//    //   //moveStop();
//    // }
//  }
//
//  else if (distance_l < 20)
//  {
//    turnRight();
//  }
//
//  else if (distance_r < 20)
//  {
//    turnLeft();
//  }
// 
//    moveForward();
//    delay(10);
////    value += (String) distance;
////    upload();

   if(distance_r < 20)
    {
      turnLeft();
       delay(100);
      //moveStop();
    }
   
    if (distance_l < 20)
    {
      turnRight();
       delay(100);
    }

//    if (distance_l < 20 && distance_r < 20)
//    {
//      rotate();
//      delay(100);
//    }
    

  if (distance <= 30 && distance >= 0)
  {
    moveStop();
    delay(100);
    moveBackward();
    delay(300);
    moveStop();
    delay(200);

    unsigned long t = millis();
    String val = String(t) + " " + String(x) + " " + String(y); 
    createCI(val);
    
    int left = lookLeft();
    delay(200);
    int right = lookRight();
    delay(200);
    if (left >= right)
    {
      turnLeft();
      moveStop();
    }
    else
    {
      turnRight();
      moveStop();
    }
  }
  else
  {
    moveForward();
    delay(10);
  }


}

int lookRight()
{
  Serial.println("Right there");
  myservo.write(50);
  delay(1000);
  Serial.print("Right-> ");
  int distance = readPing();
  delay(500);
  myservo.write(130);
  return distance;
}

int lookLeft()
{
  Serial.println("Left there");
  myservo.write(200);
  delay(1000);
  Serial.print("Left-> ");
  int distance = readPing();
  delay(500);
  myservo.write(130);
  return distance;
  delay(100);
}

int readPing() 
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(15);
  digitalWrite(TRIG_PIN, LOW);
  int distance;
  int duration;
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;
  Serial.print("UV: ");
  Serial.println(distance);
  return  distance;
}

void moveStop() 
{
  m.motor(1, FORWARD, 0);
  m.motor(2, FORWARD, 0);
  m.motor(3, FORWARD, 0);
  m.motor(4, FORWARD, 0);
  return;
}

void moveForward() {

  if (!goesForward)
  {
    goesForward = true;
    m.motor(1, FORWARD, speedSet);
    m.motor(2, FORWARD, speedSet);
    m.motor(3, FORWARD, speedSet);
    m.motor(4, FORWARD, speedSet);
    x = x + speedSet * cos(angle * pi / 180);
    y = y + speedSet * sin(angle * pi / 180);

    for (speedSet = 75; speedSet < MAX_SPEED; speedSet += 2) // slowly bring the speed up to avoid loading down the batteries too quickly
    {
      m.motor(1, FORWARD, speedSet);
      m.motor(2, FORWARD, speedSet);
      m.motor(3, FORWARD, speedSet);
      m.motor(4, FORWARD, speedSet);
      x = x + speedSet * cos(angle * pi / 180);
      y = y + speedSet * sin(angle * pi / 180);
      delay(5);
    }

  }
}

void moveBackward() {
  goesForward = false;
  m.motor(1, BACKWARD, speedSet);
  m.motor(2, BACKWARD, speedSet);
  m.motor(3, BACKWARD, speedSet);
  m.motor(4, BACKWARD, speedSet);
  x = x - speedSet * cos(angle * pi / 180);
  y = y - speedSet * sin(angle * pi / 180);
  for (speedSet = 75; speedSet < MAX_SPEED; speedSet += 2) // slowly bring the speed up to avoid loading down the batteries too quickly
  {
    m.motor(1, BACKWARD, speedSet);
    m.motor(2, BACKWARD, speedSet);
    m.motor(3, BACKWARD, speedSet);
    m.motor(4, BACKWARD, speedSet);
    x = x - speedSet * cos(angle * pi / 180);
    y = y - speedSet * sin(angle * pi / 180);
    delay(5);
  }
}

void rotate() 
{
  Serial.println("Rotating");
  m.motor(1, FORWARD, speedSet);
  m.motor(2, BACKWARD, speedSet);
  m.motor(3, BACKWARD, speedSet);
  m.motor(4, FORWARD, speedSet);
  angle -= 180;
  delay(1000);
  m.motor(1, FORWARD, speedSet);
  m.motor(2, FORWARD, speedSet);
  m.motor(3, FORWARD, speedSet);
  m.motor(4, FORWARD, speedSet);
  x = x + speedSet * cos(angle * pi / 180);
  y = y + speedSet * sin(angle * pi / 180);
}

void turnRight() 
{
  Serial.println("Turning Right");
  m.motor(1, FORWARD, speedSet);
  m.motor(2, BACKWARD, speedSet);
  m.motor(3, BACKWARD, speedSet);
  m.motor(4, FORWARD, speedSet);
  angle -= 15;
  delay(500);
  m.motor(1, FORWARD, speedSet);
  m.motor(2, FORWARD, speedSet);
  m.motor(3, FORWARD, speedSet);
  m.motor(4, FORWARD, speedSet);
  x = x + speedSet * cos(angle * pi / 180);
  y = y + speedSet * sin(angle * pi / 180);
}

void turnLeft() 
{
  Serial.println("Turning Left");
  m.motor(1, BACKWARD, speedSet);
  m.motor(2, FORWARD, speedSet);
  m.motor(3, FORWARD, speedSet);
  m.motor(4, BACKWARD, speedSet);

  angle += 15;
  delay(500);
  m.motor(1, FORWARD, speedSet);
  m.motor(2, FORWARD, speedSet);
  m.motor(3, FORWARD, speedSet);
  m.motor(4, FORWARD, speedSet);
  x = x + speedSet * cos(angle * pi / 180);
  y = y + speedSet * sin(angle * pi / 180);
}

void createCI(String val){
  Serial.print(val);
  uint8_t key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
  int i;
  char data[] = "0123456789012345";
  for(i=0;i<val.length();i++){
    data[i]=val[i];
  }
  while(i<16){
    data[i]="A";
    i++;
  }
  //aes256_enc_single(key, data);
  String body = "{\"m2m:cin\": {\"lbl\": [ \"Team-11\" ],\"con\": \"" + String(val)+ "\"}}";
  Hclient.beginRequest();
  Hclient.post("/~/in-cse/in-name/Team-11/Node-1/Data/");
  Hclient.sendHeader("Content-Length", body.length());
  Hclient.sendHeader("X-M2M-Origin", "SOUhjO:qA#OML");
  Hclient.sendHeader("Content-Type", "application/json;ty=4");
  Hclient.sendHeader("Connection", "keep-alive");
  Hclient.beginBody();

  Serial.println(body);
  Hclient.print(body);
  Hclient.endRequest();
  int status = Hclient.responseStatusCode();
  String responseBody = Hclient.responseBody();
  Serial.println(status);
  Serial.println(responseBody);
  delay(1000); 
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

}

void enable_WiFi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
}

void connect_WiFi() {
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
}


void upload()
{
  ThingSpeak.writeField(writeChannelID, Field_Number_1, value , myWriteAPIKey);
  delay(1000);
  value = "";
}