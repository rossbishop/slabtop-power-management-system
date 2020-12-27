#include "Arduino.h"
#include "pins.h"
#include "measure.h"
#include "adapter.h"
#include "charger.h"
#include "battery.h"
#include "ups.h"
#include "U8glib.h"
#include <EEPROMex.h>
#include <EEPROMVar.h>
#include <CapacitiveSensor.h>
#include <SPI.h>

//**CONST**
#define REPORT_TIME 1             //every second a report will be made
#define COULOMB_SAMPLE_TIME 100   //every 100ms a sample needs to be made
#define CURRENT_SAMPLE_TIME 1000  //every 1000ms a sample will be made
#define FACTORY_CAPACITY 1900 //mah value of battery pack out of factory
#define SET_CAPACITY_FLAG 0 //sets flag to checked state

//**VARS**

//U8G Selection
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);  // I2C / TWI 

//time
unsigned long currentTime;
unsigned long currentSampleTime;
unsigned long lastReportTime;
unsigned long lastCoulombSampleTime;
unsigned long lastCurrentSampleTime;
unsigned long lastCoulombTime;

//function vars
String batteryEmpty;

//funtion returns
int reportAdapterVoltage;
//String reportAdapterVoltageString;

int reportBatteryVoltage;
//String reportBatteryVoltageString;

long reportBatteryCurrentIn;
//String reportBatteryCurrentInString;

long reportBatteryCurrentOut;

long reportBatteryCoulombsIn;
//String reportBatteryCoulombsInString;
long batteryCoulombCountIn;

long reportBatteryCoulombsOut;
//String reportBatteryCoulombsOutString;
long batteryCoulombCountOut;

long reportBatteryMahIn;
//String reportBatteryMahInString;

long reportBatteryMahOut;
//String reportBatteryMahOutString;

int reportBatteryMahTotal;
String reportBatteryMahTotalString;

int reportCurrentOut;
//String reportCurrentOutString;

bool reportAcStatus;
String reportAcStatusString;

String reportChargingStatusString;
int reportChargingStatus;

String reportBatFetStatusString;

//String reportFullyChargedStatusString;
bool reportFullyChargedStatus;

String reportBatteryProtectionStatusString;

String reportVoltageLowString;

uint8_t reportBatteryPercentage;

bool reportPowerStatus;

long reportPowerOut;

int reportFullChargeMah;

long initialMahCapacity;

uint8_t batteryBitmapPercentage;

bool reportBatteryProtect;

long powerButtonStatus;

//EEPROM CONFIG
const int memBase = 350;
const uint8_t maxAllowedWrites = 100; 

//EEPROM VARS
const uint8_t factoryCapacityCheckAddress = 0;
const uint8_t chargeCapacityMahAddress = 1;
const uint8_t fullChargeCapacityMahAddress = 3;

int flag;

const unsigned char batteryBitmap [] = {
0x7F, 0xFF, 0xFF, 0x80, 0xFF, 0xFF, 0xFF, 0xC0, 0xC0, 0x00, 0x00, 0xC0, 0xC0, 0x00, 0x00, 0xC0,
0xC0, 0x00, 0x00, 0xE0, 0xC0, 0x00, 0x00, 0xF0, 0xC0, 0x00, 0x00, 0xF0, 0xC0, 0x00, 0x00, 0xF0,
0xC0, 0x00, 0x00, 0xF0, 0xC0, 0x00, 0x00, 0xE0, 0xC0, 0x00, 0x00, 0xC0, 0xC0, 0x00, 0x00, 0xC0,
0xFF, 0xFF, 0xFF, 0xC0, 0x7F, 0xFF, 0xFF, 0x80, 
};



//---------------------//

//UPS object/instance creation - create 
//object "upsInstance" 
//of type "Ups" 
//using constructor "Ups()"

//object creation
Measure measureInstance = Measure();
Ups     upsInstance     = Ups();
Adapter adapterInstance = Adapter();
Battery batteryInstance = Battery();
Charger chargerInstance = Charger();

CapacitiveSensor    cs_5_9 = CapacitiveSensor(5, 9);

