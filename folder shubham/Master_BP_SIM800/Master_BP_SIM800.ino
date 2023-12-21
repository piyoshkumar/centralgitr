/* THIS IS MASTER DEVICE HAS TWO SLAVE
  1. SPO2 & PR
  2. HEIGHT AND WEIGHT
*/

#include "CT_BP.h"
// #include <Ticker.h>
#include "sim800l.h"
#include <Wire.h>

#define I2C_SLAVE_ADDR_SPO2 0x55
#define I2C_SLAVE_ADDR_BMI 0x65
#define LED 2

// Ticker changer;

bool is_SIM800_Configured = false;
uint8_t spo2Data = 0;
uint8_t pulseRateData = 0;
float weight = 0.0;
float BMI = 0.0;
uint8_t height = 0;


// void change() {
// }

void setup() {
  Serial.begin(115200);                     // BP Serial
  Serial1.begin(9600, SERIAL_8N1, 32, 33);  // 32-RX,33-TX  SIM800L
  Serial2.begin(9600, SERIAL_8N1, 16, 17);  // 16-RX,17-TX  CT BP
  Wire.begin();
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  // changer.attach(0.1, change);

}  // End of setup.


// This is the Arduino main loop function.
void loop() {

  String MSG_RECEIVED = " ";
  // SIM800L Setup
  if (!is_SIM800_Configured) {
    is_SIM800_Configured = sms_setup();
  } else {
    MSG_RECEIVED = sms_read();
    read_command();
  }

  get_BP();

  RequestSlaveSPO2Data(I2C_SLAVE_ADDR_SPO2, 2);
  RequestSlaveBMIData(I2C_SLAVE_ADDR_BMI, 9);
  delay(50);
  if (spo2Data > 100) {
    spo2Data = 0;
    pulseRateData = 0;
  }
  if (pulseRateData > 250) {
    spo2Data = 0;
    pulseRateData = 0;
  }
  if (sys == 404) {
    Serial.println("E," + String(dia) + "," + String(spo2Data) + "," + String(pulseRateData) + "," + String(height) + "," + String(weight) + "," + String(BMI) + "," + MSG_RECEIVED);  // If error in BP
  } else {
    Serial.println(String(sys) + "," + String(dia) + "," + String(spo2Data) + "," + String(pulseRateData) + "," + String(height) + "," + String(weight) + "," + String(BMI) + "," + MSG_RECEIVED);
  }
}

void RequestSlaveSPO2Data(uint8_t SLAVE_ADDR, uint8_t num_of_bytes) {

  Wire.requestFrom(SLAVE_ADDR, num_of_bytes);
  while (Wire.available()) {
    spo2Data = Wire.read();       // Read SpO2 data
    pulseRateData = Wire.read();  // Read pulse rate data

    // Serial.print("SpO2: ");
    // Serial.print(spo2Data);
    // Serial.print(" PR: ");
    // Serial.println(pulseRateData);
  }
}
void RequestSlaveBMIData(uint8_t SLAVE_ADDR, uint8_t num_of_bytes) {

  byte wt[4];
  byte bmi[4];
  Wire.requestFrom(SLAVE_ADDR, num_of_bytes);
  while (Wire.available()) {
    wt[0] = Wire.read();
    wt[1] = Wire.read();
    wt[2] = Wire.read();
    wt[3] = Wire.read();

    bmi[0] = Wire.read();
    bmi[1] = Wire.read();
    bmi[2] = Wire.read();
    bmi[3] = Wire.read();

    height = Wire.read();

    memcpy(&weight, &wt, 4);
    memcpy(&BMI, &bmi, 4);
    //     Serial.print("WT: ");
    //     Serial.print(weight);
    //     Serial.print(" HT: ");
    //     Serial.print(height);
    //     Serial.print(" BMI: ");
    //     Serial.println(BMI);
  }
}
