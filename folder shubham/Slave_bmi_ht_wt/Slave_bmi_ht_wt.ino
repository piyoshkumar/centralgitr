#include "load_cell.h"
#include "lidar.h"
#include <Wire.h>
#define I2C_DEV_ADDR 0x65

float wtn;
float wt_sum;
//float bmi;

int count;
float W;
float bmi;

//Define load BUffer
int load_buffer[10];
int prev_val = 0;
int th = 0.2;


void setup() {
  //Serial.begin(9600);
  Serial.begin(115200);
  Serial2.begin(115200);
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  Wire.begin((uint8_t)I2C_DEV_ADDR);
  Wire.onRequest(sendData);  // Set up the function to be called when a request is received

  load_cell_setup();
  //  Serial2.begin(115200);
}
void loop() {
  LIdar_setup();
  //  LoadCell.update();
  W = measureWeight();
  //  Serial.print("weight = ");
  //  Serial.print(W);
  //Serial.print("  Height:");
  // Serial.print(H);
  // Serial.print(",");
  //  Serial.println(" ");

  //Serial.println(k);
  if (H > 0.0) {
    float k = ((H * H) / 10000);
    bmi = W / k;
  } else {
    bmi = 0.0;
  }

  // Serial.print("   Bmi:");
  // Serial.print(bmi);
  // Serial.println(" }");
  //  Serial.println(wtn);
  // count =  count + 1;
  // if (count == 10 || 1)
  //
  {
    digitalWrite(2, HIGH);

    Serial.println("{'W':" + String(W) + ",'H':" + String(H) + ",'BMI':" + String(bmi) + "}");
    delay(100);
    digitalWrite(2, LOW);
    //    Serial.print(W);
    //    Serial.print(",");
    //    count = 0 ;
    //delay(10);
    //
  }
}

void sendData() {
  int ht = int(H);
  uint8_t HT = (uint8_t)ht;

  Wire.beginTransmission(I2C_DEV_ADDR);  // Address of the receiver ESP32
  sendFloatNumber(W);
  sendFloatNumber(bmi);
  Wire.write(HT);
  Wire.endTransmission();
}

void FloatToHex(float f, byte* hex) {
  byte* f_byte = reinterpret_cast<byte*>(&f);  // the value of f_byte is pointer of f
  memcpy(hex, f_byte, 4);                      //hex: destination, f_byte:source, 4: number of bytes to copy(4 bytes = 32 bit(float))
}

void sendFloatNumber(float floatValue) {
  byte hex[4] = { 0 };          // create a hex array for the 4 butes
  FloatToHex(floatValue, hex);  // convert the float to hex array
  Wire.write(hex[0]);
  Wire.write(hex[1]);
  Wire.write(hex[2]);
  Wire.write(hex[3]);
}
