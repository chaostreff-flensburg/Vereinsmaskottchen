
#include <Wire.h>
#include <NewPing.h>
#define address 0x03

//Ultraschallsensoren
int hallRearEcho = 30;
int hallRearTrig = 31;
int hallFrontRightEcho = 36;
int hallFrontRightTrig = 37;
int hallFrontMiddleEcho = 35;
int hallFrontMiddleTrig = 34;
int hallFrontLeftEcho = 33;
int hallFrontLeftTrig = 32;
unsigned int entfernung[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t entfernung_request[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int distance;
int TRIGGER_PIN;
int ECHO_PIN;
int MAX_DISTANCE = 200;
int h[4][2] = {
  {hallRearTrig, hallRearEcho},
  {hallFrontRightTrig, hallFrontRightEcho},
  {hallFrontMiddleTrig, hallFrontMiddleEcho},
  {hallFrontLeftTrig, hallFrontLeftEcho}
};
int ent[4];
int i;

// A4 = SDA                 Pins die für die I2C Kommunikation am Arduino benutzt werden müssen,
// A5 = SCL                 sie werden nicht im Sektch definiert

void setup() {
  pinMode(hallRearEcho, INPUT);
  pinMode(hallRearTrig, OUTPUT);
  pinMode(hallFrontRightEcho, INPUT);
  pinMode(hallFrontRightTrig, OUTPUT);
  pinMode(hallFrontMiddleEcho, INPUT);
  pinMode(hallFrontMiddleTrig, OUTPUT);
  pinMode(hallFrontLeftEcho, INPUT);
  pinMode(hallFrontLeftTrig, OUTPUT);
  
  Serial.begin(9600);

  Wire.begin(address);                             //I2C Setup
  Wire.onRequest(requestEvent);
}

void requestEvent() {
  Wire.write(entfernung_request, 10);
}

int hallSensor(int cnt) {
  TRIGGER_PIN = h[cnt][0];
  ECHO_PIN = h[cnt][1];

  NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
  distance = 0;
  for (int i = 0; i < 2; i++) {
    distance = distance + sonar.ping_cm();          //in cm
    delay(50);                                      //zur Vermeidung von Interferrenzen der einzelnen Messungen, 29 sollte das kürzeste sein
  }
  distance = distance / 2;
  if (distance == 0){
    distance = 200;
  }
  return distance;
}

void loop() {
  for (int i = 0; i < 4; i++) {
    hallSensor(i);                                    //hinten -> rechts, mitte, links (vorne)
    entfernung[i*2+2] = distance;
  }
  
 /* for (int i = 0; i < 9; i = i + 2) {
    Serial.println(entfernung[i]);                   //zu diagnosezwecken
  }
  Serial.println("------------");
 */ 
  // zerlegen der Integer Werte in zwei Byte Werte zur I2C Übertragung
  /*
     entfernung[0]     ->    counter
     entfernung[1]     ->    ungenutzt
     entfernung[2]     ->    hinten_high_bits
     entfernung[3]     ->    hinten_low_bits
     entfernung[4]     ->    rechts_high_bits
     entfernung[5]     ->    rechts_low_bits
     entfernung[6]     ->    mitte_high_bits
     entfernung[7]     ->    mitte_low_bits
     entfernung[8]     ->    links_high_bits
     entfernung[9]     ->    links_low_bits
  */

  for (int i = 2; i < 9; i = i + 2) {
    entfernung[i + 1] = entfernung[i];                // kopieren der Messwerte
    entfernung[i] = entfernung[i] >> 8;               // verschieben der bits
  }
    entfernung[1]=entfernung[0];                      // * betrifft den counter, der Wert entfernung[0]
    entfernung[1]=entfernung[1]>>8;                   // * soll jedoch nicht verändert werden, daher
    entfernung_request[0]=(byte)entfernung[1];        // * hier eine getrennte Behandlung 
    entfernung_request[1]=(byte)entfernung[0];        // *
  
  for (int i = 2; i < 10; i++) {
    entfernung_request[i] = (byte) entfernung[i];
  }
  
  if (entfernung[0] > 60000) {                        // entfernung[0] zählt die messungen hoch damit der master prüfen kann ob es ich um neue oder alte werte handelt
    entfernung[0] = 0;
  } else {
    entfernung[0] = entfernung[0] + 1;                  
  }
}

