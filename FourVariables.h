/*
  FourVariables.h - Libreria para recorrer 4 variables cada N ms por flanco.
  Created by Juan Gahona - Scoowy, 2019.
  Released into the public domain.
*/
#ifndef FourVariables_h
#define FourVariables_h

#include "Arduino.h"

class FourVariables
{
  private:
    int _pines[6];
    int _seg;
    bool _debug;
    String _signals[16] = {
      "011110", "011110", "111110", "011110",
      "111110", "011110", "101110", "101110",
      "110011", "110011", "110111", "110011",
      "110111", "110011", "110101", "110101"
    };

  public:
    FourVariables(int V1, int A1, int R1, int V2, int A2, int R2, bool debug);
    void changeTime(int seg);
    void generateSignal(int cicle);
    int changeActive(int state, int count);
};

#endif
