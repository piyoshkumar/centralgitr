#define RXD2 16
#define TXD2 17
int dist = 0;
int average;
float H ;
//unsigned char check;
int i;
//unsigned char uart[9];
//const int HEADER = 0x59;

//int rec_debug_state = 0x01;//receive state for frame

void LIdar_setup()
{
  if (Serial2.available()) //check if serial port has data input
  {
    if (rec_debug_state == 0x01)
    {
      uart[0] = Serial2.read();
      if (uart[0] == 0x59)
      {
        check = uart[0];
        rec_debug_state = 0x02;
      }
    }
    else if (rec_debug_state == 0x02)
    { //the second byte
      uart[1] = Serial2.read();
      if (uart[1] == 0x59)
      {
        check += uart[1];
        rec_debug_state = 0x03;
      }
      else {
        rec_debug_state = 0x01;
      }
    }
    else if (rec_debug_state == 0x03)
    {
      uart[2] = Serial2.read();
      check += uart[2];
      rec_debug_state = 0x04;
    }
    else if (rec_debug_state == 0x04)
    {
      uart[3] = Serial2.read();
      check += uart[3];
      rec_debug_state = 0x05;
    }
    else if (rec_debug_state == 0x05)
    {
      uart[4] = Serial2.read();
      check += uart[4];
      rec_debug_state = 0x06;
    }
    else if (rec_debug_state == 0x06)
    {
      uart[5] = Serial2.read();
      check += uart[5];
      rec_debug_state = 0x07;
    }
    else if (rec_debug_state == 0x07)
    {
      uart[6] = Serial2.read();
      check += uart[6];
      rec_debug_state = 0x08;
    }
    else if (rec_debug_state == 0x08)
    {
      uart[7] = Serial2.read();
      check += uart[7];
      rec_debug_state = 0x09;
    }
    else if (rec_debug_state == 0x09)
    {
      uart[8] = Serial2.read();
      if (uart[8] == check)
      {
        dist = uart[2] + uart[3] * 256; //the distance
        //               Serial.println(dist);
        H = 209 - dist;
//        Serial.print("Height = ");
//        Serial.print(H);
//        Serial.println("cm");
        while (Serial2.available())
        {
          Serial2.read(); // This part is added becuase some previous packets are
        }
        //delay(200);
      }
      rec_debug_state = 0x01;
    }
  }
}
