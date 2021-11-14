/*
   BoostConverter.c

   Author: Jack Greer
   Created on: 12 Nov 2021
   Date last modified: 12 Nov 2021

*/

#include "BoostConverter.h"

/*
   Construct BoostConverter object.
*/
BoostConverter BoostConverter_construct() {
  BoostConverter boostConverter;

  boostConverter.dutyCycle = 0;
  boostConverter.desiredVout = DESIRED_V_OUT_LOW;

  // Might be unnecessary?
  boostConverter.analogVi = 0;
  boostConverter.analogVo = 0;
  boostConverter.analogVb = 0;

  return boostConverter;
}

/*
 * Take analog readings from V_in, V_out, V_b pins, and adjust duty cycle, output voltage accordingly.
 */
void BoostConverter_update(BoostConverter* boost_p){
  boost_p->analogVi = analogRead(PIN_V_IN);
  boost_p->analogVo = analogRead(PIN_V_OUT);
  boost_p->analogVb = analogRead(PIN_V_BATTERY);

  BoostConverter_updateDesiredVout(boost_p);
  BoostConverter_updateDutyCycle(boost_p);

  return;
}

/*
   This function changes the goal output voltage of the converter, depending on the
   battery voltage. This is here to prevent too much current from running through the
   bias resistor at any given time.
*/
void BoostConverter_updateDesiredVout(BoostConverter* boost_p) {

  if ((boost_p->analogVb > V_B_THRESHOLD_LOW_TO_HIGH) &&
      (boost_p->desiredVout == DESIRED_V_OUT_LOW)) {
    boost_p->desiredVout = DESIRED_V_OUT_HIGH;
  }
  else if ((boost_p->analogVb < V_B_THRESHOLD_HIGH_TO_LOW) &&
           (boost_p->desiredVout == DESIRED_V_OUT_HIGH)) {
    boost_p->desiredVout = DESIRED_V_OUT_LOW;
  }
  
  return;
}

/*
* This function calculates the required duty cycle based on the value of the input
* voltage (in our case, the output of the solar panel/AD2). If the calculated value
* is deemed safe, the duty cycle is updated to that value; otherwise, the duty cycle
* is changed to 50%.
 */
void BoostConverter_updateDutyCycle(BoostConverter* boost_p){
  // The following two lines result in the value of "temp" being as follows:
  // tempDutyCycle = 255 - (255*(1024/5)Vi) / (1024/5)Vo)
  long tempDutyCycle = ((DUTY_CYCLE_UPPER_LIMIT * (long) boost_p->analogVi) / boost_p->desiredVout); 
  tempDutyCycle = DUTY_CYCLE_UPPER_LIMIT - tempDutyCycle;
  
  if(tempDutyCycle > MAX_SAFE_DUTY_CYCLE)
    boost_p->dutyCycle = 127;   // Set duty cycle to 50% by default
  else
    boost_p->dutyCycle = (uint8_t) tempDutyCycle;

  return;  
}
