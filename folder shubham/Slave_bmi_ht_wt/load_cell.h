#include <HX711_ADC.h>


float height;
unsigned char check;
unsigned char uart[9];
const unsigned char HEADER = 0x59;

int rec_debug_state = 0x01; // receive state for frame
HX711_ADC LoadCell(15, 4); // DT, SCK
long t;
float WT = 0.0;
float weight_measure();
float measureWeight(void);
float calValue = 26.9;

float b;
char c ;
//String f;
float wt;
float gm;

void load_cell_setup()
{
  LoadCell.begin();
  long stabilisingtime = 1000;
  LoadCell.start(stabilisingtime);
  //  float f = 0.00f;   //Variable to store data read from EEPROM.
  //  calValue = (EEPROM.get(0, f));
  //  delay(100);
  //  delay(100);
  if (LoadCell.getTareTimeoutFlag())
  {
    Serial.println("Error : HX711 may not be connected properly");
  }
  else
  {
    LoadCell.setCalFactor(calValue); // set calibration value (float)
    Serial.println("Welcome");
    Serial.println("Criterion Tech BMI");
    Serial.println("calValue :" + String(calValue));
  }
}

float Load_cell_update()
{
  for (int j = 0; j < 100; j++)
  {
    LoadCell.update();
    gm = LoadCell.getData();
    gm = gm / 1000.0;
    delay(1);
  }
  return gm;
}

float measureWeight()
{
  for (int i = 0; i < 100; i++) // 100
  {
    LoadCell.update();
    delay(2); //2
  }
  float wt_sum = LoadCell.getData();
  if ((wt_sum < 0) && (wt_sum > -50))
    wt_sum = 0;
  t = millis();
  return wt_sum / 1000.0 ;
}

void hx711_tare()
{
  Serial.println(__FUNCTION__);
  float i;
  LoadCell.tareNoDelay();
}

void load_cell_check_tare_status()
{
  if (LoadCell.getTareStatus() == true)
  {
    Serial.println("Tare Completed");
  }
  else
  {
    Serial.println("Tare not  Completed");
  }
}
