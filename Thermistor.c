/*
   Thermistor.c

   Author: Jack Greer
   Created on: 3 Nov 2021
   Date last modified: 13 Nov 2021

*/

#include "Thermistor.h"

/* Simple constructor function for Thermistor object; returns initialized object assuming room temperature conditions.
    Really, this doesn't matter that much, since we only care about these parameters after the first reading, but I
    think it's better form to have this.
*/
Thermistor Thermistor_construct() {

  Thermistor thermistor;
  thermistor.resistance = R_0;
  thermistor.tempKelvin = T_0;
  thermistor.tempFahrenheit = ROOM_TEMP_F;
  thermistor.voltage = 2.5;   // At room temperature, therm. resistance should be equal to 10k and voltage at divider should be 2.5 V

  return thermistor;
}

/*
   This is the most important function for the Thermistor, which ultimately derives a temperature reading from
   taking the voltage reading from the divider circuit, calculating the thermistor resistance based on voltage value,
   and then calculating the thermistor temperature based on the resistance value.
*/
void Thermistor_update(Thermistor* therm_p){
  
  Thermistor_updateVoltage(therm_p);
  Thermistor_updateResistance(therm_p);
  Thermistor_updateTemperature(therm_p);

  return;
}

/*
 * Take an analog reading from the voltage divider and convert it into a floating point voltage value.
 */
void Thermistor_updateVoltage(Thermistor* therm_p){
  unsigned int analogVoltageReading = analogRead(PIN_READ_THERMISTOR);
  // Real voltage ~= analog reading * 5 / 1024 since analogRead() takes an input of 0-5 V and outputs 0-1023
  therm_p->voltage = (float) ( (analogVoltageReading * OPERATING_VOLTAGE) / DIGITAL_RESOLUTION);

  return;
}

/*
 * Based on the layout of the voltage divider circuit, and the voltage measurement taken in _updateVoltage,
 * calculate the resistance of the thermistor.
 */
void Thermistor_updateResistance(Thermistor* therm_p){
  // Layout of circuit: 5 V >--Thermistor--[PIN_READ_THERMISTOR]--10k-->GND
  // By voltage divider formula, V_div = (5)*(10k / (R_therm + 10k))
  // Then R_therm = (10k)*((5/V_div) - 1)
  therm_p->resistance = (THERM_VDIV_RESISTOR * (OPERATING_VOLTAGE / therm_p->voltage)) - THERM_VDIV_RESISTOR;
  
  return;
}

/*
 * Using the parameters R_0, T_0, and B_CONST (TENTATIVELY taken from the datasheet of the thermistor),
 * as well as the resistance calculated in _updateResistance, calculate the temperature of the thermistor.
 */
void Thermistor_updateTemperature(Thermistor* therm_p){
  // Based on the formula in the datasheet for thermistor resistance:
  // R_therm = (R_0)*exp(B_CONST * ( (1/T_therm) - (1/T_0) )
  // We can manipulate this formula to derive the following value for thermistor temperature T_therm:
  // T_therm = (B_CONST * T_0) / ((T_0 * ln(R/R0)) + B_CONST)
  therm_p->tempKelvin = (B_CONST * T_0) / ((T_0 * log(therm_p->resistance / R_0)) + B_CONST);

  // Convert this value to Fahrenheit using the formula K = ((F - 32) * (5/9)) + 273.15
  // From this we can derive F = ((9/5) * (K - 273.15)) + 32
  therm_p->tempFahrenheit = ( 1.8 * (therm_p->tempKelvin - 273.15)) + 32;
  
  return;
}
