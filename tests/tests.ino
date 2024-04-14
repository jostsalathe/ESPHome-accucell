#include <FastLED.h>

#include "accucell.hpp"

#define PIN_BUT_USR 13
#define PIN_BUT_BOOT 0
#define PIN_ACCUCELL_RX 34
#define PIN_ACCUCELL_TX 32
#define PIN_LEDs 4
#define PIN_EN_AMP 27
#define PIN_VBATT_HALF 35

#define NUM_LEDS 9
CRGB leds[NUM_LEDS];

HardwareSerial accucellRx(1);
Accucell6 accucell;

void setup() {
  // put your setup code here, to run once:
  digitalWrite(PIN_EN_AMP, LOW);
  pinMode(PIN_EN_AMP, OUTPUT);

  pinMode(PIN_VBATT_HALF, INPUT);
  adcAttachPin(PIN_VBATT_HALF);

  pinMode(PIN_ACCUCELL_RX, INPUT);
  accucellRx.begin(9600, SERIAL_8N1, PIN_ACCUCELL_RX, PIN_ACCUCELL_TX);

  Serial.begin(115200);
  FastLED.addLeds<NEOPIXEL, PIN_LEDs>(leds, NUM_LEDS);
}

void readAccucell() {
  uint8_t buffer[128];
  size_t available = accucellRx.available();
  size_t len = accucellRx.readBytes(buffer, available < 128 ? available : 128);
  if (accucell.add_bytes_and_chack_for_complete_message(buffer, len)) {
    // uint32_t vIn5 = analogReadMilliVolts(PIN_VBATT_HALF) * 2;
    // Serial.print("vIn5:");
    // Serial.print(vIn5);

    // Serial.print(",SW_USR1:");
    // Serial.print(digitalRead(PIN_BUT_USR));// ? "released" : "pressed ");

    // Serial.print(",SW_BOOT1:");
    // Serial.print(digitalRead(PIN_BUT_BOOT));// ? "released" : "pressed ");

    Serial.print("vIn12:");
    Serial.print(accucell.get_measured_input_voltage());
    Serial.print(",isRunning:");
    Serial.print(accucell.get_is_running());
    Serial.print(",isCharging:");
    Serial.print(accucell.get_is_charging());
    Serial.print(",vBat:");
    Serial.print(accucell.get_measured_battery_voltage());
    Serial.print(",iBat:");
    Serial.print(accucell.get_measured_battery_current());
    Serial.print(",qBat:");
    Serial.print(accucell.get_measured_charge()*1000.0);

    Serial.println();

    for (int dot = 0; dot < NUM_LEDS; ++dot) {
      leds[dot] = CRGB(CRGB::White)/8;
      FastLED.show();
      leds[dot] = CRGB::Black;
    }
    for (int dot = NUM_LEDS-2; dot > 0; --dot) {
      leds[dot] = CRGB(CRGB::White)/8;
      FastLED.show();
      leds[dot] = CRGB::Black;
    }
    FastLED.show();
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  readAccucell();
  delay(100);
}
