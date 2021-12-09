/*
   Thermistor.cpp

   Author: Jack Greer
   Created on: 13 Nov 2021
   Date last modified: 8 Dec 2021

*/

#include "Arduino.h"
#include "Thermistor.h"

/* 
 *  Simple constructor function for Thermistor object; returns initialized object assuming room temperature conditions.
 *  Really, initializing doesn't matter that much, since we only care about these parameters after the first reading, but I
 *  think it's better form to have this.
*/
Thermistor::Thermistor(int pin){
  _pin = pin;
  resistance = R_0;
  tempKelvin = T_0;
  tempFahrenheit = ROOM_TEMP_F;
  voltage = 2.5;
}

/*
   This is the most important function for the Thermistor, which ultimately derives a temperature reading from
   taking the voltage reading from the divider circuit, calculating the thermistor resistance based on voltage value,
   and then calculating the thermistor temperature based on the resistance value.
*/
void Thermistor::update(){
  Thermistor::updateVoltage();
  Thermistor::updateResistance();
  Thermistor::updateTemperature();
}

/*
 * Take an analog reading from the voltage divider and convert it into a floating point voltage value.
 */
void Thermistor::updateVoltage(){
  unsigned int analogVoltageReading = analogRead(_pin);
  // Real voltage ~= analog reading * 5 / 1024 since analogRead() takes an input of 0-5 V and outputs 0-1023
  voltage = (float) ( (analogVoltageReading * OPERATING_VOLTAGE) / DIGITAL_RESOLUTION);
}

/*
 * Based on the layout of the voltage divider circuit, and the voltage measurement taken in _updateVoltage,
 * calculate the resistance of the thermistor.
 */
void Thermistor::updateResistance(){
  // Layout of circuit: 5 V >--Thermistor--[PIN_READ_THERMISTOR]--10k-->GND
  // By voltage divider formula, V_div = (5)*(10k / (R_therm + 10k))
  // Then R_therm = (10k)*((5/V_div) - 1)
  resistance = (THERM_VDIV_RESISTOR * (OPERATING_VOLTAGE / voltage)) - THERM_VDIV_RESISTOR;
}

/*
 * Using the parameters R_0, T_0, and B_CONST (TENTATIVELY taken from the datasheet of the thermistor),
 * as well as the resistance calculated in _updateResistance, calculate the temperature of the thermistor.
 */
void Thermistor::updateTemperature(){
  // Based on the formula in the datasheet for thermistor resistance:
  // R_therm = (R_0)*exp(B_CONST * ( (1/T_therm) - (1/T_0) )
  // We can manipulate this formula to derive the following value for thermistor temperature T_therm:
  // T_therm = (B_CONST * T_0) / ((T_0 * ln(R/R0)) + B_CONST)
  tempKelvin = (B_CONST * T_0) / ( (T_0 * log((float) resistance / R_0)) + B_CONST);
  Serial.println( (float) resistance / R_0);
  Serial.println(log((float) resistance / R_0));
  Serial.println((T_0 * log((float) resistance / R_0)));

  // Convert this value to Fahrenheit using the formula K = ((F - 32) * (5/9)) + 273.15
  // From this we can derive F = ((9/5) * (K - 273.15)) + 32
  tempFahrenheit = ( 1.8 * (tempKelvin - 273.15)) + 32;
  
  return;
}
