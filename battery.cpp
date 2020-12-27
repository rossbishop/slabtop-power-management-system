#include "battery.h"
#include <EEPROMex.h>
#include <EEPROMVar.h>

//CONSTANTS
#define LOW_VOLTAGE 16000
//MAX VALUE SET HIGHER THAN POSSIBLE TO NOT BREAK THINGS FOR THE MINUTE
//#define MAX_VOLTAGE 21250
#define MAX_VOLTAGE 21250

//MAX VALUE SET HIGHER THAN POSSIBLE TO NOT BREAK THINGS FOR THE MINUTE
//#define MAX_CURRENT 20000
#define MAX_CURRENT 26000
#define FACTORY_USABLE_CAPACITY_MAH 2000

#define FROM_MINUTES 60
#define FROM_SECONDS 60

#define INITIAL_CAPACITY 3600000       //initial capacity in millicoulombs, in this case 3.6m millicoulombs or 1000mah (40% of battery capacity; i.e. the shipped value)

//constructor - create a function with same name as the class and no type
//              could have some variables and set in here
Battery::Battery() 
{
  
}

//                      type    memory address - both passed to function in argument 
int Battery::getVoltage(Measure *measureRef)
{
  //report battery voltage
  //               pointer to the memory address of ups object, specifically to the getPinVoltage function
  batteryVoltage = measureRef->getPinVoltage(BATTERY_VOLTAGE_PIN);
  return batteryVoltage;
}

bool Battery::isVoltageLow(Measure *measureRef)
{
  if (getVoltage(measureRef) <= LOW_VOLTAGE)
  {
    return true;
  }
  else
  {
    return false;
  }
}

String Battery::isVoltageLowString(Measure *measureRef)
{
  if (isVoltageLow(measureRef) == true)
  {
    return "T";
  }
  else
  {
    return "F";
  }
}

bool Battery::isOverVoltage(Measure *measureRef)
{
  if (getVoltage(measureRef) >= MAX_VOLTAGE)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool Battery::isOverCurrent(Measure *measureRef, Ups *upsRef, Adapter *adapterRef)
{
  if (getCurrentOut(measureRef, upsRef, adapterRef) >= MAX_CURRENT)
  {
    return true;
  }
  else
  {
    return false;
  }
}

long Battery::getCurrentIn(Measure *measureRef, Adapter *adapterRef)
{
  //report battery current in
  if (measureRef->getPinCurrent(BATTERY_CURRENT_IN_PIN) <= 0 || adapterRef->getStatus(measureRef) == false)
  {
    batteryCurrentIn = 0;
  }
  else
  {
    batteryCurrentIn = measureRef->getPinCurrent(BATTERY_CURRENT_IN_PIN);
  }
  return batteryCurrentIn;
}

long Battery::getCurrentOut(Measure *measureRef, Ups *upsRef, Adapter *adapterRef)
{
  if (adapterRef->getStatus(measureRef) == false)
  { 
    if (upsRef->getCurrentOut(measureRef) <= 0)
    {
      batteryCurrentOut = 0;
    }
    else
    {
      batteryCurrentOut = upsRef->getCurrentOut(measureRef);
    }
    return batteryCurrentOut;
  }
  else
  {
    batteryCurrentOut = 0;
    return batteryCurrentOut;
  }
}

void Battery::incrementCoulombCountIn(Measure *measureRef, Adapter *adapterRef)
{
  batteryCoulombCountIn += (getCurrentIn(measureRef, adapterRef) / 10);
}

long Battery::getCoulombCountIn(Measure *measureRef)
{
  return batteryCoulombCountIn;
}

void Battery::incrementCoulombCountOut(Measure *measureRef, Ups *upsRef, Adapter *adapterRef)
{
  batteryCoulombCountOut += getCurrentOut(measureRef, upsRef, adapterRef) / 10;
}

long Battery::getCoulombCountOut(Ups *upsRef, Measure *measureRef)
{
  return batteryCoulombCountOut;
}

void Battery::setInitialChargeCapacityMah(long initialCapacity)
{
  chargeCapacity = initialCapacity;
  chargeCapacity *= FROM_SECONDS; 
  chargeCapacity *= FROM_MINUTES;
  batteryCoulombCountIn += chargeCapacity;
}

long Battery::getMahOut(Ups *upsRef, Measure *measureRef)
{
  batteryMahOut = (getCoulombCountOut(upsRef, measureRef) / FROM_MINUTES) / FROM_SECONDS;
  return batteryMahOut;
}

long Battery::getMahIn(Measure *measureRef)
{
  batteryMahIn = (getCoulombCountIn(measureRef) / FROM_MINUTES) / FROM_SECONDS;
  return batteryMahIn;
}

int Battery::getMahTotal(Ups *upsRef, Measure *measureRef)
{
  batteryMahTotal = getMahIn(measureRef) - getMahOut(upsRef, measureRef);

  if(batteryMahTotal <= 0) batteryMahTotal = 0;
  return batteryMahTotal;
}

int Battery::getFullChargeMah()
{
  return fullChargeMah;
}

uint8_t Battery::getPercentage(Ups *upsRef, Measure *measureRef, Charger *chargerRef, Battery *batteryRef, Adapter *adapterRef)
{
  //percentage = (getMahTotal(upsRef, measureRef)*100)/FACTORY_USABLE_CAPACITY_MAH;
  percentage = ((long)getMahTotal(upsRef, measureRef)*100)/fullChargeMah;

  //if detected mah in is over the previously detected maximum, display battery percentage at 99% until fully charged flag is detected
  if (percentage >= 100 && chargerRef->isFullyCharged(batteryRef, measureRef, adapterRef) == false)
  {
    percentage = 99;
  }
  
  return percentage;
}

void Battery::setFullChargeCapacityMah(Charger *chargerRef, Battery *batteryRef, Adapter *adapterRef, Measure *measureRef, Ups *upsRef)
{
  if (chargerRef->isFullyCharged(batteryRef, measureRef, adapterRef) == true)
  {
    fullChargeMah = getMahTotal(upsRef, measureRef);
    //reset coulomb and Mah values
    //set Mah in to fullChargeMah (by setting coulombs in to mah in *60*60)
    //set Mah out to 0 (by setting coulombs out to 0)
    batteryCoulombCountOut = 0;
    batteryCoulombCountIn = ((long)fullChargeMah * FROM_SECONDS) * FROM_MINUTES;
  }
}

void Battery::isFullyDischarged(Measure *measureRef, Ups *upsRef)
{
  if (isVoltageLow(measureRef) == true || getMahTotal(upsRef, measureRef) <= 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}


