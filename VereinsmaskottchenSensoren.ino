#include <NewPing.h>

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

/*
  Motoren
  int R1A = 4;
  int R1B = 5;
  int L1A = 6;
  int L1B = 7;

  Bewegung
  int leftForward;
  int leftBackward;
  int rightForward;
  int rightBackward;
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
int h[4][2];
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
//Sound
//int Sound = 10;
//Serialle Kommunikation
char data[11];
int sdrop;
int incount;
int lineComplete;
int current;
unsigned long timeout;
bool timedout;

void setup() {
  pinMode(hallRearEcho, INPUT);
  pinMode(hallRearTrig, OUTPUT);
  pinMode(hallFrontRightEcho, INPUT);
  pinMode(hallFrontRightTrig, OUTPUT);
  pinMode(hallFrontMiddleEcho, INPUT);
  pinMode(hallFrontMiddleTrig, OUTPUT);
  pinMode(hallFrontLeftEcho, INPUT);
  pinMode(hallFrontLeftTrig, OUTPUT);
  h = {{hallRearTrig, hallRearEcho},
    {hallFrontRightTrig, hallFrontRightEcho},
    {hallFrontMiddleTrig, hallFrontMiddleEcho},
    {hallFrontLeftTrig, hallFrontLeftEcho}
  };

  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  pinMode(S3, INPUT);
  pinMode(S4, INPUT);
  pinMode(S5, INPUT);
  pinMode(CLP, INPUT);                            //auch in .MotorSteuerung aktiv für Notaus
  pinMode(Near, INPUT);

  //pinMode(Sound, OUTPUT);

  Serial.begin(9600);
}

//------------------------------------------------Notausfunktion: bei Vorwärtsbewegung statt delay() zu verwenden; bei Schließen des Schalters stoppt die Bewegung
/*
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
*/
//------------------------------------------------Serielle Kommunikation mit dem Pi
/*
  void readSerialData() {                                                             // Bufferform: x;x;x;x;x* mit x={0;1}
  while ( Serial.available() & incount < 10 & !lineComplete )  {
    current = Serial.read();
    if (current != 42) { //ASCII 42 == '*'                                            // solange Zeichen lesen bis das Sternchen den Datensatz abschließt
      data[incount] = current;
      incount++;
    }
    else {
      data[incount] = '\0';                                                           // puffer mit NULL Zeichen abschließen, damit das Ende der Zeichenkette durch string Operationen erkannt wird
      lineComplete = true;
    }
  }
  if (lineComplete) {                                                                 // Wenn der volle Datensatz gelesen wurde
    daten_auswerten();
    werte_beurteilen();
    reset();
    resetTimeOut();
  }
  else if (incount >= 10)  {
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
  void daten_auswerten() {
  leftForward = atoi (strtok (data, ";"));              // links forwards                                   // Zerlegen des Buffers jeweils bis zum ";"
  leftBackward = atoi (strtok (NULL, ";"));             // links backwards                                  // anschließend umformen von String zu Array
  rightForward = atoi (strtok (NULL, ";"));             // rechts forwards                                  // anschließend umformen von Array zu Integer
  rightBackward = atoi (strtok (NULL, ";"));            // rechts backwards                                 // und zuweisen zu den Variablen
  sdrop = atoi (strtok (NULL, "*"));                    // Hupe
  }
  void werte_beurteilen() {
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
  if (sdrop) {
    tone(Sound, 500);
    delay(250);
    noTone(Sound);
  }
  }
*/
/*
  void checkTimeOut() {
  if ( !timedout && millis() - timeout > 1000) {
    stopMotors();
    Serial.println("timeout");
    timedout = true;
  }
  }
*/
//------------------------------------------------HallSensorenfunktion: Es müssen die Pins übergeben werden und es wird die Entfernung zurückgegeben
/*
  int hallSensor(int pos) {                         Variante 1: händische Auswahl
  switch(pos){
    case rear:
    trig = hallRearTrig;
    echo = hallRearEcho;
    break;
    case frontRight:
    trig = hallFrontRightTrig;
    echo = hallFrontRightEcho;
    break;
    case frontMiddle:
    trig = hallFrontMiddleTrig;
    echo = hallFrontMiddleEcho;
    break;
    case frontLeft:
    trig = hallFrontLeftTrig;
    echo = hallFrontLeftEcho;
    break;
  }
  digitalWrite(trig, LOW);
  delay(5);
  digitalWrite(trig, HIGH);
  delay(10);
  digitalWrite(trig, LOW);
  dauer = pulseIn(echo, HIGH);
  entfernung = (dauer / 2) * 0.03432;             //in cm
  int ent = (int)entfernung;
  return ent;
  }
*/

void hallSensor() {                               //Variante2 2: automatischer Sensorumlauf

  for (int i = 0, i <= 3, i++) {
    TRIGGER_PIN = h[i][0];
    ECHO_PIN = h[i][1];

    NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
    
    entfernung = sonar.ping_cm;                    //in cm
    int ent [i] = entfernung;
    delay(40);                                     //zur Vermeidung von Interferrenzen der einzelnen Messungen, 29 sollte das kürzeste sein
  }
  return ent;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void loop() {
  hallSensor();
  i = 0;
  while ( Serial.available() & i < 4)  {
    Serial.println(ent[i]);
    i++;
  }
  delay(500);
}

