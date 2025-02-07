#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Keypad.h>
#include <Servo.h>
#include <WiFiS3.h>
#include <Arduino_JSON.h>

// LCD Configuration
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// WiFi and Server Configuration
const char PROGMEM ssid[] = "";
const char PROGMEM password[] = "";
const char PROGMEM serverName[] = "";
const int serverPort = 3000;

// Keypad Configuration
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Adafruit_Keypad customKeypad = Adafruit_Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Servo Configuration
Servo myServo;

// Ultrasonic Sensor Configuration
const int trigPin = 13;
const int echoPin = 12;

// Variables
String userIdInput = "";
String otpInput = "";
String currentOTP = "";
String currentUserName = "";  // Aggiunta per memorizzare il nome dell'utente corrente
long duration;
int distance;
bool userPresent = false;
bool messageShown = false;
bool waitingForOTP = false;
bool accessGranted = false;

struct UserData {
  String id;
  String nome;
  String cognome;
  String email;
};

String generateOTP() {
  String otp;
  for(byte i = 0; i < 6; i++) {
    otp += String(random(10));
  }
  return otp;
}

bool sendEmailRequest(const String &toEmail, const String &userName, String &generatedOTP) {
  WiFiClient client;
  generatedOTP = generateOTP();
  
  if (client.connect(serverName, serverPort)) {
    String postData = F("email=");
    postData += toEmail;
    postData += F("&name=");
    postData += userName;
    postData += F("&otp=");
    postData += generatedOTP;
    
    String request = F("POST /sendemail HTTP/1.1\r\n");
    request += F("Host: ");
    request += serverName;
    request += F("\r\nContent-Type: application/x-www-form-urlencoded\r\n");
    request += F("Content-Length: ");
    request += String(postData.length());
    request += F("\r\nConnection: close\r\n\r\n");
    request += postData;

    client.print(request);
    
    unsigned long timeout = millis();
    while (client.connected() && !client.available()) {
      if (millis() - timeout > 5000) {
        client.stop();
        return false;
      }
    }
    
    while (client.available()) {
      String line = client.readStringUntil('\n');
      if (line.indexOf("200 OK") > 0) {
        client.stop();
        return true;
      }
    }
    client.stop();
  }
  return false;
}

UserData getUserFromServer(const String &userId) {
  UserData userData;
  WiFiClient client;
  
  Serial.println("Tentativo di connessione al server...");
  
  if (client.connect(serverName, serverPort)) {
    Serial.println("Connesso al server");
    
    String request = F("GET /users/");
    request += userId;
    request += F(" HTTP/1.1\r\nHost: ");
    request += serverName;
    request += F("\r\nConnection: close\r\n\r\n");

    Serial.println("Invio richiesta:");
    Serial.println(request);
    
    client.print(request);
    
    unsigned long timeout = millis();
    String responseBody = "";
    bool isBody = false;
    
    while (client.connected()) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        Serial.println(line);
        
        if (line == "\r") {
          isBody = true;
          continue;
        }
        
        if (isBody) {
          responseBody += line;
        }
      }
      
      if (millis() - timeout > 5000) {
        Serial.println("Timeout nella risposta");
        break;
      }
    }
    
    Serial.println("Risposta completa dal server:");
    Serial.println(responseBody);
    
    JSONVar jsonObject = JSON.parse(responseBody);
    
    if (JSON.typeof(jsonObject) != "undefined") {
      Serial.println("JSON parsato correttamente");
      
      if (jsonObject.hasOwnProperty("id")) {
        userData.id = (const char*)jsonObject["id"];
        Serial.println("ID trovato: " + userData.id);
      }
      if (jsonObject.hasOwnProperty("nome")) {
        userData.nome = (const char*)jsonObject["nome"];
        Serial.println("Nome trovato: " + userData.nome);
      }
      if (jsonObject.hasOwnProperty("cognome")) {
        userData.cognome = (const char*)jsonObject["cognome"];
        Serial.println("Cognome trovato: " + userData.cognome);
      }
      if (jsonObject.hasOwnProperty("email")) {
        userData.email = (const char*)jsonObject["email"];
        Serial.println("Email trovata: " + userData.email);
      }
    } else {
      Serial.println("Errore nel parsing JSON");
    }
    
    client.stop();
  } else {
    Serial.println("Connessione al server fallita");
  }
  
  return userData;
}

