#include <Constellation.h>
#include <Wire.h>
#include <Adafruit_TSL2561_U.h>
#include <pgmspace.h>

#include <Constellation.h>

Adafruit_TSL2561_Unified tsl(TSL2561_ADDR_FLOAT); 

/* Arduino Wifi (ex: Wifi Shield) */
//#include <SPI.h>
//#include <WiFi.h>

/* Arduino Wifi101 (WiFi Shield 101 and MKR1000 board) */
//#include <SPI.h>
//#include <WiFi101.h>

/* ESP8266 Wifi */
#include <ESP8266WiFi.h>

/* Wifi credentials */
// char ssid[] = "Connectify-AK";
char ssid[] = "Connectify-AB";
// char password[] = "killianESP8266";
char password[] = "coucoutoi";

static boolean initMode = false;

/* Create the Constellation client */
// Constellation<WiFiClient> constellation("192.168.137.1", 8088, "ESP8266", "QCM", "20a6ce246d11435b05b821a77669eeec31c25bc7");
Constellation<WiFiClient> constellation("192.168.121.1", 8088, "ESP8266", "QCM", "38c2fe74fc46b4a22a9858916e53e6d285608c8f");

void setup(void) {
  Serial.begin(115200);  delay(10);
  
  pinMode(D1, INPUT_PULLUP);
  pinMode(D2, INPUT_PULLUP);
  pinMode(D3, INPUT_PULLUP);
  
  // Connecting to Wifi  
  Serial.print("Connecting to ");
  Serial.println(ssid);  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected. IP: ");
  Serial.println(WiFi.localIP());

  // Receive a message to be initialized
  constellation.registerMessageCallback("InitPlayers",
    [](JsonObject& json) { 
      initMode = true;
   });

  // Receive the question
  constellation.registerMessageCallback("SendQuestion",
    MessageCallbackDescriptor().setDescription("Envoi de question").addParameter("q", "qr"),
    [](JsonObject& json) { 
      initMode = false;
   });

  // Declare the package descriptor
  constellation.declarePackageDescriptor();

  // Subscribe to message group
  constellation.subscribeToGroup("ClientQCM"); 
}
  
void loop(void) {
  checkButtonsState();
  constellation.loop();
}

void checkButtonsState() {
  static int lastButtonStates[3];
  int buttonStates[3];
  for (int state : lastButtonStates) {
    state = LOW;
  }
  buttonStates[0] = digitalRead(D1);
  buttonStates[1] = digitalRead(D2);
  buttonStates[2] = digitalRead(D3);
  
  for (int i = 0; i < 3; i++) {
    if (buttonStates[i] != lastButtonStates[i] && buttonStates[i] == LOW) {
       String s = String(i);
       char cstr[sizeof(s)];
       s.toCharArray(cstr, sizeof(cstr));
       if (initMode == true) {
        constellation.sendMessage(Package, "Quiz", "NewPlayer", cstr);
        initMode = false;
       } else constellation.sendMessage(Package, "Quiz", "SendAnswer", cstr);
    }  
    lastButtonStates[i] = buttonStates[i];
  }
}
