/*
 * BoostConverter.h
 * 
 * Author: Jack Greer
 * Created on: 12 Nov 2021
 * Date last modified: 12 Nov 2021
 * 
 */

/*
#ifndef BOOSTCONVERTER_H_
#define BOOSTCONVERTER_H_
*/

#include "Arduino.h"

 /* Pins for Analog Read */
#define PIN_V_IN        A0
#define PIN_V_OUT       A2
#define PIN_V_BATTERY   A3

/* Pin for Analog Write */
#define PIN_PWM      10

/* analogVo = (V_out) * 64 (due to voltage divider resistors); analogVb = (V_battery) * 64 */
#define ANALOG_VDIV_CONVERSION_FACTOR 64

#define UPDATE_DUTY_CYCLE_WAIT 5000   // Update duty cycle every 5 seconds
#define MAX_SAFE_DUTY_CYCLE    170    // Equivalent to D = 0.67
                                      // Worst case scenario 5 V in -> 5(1/0.33) -> 15.15 V output

#define DUTY_CYCLE_UPPER_LIMIT      255    // analogWrite() duty cycle an 8 bit unsigned int, not a float
#define DESIRED_V_OUT_HIGH          2457   // = 12 V * (1024 / 5)
#define DESIRED_V_OUT_LOW           2048   // = 10 V * (1024 / 5)
#define V_B_THRESHOLD_LOW_TO_HIGH   576    // 9 V * (1024 / 5) * (5 / 16) = 576
#define V_B_THRESHOLD_HIGH_TO_LOW   550

/* 
 *  To keep the variables related to the boost converter/PWM feedback organized, I created a structure for
 *  the boost converter.
 */
struct _BoostConverter{
  unsigned int analogVi;
  unsigned int analogVo;
  unsigned int analogVb;

  unsigned int desiredVout;
  
  unsigned int dutyCycle;
};
typedef struct _BoostConverter BoostConverter;

// Construct Boost Converter object and set up PWM feedback (change prescaler on Timer 1, set digital pin 10 to output)
BoostConverter BoostConverter_construct();

// Take analog readings from V_in, V_out, V_b pins, and adjust duty cycle, output voltage accordingly.
void BoostConverter_update(BoostConverter* boost_p);

// Changes the goal output voltage of the boost converter depending on battery voltage.
//void BoostConverter_updateDesiredVout(int analogVb, int* desiredVout_p);
void BoostConverter_updateDesiredVout(BoostConverter* boost_p);

// Changes duty cycle of boost converter depending on input voltage from solar panel (AD2)
//void BoostConverter_updateDutyCycle(int analogVi, int desiredVout, int* dutyCycle8bit_p);
void BoostConverter_updateDutyCycle(BoostConverter* boost_p);

//#endif /*BOOSTCONVERTER_H_*/
