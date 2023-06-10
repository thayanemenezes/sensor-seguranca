#include <Ultrasonic.h> 
#include <WiFi.h>
#include <ThingSpeak.h>
#include "twilio.hpp"

#define SECRET_WRITE_APIKEY "9RDJEP32BCX1EZUW"
#define SECRET_READ_APIKEY "B54WLJBGF21Y7PLN"
#define SECRET_CH_ID 2175134
#define tempo 10 // Declara constante de tempo

// WiFiServer server(80);
WiFiClient client;

const char* ssid = "NOME_REDE_INTRNET";      // your network SSID (name)
const char* password = "SENHA_INTERNET";  // your network password

unsigned long channelNumber = SECRET_CH_ID;
const char* writeAPIKey = SECRET_WRITE_APIKEY;

const int echoPin = 19;  //PINO DIGITAL UTILIZADO PELO HC-SR04 ECHO(RECEBE)
const int trigPin = 18;   //PINO DIGITAL UTILIZADO PELO HC-SR04 TRIG(ENVIA)

const int pinoBuzzer = 27;  //PINO DIGITAL EM QUE O BUZZER ESTÁ CONECTADO

Ultrasonic ultrasonic(trigPin, echoPin);  //INICIALIZANDO OS PINOS

float distancia;
long duration;

int frequencia = 0;

// Values from Twilio (find them on the dashboard)
static const char *account_sid = "ACff1349f105c66c2393448a4b9ed9b3ce";
static const char *auth_token = "3617459a52442ddac18f020a32c74a44";

// Phone number should start with "+<countrycode>"
static const char *from_number = "whatsapp:+14155238886";
static const char *to_number = "whatsapp:+558589321462";
static const char *message = "Alerta sensor de segurança: nova movimentação detectada, verifique a entrada de sua casa.";

Twilio *twilio;

void setup() {
  pinMode(pinoBuzzer, OUTPUT);  //DECLARA O PINO COMO SENDO SAÍDA
  pinMode(echoPin, INPUT);      //DEFINE O PINO COMO ENTRADA (RECEBE)
  pinMode(trigPin, OUTPUT);     //DEFINE O PINO COMO SAÍDA (ENVIA)
  ThingSpeak.begin(client); 
  Serial.begin(115200);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
   delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distancia = duration * 0.034 / 2;

  unsigned long tempoSom = pulseIn(echoPin, HIGH);
  Serial.println("TEMPO SOM:");
  Serial.println(tempoSom);

  if(distancia >= 60 || distancia <=0) {
    Serial.println("Sensor desativado.");
    Serial.println("Nenhum objeto detecado.");
  } else {
  Serial.println("Sensor ativado.");
  Serial.println("Distancia do objeto: ");
  Serial.println(distancia);

  twilio = new Twilio(account_sid, auth_token);
  delay(1000);
  String responseTwilioAPi;
  bool success = twilio->send_message(to_number, from_number, message, responseTwilioAPi);
  if (success) {
    Serial.println("Sent message successfully!");
  } else {
    Serial.println(responseTwilioAPi);
  }

  for (frequencia = 150; frequencia < 1800; frequencia += 1) { // Tone que produz sirene de polícia
    tone(pinoBuzzer, frequencia, tempo);
    delay(2);
    }
  for (frequencia = 1800; frequencia > 150; frequencia -= 1) { // Tone que produz sirene de polícia
    tone(pinoBuzzer, frequencia, tempo);
    delay(2);
    }
  }

  int response = ThingSpeak.writeField(channelNumber, 1, distancia, writeAPIKey);
  if (response == 200) {
    Serial.println(response);
    Serial.println("Channel update successful ");
  } else {
    Serial.println(response);
    Serial.println("Problem updating channel. HTTP error code ");
  }

  delay(1000);
}