void setup() 
{ 
  //pinMode(2, OUTPUT);
  //pinMode(6, INPUT);
  cs_5_9.set_CS_Timeout_Millis(2000);
  cs_5_9.set_CS_AutocaL_Millis(0xFFFFFFFF);
  Serial.begin(115200);
  
//  while (!Serial)
//  {
//    
//  }
  
  Serial.println(F("SLAB Power Management System V0.6"));
  
  EEPROM.setMemPool(memBase, EEPROMSizeNano);
  EEPROM.setMaxAllowedWrites(maxAllowedWrites);

  initialMahCapacity = EEPROM.readInt(chargeCapacityMahAddress);
  flag = EEPROM.read(factoryCapacityCheckAddress);

  Serial.println("Mah before check:");
  Serial.println(initialMahCapacity);
  Serial.println("Flag before check:");
  Serial.println(flag);

  delay(2000);
  
  //if factory capacity has never been set, then set the charge capacity of the battery in EEPROM to the factory value, then set the flag
  if (EEPROM.read(flag == 255))
  {
    EEPROM.writeInt(chargeCapacityMahAddress, FACTORY_CAPACITY);
    EEPROM.write(factoryCapacityCheckAddress, SET_CAPACITY_FLAG);
  }

  initialMahCapacity = EEPROM.readInt(chargeCapacityMahAddress);
  batteryInstance.setInitialChargeCapacityMah(initialMahCapacity);
  flag = EEPROM.read(factoryCapacityCheckAddress);

  Serial.println("Mah after check:");
  Serial.println(initialMahCapacity);
  Serial.println("Flag after check:");
  Serial.println(flag);

  //SCREEN SETTINGS
  
  // flip screen, if required
  //u8g.setRot180();
  
  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 )
    u8g.setColorIndex(255);     // white
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT )
    u8g.setColorIndex(3);         // max intensity
  else if ( u8g.getMode() == U8G_MODE_BW )
    u8g.setColorIndex(1);         // pixel on 
  //u8g.s etContrast(0x30); 
  
  pinMode(13, OUTPUT);          
  digitalWrite(13, HIGH);  
  u8g_prepare(); 

  //setup serial communication


//  // Wait for USB Serial 
//  while (!Serial) {
//    SysCall::yield();
//  }
//  delay(1000);

}

void u8g_prepare(void) 
{
  u8g.setFont(u8g_font_6x10);
  u8g.setFontRefHeightExtendedText();
  u8g.setDefaultForegroundColor();
  u8g.setFontPosTop();
}

void updateFunctions()
{
  
  //SYSTEM MANGAGEMENT
  //upsInstance.protectBattery(&batteryInstance, &measureInstance, &adapterInstance, &chargerInstance);
  
  batteryInstance.setFullChargeCapacityMah(&chargerInstance, &batteryInstance, &adapterInstance, &measureInstance, &upsInstance);
  
  //UPDATE FUNCTIONS
  reportBatteryCoulombsIn = batteryInstance.getCoulombCountIn(&measureInstance);
  reportBatteryCoulombsOut = batteryInstance.getCoulombCountOut(&upsInstance, &measureInstance);
  reportBatteryCurrentIn = batteryInstance.getCurrentIn(&measureInstance, &adapterInstance);
  reportBatteryCurrentOut = batteryInstance.getCurrentOut(&measureInstance, &upsInstance, &adapterInstance);
  reportCurrentOut = upsInstance.getCurrentOut(&measureInstance);
  reportBatteryMahIn = batteryInstance.getMahIn(&measureInstance);
  reportBatteryMahOut = batteryInstance.getMahOut(&upsInstance, &measureInstance);
  reportBatteryMahTotal = batteryInstance.getMahTotal(&upsInstance, &measureInstance);
  reportBatteryPercentage = batteryInstance.getPercentage(&upsInstance, &measureInstance, &chargerInstance, &batteryInstance, &adapterInstance);  
  reportAcStatus = adapterInstance.getStatus(&measureInstance);
  reportFullyChargedStatus = chargerInstance.isFullyCharged(&batteryInstance, &measureInstance, &adapterInstance);
  reportPowerStatus = upsInstance.isSystemOn(&measureInstance, &adapterInstance);
  reportPowerOut = upsInstance.getPowerOut(&measureInstance, &adapterInstance);
  reportFullChargeMah = batteryInstance.getFullChargeMah();

}

