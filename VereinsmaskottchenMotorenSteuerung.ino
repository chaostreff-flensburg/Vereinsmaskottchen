#include <Wire.h>
#define adress 0x01
        /*  0x02 Sensoren
         *   
         */

/*
  _________________________________
  |             O  O              |   rearhall  Echo:lila(29)  Trig:blau(28)
  | ____                     ____ |
  |rechts|                 | links|   KetteR    R1A:orange(4)   R1B:grün(5)
  |      |                 |      |   KetteL    L1A:weiß(6)     L1B:blau(7)
  |      |                 |      |
  |      |      Sound      |      |   Sound(10)
  |      |                 |      |
  |      |                 |      |
  | ____ |                 | ____ |
  |                               |
  |   O  O                 O  O   |   fronthall  Rechts   Echo:schwarz(36)  Trig:weiß(37)
  |             O  O              |              Mitte    Echo:orange(35)  Trig:gelb(34)
  |_______________________________|              Links    Echo:grün(33)  Trig:blau(32)
  [S5]  [S4]  [S3]  [S2]  [S1]      S1:orange(54)  S2:gelb(52)  S3:grün(50)  S4:blau(48)  S5:braun(46)
         [near]  [CLP]              near:grau(42)  CLP:weiß(44)


*/

//Motoren
int R1A = 4;
int R1B = 5;
int L1A = 6;
int L1B = 7;

//Bewegung
int leftForward;
int leftBackward;
int rightForward;
int rightBackward;

//Sensorboard
int CLP = 44;

//Kommunikation
char data[9];
int incount;
int lineComplete;
int current;
unsigned long timeout;
bool timedout;
// A4 = SDA                 Pins die für die I2C Kommunikation am Arduino benutzt werden müssen,
// A5 = SCL                 sie werden nicht im Sektch definiert

int sendData;
void setup() {

//Motoren
  pinMode(R1A, OUTPUT);
  pinMode(R1B, OUTPUT);
  pinMode(L1A, OUTPUT);
  pinMode(L1B, OUTPUT);
  
  pinMode(CLP, INPUT);                            //wird für Notaus benötigt

  Serial.begin(9600);                             //wird für serielle Kommunikation UND I2C benötigt

  Wire.begin(adress);                             //I2C Setup
  Wire.onReceive(readSerialData);
  Wire.onRequest(requestData);
}
//------------------------------------------------ Bewegungsfunktionen
void backward() {
  backwardRight();
  backwardLeft();
}
void backwardRight() {
  digitalWrite(R1B, LOW);
  digitalWrite(R1A, HIGH);
}
void backwardLeft() {
  digitalWrite(L1A, LOW);
  digitalWrite(L1B, HIGH);
}
void forward() {
  forwardRight();
  forwardLeft();
}
void forwardRight() {
  digitalWrite(R1A, LOW);
  digitalWrite(R1B, HIGH);
}
void forwardLeft() {
  digitalWrite(L1B, LOW);
  digitalWrite(L1A, HIGH);
}
void stopRight() {
  digitalWrite(R1A, LOW);
  digitalWrite(R1B, LOW);
}
void stopLeft() {
  digitalWrite(L1A, LOW);
  digitalWrite(L1B, LOW);
}
void stopMotors() {
  stopRight();
  stopLeft();
}
//------------------------------------------------ Notausfunktion: bei Vorwärtsbewegung statt delay() zu verwenden; bei Schließen des Schalters stoppt die Bewegung
void delayCheck(int d) {
  for (int i = 0; i <= d; i = i + 50) {
    if (digitalRead(CLP) == 1) {
      stopMotors();
      backward();
      delay(500);
      stopMotors();

      delay(100000000);
    }
    delay(50);
  }
}
//------------------------------------------------ Kommunikation mit dem Pi
void readSerialData(int take) {                                                // Bufferform: x;x;x;x* mit x={0;1}
  while ( Wire.available() & incount < 8 & !lineComplete )  {
    current = Wire.read();
    if (current != 42) { //ASCII 42 == '*'                             // solange Zeichen lesen bis das Sternchen den Datensatz abschließt
      data[incount] = current;
      incount++;
    }
    else {
      data[incount] = '\0';                                            // puffer mit NULL Zeichen abschließen, damit das Ende der Zeichenkette durch string Operationen erkannt wird
      lineComplete = true;
    }
  }
  if (lineComplete) {                                                  // Wenn der volle Datensatz gelesen wurde
    daten_checken();
    reset();
    resetTimeOut();
  }
  else if (incount >= 8)  {
    reset();
  }
}
void reset() {
  incount = 0;
  lineComplete = false;
}
void resetTimeOut() {
  timeout = millis();
  timedout = false;
}
void daten_checken(){                                   //üerprüft die empfangene Datenstruktur 
  if( (data[0] == 0 || data[0] == 1) &&
      (data[2] == 0 || data[2] == 1) &&
      (data[4] == 0 || data[4] == 1) &&
      (data[6] == 0 || data[6] == 1) ) {
        daten_auswerten();
        werte_interpretieren();
      }
  else{
    sendData = 200;                       //Data-Fehler
    requestData();                       // vom Pi zu interpretieren
  }
}
void requestData(){
  Wire.write(sendData);
}
void daten_auswerten() {
  leftForward = atoi (strtok (data, ";"));              // Zerlegen des Buffers jeweils bis zum ";"
  leftBackward = atoi (strtok (NULL, ";"));             // anschließend umformen von String zu Array
  rightForward = atoi (strtok (NULL, ";"));             // anschließend umformen von Array zu Integer
  rightBackward = atoi (strtok (NULL, "*"));            // und zuweisen zu den Variablen
}
void werte_interpretieren() {
  if ((leftForward == 0) && (rightForward == 0) && (leftBackward == 0) && (rightBackward == 0)) {
    stopMotors();
  }
  if (leftForward && rightForward) {
    forward();
  }
  else {
    if (leftForward) {
      forwardLeft();
    }
    if (rightForward) {
      forwardRight();
    }
  }
  if (leftBackward && rightBackward) {
    backward();
  }
  else {
    if (leftBackward) {
      backwardLeft();
    }
    if (rightBackward) {
      backwardRight();
    }
  }
}

void checkTimeOut() {
  if ( !timedout && millis() - timeout > 250) {
    stopMotors();
    sendData = 100;                                //timeout
    requestData();
    timedout = true;
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void loop() {

  checkTimeOut();
}
