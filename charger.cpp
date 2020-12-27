#include "charger.h"

//CONSTANTS
#define CONSTANT_NAME 0

#define MIN_CHARGING_CURRENT 200
#define FULLY_CHARGED_VOLTAGE 21000

//constructor - create a function with same name as the class and no type
//              could have some variables and set in here
Charger::Charger() 
{
  
}

bool Charger::isCharging(Battery *batteryRef, Measure *measureRef, Adapter *adapterRef)
{
  //if (batteryRef->getVoltage(measureRef) < FULLY_CHARGED_VOLTAGE && batteryRef->getCurrentIn(measureRef, adapterRef) > MIN_CHARGING_CURRENT && adapterRef->getStatus(measureRef) == true)
  if (batteryRef->getCurrentIn(measureRef, adapterRef) > MIN_CHARGING_CURRENT && adapterRef->getStatus(measureRef) == true)
  {
    return true;
  }    
  else
  {
    return false;
  }
}

String Charger::isChargingString(Battery *batteryRef, Measure *measureRef, Adapter *adapterRef)
{
  return isCharging(batteryRef, measureRef, adapterRef) ? "T" : "F"; 
}

bool Charger::isFullyCharged(Battery *batteryRef, Measure *measureRef, Adapter *adapterRef)
{
  if ((isCharging(batteryRef, measureRef, adapterRef) == false) && (batteryRef->getVoltage(measureRef) >= FULLY_CHARGED_VOLTAGE) && (adapterRef->getStatus(measureRef) == true))
  {
    return true;
  }
  else
  {
    return false;
  }
}

void Charger::setChargerStatus(bool state)
{
  chargerStatus = state;
}

bool Charger::isEnabled()
{
  return chargerStatus;
}

