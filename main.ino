#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME "smart farm"
#define BLYNK_AUTH_TOKEN ""

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

//wiFi Username and Password
char ssid[] = "";
char pass[] = "";

//TELEGRAM 
String botToken = "";
String chatID = "";

//GROQ API
String groqApiKey = "";

#define SOIL_PIN 34
#define RELAY_PIN 26
#define DHTPIN 4
#define PIR_PIN 27
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// Soil calibration
int dryValue = 4000;
int wetValue = 1500;

//TELEGRAM 
void sendTelegram(String message) {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient https;

  String url = "https://api.telegram.org/bot" + botToken + "/sendMessage";
  https.begin(client, url);
  https.addHeader("Content-Type", "application/x-www-form-urlencoded");

  String data = "chat_id=" + chatID + "&text=" + message;
  https.POST(data);

  https.end();
}

//AI (GROQ) 
String getAIAdvice(int soil, float temp, float hum) {

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient https;

  https.begin(client, "https://api.groq.com/openai/v1/chat/completions");
  https.addHeader("Content-Type", "application/json");
  https.addHeader("Authorization", "Bearer " + groqApiKey);

  String prompt = "Soil moisture is " + String(soil) +
                  "%. Temperature is " + String(temp) +
                  "C. Humidity is " + String(hum) +
                  "%. Give short irrigation advice in 2 lines only.";

  String body = "{";
  body += "\"model\":\"llama-3.1-8b-instant\",";
  body += "\"messages\":[{\"role\":\"user\",\"content\":\"" + prompt + "\"}]";
  body += "}";

  int httpCode = https.POST(body);

  Serial.print("AI HTTP Code: ");
  Serial.println(httpCode);

  if (httpCode > 0) {

    String response = https.getString();
    Serial.println("AI Raw Response:");
    Serial.println(response);

    StaticJsonDocument<4096> doc;
    DeserializationError error = deserializeJson(doc, response);

    if (!error) {
      if (doc.containsKey("choices")) {
        https.end();
        return doc["choices"][0]["message"]["content"].as<String>();
      }
    }
  }

  https.end();
  return "AI service unavailable.";
}

//SENSOR UPDATE
void sendSensorData() {

  int soilRaw = analogRead(SOIL_PIN);
  int soilPercent = map(soilRaw, dryValue, wetValue, 0, 100);
  soilPercent = constrain(soilPercent, 0, 100);

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int motion = digitalRead(PIR_PIN);

  Serial.print("Raw Soil: ");
  Serial.println(soilRaw);

  Serial.print("Soil %: ");
  Serial.println(soilPercent);

  Serial.print("Motion: ");
  Serial.println(motion);

  Blynk.virtualWrite(V0, soilPercent);
  Blynk.virtualWrite(V1, temp);
  Blynk.virtualWrite(V2, hum);
  Blynk.virtualWrite(V4, motion);

  static bool motionSent = false;

  if (motion == 1 && !motionSent) {
    sendTelegram("⚠ Motion detected in farm!");
    motionSent = true;
  }

  if (motion == 0) {
    motionSent = false;
  }
}

//AI UPDATE 
void sendAIUpdate() {

  int soilRaw = analogRead(SOIL_PIN);
  int soilPercent = map(soilRaw, dryValue, wetValue, 0, 100);
  soilPercent = constrain(soilPercent, 0, 100);

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  String advice = getAIAdvice(soilPercent, temp, hum);

  sendTelegram("🌱 AI Farm Update:\n\n" + advice);
}

// Pump control
BLYNK_WRITE(V3) {
  digitalWrite(RELAY_PIN, param.asInt());
}

//SETUP
void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  pinMode(PIR_PIN, INPUT);

  dht.begin();

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(2000L, sendSensorData);
  timer.setInterval(180000L, sendAIUpdate);
}

//LOOP
void loop() {
  Blynk.run();
  timer.run();
}
