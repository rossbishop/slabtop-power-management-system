#include "adapter.h"

//CONSTANTS

//AC
#define AC_MIN_MILLIVOLTS 18000
#define MAX_CHARGER_CURRENT 14000

//constructor - create a function with same name as the class and no type
//              could have some variables set in here
Adapter::Adapter() 
{
  
}

bool Adapter::getStatus(Measure *measureRef)
{
  if (getVoltage(measureRef) > AC_MIN_MILLIVOLTS)
  {
    return true;
  }
  else
  {
    return false;
  }
}

String Adapter::getStatusString(Measure *measureRef)
{
  return getStatus(measureRef) ? "T" : "F"; 
}

int Adapter::getVoltage(Measure *measureRef)
{  
  adapterVoltage = measureRef->getPinVoltage(ADAPTER_VOLTAGE_PIN);
  return adapterVoltage;
}

int Adapter::getMaxChargerCurrent()
{
  return MAX_CHARGER_CURRENT;
}

