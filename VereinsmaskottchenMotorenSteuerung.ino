#include <Wire.h>
#define address 0x02
// A4 = SDA                 Pins die für die I2C Kommunikation am Arduino benutzt werden müssen,
// A5 = SCL                 sie werden nicht im Sektch definiert

//Motoren
int R1A = 4;
int R1B = 5;
int L1A = 6;
int L1B = 7;
int ENA_L = 8;              // Motorengeschwindigkeit
int ENA_R = 9;              //       -"-


//Bewegung
int leftForward;
int leftBackward;
int rightForward;
int rightBackward;
int motion_delay = 200;                          // dauer der Bewegung in ms

byte motion[] = {0, 0, 0, 0,0,0};
byte motionstorage[]={0,0,0,0,0,0};
bool do_motion;

void setup() {
  //Motoren
  pinMode(R1A, OUTPUT);
  pinMode(R1B, OUTPUT);
  pinMode(L1A, OUTPUT);
  pinMode(L1B, OUTPUT);
  pinMode(ENA_L, OUTPUT);
  pinMode(ENA_R, OUTPUT);
  Serial.begin(9600);
  Wire.begin(address);                             //I2C Setup
  Wire.onReceive(receiveEvent);
  do_motion = false;
}



void backwardRight() {
  digitalWrite(R1B, LOW);
  digitalWrite(R1A, HIGH);
  analogWrite(ENA_R, motion[5]);
}
void backwardLeft() {
  digitalWrite(L1A, LOW);
  digitalWrite(L1B, HIGH);
  analogWrite(ENA_L, motion[4]);
}
void forwardRight() {
  digitalWrite(R1A, LOW);
  digitalWrite(R1B, HIGH);
  analogWrite(ENA_R, motion[5]);
}
void forwardLeft() {
  digitalWrite(L1B, LOW);
  digitalWrite(L1A, HIGH);
  analogWrite(ENA_L, motion[4]);
}
void stopRight() {
  digitalWrite(R1A, LOW);
  digitalWrite(R1B, LOW);
  analogWrite(ENA_R, 0);
}
void stopLeft() {
  digitalWrite(L1A, LOW);
  digitalWrite(L1B, LOW);
  analogWrite(ENA_L, 0);
}
void stopMotors() {
  stopRight();
  stopLeft();
}


// Empfang der Komandos
void receiveEvent(int SIZE) {
  for (int i=0;i<6;i++){
    motionstorage[i] = 0;                               // löschen der zuletzt empfangenen Werte
  }
  if (SIZE != 6) {                                      // sollten mehr oder weniger als vier bytes kommen,
    while (Wire.available()) {                          // so werden diese angenommen und verworfen.
      Wire.read();
    }
    return;
  }
  else {
    for (int i = 0; i < 6; i++) {
      if (Wire.available()) {
        motionstorage[i] = Wire.read();
      }
    }
  }
  for(int i =0;i<6;i++){
    motion[i]=motionstorage[i];
  }
  do_motion = true;
}


// Umsetzen der Bewegung
void movement() {
  if (motion[0] == 0 && motion[1] == 0 && motion[2] == 0 && motion[3] == 0 ) {
    stopMotors();
  }
  if (motion[0] == 1) {
    forwardLeft();
  } else {
    if (motion[1] == 1) {
      backwardLeft();
    }
  }
  if (motion[2] == 1) {
    forwardRight();
  } else {
    if (motion[3] == 1) {
      backwardRight();
    }
  }
  //do_motion = false;
  //delay(motion_delay);
  //stopMotors();
}


void loop() {
  if (do_motion){
    movement();
  }
}
