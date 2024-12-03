#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <SPI.h>

// Define hardware type, size, and output pins:
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

// Explicação Display
// Display | Arduino
//   VCC   |   5v
//   GND   |   GND
//   DIN   |   11
//   CS    |   10
//   CLK   |   13
#define MAX_DEVICES 8  
#define CLK_PIN   18 // VSPI_SCK
#define DATA_PIN  23 // VSPI_MOSI
#define CS_PIN    5  // VSPI_SS

// Criando Objeto "Display" usando a Class MD_PAROLA da própria lib
MD_Parola Display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// Função responsável por inicializar display
void initDisplay(){
  Display.begin();
  Display.setIntensity(5);
  Display.displayClear();  
}

// Setando placar no display
void setPlacar(int meta, int alcancado) {
  Display.setTextEffect(1, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  Display.setTextAlignment(PA_LEFT);
  //Serial.println("Tampas"+String(alcancado));
  Display.print("Tampas"+String(alcancado));
}

// -------------------------------------------------------------------------------- Display ^
// -------------------------------------------------------------------------------- Wifi v

#include <WiFi.h>
#include <HTTPClient.h>;
#include <ArduinoJson.h>;

const char* ssid = "Nome da rede";
const char* password = "senha da rede";

const String apiUrl = "api/endpoint";

void wifiConfig(){
  WiFi.begin(ssid, password);
  Serial.println("Configuring access point...");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("Conectado");
  Serial.println(WiFi.localIP());
  Serial.println("");
}

void saveCap(){
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("+");

    HTTPClient client;

    client.begin(apiUrl);
   
    int httpCode = client.GET();

    client.addHeader("Connection", "close");
      
    if (httpCode > 0) {
      String payload = client.getString();
      Serial.println("\nStatuscode: " + String(httpCode));
      Serial.println(payload);
    } else {
      Serial.println("Error on HTTP Request");
      Serial.println("\nStatuscode: " + String(httpCode));
    }
  }else{
         delay(100); 
  }
}

// -------------------------------------------------------------------------------- Sensor v

// Explicação Sensor

//   Sensor  | Arduino
//   Marrom  |   5v
//   Azul    |   GND
//   Preto   |   Data

const int sensor = 34;
//const int sensor = 2;

boolean hasMoviment(){
  //return (digitalRead(sensor) == LOW);
  Serial.println(analogRead(sensor));
  return (analogRead(sensor) < 3000);
}

// -------------------------------------------------------------------------------- Variáveis de "Software" v

int meta = 10000;
int alcancado = 0;

void verifyGoal(){
  if(alcancado >= meta){
    alcancado = 0; 
  }
}

void increment(){
  if(hasMoviment()){
    alcancado++;    
    
    saveCap();
    
    verifyGoal();
  }  
}

// -------------------------------------------------------------------------------- Main v

void setup() {
  Serial.begin(115200);
  
  initDisplay();
  
  wifiConfig();
  
  pinMode(sensor, INPUT);
}

void loop() {
  setPlacar(meta, alcancado);
  increment();
}