void handleUserIdentification() {
  customKeypad.tick();
  
  if (accessGranted) {
    return;
  }
  
  while(customKeypad.available()) {
    keypadEvent e = customKeypad.read();
    if (e.bit.EVENT == KEY_JUST_PRESSED) {
      char key = e.bit.KEY;
      
      if (waitingForOTP) {
        if (isDigit(key)) {
          otpInput += key;
          lcd.clear();
          lcd.print(F("OTP: "));
          lcd.print(otpInput);
        } else if (key == '#' && otpInput.length() > 0) {
          if (otpInput == currentOTP) {
            lcd.clear();
            lcd.print(F("Benvenuto "));
            lcd.setCursor(0, 1);
            lcd.print(currentUserName);
            myServo.write(90);
            accessGranted = true;
            waitingForOTP = false;
          } else {
            lcd.clear();
            lcd.print(F("OTP Errato"));
            delay(2000);
            otpInput = "";
            lcd.clear();
            lcd.print(F("Inserisci OTP:"));
          }
        } else if (key == '*') {
          otpInput = "";
          lcd.clear();
          lcd.print(F("Inserisci OTP:"));
        }
      } else {
        if (isDigit(key)) {
          userIdInput += key;
          lcd.clear();
          lcd.print(F("ID: "));
          lcd.print(userIdInput);
        } else if (key == '#' && userIdInput.length() > 0) {
          lcd.clear();
          lcd.print(F("Richiesta..."));
          UserData user = getUserFromServer(userIdInput);
          
          if (user.id.length() > 0) {
            currentUserName = user.nome;
            lcd.clear();
            lcd.print(F("Invio email..."));
            if (sendEmailRequest(user.email, user.nome, currentOTP)) {
              lcd.clear();
              lcd.print(F("Email inviata!"));
              delay(2000);
              lcd.clear();
              lcd.print(F("Inserisci OTP:"));
              waitingForOTP = true;
            } else {
              lcd.clear();
              lcd.print(F("Errore email!"));
              delay(2000);
              lcd.clear();
              lcd.print(F("Inserisci ID:"));
            }
          } else {
            lcd.clear();
            lcd.print(F("Utente non"));
            lcd.setCursor(0, 1);
            lcd.print(F("trovato!"));
            delay(2000);
            lcd.clear();
            lcd.print(F("Inserisci ID:"));
          }
          userIdInput = "";
        } else if (key == '*') {
          userIdInput = "";
          lcd.clear();
          lcd.print(F("Inserisci ID:"));
        }
      }
    }
  }
}

void setup() {
  Serial.begin(9600);
  customKeypad.begin();
  lcd.init();
  lcd.backlight();
  myServo.attach(10);
  myServo.write(0);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.print(F("Connessione a "));
  Serial.println(ssid);
  lcd.print(F("Connessione WiFi"));
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println(F("WiFi connesso!"));
  lcd.clear();
  lcd.print(F("WiFi connesso"));
  delay(2000);
  lcd.clear();
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  if (distance < 50) {
    if (!userPresent) {
      userPresent = true;
      messageShown = false;
      waitingForOTP = false;
      accessGranted = false;
      userIdInput = "";
      otpInput = "";
      currentOTP = "";
      currentUserName = "";
    }

    if (!messageShown && !accessGranted) {
      lcd.backlight();
      lcd.clear();
      lcd.print(F("Inserisci ID:"));
      messageShown = true;
    }

    handleUserIdentification();
  } else {
    if (userPresent) {
      lcd.noBacklight();
      lcd.clear();
      myServo.write(0);
      userPresent = false;
      messageShown = false;
      waitingForOTP = false;
      accessGranted = false;
      userIdInput = "";
      otpInput = "";
      currentOTP = "";
      currentUserName = "";
    }
  }
}
