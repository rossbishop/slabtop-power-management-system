/*
 * battery.h - library for the batterycontrol system for SLAB
 * Created by Ross Bishop
 * Copyright 2016
*/

#ifndef battery_h
#define battery_h

#include "Arduino.h"
#include "pins.h"
#include "ups.h"
#include "measure.h"
#include "adapter.h"
#include "charger.h"

class Ups;
class Charger;

class Battery

{
  public:

    //constructor definition
    Battery();

    //functions
    int getVoltage(Measure *measureRef);
    
    bool isVoltageLow(Measure *measureRef);
    String isVoltageLowString(Measure *measureRef);

    bool isOverVoltage(Measure *measureRef);

    bool isOverCurrent(Measure *measureRef, Ups *upsRef, Adapter *adapterRef);
    
    long getCurrentIn(Measure *measureRef, Adapter *adapterRef);
    long getCurrentOut(Measure *measureRef, Ups *upsRef, Adapter *adapterRef);
    
    void incrementCoulombCountIn(Measure *measureRef, Adapter *adapterRef);
    long getCoulombCountIn(Measure *measureRef);

    void incrementCoulombCountOut(Measure *measureRef, Ups *upsRef, Adapter *adapterRef);
    long getCoulombCountOut(Ups *upsRef, Measure *measureRef);

    void setInitialCapacity();
    
    long getMahIn(Measure *measureRef);
    long getMahOut(Ups *upsRef, Measure *measureRef);
    int getMahTotal(Ups *upsRef, Measure *measureRef);

    uint8_t getPercentage(Ups *upsRef, Measure *measureRef, Charger *chargerRef, Battery *batteryRef, Adapter *adapterRef);

    void setInitialChargeCapacityMah(long initialCapacity);
    
    void setFullChargeCapacityMah(Charger *chargerRef, Battery *batteryRef, Adapter *adapterRef, Measure *measureRef, Ups *upsRef);
    void isFullyDischarged(Measure *measureRef, Ups *upsRef);

    int getFullChargeMah();

  private:

    //getInputVoltage vars
    int batteryVoltage;

    //getBatteryCurrentIn vars
    long batteryCurrentIn;

    //getBatteryCurrentOut vars
    long batteryCurrentOut;

    //getBatteryCoulombsIn vars
    long batteryCoulombCountIn;

    //getCoulombCountOut vars
    long batteryCoulombCountOut;

    //getInitialCapacity vars
    int initialCapacity;

    //getMahIn vars
    long batteryMahIn;

    //getMahOut vars
    long batteryMahOut;

    //getMahTotal vars
    int batteryMahTotal;

    //getPercentage vars
    long percentage;

    //setFullChargeMah vars
    int fullChargeMah = 2000;

    long chargeCapacity;
  
};

#endif

