/*
 * WirelessSensorNode.ino
 * 
 * Author: Jack Greer
 * Created on: 13 Nov 2021
 * Date last modified: 13 Nov 2021
 * 
 */

#include "BoostConverter.h"
#include "Thermistor.h"
//#include "Bluetooth.h"
#include <SoftwareSerial.h>

/* Wait Times for Recording/Reporting Data */
#define WAIT_5_SEC    5000    // 5000 ms - used for easy testing
#define WAIT_30_SEC   30000   // 30,000 ms - used for more professional testing
#define WAIT_5_MIN    300000  // 300,000 ms - in the real implementation of the circuit, data
                              // should be reported every 5 minutes

/* Bluetooth Defines */
#define PIN_RX        9
#define PIN_TX        8
#define BT_BAUDRATE   9600    // Bluetooth chip uses a baudrate of 9600

// Print the parameters of the Thermistor object to the serial monitor
void Thermistor_serialPrintStatus(Thermistor* therm_p);

// Print boost converter statistics to serial monitor
void BoostConverter_serialPrintStatus(BoostConverter* boost_p);

BoostConverter boostConverter;
Thermistor thermistor;
SoftwareSerial BTserial(PIN_RX, PIN_TX);

void setup() {
  Serial.begin(9600);
  Serial.print("Sketch:    ");
  Serial.println(__FILE__);
  Serial.print("Uploaded:  ");
  Serial.println(__DATE__);
  
  // boostConverter = BoostConverter_construct();
  thermistor = Thermistor_construct();
  // bluetooth = Bluetooth_construct();

  // Set prescaler (last three bits) of Timer 1 register to 001
  // This creates the maximum output frequency for Timer 1 (31.25 kHz)
  TCCR1B = TCCR1B & B11111000 | B00000001;

  pinMode(PIN_PWM, OUTPUT);
}

void loop() {
  static int lastPWMUpdateTime = millis();
  static int lastTemperatureUpdateTime = millis();

  int currentTime = millis();
  if( (currentTime - lastPWMUpdateTime >= UPDATE_DUTY_CYCLE_WAIT) || (currentTime < lastPWMUpdateTime) ){
    BoostConverter_update(&boostConverter);
    BoostConverter_serialPrintStatus(&boostConverter);
  }

  if( (currentTime - lastTemperatureUpdateTime >= WAIT_5_SEC) || (currentTime < lastTemperatureUpdateTime) ){
    Thermistor_update(&thermistor);
    Thermistor_serialPrintStatus(&thermistor);

    // Unsure right now as to how  bluetooth data will be managed; this is just a guess as to what it'll look like
    // Bluetooth_sendTemperatureData(&bluetooth);
  }

  //analogWrite(PIN_PWM, boostConverter.dutyCycle);
}

/*
 * Very simple function that just prints the values of the BoostConverter object to the serial monitor.
 */
void BoostConverter_serialPrintStatus(BoostConverter* boost_p){

  Serial.println("---------Incoming Data---------");
  Serial.println();

  Serial.println("INPUT VOLTAGE (0-1023):");
  Serial.println(boost_p->analogVi);
  Serial.println();

  Serial.println("OUTPUT VOLTAGE:");
  Serial.println(boost_p->analogVo);
  Serial.println();

  Serial.println("BATTERY VOLTAGE:");
  Serial.println(boost_p->analogVb);
  Serial.println();

  Serial.println("DUTY CYCLE (0-255):");
  Serial.println(boost_p->dutyCycle);
  Serial.println();
  
  Serial.println("---Converted Data---");
  Serial.println();

  Serial.println("REAL INPUT VOLTAGE (V):");
  Serial.println((float) boost_p->analogVi * 5 / 1024);
  
  Serial.println("REAL OUTPUT VOLTAGE (V):");
  Serial.println((float) boost_p->analogVo / ANALOG_VDIV_CONVERSION_FACTOR);
  Serial.println();
  
  Serial.println("REAL BATTERY VOLTAGE (V):");
  Serial.println((float) boost_p->analogVb / ANALOG_VDIV_CONVERSION_FACTOR);
  Serial.println();

  Serial.println("DUTY CYCLE (0-1):");
  Serial.println((float) boost_p->dutyCycle / 255);
  Serial.println();
  
  return;
}

/*
 * Very simple function that prints the parameters of the Thermistor object to the serial monitor.
 */
void Thermistor_serialPrintStatus(Thermistor* therm_p){

  Serial.println("----Thermistor Data----");
  Serial.println();

  Serial.println("VOLTAGE DIVIDER OUTPUT:");
  Serial.println(therm_p->voltage);

  Serial.println("THERMISTOR RESISTANCE:");
  Serial.println(therm_p->resistance);

  Serial.println("TEMPERATURE (K):");
  Serial.println(therm_p->tempKelvin);

  Serial.println("TEMPERATURE (F):");
  Serial.println(therm_p->tempFahrenheit);

  Serial.println("--END OF TRANSMISSION--");
  Serial.println("------Stay Wavvvy------");
  Serial.println();

  return;
}
