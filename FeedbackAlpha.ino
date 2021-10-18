// Filename:           FeedbackAlpha.ino
// Author:             Jack Greer
// Created:            October 17th, 2021 (Beta created September 29th, 2021)
// Date Last Modified: October 17th, 2021
// Description:        Refined feedback control software that creates a constant output voltage
//                     for the DC/DC converter rather than a constant output current.
//                     This is simply done using the formula D = 1 - (V_in/V_out)

/* Pins for Analog Read */
#define PIN_V_IN      A0
#define PIN_V_OUT     A1
#define PIN_V_BATTERY A2

/* Pin for Analog Write */
#define PIN_PWM      10

/* analogVo = (V_out) * 64; analogVb = (V_battery) * 64 */
#define ANALOG_VDIV_CONVERSION_FACTOR 64

#define UPDATE_DUTY_CYCLE_WAIT 5000   // Update duty cycle every 5 seconds

#define DUTY_CYCLE_UPPER_LIMIT 255    // analogWrite() duty cycle an 8 bit unsigned int, not a float
#define DESIRED_V_OUT          2293   // = 11.2 V * (1024 / 5)

void updateDutyCycle(int analogVi, int* dutyCycle8bit);
void serialPrintBatteryStatus(int analogVi, int analogVo, int analogVb, int dutyCycle);

void setup(){
  // Set prescaler (last three bits) of Timer 1 register to 001
  // This creates the maximum output frequency for Timer 1 (31.25 kHz)
  TCCR1B = TCCR1B & B11111000 | B00000001;

  pinMode(PIN_PWM, OUTPUT);
  Serial.begin(9600); // This will be used for reading voltage during testing
}

void loop(){
  static int lastPWMUpdateTime = millis();
  static int dutyCycle8bit;   // Range: 0-255

  // Update duty cycle every 5 seconds by comparing the current time with the previous update time
  int currentTime = millis();
  if( (currentTime - lastPWMUpdateTime >= UPDATE_DUTY_CYCLE_WAIT) || (currentTime < lastPWMUpdateTime) ){
    // Right now, we only need to read analogVi, but I'm expecting to add functionality related
    // to analogVo, analogVb later (such as turning off the PWM when Vb > 10.5 V, or when the rate
    // of change of Vb has gotten close to 0)
    int analogVi = analogRead(PIN_V_IN);
    int analogVo = analogRead(PIN_V_OUT);
    int analogVb = analogRead(PIN_V_BATTERY);

    updateDutyCycle(analogVi, &dutyCycle8bit);
    serialPrintBatteryStatus(analogVi, analogVo, analogVb, dutyCycle8bit);

    lastPWMUpdateTime = currentTime;
  } // End of duty cycle update conditional

  analogWrite(PIN_PWM, dutyCycle8bit);
}

/*
* Description of how it works
* 
* @param analogVi:
*   dutyCycle8Bit:
*/
void updateDutyCycle(int analogVi, int* dutyCycle8bit){
  *dutyCycle8bit = DUTY_CYCLE_UPPER_LIMIT - (DUTY_CYCLE_UPPER_LIMIT * analogVi / DESIRED_V_OUT);

  return;
}

/*
* Very simple function that just prints the analogRead() data of the circuit to the serial monitor.
* It will not be present in the final code.
* 
* @param analogVi:
*        analogVo:
*        analogVb:
*   dutyCycle8bit:
*/
void serialPrintBatteryStatus(int analogVi, int analogVo, int analogVb, int dutyCycle){

  Serial.println("---------Incoming Data---------");
  Serial.println();

  Serial.println("INPUT VOLTAGE (0-1023):");
  Serial.println(analogVi);
  Serial.println();

  Serial.println("OUTPUT VOLTAGE:");
  Serial.println(analogVo);
  Serial.println();

  Serial.println("BATTERY VOLTAGE:");
  Serial.println(analogVb);
  Serial.println();
  
  Serial.println("DUTY CYCLE (0-255):");
  Serial.println(dutyCycle);
  Serial.println();
  
  Serial.println("---Converted Data---");
  Serial.println();

  Serial.println("REAL INPUT VOLTAGE (V):");
  Serial.println((float) analogVi * 1024 / 5);
  
  Serial.println("REAL OUTPUT VOLTAGE (V):");
  Serial.println((float) analogVo / ANALOG_VDIV_CONVERSION_FACTOR);
  Serial.println();
  
  Serial.println("REAL BATTERY VOLTAGE (V):");
  Serial.println((float) analogVb / ANALOG_VDIV_CONVERSION_FACTOR);
  Serial.println();

  Serial.println("DUTY CYCLE (0-1):");
  Serial.println((float) dutyCycle / 255);
  Serial.println();
  
  return;
}
