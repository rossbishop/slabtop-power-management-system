/*
 * charger.h - library for the batterycontrol system for SLAB
 * Created by Ross Bishop
 * Copyright 2016
*/

#ifndef charger_h
#define charger_h

#include "Arduino.h"
#include "pins.h"
#include "battery.h"
#include "adapter.h"
#include "measure.h"
#include "ups.h"

class Battery;

class Charger

{
  public:

    Charger();

    bool isCharging(Battery *batteryRef, Measure *measureRef, Adapter *adapterRef);
    bool isFullyCharged(Battery *batteryRef, Measure *measureRef, Adapter *adapterRef);
    String isChargingString(Battery *batteryRef, Measure *measureRef, Adapter *adapterRef);
    void setChargerStatus(bool state);
    bool isEnabled();

  private:

    bool chargerStatus;
  
};

#endif

