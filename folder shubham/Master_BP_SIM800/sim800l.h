#include "HardwareSerial.h"
#define sim800_Serial Serial1
void clear_buffer() {
  while (sim800_Serial.available()) {
    sim800_Serial.read();  // Read and discard any characters in the buffer
  }
}

bool sms_setup() {

  sim800_Serial.println("AT");
  delay(100);
  sim800_Serial.println("ATE0");
  delay(100);
  // Check if SIM800 is connected to the network
  sim800_Serial.println("AT+CREG?");
  delay(100);
  String response = sim800_Serial.readStringUntil('\n');
  // Serial.println(response);
  if (response.indexOf("+CREG: 0,1") != -1) {
    // Serial.println("SIM800 is connected to the network.");
    sim800_Serial.println("AT+CMGF=1");  // Set SMS mode to text
    delay(1000);
    sim800_Serial.println("AT+CNMI=2,2,0,0,0");  // Enable receiving SMS notifications
    delay(1000);
    return true;
  } else {
    // Serial.println("Waiting for SIM800 to connect to the network...");
    return false;
  }
}


String getPhoneNumber(String sms) {
  int index1 = sms.indexOf("\"");
  int index2 = sms.indexOf("\"", index1 + 1);
  return sms.substring(index1 + 1, index2);
}

void sendSMS_command(String phoneNumber, String message) {
  // clear_buffer();
  sim800_Serial.println("AT+CMGS=\"" + phoneNumber + "\"");
  // clear_buffer();
  delay(100);
  // clear_buffer();
  sim800_Serial.println(message);
  // clear_buffer();
  delay(100);
  // clear_buffer();
  sim800_Serial.println((char)26);  // End of message character
  // clear_buffer();
  delay(100);
  // Serial.println("Response sent to: " + phoneNumber);
}

String sms_read() {
  if (sim800_Serial.available() > 1) {
    delay(10);
    String sms1 = sim800_Serial.readStringUntil('\n');
    String sms2 = sim800_Serial.readStringUntil('\n');
    String sms3 = sim800_Serial.readStringUntil('\n');
    // MSG_RECEIVED = sms3;
    // Serial.println("1" + sms1);
    // Serial.println("2" + sms2);
    // Serial.println("3" + sms3);
    // if (sms.indexOf("DATA") != -1) {
    //   String response = sms_data;
    //   String phoneNumber = getPhoneNumber(sms);
    //   sendSMS_command(phoneNumber, response);
    // }
    return sms3;
  }
  return " ";
}
void checkNetworkStatus() {
  // Get network registration status
  sim800_Serial.println("AT+CREG?");
  // clear_buffer();
  delay(100);

  while (sim800_Serial.available()) {
    String response = sim800_Serial.readStringUntil('\n');
    // Serial.println(response);
    if (response.startsWith("+CREG: ")) {
      int status = response.substring(9).toInt();  // Extract the status code
      // int status1 = response.substring(9).toInt();  // Extract the status code
      // Serial.println("status: " + String(status));
      // Serial.println("status1: " + String(status1));
      switch (status) {
        case 0:
          Serial.println("Not registered, not searching for a new operator.");
          break;
        case 1:
          Serial.println("Registered, home network.");
          break;
        case 2:
          Serial.println("Not registered, but searching for a new operator.");
          break;
        case 3:
          Serial.println("Registration denied.");
          break;
        case 4:
          Serial.println("Unknown.");
          break;
        case 5:
          Serial.println("Registered, roaming.");
          break;
        default:
          Serial.println("Unknown status.");
          break;
      }
    }
  }
}

void read_command() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');  // Read the entire line
    if (command.startsWith("network")) {
      // Serial.println("Netwok Search Mode");
      delay(100);
      checkNetworkStatus();
    } else if (command.startsWith("restart")) {
      // Serial.println("ESP32 Restart");
      delay(100);
      ESP.restart();
    } else if (command.startsWith("sms")) {
      // Serial.println("SMS Mode");
      delay(100);
      String mob = command.substring(4, command.indexOf(',', 4));       // Extract mobile number
      String message = command.substring(command.indexOf(',', 4) + 1);  // Extract message
      // Serial.println("mobile No. :" + String(mob));
      // Serial.println("Message. :" + String(message));
      sendSMS_command(mob, message);
    } else {
      // Serial.println("Not valid Command");
    }
  }
}
