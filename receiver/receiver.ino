#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Change to 0x3F if blank

const int SENSOR_PIN = A0;

const unsigned long BIT_TIME   = 150;
const unsigned long START_TIME = 600;
const unsigned long GAP_TIME   = 300;

int threshold = 50;

bool lightDetected() {
  return analogRead(SENSOR_PIN) >= threshold;
}

// Waits for the HIGH start bit, then samples 8 data bits dead-center
byte receiveFramedByte() {
  // 1. Wait for START bit (HIGH)
  unsigned long timeout = millis();
  while (!lightDetected()) {
    if (millis() - timeout > 3000) return 0; // Timeout safeguard
    delayMicroseconds(50);
  }

  // 2. Wait out remainder of START bit + half of first bit to hit center
  delay(BIT_TIME + (BIT_TIME / 2));

  // 3. Sample 8 data bits
  byte data = 0;
  for (byte i = 0; i < 8; i++) {
    if (lightDetected()) {
      data |= (1 << i);
    }
    delay(BIT_TIME);
  }

  // 4. Wait out STOP bit (LOW)
  delay(BIT_TIME / 2);

  return data;
}

bool waitForPreamble() {
  if (!lightDetected()) {
    return false;
  }

  unsigned long startTime = millis();

  // Validate start pulse duration (~500ms threshold for 600ms pulse)
  while (lightDetected()) {
    if (millis() - startTime >= (START_TIME - 100)) {
      unsigned long waitEnd = millis();
      while (lightDetected()) {
        if (millis() - waitEnd > 2000) return false;
        delay(1);
      }
      return true;
    }
    delay(1);
  }
  return false;
}

void showWaiting() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("VLC RECEIVER");
  lcd.setCursor(0, 1);
  lcd.print("WAITING...");
}

void setup() {
  Serial.begin(9600);
  analogReference(INTERNAL); // 1.1V reference

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Calibrating...");

  for (int i = 0; i < 8; i++) {
    analogRead(SENSOR_PIN);
    delay(20);
  }

  long sum = 0;
  for (int i = 0; i < 25; i++) {
    sum += analogRead(SENSOR_PIN);
    delay(30);
  }

  int baseline = sum / 25;
  threshold = baseline + 30; // Threshold above room light

  showWaiting();

  Serial.println("--- VLC RX READY (Framed) ---");
  Serial.print("Baseline: ");
  Serial.print(baseline);
  Serial.print(" | Threshold: ");
  Serial.println(threshold);
}

void loop() {
  if (!waitForPreamble()) {
    return;
  }

  Serial.println("\n[PREAMBLE DETECTED]");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RECEIVING...");

  // 1. Read message length
  byte messageLength = receiveFramedByte();

  Serial.print("Payload Length: ");
  Serial.println(messageLength);

  if (messageLength == 0 || messageLength > 16) {
    Serial.println("Invalid Length: Dropped");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("INVALID LENGTH");

    delay(1500);
    showWaiting();
    return;
  }

  // 2. Read characters
  String message = "";
  byte checksum = 0;

  for (byte i = 0; i < messageLength; i++) {
    byte c = receiveFramedByte();
    message += (char)c;
    checksum ^= c;
  }

  // 3. Read checksum
  byte receivedChecksum = receiveFramedByte();

  Serial.print("Message: ");
  Serial.println(message);

  Serial.print("Checksum Calc: ");
  Serial.print(checksum);

  Serial.print(" | Recv: ");
  Serial.println(receivedChecksum);

  // 4. Update LCD display
  lcd.clear();
  lcd.setCursor(0, 0);

  if (checksum == receivedChecksum) {
    Serial.println("STATUS: SUCCESS");
    lcd.print("RX OK [VERIFIED]");
  } else {
    Serial.println("STATUS: CORRUPT");
    lcd.print("RX MSG:");
  }

  lcd.setCursor(0, 1);
  lcd.print(message);

  for (int i = message.length(); i < 16; i++) {
    lcd.print(" ");
  }

  delay(6000);
  showWaiting();
}
