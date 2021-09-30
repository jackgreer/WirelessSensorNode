// Filename:           FeedbackBeta.ino
// Author:             Jack Greer
// Created:            September 29th, 2021
// Date Last Modified: September 30th, 2021 
// Description:        First draft of PWM feedback control for DC/DC boost converter.
//                     Uses the voltage difference across load resistor R_L to determine
//                     the output current; if it's too low, increase the duty cycle, and
//                     if it's too high, set the duty cycle to a low value.

// Right now, the code for this functionality is in loop(), but it might be moved later.
// float updateDutyCycle(int outputCurrent, float dutyCycle);

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
  float outputVoltage; // Measured in V
  float outputCurrent; // Measured in mA
  float dutyCycle;

  int analogVb; // Measured 0-1023
  int analogVo; // Measured 0-1023

  // TODO: Write code that takes analog input for battery voltage and output voltage
  analogVb = analogRead(A0);
  analogVo = analogRead(A1);

  // Note these conversions are oversimplified; once the voltage dividers for the
  // analog inputs are finalized, we will have to do a bit more algebra.
  batteryVoltage = (analogVb) * 1023 / 5;
  outputVoltage = (analogVo) * 1023 / 5;

  // I_out = (V_o - V_b) / R_L; R_L = 22 Ohms
  outputCurrent = (outputVoltage - batteryVoltage) * 1000 / 22;

  // TODO: Set up functionality to print batteryVoltage, outputVoltage, outputCurrent,
  // and dutyCycle to the serial monitor with a delay.

  // Set a cap on the duty cycle at 0.7 as a safety measure
  if(outputCurrent < 90 && dutyCycle < 0.7){
    dutyCycle += 0.01;
  }
  else if(outputCurrent > 105){
    // If current is too high, set the duty cycle extremely low as a safety measure
    // This value might be adjusted later
    dutyCycle = 0.2; // Maximum voltage output should be (5 V) * (1/0.8) = 6.25 V
  }

  analogWrite(10, dutyCycle * 255);
  */

  analogWrite(10, 0.554 * 255);
  
}
