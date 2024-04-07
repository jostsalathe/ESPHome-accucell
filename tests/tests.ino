#include <FastLED.h>

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

bool checkSumValid(uint8_t *buf) {
  uint32_t receivedSum = ((buf[73] & 0xF) << 4) + (buf[74] & 0xF);
  uint32_t calculatedSum = 0;
  for (int i=1; i <= 72; ++i) {
    calculatedSum += buf[i];
  }
  calculatedSum = calculatedSum%0x100;
  return receivedSum == calculatedSum;
}

uint8_t byte2value(uint8_t b) {
  return b & 0x7F;
}

void readAccucell() {
  uint8_t buffer[128];
  size_t len = accucellRx.readBytesUntil(0x7D, buffer, 128);
  if (len != 75) {
    Serial.print("ERROR: Received message with ");
    Serial.print(len);
    Serial.println(" bytes but expected 76.");
  } else if(buffer[0] != 0x7B) {
    Serial.print("ERROR: First byte was ");
    Serial.print(buffer[0], HEX);
    Serial.println(" instead of 7B.");
  } else if(!checkSumValid(buffer)) {
    Serial.print("ERROR: Checksum does not match.");
  } else {
    Serial.print("received: ");
    for (int i=1; i < 75; ++i) {
      Serial.print(buffer[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

  }
}

void loop() {
  // put your main code here, to run repeatedly:
  uint32_t vIn = analogReadMilliVolts(PIN_VBATT_HALF) * 2;
  Serial.print("vIn = ");
  Serial.print(vIn);
  Serial.println(" mV");

  Serial.print("SW_USR1 ");
  Serial.println(digitalRead(PIN_BUT_USR) ? "released" : "pressed ");

  Serial.print("SW_BOOT1 ");
  Serial.println(digitalRead(PIN_BUT_BOOT) ? "released" : "pressed ");

  readAccucell();

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
