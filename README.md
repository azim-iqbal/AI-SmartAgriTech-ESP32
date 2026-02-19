![ESP32](https://img.shields.io/badge/ESP32-IoT-blue)
![AI](https://img.shields.io/badge/AI-Groq-green)
![Blynk](https://img.shields.io/badge/Blynk-IoT-orange)

# AI-SmartAgriTech-ESP32
AI-powered Smart Farm Monitoring System using ESP32, Blynk, Telegram Bot and Groq AI for intelligent irrigation recommendations.

Overview:
This project is an IoT-based Smart Farm Monitoring System built using ESP32, environmental sensors, and AI integration. It enables real-time monitoring of soil moisture, temperature, humidity, and motion detection while providing intelligent irrigation recommendations using AI.

The system integrates with:
-> ESP32 Microcontroller
-> DHT11 Temperature & Humidity Sensor
-> Soil Moisture Sensor
-> PIR Motion Sensor
-> Blynk IoT Dashboard
-> Telegram Bot Alerts
-> Groq AI (LLaMA 3.1 Model)

Features:
-> Real-time soil moisture monitoring
-> Temperature & humidity tracking
-> Motion detection alerts via Telegram
-> AI-generated irrigation advice
-> Remote monitoring using Blynk mobile app
-> Relay-controlled irrigation pump

AI Integration:
The system sends environmental data to the Groq AI API, which analyzes:
-> Soil moisture percentage
-> Temperature
-> Humidity
The AI generates short irrigation recommendations which are automatically sent to Telegram.

Architecture:
ESP32 → Sensor Data Collection
→ Blynk Dashboard (Live Monitoring)
→ Groq AI API (Analysis)
→ Telegram Bot (Alerts & AI Advice)

API documentation:
->> Groq AI API-
Used for generating intelligent irrigation advice.

Endpoint:
POST https://api.groq.com/openai/v1/chat/completions

Headers:
Content-Type: application/json
Authorization: Bearer YOUR_GROQ_API_KEY

Request Body Example:
{
  "model": "llama-3.1-8b-instant",
  "messages": [
    {
      "role": "user",
      "content": "Soil moisture is 45%, temperature is 28C. Give irrigation advice."
    }
  ]
}

Response Format:
{
  "choices": [
   {
      "message": {
        "content": "Soil moisture is adequate. Irrigation not required today."
      }
    }
  ]
}

->> Telegram Bot API-
Used for sending real-time alerts and AI recommendations.

Endpoint:
POST https://api.telegram.org/bot<token>/sendMessage

Parameters:
chat_id
text
Example:
chat_id=123456789
text=Motion detected in farm

Hardware Components:
-> ESP32
-> Soil Moisture Sensor
-> DHT11 Sensor
-> PIR Motion Sensor
-> Relay Module
-> Jumper Wires
-> Power Supply

By Team TriHacktrix
