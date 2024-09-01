#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> // Tambahkan ini untuk menggunakan fungsi JSON

const char* ssid = "Arabia123";
const char* password = "1sampai8";

const int jumlah_buzzer = 6;
const int jumlah_led = 6;
const int port_buzzer[jumlah_buzzer] = { 32, 25, 27, 33, 26, 14 };
const int port_led[jumlah_led] = { 18, 5, 2, 19, 4, 15 };

// Fungsi untuk menghubungkan ke WiFi
void setupWiFi() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void setup() {
  Serial.begin(9600);
  setupWiFi();

  // Set pin LED dan Buzzer sebagai output
  for (int i = 0; i < jumlah_led; i++) {
    pinMode(port_led[i], OUTPUT);
    digitalWrite(port_led[i], LOW); // Pastikan semua LED mati saat startup
  }

  for (int i = 0; i < jumlah_buzzer; i++) {
    pinMode(port_buzzer[i], OUTPUT);
    digitalWrite(port_buzzer[i], LOW); // Pastikan semua Buzzer mati saat startup
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Mengakses URL Firebase untuk mendapatkan status LED dengan token otorisasi
    http.begin("https://testingapplabu-default-rtdb.firebaseio.com/.json?auth=gLqV84CSfYOrrcbYOtRdZG6UZ03ZD2sivKUoHLOB");
    int httpCode = http.GET();

    if (httpCode > 0) { 
      String payload = http.getString();
      Serial.println(httpCode); 
      Serial.println("Response: " + payload);

      // Mem-parsing JSON respons
      DynamicJsonDocument jsonDoc(1024); // Alokasikan memori untuk dokumen JSON
      DeserializationError error = deserializeJson(jsonDoc, payload);

      if (error) {
        Serial.println("Error parsing JSON: " + String(error.c_str()));
        return;
      }

      // Kontrol LED dan Buzzer berdasarkan status dari Firebase
      for (int i = 0; i < jumlah_led; i++) {
        String ledStatus = jsonDoc["IOT" + String(i + 1)].as<String>();

        if (ledStatus == "ON") {
          digitalWrite(port_led[i], HIGH);
          digitalWrite(port_buzzer[i], HIGH);
          delay(100);  // Buzzer menyala selama 100ms
          digitalWrite(port_buzzer[i], LOW);
          Serial.println("IOT" + String(i + 1) + " - LED ON, Buzzer Sounded");
        } else {
          digitalWrite(port_led[i], LOW);
          Serial.println("IOT" + String(i + 1) + " - LED OFF");
        }
      }
    } else {
      Serial.println("Error on HTTP request");
    }

    http.end();
  }

  delay(1000);  // Cek status setiap 1 detik
}
