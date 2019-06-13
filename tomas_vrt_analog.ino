/*
   Programecek na ovladani cerpadla ve vrtu podle hladiny vody ve vrtu a v precepravaci nadrzi

   Cerpadlo je vypnute, kdyz:
    - hladina ve vrtu je pod VRT_SPODNI || hladina v jimce je nad JIMKA_HORNI
    - pokud ani jedno neni pravda, tak se cerpadlo zapne pokud je hladina v jimce pod JIMKA_SPODNI

*/

int PIN_VRT_HORNI = A0;
int PIN_VRT_SPODNI = A1;
int PIN_JIMKA_HORNI = A2;
int PIN_JIMKA_SPODNI = A3;

int PIN_RELE = 10;

bool cerpadloBezi = false;
bool chciNaplnitJimku = false;

enum tankState {
  EMPTY,
  MID,
  FULL
};

enum tankState jimka;
enum tankState vrt;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_VRT_HORNI, INPUT_PULLUP);
  pinMode(PIN_VRT_SPODNI, INPUT_PULLUP);
  pinMode(PIN_JIMKA_HORNI, INPUT_PULLUP);
  pinMode(PIN_JIMKA_SPODNI, INPUT_PULLUP);

  pinMode(PIN_RELE, OUTPUT);
  digitalWrite(PIN_RELE, HIGH);
}

// the loop function runs over and over again forever
void loop() {
  Serial.println(cerpadloBezi);
  Serial.println(digitalRead(PIN_RELE));
  printAnalogStates();

  delay(500);
  updateTankStates();
  controlCerpadlo();
}

void controlCerpadlo() {
  if (vrt == EMPTY) {
    stopCerpadlo();
    return;
  }
  
  if (jimka == FULL) {
    stopCerpadlo();
    chciNaplnitJimku = false;
    return;
  }

  if (!chciNaplnitJimku) {
    stopCerpadlo();

    if (jimka == EMPTY) {
      chciNaplnitJimku = true;  
    }
    return;
  } 
  
  if (chciNaplnitJimku && vrt == FULL) {
    startCerpadlo();
    return;
  }

}

void startCerpadlo() {
  if (!cerpadloBezi) {
    digitalWrite(PIN_RELE, LOW);
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("Cerpadlo start");
    printAnalogStates();
    cerpadloBezi = true;
  }
}

void stopCerpadlo() {
  if (cerpadloBezi) {
    digitalWrite(PIN_RELE, HIGH);
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Cerpadlo stop");
    printAnalogStates();
    cerpadloBezi = false;
  }
}

int getSensorPercentage(int sensorValue)
{
  return ((sensorValue * 10) / 1024 * 10);
}

bool submerged(int pin) {
  return (getSensorPercentage(analogRead(pin)) <= 70);
}

bool dry(int pin) {
  return !(submerged(pin));
}

void updateTankStates() {
  // jimka
  if (submerged(PIN_JIMKA_HORNI)) {
    jimka = FULL;
  } else if (dry(PIN_JIMKA_SPODNI)) {
    jimka = EMPTY;
  } else {
    jimka = MID;
  }

  // vrt
  if (submerged(PIN_VRT_HORNI)) {
    vrt = FULL;
  } else if (dry(PIN_VRT_SPODNI)) {
    vrt = EMPTY;
  } else {
    vrt = MID;
  }

  Serial.println("jimka " + String(jimka) + " vrt " + String(vrt) + " chciNaplnit " + String(chciNaplnitJimku)); 
}

void printAnalogStates() {
  Serial.println(String(analogRead(PIN_VRT_HORNI)) + " " + String(analogRead(PIN_VRT_SPODNI)) + " " + String(analogRead(PIN_JIMKA_HORNI)) + " " + String(analogRead(PIN_JIMKA_SPODNI)));
  Serial.println(String(getSensorPercentage(analogRead(PIN_VRT_HORNI))) + " " + String(getSensorPercentage(analogRead(PIN_VRT_SPODNI))) + " " + String(getSensorPercentage(analogRead(PIN_JIMKA_HORNI))) + " " + String(getSensorPercentage(analogRead(PIN_JIMKA_SPODNI))));
}

