// #include <Arduino.h>

#include "methods.h"
#include "FastLED.h"

#define NUM_LEDS 20

CRGB leds[NUM_LEDS];

// debugging through serial monitor
bool debug = true;
float charge;
float standby;

void setup()
{

  Serial.begin(9600);

  /*
   * Initialisation for various peripherals:
   *      - bluetooth connection
   *      - PWM channels for HV and buzzer
   *      - RGB LED pins
   *      - ADC input pins (batt_voltage, tube_voltage)
   */

  // initialise ADC pins for batt and tube voltage monitoring
  adcAttachPin(BATTERY_PIN);
  adcAttachPin(GMTUBE_PIN);

  // initialise PWMs for boost converter and buzzer
  setPWM();

  // initialise interrupt service routines
  setISR();

  // initialise RGB LED
  FastLED.addLeds<WS2812B, RGB_PIN, GRB>(leds, NUM_LEDS);

  // initiate bluetooth connection
  initBLE(); //UUID);
}

void loop()
{

  // constantly check for dev's input
  checkForCmd(UUID);

  /*
 * The detector will send data of [ CPM, batt_voltage, tube_voltage ] via BLE every 1 second
 */

  leds[0] = CRGB(255, 0, 0);
  FastLED.show();

  ledcWrite(pwm.channel, pwm.dutyCycle);

  // interrupt program when one second has passed
  if (int_timer.isr_count > 0)
  {
    portENTER_CRITICAL(&timerMux);
    int_timer.isr_count--;
    portEXIT_CRITICAL(&timerMux);

    charge = ReadVoltage(CHRG_PIN);
    standby = ReadVoltage(STBY_PIN);
    collectData(pulse.cpm);
    timerReady = true;
    pulse.cpm = 0;
  }

  // interrupt program when a pulse is detected
  if (pulse.count > 0)
  {
    portENTER_CRITICAL(&mux);
    pulse.count--;
    portEXIT_CRITICAL(&mux);

    // not an efficient method at high cpm rate
    ledcWrite(buzzer.channel, buzzer.dutyCycle); // on the buzzer
    delay(5);
    ledcWrite(buzzer.channel, 0); // off the buzzer

    // leds[0] = CRGB(0, 255, 0);
    // FastLED.show();

    pulse.cpm++;
  }

  /****************************************************
   * BLE section
   *****************************************************
   */

  // check if timer is ready and device is connected
  if (deviceConnected && timerReady)
  {
    leds[0] = CRGB(0, 0, 255);
    FastLED.show();

    String str = "";
    str += BLEdata.cpm;
    str += ",";
    str += BLEdata.batt_voltage;
    str += ",";
    str += BLEdata.tube_voltage;

    if (debug)
    {
      Serial.print("Device connected");
      Serial.print("\t");
      Serial.println("Sending data: " + str);
      Serial.println("Charge value: " + String(charge));
      Serial.println("Standby value: " + String(standby));
    }

    // package the value and send through one channel
    pCharacteristic->setValue((char *)str.c_str());
    pCharacteristic->notify();

    timerReady = false;
  }

  // disconnecting
  if (!deviceConnected && oldDeviceConnected)
  {
    delay(500);                  // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    if (debug)
    {
      Serial.print("Device disconnected");
      Serial.print("\t");
      Serial.println("start advertising");
    }

    oldDeviceConnected = deviceConnected;
  }

  // connecting
  if (deviceConnected && !oldDeviceConnected)
  {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }

  /****************************************************
   * BLE section end
   *****************************************************
   */
}