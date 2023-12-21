#include <Arduino.h>

// byte highByte;
// byte lowByte;
uint16_t sys = 0;
uint16_t dia = 0;

char readSerialbytes() {
  while (!Serial2.available() > 1) {}
  return Serial2.read();
}

void get_BP() {
  int bytesRead = 0;
  if (readSerialbytes() == 0xFD) {
    if (readSerialbytes() == 0xFD) {
      char readBytes = readSerialbytes();
      // Serial.println(readBytes, HEX);
      if (readBytes == 0xFB) {
        byte highByte = readSerialbytes();
        byte lowByte = readSerialbytes();
        uint16_t mmHg = (highByte << 8) | lowByte;
        sys = mmHg;
        dia = 0;
        // Serial.print("BP,");
        // Serial.print(mmHg);
        // Serial.println(",0");
      } else if (readBytes == 0xFC) {
        byte systolic = readSerialbytes();
        byte diastolic = readSerialbytes();
        byte pr = readSerialbytes();
        sys = systolic;
        dia = diastolic;
        // Serial.print("BP,");
        // Serial.print(sys);
        // Serial.print(",");
        // Serial.println(dia);
        // Serial.print(",");
        // Serial.println(pr);
      } else if (readBytes == 0xFD) {

        // Serial.print("BP,E,");
        byte error = readSerialbytes();
        sys = 404;  // 404 for error code
        dia = error;
        // Serial.println(error);
      }
    }
  } 
}
