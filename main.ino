#define BLYNK_USE_DIRECT_CONNECT

// Include required libraries
#include <Wire.h>
#include <BlynkSimpleSerialBLE.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <SPI.h>

// Define the pin connections
#define SOIL_MOISTURE_PIN A0         // Analog pin for soil moisture sensor
#define DHT_PIN 2                    // Digital pin for DHT sensor
#define RELAY_PIN 3                  // Digital pin for controlling water pump (Relay)

// Define sensor type for DHT (DHT11 or DHT22)
#define DHTTYPE DHT11

// Initialize the DHT sensor
DHT dht(DHT_PIN, DHTTYPE);

// Blynk authentication token
char auth[] = "Your_Blynk_Auth_Token";

// Soil moisture threshold level (below this level, the pump will turn on)
const int SOIL_MOISTURE_THRESHOLD = 300;

// Variables to store sensor values
float temperature = 0.0;             // Temperature value from DHT sensor
float humidity = 0.0;                // Humidity value from DHT sensor
int soilMoisture = 0;                // Soil moisture value from the analog sensor

// Blynk timer for periodic updates
BlynkTimer timer;

// Function to read sensor values and send them to Blynk
void readAndSendSensorData() {
  // Read temperature and humidity from DHT sensor
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  // Read soil moisture sensor value
  soilMoisture = analogRead(SOIL_MOISTURE_PIN);

  // Print sensor data to the serial monitor
  Serial.print("Soil Moisture: ");
  Serial.println(soilMoisture);
  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Humidity: ");
  Serial.println(humidity);

  // Send sensor values to Blynk app (virtual pins V0, V1, and V2)
  Blynk.virtualWrite(V0, soilMoisture);  // Send soil moisture value to V0
  Blynk.virtualWrite(V1, temperature);   // Send temperature value to V1
  Blynk.virtualWrite(V2, humidity);      // Send humidity value to V2

  // Control the water pump based on soil moisture level
  if (soilMoisture < SOIL_MOISTURE_THRESHOLD) {
    // Turn on the water pump if soil moisture is below the threshold
    digitalWrite(RELAY_PIN, HIGH);
    Blynk.virtualWrite(V3, "ON");  // Update water pump status to ON in Blynk app
    Serial.println("Water Pump: ON");
  } else {
    // Turn off the water pump if soil moisture is above the threshold
    digitalWrite(RELAY_PIN, LOW);
    Blynk.virtualWrite(V3, "OFF"); // Update water pump status to OFF in Blynk app
    Serial.println("Water Pump: OFF");
  }
}

// Setup function for initializing components
void setup() {
  // Start serial communication for debugging
  Serial.begin(9600);
  Serial.println("Initializing IoT-based Irrigation Controller...");

  // Initialize Blynk over BLE
  Blynk.begin(auth, Serial);

  // Initialize DHT sensor
  dht.begin();

  // Set pin modes
  pinMode(SOIL_MOISTURE_PIN, INPUT);  // Set soil moisture sensor pin as input
  pinMode(RELAY_PIN, OUTPUT);         // Set relay pin as output

  // Initialize relay to OFF state
  digitalWrite(RELAY_PIN, LOW);

  // Schedule the readAndSendSensorData function to run every 10 seconds
  timer.setInterval(10000L, readAndSendSensorData);
}

// Main loop function
void loop() {
  // Run Blynk to handle BLE communication
  Blynk.run();

  // Run the timer to call scheduled functions
  timer.run();
}
