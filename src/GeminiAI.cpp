#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "************";                                             //WIFI ssid
const char* password = "*************";                                        //WIFI password
const char* Gemini_Token = "*************";                                    //Your Gemini AI API
const char* Gemini_Max_Tokens = "150";                                         //The word limit of the answer you get
String Ask = "";

HTTPClient https;

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  while (!Serial);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() 
{
  AskingFunc();
}

void AskingFunc(){
  
  Serial.println("");
  Serial.println("Ask your Question : ");
  while (!Serial.available());
  while (Serial.available()) {
    char add = Serial.read();
    Ask = Ask + add;
    delay(1);
  }
  int len = Ask.length();
  Ask = Ask.substring(0, (len - 1));
  Ask = "\"" + Ask + "\"";
  Serial.println("");
  Serial.print("Asking Your Question : ");
  Serial.println(Ask);


  if (https.begin("https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash-latest:generateContent?key=" + (String)Gemini_Token)) {  // HTTPS

    https.addHeader("Content-Type", "application/json");
    String payload = String("{\"contents\": [{\"parts\":[{\"text\":" + Ask + "}]}],\"generationConfig\": {\"maxOutputTokens\": " + (String)Gemini_Max_Tokens + "}}");

    int httpCode = https.POST(payload);


    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      String payload = https.getString();

      DynamicJsonDocument doc(1024);

      deserializeJson(doc, payload);
      String Answer = doc["candidates"][0]["content"]["parts"][0]["text"];

      Serial.println("");
      Serial.println("Here is your Answer: ");
      Serial.println("");
      Serial.println(Answer);
    } else {
      Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
    }
    https.end();
  } else {
    Serial.printf("[HTTPS] Unable to connect\n");
  }
  Ask = "";
}