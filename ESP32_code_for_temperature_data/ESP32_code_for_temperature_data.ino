#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ==== DS18B20 Setup ====
#define ONE_WIRE_BUS 33
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// ==== LCD (16x2) ====
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ==== WiFi Config ====
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// ==== Server Config ====
const char* serverName = "http://fiftechbd.com/en/wc/insert.php"; 
// insert.php

void setup() {
  Serial.begin(115200);

  // Init LCD
  Wire.begin(21, 22);  
  lcd.init();
  lcd.backlight();

  // Start DS18B20
  sensors.begin();

  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");

  // Connect WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected!");
  delay(1500);
  lcd.clear();
}

void loop() {
  sensors.requestTemperatures();   
  float tempC = sensors.getTempCByIndex(0);  

  if (tempC == DEVICE_DISCONNECTED_C) {
    Serial.println("Error: DS18B20 not detected!");
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error    ");
  } else {
    // Print to LCD
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(tempC, 2);  
    lcd.print((char)223);
    lcd.print("C   ");

    // Fahrenheit
    float tempF = sensors.toFahrenheit(tempC);
    lcd.setCursor(0, 1);
    lcd.print("Temp: ");
    lcd.print(tempF, 2);
    lcd.print((char)223);
    lcd.print("F   ");

    // Send data to server
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;

      http.begin(serverName);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      String postData = "temperature=" + String(tempC);
      int httpResponseCode = http.POST(postData);

      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("Server Response: " + response);
      } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }

      http.end();
    } else {
      Serial.println("WiFi Disconnected!");
    }
  }

  delay(5000);
}