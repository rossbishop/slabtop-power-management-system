/*
 * measure.h - library for the batterycontrol system for SLAB
 * Created by Ross Bishop
 * Copyright 2016
*/

#ifndef measure_h
#define measure_h

#include "Arduino.h"
#include "pins.h"

class Measure

{
  public:

    Measure();
    
    int getPinVoltage(int pin);
    long getPinCurrent(int pin);
    long getPower(int voltagePin, int currentPin);

  private:
    
    //getPinVoltage vars
    int rawVoltage;
    int voltage;

    //getCurrentOut vars
    long rawCurrent;
    long currentMv;
    long currentMvAdjusted;
    long current;
    long currentOut;

    //getPower vars
    long power;
};

#endif

