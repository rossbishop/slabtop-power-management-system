/*
 * ups.h - library for the batterycontrol system for SLAB
 * Created by Ross Bishop
 * Copyright 2016
*/

#ifndef ups_h
#define ups_h

#include "Arduino.h"
#include "pins.h"
#include "measure.h"
#include "battery.h"
#include "adapter.h"
#include "charger.h"

typedef enum
{
  sleep,
  active
} powerModes;

class Battery;
class Charger;

class Ups

{
  public:
    
    Ups();

    //functions
    int getCurrentOut(Measure *measureRef);
    void setBatFet(bool state);
    bool protectBattery(Battery *batteryRef, Measure *measureRef, Adapter *adapterRef, Charger *chargerRef);
    //String protectBatteryString(Battery *batteryRef, Measure *measureRef, Adapter *adapterRef);
    bool isSystemOn(Measure *measureRef, Adapter *adapterRef);
    long getPowerOut(Measure *measureRef, Adapter *adapterRef);
    powerModes getPowerMode(Measure *measureRef, Adapter *adapterRef);
    void setChargeFet(bool state);
    bool getBatFetStatus();
    String getBatFetStatusString();

    
  private:

    //getCurrentOut vars
    int currentOut;

    long power;

    bool systemStatus;

    bool batFetStatus;

    powerModes currentPowerMode;
      
};

#endif

