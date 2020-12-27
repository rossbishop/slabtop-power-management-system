#include "measure.h"

//CONSTANTS
//voltage
#define ADC_STEPS_MILLIVOLTS 4.9

//current
#define CURRENT_FLOAT_VOLTAGE 2500
#define CURRENT_MULTIPLIER 10

//constructor - create a function with same name as the class and no type
//              could have some variables and set in here
Measure::Measure() 
{
  
}

int Measure::getPinVoltage(int pin)
{
  rawVoltage = analogRead(pin);
  voltage = (rawVoltage * ADC_STEPS_MILLIVOLTS) * 5;
  return voltage;
}

long Measure::getPinCurrent(int pin)
{
  rawCurrent = analogRead(pin);
  currentMv = rawCurrent * ADC_STEPS_MILLIVOLTS;
  currentMvAdjusted = currentMv - CURRENT_FLOAT_VOLTAGE;
  current = currentMvAdjusted * CURRENT_MULTIPLIER;

  //MIGHT NEED REMOVING
  if(current <= 0) current = 0;
  
  return current;
}

long Measure::getPower(int voltagePin, int currentPin)
{
  power = (getPinVoltage(voltagePin) * getPinCurrent(currentPin))/1000000;
  if (power < 0) power = 0;
  
  return power;
}

