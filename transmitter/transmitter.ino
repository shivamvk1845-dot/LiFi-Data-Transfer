#define LED_PIN 3 
 
const unsigned long BIT_TIME   = 150; 
const unsigned long START_TIME = 600; 
const unsigned long GAP_TIME   = 300; 
 
void sendRawBit(bool bitValue) { 
  digitalWrite(LED_PIN, bitValue ? HIGH : LOW); 
  delay(BIT_TIME); 
} 
 
void sendFramedByte(byte data) { 
  sendRawBit(HIGH); 
 
  for (byte i = 0; i < 8; i++) { 
    sendRawBit((data >> i) & 1); 
  } 
 
  sendRawBit(LOW); 
} 
 
void sendMessage(String msg) { 
  if (msg.length() > 16) { 
    msg = msg.substring(0, 16); 
  } 
 
  digitalWrite(LED_PIN, LOW); 
  delay(200); 
 
  digitalWrite(LED_PIN, HIGH); 
  delay(START_TIME); 
 
  digitalWrite(LED_PIN, LOW); 
  delay(GAP_TIME); 
 
  sendFramedByte((byte)msg.length()); 
 
  byte checksum = 0;
  for (byte i = 0; i < msg.length(); i++) { 
    byte c = (byte)msg[i]; 
    sendFramedByte(c); 
    checksum ^= c; 
  } 
 
  sendFramedByte(checksum); 
 
  digitalWrite(LED_PIN, LOW); 
  delay(400); 
} 
 
void setup() { 
  pinMode(LED_PIN, OUTPUT); 
  digitalWrite(LED_PIN, LOW); 
 
  Serial.begin(9600); 
  delay(800); 
 
  Serial.println("--- VLC TX READY (Framed) ---"); 
  Serial.println("Type message and press ENTER:"); 
} 
 
void loop() { 
  if (Serial.available()) { 
    String raw = Serial.readString(); 
    String cleanMsg = ""; 
 
    for (int i = 0; i < raw.length(); i++) { 
      char c = raw[i]; 
      if (c >= 32 && c <= 126) { 
        cleanMsg += c; 
      } 
    } 
 
    if (cleanMsg.length() > 0) { 
      Serial.print("Transmitting: "); 
      Serial.print(cleanMsg); 
      Serial.print(" (Len: "); 
      Serial.print(cleanMsg.length()); 
      Serial.println(")"); 
 
      sendMessage(cleanMsg); 
 
      Serial.println("Done.\n"); 
    } 
  } 
}
