#include "BLEDevice.h"
#include <esp_task_wdt.h>
#include <Wire.h>
#define I2C_DEV_ADDR 0x55
#define LED 2
uint8_t spo2Data = 0;
uint8_t pulseRateData = 0;


/* Specify the Service UUID of Server */
static BLEUUID serviceUUID("6e400001-b5a3-f393-e0a9-e50e24dcca9e");
/* Specify the Characteristic UUID of Server */
static BLEUUID charUUID("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");

//#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
//#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

String mac_address = "";
long int rex_d = 0;

static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic,
                           uint8_t* pData, size_t length, bool isNotify) {

  // Serial.println(length);
  //  Serial.print("data: ");
  uint8_t spo2[20];
  for (int i = 0; i < length; i++) {
    spo2[i] = pData[i];
  }
  //  Serial.println();
  if (length == 20) {
    if (spo2[0] == 170 && spo2[1] == 85 && spo2[2] == 15) {
      if (spo2[3] == 8) {
        spo2Data = spo2[5];
        pulseRateData = spo2[6];
        Serial.print("{'SPO2':");
        Serial.print(spo2Data);
        Serial.print(", ");
        Serial.print("'PR': ");
        Serial.print(pulseRateData);
        Serial.println("}");
      }
    }
  }
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    digitalWrite(LED, HIGH);
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
    digitalWrite(LED, LOW);
  }
};

/* Start connection to the BLE Server */
bool connectToServer() {
  Serial.print("Connecting..");
  //  Serial.println(myDevice->getAddress().toString().c_str());
  mac_address = (myDevice->getAddress().toString().c_str());
  if (mac_address == "d7:9e:58:58:8d:bc")  //"d6:c4:61:1e:c2:6e"

  {

    BLEClient* pClient = BLEDevice::createClient();
    //    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    /* Connect to the remote BLE Server */
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    //    Serial.println(" - Connected to server");

    /* Obtain a reference to the service we are after in the remote BLE server */
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    //    Serial.println(" - Found our service");


    /* Obtain a reference to the characteristic in the service of the remote BLE server */
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    //    Serial.println(" - Found our characteristic");

    /* Read the value of the characteristic */
    /* Initial value is 'Hello, World!' */
    if (pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("The characteristic value was: ");
      Serial.println(value.c_str());
    }

    if (pRemoteCharacteristic->canNotify()) {
      pRemoteCharacteristic->registerForNotify(notifyCallback);
    }

    connected = true;
    return true;
  } else {
    Serial.print("Device not Found");
    connected = false;
    return false;
  }
}
/* Scan for BLE servers and find the first one that advertises the service we are looking for. */
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  /* Called for each advertising BLE server. */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.println("Ble found: ");
    Serial.println(advertisedDevice.toString().c_str());

    /* We have found a device, let us now see if it contains the service we are looking for. */
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;
    }
  }
};

void sendData() {

  Wire.beginTransmission(I2C_DEV_ADDR);  // Address of the receiver ESP32
  Wire.write(spo2Data);
  Wire.write(pulseRateData);
  Wire.endTransmission();
}

void setup() {

  Serial.begin(115200);
  Wire.begin((uint8_t)I2C_DEV_ADDR);
  Wire.onRequest(sendData);  // Set up the function to be called when a request is received
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  BLEDevice::init("ESP32 - BLE - Client");
  esp_task_wdt_init(30, true);  // Timeout in seconds, panic if timeout is reached
  esp_task_wdt_add(NULL);       // Add current task to the watchdog

  /* Retrieve a Scanner and set the callback we want to use to be informed when we
      have detected a new device.  Specify that we want active scanning and start the
      scan to run for 5 seconds. */
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(false);
}

void loop() {
  esp_task_wdt_reset();

  /* If the flag "doConnect" is true, then we have scanned for and found the desired
    BLE Server with which we wish to connect.  Now we connect to it.  Once we are
    connected we set the connected flag to be true. */
  if (doConnect == true)

  {
    if (connectToServer()) {
      Serial.println("Connected");
    } else {
      Serial.println("Failed to connect");
    }
    doConnect = false;
  }

  /* If we are connected to a peer BLE Server, update the characteristic each time we are reached
    with the current time since boot */
  if (connected) {
    String newValue = "Time since boot: " + String(millis());
    //        Serial.println("Setting new characteristic value to \"" + newValue + "\""); /

    /* Set the characteristic's value to be the array of bytes that is actually a string */
    //    pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
    /* You can see this value updated in the Server's Characteristic */
  } else if (doScan) {
    BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino

    // Delay or other processing
    delay(1000);
  }
  // BLEDevice::getScan()->clearResults();
  delay(1000); /* Delay a second between loops */
  //  Serial.println(doScan);
  //Serial.println("loop");
}
