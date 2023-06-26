#include <ArduinoBLE.h>

// Constants for the path loss model
const float d0 = 1.0;             // Reference distance
const float P0 = -30.0;           // Reference power at d0
const float n = 3.2;              // Path loss exponent

// Function to calculate the distance between two BLE devices
float calculateDistance(float rssi) {
  float Pr = P0 - (10 * n * log10(d0)) - rssi;
  float d = pow(10, (Pr - P0) / (10 * n));
  return d;
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  Serial.println("Bluetooth Low Energy Central scan");

  BLE.scan();
}

void loop() {
  BLE.scan();
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    Serial.print(peripheral.address());
    Serial.print("-");
    
    int rssi = peripheral.rssi();
    float distance = calculateDistance(rssi);

    Serial.print(distance, 1); // Print with 1 decimal place
    Serial.println();
  }

  delay(1);
}
