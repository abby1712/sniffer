#include "MicroFlow.h"
#include <Arduino_HTS221.h>
#include <ArduinoBLE.h>

int topology[] = {2, 2, 16, 1};
float temperature = 0;
float humidity = 0;
const float d0 = 1.0;             // Reference distance
const float P0 = -30.0;           // Reference power at d0
double weights[] = {0.03425892, 0.026571255, 0.11199758, 0.69698846, -0.061549675, -0.07260134, -0.13047796, -0.16550048, -0.060406506, -0.53315794, -0.47166237, 0.14521004, 0.29991212, -0.30560905, 0.075808465, 0.3016493, 0.03449348, -0.50614744, 0.605468, -0.31280586, 0.48760414, 0.006648176, 0.36837736, 0.13635626, -0.31100145, 0.1319251, 0.44017872, -0.06715291, 0.26286495, 0.35348117, 0.22982311, 0.5166191, -0.01108365, 0.25988382, 0.10445357, 0.14746594, -0.111512326, 0.4157653, -0.009750205, 0.11018192, -0.15631342, 0.14713989, 0.18731157, 0.97113925, 0.4549763, -0.5843651, -0.06062783, 0.0071255756, 0.35742944, 0.22381748, -0.22671308, 0.32191718};
double biases[] = {0.58959824, -0.0542794, -1.0994182, -0.010163374, -0.12686455, 0.24927843, 0.0, 0.13698052, 0.2799209, 2.886059, 0.35286123, -0.42221913, -0.75395983, 0.024243468, 0.35395432, 0.24757962, -1.3774092, 0.23756453, 0.39847994};
int layers = 4;
MicroMLP mlp(layers, topology, weights, biases, RELU); //Create the Multi-Layer Perceptron
void setup() {
  Serial.begin(9600);

  if (!HTS.begin()) {
    Serial.println("Failed to initialize humidity temperature sensor!");
    while (1);
  }
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  Serial.println("Bluetooth Low Energy Central scan");

  BLE.scan();

}
void loop()
{
  BLE.scan();

  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    Serial.print(peripheral.address());
    Serial.print("-");
    float temperature = HTS.readTemperature();
    float humidity    = HTS.readHumidity();
    delay(100);
    double inputs[] = {temperature, humidity};
    double output[0] = { };
    mlp.feedforward(inputs, output);
    delay(100);
    int rssi = peripheral.rssi();
    float Pr = P0 - (10 * output[0] * log10(d0)) - rssi;
    float d = pow(10, (Pr - P0) / (10 * output[0]));
    Serial.print(d,1);// Print with 1 decimal place
    delay(100);
    Serial.println();
  }
}
