/*
  FourVariables.cpp - Libreria para recorrer 4 variables cada N ms por flanco.
  Created by Juan Gahona - Scoowy, 2019.
  Released into the public domain.
*/

// Importamos las declaraciones de arduino
// y las definiciones de nuestra libreria
#include "Arduino.h"
#include "FourVariables.h"

// Constructor de la clase FourVariables con debug
FourVariables::FourVariables(int V1, int A1, int R1, int V2, int A2, int R2, bool debug)
{
  // Creamos atributos globales y privados "_"
  _pines[0] = V1;
  _pines[1] = A1;
  _pines[2] = R1;
  _pines[3] = V2;
  _pines[4] = A2;
  _pines[5] = R2;
  _debug = debug;
  _seg = 0;

  Serial.println(sizeof(_pines));
  for (int i = 0; i < sizeof(_pines); i++)
  {
    pinMode(_pines[i], OUTPUT);
  }
}

// Funcion para cambiar los segudos que duran cada flanco
void FourVariables::changeTime(int seg)
{
  _seg = seg;
}

// Funcion que envia una señal en base al array
void FourVariables::generateSignal(int cicle)
{
  for (int i = 0; i < (int) _signals[cicle].length(); i++) {
    if (_debug) {
      Serial.print(_signals[cicle][i]);
    }
    if (digitalRead(_pines[i]) == HIGH and _signals[cicle][i] == '0')
    {
      digitalWrite(_pines[i], LOW);
    }
    else if (digitalRead(_pines[i]) == LOW and _signals[cicle][i] == '1')
    {
      digitalWrite(_pines[i], HIGH);
    }
  }
  Serial.print(" | ");
}

// Funcion que devuelve el numero de cambio
int FourVariables::changeActive(int state, int count)
{
  if (state == HIGH)
  {
    // Se comprueba en que cambio esta el ciclo actualmente
    if (count >= 0 && count <= 7)
    {
      // Inicio del primer cambio
      // count = 6;
      return 1;
    }
    else if (count >= 8 && count <= 15)
    {
      // Inicio del segundo cambio
      // count = 14;
      return 2;
    }
  }

  return 0;
}
