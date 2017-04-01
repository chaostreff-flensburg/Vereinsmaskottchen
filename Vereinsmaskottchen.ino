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
//Echo
  int RearHallEcho = 29;
  int RearHallTrig = 28;
  long entfernungRearHall;
  int FrontRHallEcho = 36;
  int FrontRHallTrig = 37;
  long entfernungFrontRHall;
  int FrontMHallEcho = 35;
  int FrontMHallTrig = 34;
  long entfernungFrontMHall;
  int FrontLHallEcho = 33;
  int FrontLHallTrig = 32;
  long entfernungFrontLHall;
  long dauer;
  long entfernung;
//Sensorboard
  int S1 = 54;
  int S2 = 52;
  int S3 = 50;
  int S4 = 48;
  int S5 = 46;
  int CLP = 44;
  int Near = 42;
//Sound
  int Sound = 10;
//Serialle Variablen
  char data[11];
  int slf;
  int slr;
  int srf;
  int srr;
  int sdrop;
  int incount;
  int lineComplete;
  int current;
  unsigned long timeout;
  bool timedout;

void setup() {    
  pinMode(R1A, OUTPUT);
  pinMode(R1B, OUTPUT);
  pinMode(L1A, OUTPUT);
  pinMode(L1B, OUTPUT);

  pinMode(RearHallEcho, INPUT);
  pinMode(RearHallTrig, OUTPUT);
  pinMode(FrontRHallEcho, INPUT);
  pinMode(FrontRHallTrig, OUTPUT);
  pinMode(FrontMHallEcho, INPUT);
  pinMode(FrontMHallTrig, OUTPUT);
  pinMode(FrontLHallEcho, INPUT);
  pinMode(FrontLHallTrig, OUTPUT);

  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  pinMode(S3, INPUT);
  pinMode(S4, INPUT);
  pinMode(S5, INPUT);
  pinMode(CLP, INPUT);
  pinMode(Near, INPUT);

  pinMode(Sound, OUTPUT);

  Serial.begin(9600);
}
//------------------------------------------------Bewegungsfunktionen
void backwards() {
  backwardsRight();
  backwardsLeft();
}
void backwardsRight() {
  digitalWrite(R1B, LOW);
  digitalWrite(R1A, HIGH);
}
void backwardsLeft() {
  digitalWrite(L1A, LOW);
  digitalWrite(L1B, HIGH);
}
void forwards() {
  forwardsRight();
  forwardsLeft();
}
void forwardsRight() {
  digitalWrite(R1A, LOW);
  digitalWrite(R1B, HIGH);
}
void forwardsLeft() {
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
//------------------------------------------------Notausfunktion: bei Vorwärtsbewegung statt delay() zu verwenden; bei Schließen des Schalters stoppt die Bewegung
void delayCheck(int d) {
  for (int i = 0; i <= d; i = i + 50) {
    if (digitalRead(CLP) == 1) {
      stopMotors();
      backwards();
      delay(500);
      stopMotors();

      delay(100000000);
    }
    delay(50);
  }
}
//------------------------------------------------Serielle Kommunikation mit dem Pi zur Steuerung mit einem Gamepad
void readSerialData() {                                                               // Bufferform: x;x;x;x;x* mit x={0;1}
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
  slf = atoi (strtok (data, ";"));              // links forwards                                   // Zerlegen des Buffers jeweils bis zum ";"
  slr = atoi (strtok (NULL, ";"));              // links backwards                                  // anschließend umformen von String zu Array
  srf = atoi (strtok (NULL, ";"));              // rechts forwards                                  // anschließend umformen von Array zu Integer 
  srr = atoi (strtok (NULL, ";"));              // rechts backwards                                 // und zuweisen zu den Variablen
  sdrop = atoi (strtok (NULL, "*"));            // Hupe
}
void werte_beurteilen() {                                                                           
  if ((slf == 0) && (srf == 0) && (slr == 0) && (srr == 0)) {                 
    stopMotors();
    Serial.println("Stop");
  }
  if (slf && srf) {
    forwards();
    Serial.println("Forw");
  }
  else {
    if (slf) {
      forwardsLeft();
      Serial.println("ForwL");
    }
    if (srf) {
      forwardsRight();
      Serial.println("ForwR");
    }
  }
  if (slr && srr) {
    backwards();
    Serial.println("Back");
  }
  else {
    if (slr) {
      backwardsLeft();
      Serial.println("BackL");
    }
    if (srr) {
      backwardsRight();
      Serial.println("BackR");
    }
  }
  if (sdrop) {
    tone(Sound, 500);
    delay(250);
    noTone(Sound);
  }
  Serial.print(R1A);
  Serial.print(R1B);
  Serial.print(L1A);
  Serial.println(L1B);
}
void checkTimeOut() {
  if ( !timedout && millis() - timeout > 1000) {
    stopMotors();
    Serial.println("timeout");
    timedout = true;
  }
}
//------------------------------------------------HallSensorenfunktion: Es müssen die Pins übergeben werden und es wird die Entfernung zurückgegeben
int HallSensor(int echo, int trig) {              //Pins
  digitalWrite(trig, LOW);
  delay(5);
  digitalWrite(trig, HIGH);
  delay(10);
  digitalWrite(trig, LOW);
  dauer = pulseIn(echo, HIGH);
  entfernung = (dauer / 2) * 0.03432;             //in cm
  int re = (int)entfernung;
  return re;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------
void loop() {
  //Serielle Kommunikation
  readSerialData();
  checkTimeOut();

  //Autonom
  /*
    forwards();
    delayCheck(1000);
    backwards();
    delayCheck(1000);
    stopMotors();
    delayCheck(1000);
  */
}

