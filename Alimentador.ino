#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>


//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "CelularLucas"
#define WIFI_PASSWORD "tntw5590"

// Insert Firebase project API Key
#define API_KEY "AIzaSyBjPzXUPm8deT0OrJRYg0CwfXVunJsYIKg"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://alimentadorpcs-default-rtdb.firebaseio.com/"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int intValue;
bool signupOK = false;

//NTP setup
#include "time.h"
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -10800;
const int   daylightOffset_sec = 0;

#define margemDelay 3

//Servo setup
#include <ESP32Servo.h>
#define PIN_SG90 22 //pino usado para o servo
Servo sg90;


void setup() {
  //Firebase
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  // WiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;


  /*authentication*/

  auth.user.email = "lucas.f.lacerda@usp.br";
  auth.user.password = "123456";
  /* Sign up */


  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  //NTP
  int dummy;
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  unixUpdate(&dummy);

  //Servo
  sg90.setPeriodHertz(50); // PWM frequency for SG90
  sg90.attach(PIN_SG90, 500, 2400); // Minimum and maximum pulse width (in µs) to go from 0° to 180
}

void loop() {
  delay(1000);
  //Firebase cloud
  int unix_cloud, valorCloud;
  unixCloud(&valorCloud, &unix_cloud);

  //NTP
  int unix_time;
  unixUpdate(&unix_time);

  if(horaCerta(unix_cloud, unix_time)){
    Serial.println("Ativando servo");
    ativar_Servo();
  }

  printf("dado = %d; unix_cloud = %d; unix_time fun= %d\n", valorCloud, unix_cloud, unix_time);
}

bool horaCerta(int horaCloud, int horaReal){
  if(horaReal >= horaCloud && horaReal <= horaCloud+margemDelay)
    return true;
  else
    return false;
}

void unixUpdate(int *unix_time){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  char horas[3];
  int hora10, hora1;
  char minutos[3];
  int minuto10, minuto1;
  char segundos[3];
  int segundo10, segundo1;
  
  strftime(horas,3, "%H", &timeinfo);
  hora10 = horas[0]-48;
  hora1 = horas[1]-48;
  
  strftime(minutos,3, "%M", &timeinfo);
  minuto10=minutos[0]-48;
  minuto1=minutos[1]-48;

  strftime(segundos,3, "%S", &timeinfo);
  segundo10=segundos[0] -48;
  segundo1=segundos[1]-48;

  *unix_time = segundo1+10*segundo10+60*minuto1+600*minuto10+3600*hora1+36000*hora10;
}

void unixCloud(int *valorCloud, int *unix_cloud){
  Firebase.RTDB.getInt(&fbdo, "/banco/hora", &*valorCloud);
  int a = *valorCloud;
  *unix_cloud = 0;
  *unix_cloud+= (a % 10) *60;
  a/=10;
  *unix_cloud+= (a % 10) *600;
  a/=10;
  *unix_cloud+= (a % 10) *3600;
  a/=10;
  *unix_cloud+= (a % 10) *36000;
}

void ativar_Servo(){
  for (int pos = 0; pos <= 180; pos += 1) {
    sg90.write(pos);
    delay(10);
  }
  Serial.println("Servo ativado");
  delay(margemDelay*1000 - 1800);
}