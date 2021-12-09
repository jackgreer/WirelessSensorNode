/*
   BoostConverter.cpp

   Author: Jack Greer
   Created on: 13 Nov 2021
   Date last modified: 8 Dec 2021

*/

#include "Arduino.h"
#include "BoostConverter.h"

/*
 * Constructor function for BoostConverter object
 */
BoostConverter::BoostConverter(int pin_Vin, int pin_Vout, int pin_Vb){
  _pin_Vin = pin_Vin;
  _pin_Vout = pin_Vout;
  _pin_Vb = pin_Vb;

  analogVi = 0;
  analogVo = 0;
  analogVb = 0;

  desiredVout = DESIRED_V_OUT_LOW;
  dutyCycle = 0;
}

/*
 * Take analog readings from V_in, V_out, V_b pins, and adjust duty cycle, output voltage accordingly.
 */
void BoostConverter::update(){
  analogVi = analogRead(_pin_Vin);
  analogVo = analogRead(_pin_Vout);
  analogVb = analogRead(_pin_Vb);

  updateDesiredVout();
  updateDutyCycle();
}

/*
   This function changes the goal output voltage of the converter, depending on the
   battery voltage. This is here to prevent too much current from running through the
   bias resistor at any given time.
*/
void BoostConverter::updateDesiredVout(){
  if((analogVb >= V_B_THRESHOLD_LOW_TO_HIGH) && (desiredVout != DESIRED_V_OUT_HIGH)){
    desiredVout = DESIRED_V_OUT_HIGH;
  }
  else if((analogVb <= V_B_THRESHOLD_HIGH_TO_LOW) && (desiredVout != DESIRED_V_OUT_LOW)){
    desiredVout = DESIRED_V_OUT_LOW;
  }
}

/*
* This function calculates the required duty cycle based on the value of the input
* voltage (in our case, the output of the solar panel/AD2). If the calculated value
* is deemed safe, the duty cycle is updated to that value; otherwise, the duty cycle
* is changed to 50%.
 */
void BoostConverter::updateDutyCycle(){
  // The following two lines result in the value of "temp" being as follows:
  // tempDutyCycle = 255 - (255*(1024/5)Vi) / (1024/5)Vo)
  unsigned long tempDutyCycle = ((DUTY_CYCLE_UPPER_LIMIT * (unsigned long) analogVi) / desiredVout); 
  tempDutyCycle = DUTY_CYCLE_UPPER_LIMIT - tempDutyCycle;
  
  if(tempDutyCycle > MAX_SAFE_DUTY_CYCLE)
    dutyCycle = 127;   // Set duty cycle to 50% by default
  else
    dutyCycle = (unsigned int) tempDutyCycle;

  return;  
}
