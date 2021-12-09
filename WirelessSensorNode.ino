/*
   WirelessSensorNode.ino

   Author: Jack Greer
   Created on: 13 Nov 2021
   Date last modified: 8 Dec 2021

*/

#include "BoostConverter.h"
#include "Thermistor.h"
#include <SoftwareSerial.h>

/* Pin Defines */
#define PIN_THERMISTOR  A1     // Pin used to read temperature (via voltage divider)

#define PIN_V_IN        A0     // Read boost converter input voltage (from solar panel/AD2)
#define PIN_V_OUT       A2     // Read output of boost converter (through voltage divider)
#define PIN_V_BATTERY   A3     // Read battery voltage (through voltage divider)

#define PIN_PWM       10       // Drive PWM to boost converter
#define PIN_TX        9        // SoftwareSerial TX - connected to RX of Bluetooth chip (through voltage divider)
#define PIN_RX        8        // SoftwareSerial RX - Connected to TX of Bluetooth chip (no voltage divider)

/* Wait Times for Recording/Reporting Data, updating Boost Converter */
#define WAIT_100_MS           100
#define WAIT_1_SEC            1000
#define WAIT_5_SEC            5000
#define WAIT_30_SEC           30000
#define WAIT_5_MIN            300000
// In real-life implementation, thermistor data should be reported every 5 minutes

#define THERM_UPDATE_INTERVAL      WAIT_1_SEC
#define PWM_UPDATE_INTERVAL        WAIT_30_SEC

/* Bluetooth Defines */
#define BT_BAUDRATE   9600    // Bluetooth chip uses a baudrate of 9600

/* Function Defines */
// Print the parameters of the Thermistor object to the serial monitor (not used in wireless mode)
//void Thermistor_serialPrintStatus(Thermistor thermistor);

// Print boost converter statistics to serial monitor (not used in wireless mode)
//void BoostConverter_serialPrintStatus(BoostConverter boostConverter);

// Send temperature data via Bluetooth
void serialSendThermistorData(SoftwareSerial swSerial, Thermistor therm);

/* Instantiate Objects */

BoostConverter boostConverter(PIN_V_IN, PIN_V_OUT, PIN_V_BATTERY);
Thermistor thermistor(PIN_THERMISTOR);
SoftwareSerial btSerial(PIN_RX, PIN_TX);

void setup() {

  Serial.begin(9600);
  
  btSerial.begin(BT_BAUDRATE);
  boostConverter.update();    // Initialize duty cycle based on input voltage
  
  // Set prescaler (last three bits) of Timer 1 register to 001
  // This creates the maximum output frequency for Timer 1 (31.25 kHz)
  TCCR1B = TCCR1B & B11111000 | B00000001;

  pinMode(PIN_PWM, OUTPUT);
  
}

void loop() {
  // Don't want to record data every single program loop, because that's too fast
  // In order to implement a non-blocking loop, check the current time of the program every loop,
  // and check how long it's been since the last update; if it's been more than the desired interval
  // (for instance, for temperature recording, THERM_UPDATE_INTERVAL
  static int lastPWMUpdateTime = millis();
  static int lastTemperatureUpdateTime = millis();
  static int lastBluetoothUpdateTime = millis();

  int currentTime = millis();


    if( (currentTime - lastPWMUpdateTime >= PWM_UPDATE_INTERVAL) || (currentTime < lastPWMUpdateTime) ){
    boostConverter.update();

    // Uncomment this line when testing WITH the Arduino plugged in via USB.
    //BoostConverter_serialPrintStatus(boostConverter);

    lastPWMUpdateTime = currentTime;
    }


  if ( (currentTime - lastTemperatureUpdateTime >= THERM_UPDATE_INTERVAL) || (currentTime < lastTemperatureUpdateTime) ) {
    thermistor.update();

    // Uncomment this line when testing WITH the Arduino plugged in via USB.
    Thermistor_serialPrintStatus(thermistor);
    
    serialSendThermistorData(btSerial, thermistor);

    Serial.println("... big chillin ...");

    lastTemperatureUpdateTime = currentTime;
  }

  // Drive PWM to boost converter circuit
  analogWrite(PIN_PWM, boostConverter.dutyCycle);
  //analogWrite(PIN_PWM, 153);

}

/*
   Very simple function that just prints the values of the BoostConverter object to the serial monitor.
   This function is only used for testing, and is not used for wireless mode.
*/
void BoostConverter_serialPrintStatus(BoostConverter boost) {

  Serial.println("---------Incoming Data---------");
  Serial.println();

  Serial.println("INPUT VOLTAGE (0-1023):");
  Serial.println(boost.analogVi);
  Serial.println();

  Serial.println("OUTPUT VOLTAGE:");
  Serial.println(boost.analogVo);
  Serial.println();

  Serial.println("BATTERY VOLTAGE:");
  Serial.println(boost.analogVb);
  Serial.println();

  Serial.println("DUTY CYCLE (0-255):");
  Serial.println(boost.dutyCycle);
  Serial.println();

  Serial.println("---Converted Data---");
  Serial.println();

  Serial.println("REAL INPUT VOLTAGE (V):");
  Serial.println((float) boost.analogVi * 5 / 1024);

  Serial.println("REAL OUTPUT VOLTAGE (V):");
  Serial.println((float) boost.analogVo / ANALOG_VDIV_CONVERSION_FACTOR);
  Serial.println();

  Serial.println("REAL BATTERY VOLTAGE (V):");
  Serial.println((float) boost.analogVb / ANALOG_VDIV_CONVERSION_FACTOR);
  Serial.println();

  Serial.println("DUTY CYCLE (0-1):");
  Serial.println((float) boost.dutyCycle / 255);
  Serial.println();

  return;
}


/*
   Very simple function that prints the parameters of the Thermistor object to the serial monitor.
   This function is only here for testing, and is not used for wireless mode.
*/
void Thermistor_serialPrintStatus(Thermistor therm) {

  Serial.println("----Thermistor Data----");
  Serial.println();

  Serial.println("VOLTAGE DIVIDER OUTPUT:");
  Serial.println(therm.voltage);

  Serial.println("THERMISTOR RESISTANCE:");
  Serial.println(therm.resistance);

  Serial.println("TEMPERATURE (K):");
  Serial.println(therm.tempKelvin);

  Serial.println("TEMPERATURE (F):");
  Serial.println(therm.tempFahrenheit);

  Serial.println("--END OF TRANSMISSION--");
  Serial.println("------Stay Wavvvy------");
  Serial.println();

  return;
}

/*
   Transmit thermistor temperature data via UART
*/
void serialSendThermistorData(SoftwareSerial swSerial, Thermistor therm) {
  swSerial.println(int(therm.tempFahrenheit));   // Send temperature in Kelvin to desktop app (serial port)
  Serial.println("I just sent data fr fr");
  Serial.println(therm.tempKelvin);

  return;
}
