/*
   Thermistor.h

   Author: Jack Greer
   Created on: 3 Nov 2021
   Date last modified: 13 Nov 2021

*/

#ifndef THERMISTOR_H_
#define THERMISTOR_H_

#include "Arduino.h"

/* Arduino Hardware Defines */
#define DIGITAL_RESOLUTION   1024   // Output of analogRead() is between 0 and 1024
#define OPERATING_VOLTAGE    5.0    // analogRead()'s full scale range is 0 to 5 V; note 5 V is the operating
                                    // voltage of the Arduino when powered by the 9V battery

#define THERM_VDIV_RESISTOR  9950  // 10k Ohm resistor (measured value 9.95k) in series with thermistor

/* Thermistor Constant Defines */
#define B_CONST 3350  // From datasheet, B = 3350-3399 Kelvin
#define T_0   293.15     // Room temperature = 293.15 Kelvin
#define R_0   10000   // Resistance in ambient temperature = 10k Ohm

#define ROOM_TEMP_F   67.73   // Room temperature in Fahrenheit = 67.73 F
// TODO: note these values may be altered to correspond with actual measurements taken later

/*
    Thermistor structure - serves as an object that contains all the intrinsic parameters
    of the thermistor (T_0, R_0, B_CONST) as well as the relevant physical measurements that
    are used to interpret the temperature of the environment.
*/

class Thermistor
{
  public:
    // Public Member Functions
    Thermistor(int pin);
    void update();
    void updateVoltage();
    void updateResistance();
    void updateTemperature();

    // Public Member Variables
    int   resistance;
    float tempKelvin;
    float tempFahrenheit;
    float voltage;
    
  private:
    int   _pin;

};

/*
struct _Thermistor
{
  unsigned int resistance;      // Resistance of thermistor (contingent upon temperature)
  unsigned int tempKelvin;      // Temperature of thermistor in Kelvin
  float        tempFahrenheit;  // Temperature of thermistor in Fahrenheit
  float        voltage;         // Voltage between 10k resistor and thermistor in divider circuit, measured in V (rather than 0-1024)
};
typedef struct _Thermistor Thermistor;

// Construct Thermistor object
Thermistor Thermistor_construct();

// Get actual temperature value - will include all the other functions below
// Basically the main() function for the thermistor
void Thermistor_update(Thermistor* therm_p);

// Read voltage from divider circuit
void Thermistor_updateVoltage(Thermistor* therm_p);

// Derive resistance of thermistor using the reading from _getDividedVoltage()
void Thermistor_updateResistance(Thermistor* therm_p);

// Derive temperature (in Kelvin) using resistance value from _getThermistorResistance(), and convert to Fahrenheit
void Thermistor_updateTemperature(Thermistor* therm_p);
*/

#endif /* THERMISTOR_H_ */
