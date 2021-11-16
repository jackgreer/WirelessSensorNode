/*
 * BoostConverter.h
 * 
 * Author: Jack Greer
 * Created on: 13 Nov 2021
 * Date last modified: 13 Nov 2021
 */

#ifndef BOOSTCONVERTER_H_
#define BOOSTCONVERTER_H_

#include "Arduino.h"

/* analogVo = (V_out) * 64 (due to voltage divider resistors); analogVb = (V_battery) * 64 */
#define ANALOG_VDIV_CONVERSION_FACTOR 64

#define MAX_SAFE_DUTY_CYCLE    170    // Equivalent to D = 0.67
                                      // Worst case scenario 5 V in -> 5(1/0.33) -> 15.15 V output

#define DUTY_CYCLE_UPPER_LIMIT      255    // analogWrite() duty cycle an 8 bit unsigned int, not a float
#define DESIRED_V_OUT_HIGH          2457   // = 12 V * (1024 / 5)
#define DESIRED_V_OUT_LOW           2048   // = 10 V * (1024 / 5)
#define V_B_THRESHOLD_LOW_TO_HIGH   576    // 9 V * (1024 / 5) * (5 / 16) = 576
#define V_B_THRESHOLD_HIGH_TO_LOW   550

class BoostConverter
{
  public:
    // Public Member Functions
    BoostConverter(int pin_Vin, int pin_Vout, int pin_Vb);
    void update();
    void updateDesiredVout();
    void updateDutyCycle();

    // Public Member Variables
    unsigned int analogVi;
    unsigned int analogVo;
    unsigned int analogVb;
    unsigned int desiredVout;
    unsigned int dutyCycle;
    
  private:
    int _pin_Vin;
    int _pin_Vout;
    int _pin_Vb;

};

#endif /* BOOSTCONVERTER_H_ */
