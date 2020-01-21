/* Main.ino codigo que controla un prototipo de SmartCity

   Created: ma.nov.19 2019
   Processor: Arduino Uno
   Compiler:  Arduino AVR
   Autor: Juan Gahona - Scoowy
*/
#include <SoftwareSerial.h>
#include "FourVariables.h"

// Definimos las variables y constantes

// START CONSTANTES
// Pines de interrupciones
const int PIN_INT2 = 2; // Pin interrupcion de semaforo
const int PIN_INT3 = 3; // Entrada sensor de LDR
// Pines de los semaforos.
const int PIN_SV1 = 4;
const int PIN_SA1 = 5;
const int PIN_SR1 = 6;
const int PIN_SV2 = 7;
const int PIN_SA2 = 8;
const int PIN_SR2 = 9;

// Pines libres
const int PIN_B10 = 10; // Pulsador de suelo
const int PIN_B11 = 11; // BT RX
const int PIN_B12 = 12; // BT TX

// Pin de Luces
const int PIN_LIGHT = 13; // Salida de luces

// Pin pileta
const int PIN_PILETA = PIN_A0;

// Listas de colores en ciclos
const int CICLOS_LARGO[4] = {0, 1, 8 , 9};
const int CICLOS_BLINK[8] = {2, 3, 4 , 5, 10, 11, 12, 13};
const int CICLOS_CORTO[4] = {6, 7 , 14, 15};

// END CONSTANTES


// START VARIABLES
int cicle = 0;
int cicleTime = 3000;
int timeMultiplier = 1;
String bluetoothLine = "";
bool isAutoLight = true;
int isLight = LOW;
int isPileta = LOW;
// END VARIABLES

// START VOLATILES
volatile int stateSem = LOW;
// END VOLATILES

// START INSTANCIAS
SoftwareSerial SerialBT(PIN_B11, PIN_B12);
FourVariables pines(PIN_SV1, PIN_SA1, PIN_SR1, PIN_SV2, PIN_SA2, PIN_SR2, true);
// END INSTANCIAS

void setup() {
  // Comunicacion serial del terminal
  Serial.begin(9600);

  // Comunicacion Serial del Bluetooth
  SerialBT.begin(38400);

  // Set semaforos en estado inicial perteneciente al ciclo 0
  pines.generateSignal(cicle);

  // Set del pin de luces como salida
  pinMode(PIN_LIGHT, OUTPUT);

  // Set del pin de pileta como salida
  pinMode(PIN_PILETA, OUTPUT);

  // Ser del pin 10 como entrada da LOW al pulsar el boton
  pinMode(PIN_B10, INPUT_PULLUP);

  // Set del pin 2 como entrada de HIGH al pulsar el boton pull_DOWN externo
  pinMode(PIN_INT2, INPUT);
  pinMode(PIN_INT3, INPUT);

  // Set de las interrupcion RISING - Bajo > Alto | FALLING - Alto > Bajo
  attachInterrupt(digitalPinToInterrupt(PIN_INT2), petChange, RISING);
  //  attachInterrupt(digitalPinToInterrupt(PIN_INT3), toggleLight(), RISING)
}

void loop() {

  // START LEER BLUETOOTH
  if (SerialBT.available()) {
    bluetoothLine = SerialBT.readString();
    Serial.println(bluetoothLine);
    if (bluetoothLine.equals("ONAUTO")) {
      Serial.println("Entro a ONAUTO");
      isAutoLight = true;
    } else if (bluetoothLine.equals("OFFAUTO")) {
      Serial.println("Entro a OFFAUTO");
      isAutoLight = false;
    } else if (bluetoothLine.equals("ONLUC")) {
      Serial.println("Entro a ONLUC");
      isLight = HIGH;
    } else if (bluetoothLine.equals("OFFLUC")) {
      Serial.println("Entro a OFFLUC");
      isLight = LOW;
    } else if (bluetoothLine.equals("ONPIL")) {
      Serial.println("Entro a ONPIL");
      isPileta = HIGH;
    } else if (bluetoothLine.equals("OFFPIL")) {
      Serial.println("Entro a OFFPIL");
      isPileta = LOW;
    }
  }
  // END LEER BLUETOOTH

  // START CONTROL LUCES Y PILETA BLUETOOTH
  if (isAutoLight) {
    isLight = digitalRead(PIN_INT3);
    if (isLight == HIGH) {
      digitalWrite(PIN_LIGHT, HIGH);
    } else {
      digitalWrite(PIN_LIGHT, LOW);
    }
  } else {
    digitalWrite(PIN_LIGHT, isLight);
  }

  if (isPileta == HIGH) {
    digitalWrite(PIN_PILETA, HIGH);
  } else {
    digitalWrite(PIN_PILETA, LOW);
  }
  // END CONTROL LUCES Y PILETA BLUETOOTH

  // START CAMBIO SEMAFOROS
  // Obtenemos la posicion de cambio de estado de semaforos
  int flagMitad = pines.changeActive(stateSem, cicle);
  Serial.println(flagMitad);

  // Se coprueba si exite o no la peticion de cambio
  if (flagMitad != LOW) {
    Serial.print("PIN 10 es: ");
    Serial.println(digitalRead(PIN_B10));
    // Se determina si es factible el cambio de estado al no existir vehiculos esperando
    // Si la variable es LOW hay vehiculos esperando
    if (digitalRead(PIN_B10)) {
      Serial.println("No Hay vehiculos");
      if (flagMitad == 1) {
        cicle = 6;
        Serial.println("Salto al ciclo 6");
      } else {
        cicle = 14;
        Serial.println("Salto al ciclo 14");
      }
    } else {
      Serial.println("Hay vehiculos");
    }
    // Reset de la bariable volatil
    stateSem = LOW;
  }

  // END CAMBIO SEMAFOROS

  // START SECUENCIA SEMAFOROS
  // Generamos las señales pertenecientes al ciclo en cuestion
  pines.generateSignal(cicle);

  // Variamos tiempos entre colores Verde = 3s | Amarillo = 1s | Rojo = 2s por ciclo
  if (inArray(cicle, CICLOS_LARGO, sizeof(CICLOS_LARGO))) {
    Serial.print("Ciclo LARGO ");
    Serial.println(cicle);
    cicleTime = 3000;
  } else if (inArray(cicle, CICLOS_BLINK, sizeof(CICLOS_BLINK))) {
    Serial.print("Ciclo BLINK ");
    Serial.println(cicle);
    cicleTime = 1000;
  } else if (inArray(cicle, CICLOS_CORTO, sizeof(CICLOS_CORTO))) {
    Serial.print("Ciclo CORTO ");
    Serial.println(cicle);
    cicleTime = 2000;
  }

  //Tiempo de espera entre cada ciclo * un multiplicador
  delay(cicleTime * timeMultiplier);

  // Control del contador de ciclos
  cicle++;
  if (cicle == 16)
  {
    cicle = 0;
  }

  // END SECUENCIA SEMAFOROS

}

// Funcion ISR (Rutina del Servicio de Interrupciones)
void petChange() {
  stateSem = HIGH;
}


// Funcion que devuelve True si encuentra el nuemro en el array
bool inArray(int num, int numeros[], int numCiclos) {
  for (int i = 0; i < numCiclos; i++) {
    if (numeros[i] == num) {
      return true;
    }
  }
  return false;
}
