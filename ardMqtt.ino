#include <Servo.h>
#include <UnoWiFiDevEd.h>
//Define mqtt constants
#define CONNECTOR "mqtt"
#define outTopic "iot-assignment3/data"
#define inTopic "iot-assignment3/response"
//Define arduino pins
const int red = 3, yellow = 2, trig = 5, echo = 6, soil = A1, light = A2, light2 = A3, temp = A0, servo = 9;
unsigned long previousMillis = 0;
Servo myservo;
CiaoData cData;
String soilCondition, intruderDectectResult;
void setup() {
  Serial.begin(115200);
  Ciao.begin();
  myservo.attach(servo);
  myservo.write(0);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();
  //Get serial data send by the mqtt broker server
  cData = Ciao.read(CONNECTOR, inTopic);
  //Check if its not empty, then proceed
  if(!cData.isEmpty()) {
    String response = cData.get(2);
    int index = response.indexOf(',');
    soilCondition = response.substring(0, index);
    intruderDectectResult = response.substring(index+1);
    Serial.print(F("Response: "));
    Serial.println(soilCondition + "," + intruderDectectResult);
    //Check for server response and operate the servo motor and light up the correspond led depensing on result received
    if(soilCondition == "dry"){
      myservo.write(174);
      digitalWrite(yellow, HIGH);
    }
    //Check for server response, operate the servo motor and turn the led off
    else if (soilCondition == "wet") {
      digitalWrite(yellow, LOW);
      myservo.write(0);
    }
    //Check for server response and light up the correspond led
    if(intruderDectectResult == "intruded")
      digitalWrite(red, HIGH);
    //Check for server response and turn off the correspond led
    else if (intruderDectectResult == "safe")
      digitalWrite(red, LOW);
  }
  //Collect sensors data and send it to the mqtt broker server for processing
  if (currentMillis - previousMillis >= 2000) {
    previousMillis = currentMillis;
    int arddata[4];
    arddata[0] = analogRead (soil);
    arddata[1] = analogRead (temp);
    arddata[2] = analogRead (light);
    arddata[3] = analogRead (light2);
    Ciao.write(CONNECTOR, outTopic, String(arddata[0]) + "," + String(arddata[1]) + "," + String(arddata[2]) + "," + String(arddata[3]) + "," + String(readUltraSonic()));
    Serial.println(String(arddata[0]) + "," + String(arddata[1]) + "," + String(arddata[2]) + "," + String(arddata[3]) + "," + String(readUltraSonic()));
    if(cData.isEmpty()){
      Ciao.begin();
    }
  }
  delay(1);
}
//This function will manage the activation, receiving, and calculate the perceiving distance in cm with the ultrasonic sensor
long readUltraSonic(){
  long duration, cm;
  //Clears the trigPin
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  //Set the trigPin on HIGH state for 10us
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  //Reads the echoPin
  duration = pulseIn(echo, HIGH);
  //Calculate distance in centimeter
  cm = duration * 0.034 / 2;
  return cm;
}
