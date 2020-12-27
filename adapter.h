/*
 * adapter.h - library for the batterycontrol system for SLAB
 * Created by Ross Bishop
 * Copyright 2016
*/

#ifndef adapter_h
#define adapter_h

#include "Arduino.h"
#include "pins.h"
#include "measure.h"

class Adapter

{
  public:

    Adapter();
    bool getStatus(Measure *measureRef);
    String getStatusString(Measure *measureRef);
    int getVoltage(Measure *measureRef);
    int getMaxChargerCurrent();
    
  private:
    
    //getAdapterVoltage vars
    int adapterVoltage;
  
};

#endif

