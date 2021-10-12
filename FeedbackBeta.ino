// Filename:           FeedbackBeta.ino
// Author:             Jack Greer
// Created:            September 29th, 2021
// Date Last Modified: October 12th, 2021
// Description:        Early draft of PWM feedback control for DC/DC boost converter.
//                     Uses the voltage difference across resistor R_bias to determine
//                     the output current; if it's too low, increase the duty cycle, and
//                     if it's too high, set the duty cycle to a low value.

#define pinVo A0              // V_out read from pin A0
#define pinVb A1              // V_battery read from pin A1

#define LOWER_ANALOG_DIFFERENCE_LIMIT 126   // Minimum voltage drop across R_bias = 1.98 V
#define UPPER_ANALOG_DIFFERENCE_LIMIT 168 // Maximum voltage drop across R_bias = 2.64 V

#define ANALOG_VDIV_CONVERSION_FACTOR 64

#define UPDATE_DUTY_CYCLE_WAIT 5000     // Update duty cycle every 5 seconds

void updateDutyCycle(int analogVo, int analogVb, float* dutyCycle_p);
void serialPrintBatteryStatus(int analogVo, int analogVb, float dutyCycle);

void setup() {
  // Set prescaler (last three bits) of Timer 1 register to 001
  // This creates the maximum output frequency for Timer 1 (31.25 kHz)
  TCCR1B = TCCR1B & B11111000 | B00000001;

  pinMode(10, OUTPUT);
  Serial.begin(9600); // This will be used for reading voltage during testing
}

void loop() {

/*
  const int loadResistance = 22; // Measured in Ohms
  
  float batteryVoltage; // Measured in V
  float outputVoltage;  // Measured in V
  float outputCurrent;  // Measured in mA
*/
  
  static int lastPWMUpdateTime = millis();
  static float dutyCycle = 0.25;
  
  int analogVb;   // Measured 0-1023
  int analogVo;   // Measured 0-1023
  int currentTime;

/*
  // TODO: Write code that takes analog input for battery voltage and output voltage
  analogVb = analogRead(A0);
  analogVo = analogRead(A1);

  // Note these conversions are oversimplified; once the voltage dividers for the
  // analog inputs are finalized, we will have to do a bit more algebra.
  batteryVoltage = (analogVb) * 1023 / 5;
  outputVoltage = (analogVo) * 1023 / 5;

  // I_out = (V_o - V_b) / R_L; R_L = 22 Ohms
  outputCurrent = (outputVoltage - batteryVoltage) * 1000 / 22;

  // Set a cap on the duty cycle at 0.7 as a safety measure
  if(outputCurrent < 90 && dutyCycle < 0.7){
  dutyCycle += 0.01;
  }
  else if(outputCurrent > 105){
  // If current is too high, set the duty cycle extremely low as a safety measure
  // This value might be adjusted later
  dutyCycle = 0.2; // Maximum voltage output should be (5 V) * (1/0.8) = 6.25 V
  }
*/

  // We only want to update the duty cycle every 5 seconds or so, because refreshing it
  // every loop cycle would be excessive. Thankfully, the Arduino makes it very easy to 
  // implement a non-blocking delay using the millis() function, which yields the time since
  // the program was started.
  // TODO: figure out what to do if millis() gets overflow. this could be as simple as
  // if(currentTime < lastPWMUpdateTime)
  currentTime = millis();
  if( (currentTime - lastPWMUpdateTime >= 5000) || (currentTime < lastPWMUpdateTime) ){
  analogVo = analogRead(pinVo);
  analogVb = analogRead(pinVb);
  
  updateDutyCycle(analogVo, analogVb, &dutyCycle);
  serialPrintBatteryStatus(analogVo, analogVb, dutyCycle);
  
  lastPWMUpdateTime = currentTime;
  }

  // TODO: Set up functionality to print batteryVoltage, outputVoltage, outputCurrent,
  // and dutyCycle to the serial monitor with a delay.
  
  analogWrite(10, dutyCycle * 255);

  // Actual PWM driver: write to pin 10 with a duty cycle of 0.554 (5 V -> 11.2 V)
//  analogWrite(10, 0.554 * 255);
  
}

// This function checks if the current is too high or too low by evaluating if the difference
// between output voltage and battery voltage is too high/low, using pre-calculated parameters
// (namely LOWER_ANALOG_DIFFERENCE_LIMIT and UPPER_ANALOG_DIFFERENCE_LIMIT)
// and adjusts the duty cycle accordingly.

// In the future, to make this clearer, it would probably be a good idea to actually calculate
// the real current using variables/constants for R_bias, voltage division factors, etc., so
// the code would be more portable and easier for the reader to understand.
void updateDutyCycle(int analogVo, int analogVb, float* dutyCycle_p){
  
  // If voltage difference is too low (implying current < 90mA), increase duty cycle
  // However, set a cap on the duty cycle at 0.7 as a safety measure
  if( ((analogVo - analogVb) < LOWER_ANALOG_DIFFERENCE_LIMIT) && *dutyCycle_p <= 0.7){
    //*dutyCycle_p = *dutyCycle_p + 0.01;
    Serial.println("INCREASING DUTY CYCLE BY 0.01...");
    }
  
  // If current is too high, set the duty cycle extremely low as a safety measure
  // This value might be adjusted later
  else if( (analogVo - analogVb) > UPPER_ANALOG_DIFFERENCE_LIMIT){
    //*dutyCycle_p = 0.25;
    Serial.println("SETTING DUTY CYCLE TO 0.25...");
    }
    
  return;
}

// TODO: write this function
void serialPrintBatteryStatus(int analogVo, int analogVb, float dutyCycle){

  Serial.println("OUTPUT VOLTAGE (0-1023):");
  Serial.println(analogVo);
  Serial.println();
  
  Serial.println("BATTERY VOLTAGE(0-1023):");
  Serial.println(analogVb);
  Serial.println();
  
  Serial.println("DUTY CYCLE (0-1):");
  Serial.println(dutyCycle);
  Serial.println();
  
  Serial.println("------------------------");
  Serial.println();
  
  Serial.println("REAL OUTPUT VOLTAGE (V):");
  Serial.println((float) analogVo / ANALOG_VDIV_CONVERSION_FACTOR);
  Serial.println();
  
  Serial.println("REAL BATTERY VOLTAGE (V):");
  Serial.println((float) analogVo / ANALOG_VDIV_CONVERSION_FACTOR);
  Serial.println();
  
  // TODO: Consider adding output current readings, expected duty cycle for
  // desired Vo = 11.2 V or desired current = 100 mA

  return;
}
