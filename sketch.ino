#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>

// Pin Definitions
#define DS18B20_PIN1 14
#define DS18B20_PIN2 12
#define DHT_PIN 4
#define DHT_TYPE DHT20

// Relay pins
const int relayPins[] = {16, 17, 18, 19, 21};
const int numRelays = 5;

// Potentiometer pins for fan simulation
const int potPins[] = {32, 33, 34}; // Assuming 3 fans
const int numPots = 3;

// Initialize sensors
OneWire oneWire1(DS18B20_PIN1);
OneWire oneWire2(DS18B20_PIN2);
DallasTemperature ds18b20_1(&oneWire1);
DallasTemperature ds18b20_2(&oneWire2);
DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(115200);

  // Initialize relays
  for (int i = 0; i < numRelays; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH);  // Turn off relays (active low)
  }

  // Initialize potentiometer pins
  for (int i = 0; i < numPots; i++) {
    pinMode(potPins[i], INPUT);
  }

  // Start sensors
  ds18b20_1.begin();
  ds18b20_2.begin();
  dht.begin();

  Serial.println("System initialized");
}

void loop() {
  // Read and print temperatures from DS18B20 sensors
  ds18b20_1.requestTemperatures();
  ds18b20_2.requestTemperatures();
  float temp1 = ds18b20_1.getTempCByIndex(0);
  float temp2 = ds18b20_2.getTempCByIndex(0);

  Serial.print("DS18B20 #1 Temperature: ");
  Serial.print(temp1);
  Serial.println(" °C");
  Serial.print("DS18B20 #2 Temperature: ");
  Serial.print(temp2);
  Serial.println(" °C");

  // Read and print temperature and humidity from DHT20
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (!isnan(humidity) && !isnan(temperature)) {
    Serial.print("DHT20 Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
    Serial.print("DHT20 Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
  } else {
    Serial.println("Failed to read from DHT sensor!");
  }

  // Control relays (LEDs)
  for (int i = 0; i < 2; i++) {
    if (Serial.available()) {
      char input = Serial.read();
      if (input == '1' + i) {
        digitalWrite(relayPins[i], !digitalRead(relayPins[i])); // Toggle LED state
        Serial.print("LED ");
        Serial.print(i + 1);
        Serial.println(digitalRead(relayPins[i]) == LOW ? " ON" : " OFF");
      }
    }
  }

  // Read potentiometers and control fan relays
  for (int i = 0; i < numPots; i++) {
    int potValue = analogRead(potPins[i]);
    int fanSpeed = map(potValue, 0, 4095, 0, 100); // Map potentiometer value to 0-100%

    Serial.print("Fan ");
    Serial.print(i + 1);
    Serial.print(" Speed: ");
    Serial.print(fanSpeed);
    Serial.println("%");

    // Control fan relay based on potentiometer value
    if (fanSpeed > 0) {
      digitalWrite(relayPins[i + 2], LOW); // Turn on fan (active low)
    } else {
      digitalWrite(relayPins[i + 2], HIGH); // Turn off fan
    }
  }

  Serial.println("------------------------");
  delay(2000); // Wait for 2 seconds before next reading
}