#include <Wire.h>
#include <NewPing.h>
#define adress 0x02
/*  0x01 MotorenSteuerung
                     xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx   Debuggen   xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
*/
/*
  _________________________________
  |             O  O              |   rearhall  Echo:lila(29)  Trig:blau(28)
  | ____                     ____ |
  |rechts|                 | links|
  |      |                 |      |
  |      |                 |      |
  |      |      Sound      |      |
  |      |                 |      |
  |      |                 |      |
  | ____ |                 | ____ |
  |                               |
  |   O  O                 O  O   |   fronthall  Rechts   Echo:schwarz(36)  Trig:weiß(37)
  |             O  O              |              Mitte    Echo:orange(35)  Trig:gelb(34)
  |_______________________________|              Links    Echo:grün(33)  Trig:blau(32)
  [S5]  [S4]  [S3]  [S2]  [S1]      S1:orange(54)  S2:gelb(52)  S3:grün(50)  S4:blau(48)  S5:braun(46)
  [near]  [CLP]                     near:grau(42)  CLP:weiß(44)


*/

//Ultraschallsensoren
int hallRearEcho = 29;
int hallRearTrig = 28;
int hallFrontRightEcho = 36;
int hallFrontRightTrig = 37;
int hallFrontMiddleEcho = 35;
int hallFrontMiddleTrig = 34;
int hallFrontLeftEcho = 33;
int hallFrontLeftTrig = 32;
int entfernung;
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

//Sensorboard: Infrarot, Hardware
int S1 = 54;
int S2 = 52;
int S3 = 50;
int S4 = 48;
int S5 = 46;
int CLP = 44;
int Near = 42;

// A4 = SDA                 Pins die für die I2C Kommunikation am Arduino benutzt werden müssen,
// A5 = SCL                 sie werden nicht im Sektch definiert

int empfang = 0;
int sendData = 0;
void setup() {
  pinMode(hallRearEcho, INPUT);
  pinMode(hallRearTrig, OUTPUT);
  pinMode(hallFrontRightEcho, INPUT);
  pinMode(hallFrontRightTrig, OUTPUT);
  pinMode(hallFrontMiddleEcho, INPUT);
  pinMode(hallFrontMiddleTrig, OUTPUT);
  pinMode(hallFrontLeftEcho, INPUT);
  pinMode(hallFrontLeftTrig, OUTPUT);

  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  pinMode(S3, INPUT);
  pinMode(S4, INPUT);
  pinMode(S5, INPUT);
  pinMode(CLP, INPUT);                            //auch in MotorSteuerung aktiv für Notaus
  pinMode(Near, INPUT);

  Serial.begin(9600);

  Wire.begin(adress);                             //I2C Setup
  Wire.onReceive(readSerialData);
  Wire.onRequest(requestData);  
}

//------------------------------------------------HallSensorenfunktion: Es müssen die Pins übergeben werden und es wird die Entfernung zurückgegeben
int hallSensor(int cnt) {  
    TRIGGER_PIN = h[cnt][0];
    ECHO_PIN = h[cnt][1];

    NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

    entfernung = sonar.ping_cm();                    //in cm                                    
    return entfernung;
}

void readSerialData(int take){
  while (Wire.available()){
    empfang = Wire.read();
  }
}

void requestData(){
  Wire.write(sendData);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
void loop() {
  while ( Wire.available() )  {
    for (int i = 0; i < 4; i++) {
      sendData = hallSensor(i);
      requestData();
      delay(40);                        //zur Vermeidung von Interferrenzen der einzelnen Messungen, 29 sollte das kürzeste sein
    }
  }
}