void loop() {

  powerButtonStatus =  cs_5_9.capacitiveSensor(30);
  
  updateFunctions();
  
  //set the current time to check for reporting
  currentTime = (millis() / 1000);
  currentSampleTime = millis();

  batteryBitmapPercentage = reportBatteryPercentage / 5;

  //Coulombs sampled every 100 milliseconds both in and out
  if (currentSampleTime > (lastCoulombSampleTime + COULOMB_SAMPLE_TIME))
  { 
    batteryInstance.incrementCoulombCountIn(&measureInstance, &adapterInstance);
    batteryInstance.incrementCoulombCountOut(&measureInstance, &upsInstance, &adapterInstance);
    lastCoulombSampleTime = millis();
  }

  if (currentTime > (lastReportTime + REPORT_TIME))
  {
    //report adapter voltage
    //reportAdapterVoltageString = "Input Voltage (mV): ";
    reportAdapterVoltage = adapterInstance.getVoltage(&measureInstance);

    //report battery voltage
    reportBatteryVoltage = batteryInstance.getVoltage(&measureInstance);
    //reportBatteryVoltageString = "Battery Voltage (mV): ";
    
    //report coulombs into battery
    //reportBatteryCoulombsInString = "Battery Charge In (mC): ";

    //report coulombs out of battery
    //reportBatteryCoulombsOutString = "Battery Charge Out (mC): ";

    //report current into battery
    //reportBatteryCurrentInString = "Current Into Battery (mA): ";
    
    //Serial.println(reportBatteryCurrentInString);
    //Serial.println(reportBatteryCurrentIn);

    //report current out of UPS
    //reportCurrentOutString = "Current Out of UPS (mA): ";

    //report mah into battery
    //reportBatteryMahInString = "Battery Capacity In (mAh): ";
    
    //report mah out of battery
    //reportBatteryMahOutString = "Battery Capacity Out (mAh): ";

    //report mah capacity of battery
    //reportBatteryMahTotalString = "Battery Capacity Total (mAh): ";
    //Serial.println(reportBatteryMahTotal);

    //report AC status
    reportAcStatusString = adapterInstance.getStatusString(&measureInstance);

    //report charging status
    reportChargingStatusString = chargerInstance.isChargingString(&batteryInstance, &measureInstance, &adapterInstance);
    //Serial.println(reportChargingStatusString);

    //report batfet status
    reportBatFetStatusString = upsInstance.getBatFetStatusString();
    
    //report fully charged   
    //reportBatteryProtectionStatusString = upsInstance.protectBatteryString(&batteryInstance, &measureInstance, &adapterInstance);

    reportVoltageLowString = batteryInstance.isVoltageLowString(&measureInstance);

    reportBatteryProtect = upsInstance.protectBattery(&batteryInstance, &measureInstance, &adapterInstance, &chargerInstance);
    
    //OUTPUT TO SCREEN
    u8g.firstPage(); 
    do 
    { 
      draw();
    } while( u8g.nextPage() );

    lastReportTime = (millis() / 1000);

    Serial.println("---");
//   Serial.println("Mah In:");
//   Serial.println(reportBatteryMahIn);
//   Serial.println("Mah Out:");
//   Serial.println(reportBatteryMahOut);
//   Serial.println("Bat Current in:");
//   Serial.println(reportBatteryCurrentIn);
//   Serial.println("Bat Current out:");
//   Serial.println(reportBatteryCurrentOut);
//   Serial.println("System Current out:");
//   Serial.println(reportCurrentOut);
//   Serial.println("---");
//   Serial.println("---");
//   Serial.println("Coulombs In:");
//   Serial.println(reportBatteryCoulombsIn);
//   Serial.println("Coulombs Out");
//   Serial.println(reportBatteryCoulombsOut);
//   Serial.println("---");
//   Serial.println("Mah Total:");
//   Serial.println(reportBatteryMahTotal);
//   Serial.println("Power Out of UPS: ");
//   Serial.println(reportPowerOut);
//   Serial.println("Is System On?: ");
//   Serial.println(reportPowerStatus);
//   Serial.println("Fully Charged?:");
//   Serial.println(reportFullyChargedStatus);
//   Serial.println("Percentage: ");
//   Serial.println(reportBatteryPercentage);
//   Serial.println("Full Mah: ");
//   Serial.println(reportFullChargeMah);
    
//   Serial.println(reportBatteryProtect);
//   Serial.println(F("Voltage Low String"));
//   Serial.println(reportVoltageLowString);
//   Serial.println(F("BatFet String:"));
//   Serial.println(reportBatFetStatusString);

    Serial.println(powerButtonStatus);
  }

}

void draw(void) 
{ 
  //report charging status
  u8g.setPrintPos(4,12);
  u8g.print(F("Chg:"));
  u8g.setPrintPos(30,12);
  u8g.print(reportChargingStatusString);  
  
  //report percentage
  u8g.setPrintPos(42,12);
  u8g.print(reportBatteryPercentage);
  u8g.setPrintPos(57,12);
  u8g.print(F("%"));

  u8g.setPrintPos(70,12);
  u8g.print(F("BFT:"));
  u8g.setPrintPos(95,12);
  u8g.print(reportBatFetStatusString);
  
  //report battery capacity total
  u8g.setPrintPos(4,36);
  u8g.print(F("Mah: "));
  u8g.setPrintPos(31,36);
  u8g.print(reportBatteryMahTotal);
  
  //report battery voltage
  u8g.setPrintPos(4,24);
  u8g.print(F("Voltage:"));
  u8g.setPrintPos(60,24);
  u8g.print(reportBatteryVoltage);

  //report AC status
  u8g.setPrintPos(4,48);
  u8g.print(F("AC:"));
  u8g.setPrintPos(25,48);
  u8g.print(reportAcStatusString);

  //show battery bitmap
  u8g.drawBitmap(40,48,4,14,batteryBitmap);
  u8g.drawBox(43,51,batteryBitmapPercentage,8);

//  //report battery protection status
//  u8g.setPrintPos(57,36);
//  u8g.print(reportBatteryProtectionStatusString);
  u8g.setPrintPos(60, 36);
  u8g.print(F("Pwr:"));
  u8g.setPrintPos(85, 36);
  u8g.print(reportPowerOut);

}



