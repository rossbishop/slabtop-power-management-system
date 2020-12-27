#include "ups.h"

//***CONSTANTS***//

#define POWER_ON_WATTAGE 10

Ups::Ups()
{

}

int Ups::getCurrentOut(Measure *measureRef)
{
  currentOut = measureRef->getPinCurrent(UPS_CURRENT_OUT_PIN);
  return currentOut;
}

void Ups::setChargeFet(bool state)
{
  if (state = true)
  {
    digitalWrite (CHARGE_FET_PIN, HIGH);
  }
  else if (state = false)
  {
    digitalWrite (CHARGE_FET_PIN, LOW);
  }
}

bool Ups::protectBattery(Battery *batteryRef, Measure *measureRef, Adapter *adapterRef, Charger *chargerRef)
{

    if (batteryRef->isVoltageLow(measureRef) == true && adapterRef->getStatus(measureRef) == false)
    {
      setBatFet(true);
      return true;
    }
    else
    {
      setBatFet(false);
      return false;
    }
  
}

void Ups::setBatFet(bool stated)
{
  if (stated = true)
  {
    digitalWrite (BAT_FET_PIN, HIGH);
    batFetStatus = true;
  }
  else
  {
    digitalWrite (BAT_FET_PIN, LOW);
    batFetStatus = false;
  }
}

bool Ups::getBatFetStatus()
{
  return batFetStatus;
}

String Ups::getBatFetStatusString()
{
  //return getBatFetStatus() ? "T" : "F"; 
  if (getBatFetStatus() == true)
  {
    return "T";
  }
  else
  {
    return "F";
  }
}

long Ups::getPowerOut(Measure *measureRef, Adapter *adapterRef)
{
  if (adapterRef->getStatus(measureRef) == true)
  {
    power = measureRef->getPower(ADAPTER_VOLTAGE_PIN, UPS_CURRENT_OUT_PIN);
    return power;
  }
  else
  {
    power = measureRef->getPower(BATTERY_VOLTAGE_PIN, UPS_CURRENT_OUT_PIN);
    return power;
  }     
}

bool Ups::isSystemOn(Measure *measureRef, Adapter *adapterRef)
{
  systemStatus = (getPowerOut(measureRef, adapterRef) > POWER_ON_WATTAGE) ? true : false;
  return systemStatus;
}

powerModes Ups::getPowerMode(Measure *measureRef, Adapter *adapterRef)
{
  if(isSystemOn(measureRef, adapterRef) == false)
  {
    currentPowerMode = sleep;
  }
  else if (isSystemOn(measureRef, adapterRef) == true)
  {
    currentPowerMode = active;
  }
}

