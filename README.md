# SISTEMA DI AUTENFICAZIONE CON OTP

## Descrizione
Il progetto **IoT Security** si propone di migliorare la sicurezza nell'accesso ai dispositivi IoT attraverso un meccanismo di autenticazione basato su **OTP (One-Time Password)**. Il sistema utilizza un **server Node.js** per gestire l'autenticazione e un **firmware Arduino** per il controllo hardware dei dispositivi connessi.

## Componenti del Progetto
Il progetto si compone di due moduli principali:

### 1. Server di Autenticazione (Node.js)
- Implementato utilizzando **Express.js** per la gestione delle richieste HTTP.
- Gestisce una lista di utenti simulata.
- Invia OTP via email tramite **Nodemailer**.
- Fornisce API REST per la registrazione e l'autenticazione degli utenti.
- Configurato per l'ascolto su una porta specifica (`3000`).

### 2. Firmware IoT (Arduino)
- Sviluppato in **C++** utilizzando l'IDE di Arduino.
- Comunica con il server Node.js tramite richieste HTTP.
- Gestisce l'attivazione/disattivazione di un dispositivo basandosi sull'OTP ricevuto.
- Include logica di timeout per garantire la sicurezza dell'accesso.

## Architettura del Sistema
Il sistema si basa su un'architettura client-server:

1. **L'utente richiede l'accesso** al dispositivo IoT tramite un'interfaccia (web/app o direttamente dal dispositivo).
2. **Il server genera un OTP** e lo invia via email all'utente.
3. **L'utente inserisce l'OTP** nel dispositivo IoT.
4. **Il dispositivo invia l'OTP al server per la verifica**.
5. **Se valido, il dispositivo viene attivato**; altrimenti, l'accesso viene negato.

## Installazione e Configurazione
### Requisiti
- **Node.js** e **npm** installati
- Un account Gmail per l'invio delle email OTP
- Un dispositivo **Arduino** compatibile con connessione Internet (es. ESP8266, ESP32 o Arduino Uno R4 Wifi)

### Setup del Server
1. Clonare il repository:
   git clone https://github.com/fil1409/ProgettoIoTSecurity.git
   cd ProgettoIoTSecurity
2. Installare le dipendenze:
   npm install
3. Configurare le credenziali email in server.js.
4. Avviare il server:
   node server.js

### Setup del Firmware IoT
1. Aprire ProgettoIotSecurity.ino con Arduino IDE.
2. Configurare i parametri di rete per la connessione al server.
3. Caricare il codice sulla scheda Arduino.
4. Testare la comunicazione con il server e la verifica OTP.

# AUTHENTICATION SYSTEM WITH OTP

## Description
The **IoT Security** project aims to enhance access security for IoT devices through an authentication mechanism based on **OTP (One-Time Password)**. The system uses a **Node.js server** to manage authentication and an **Arduino firmware** to control the connected hardware devices.

## Project Components
The project consists of two main modules:

### 1. Authentication Server (Node.js)
- Implemented using **Express.js** to handle HTTP requests.
- Manages a simulated user list.
- Sends OTP via email using **Nodemailer**.
- Provides REST API for user registration and authentication.
- Configured to listen on a specific port (`3000`).

### 2. IoT Firmware (Arduino)
- Developed in **C++** using the Arduino IDE.
- Communicates with the Node.js server via HTTP requests.
- Manages device activation/deactivation based on the received OTP.
- Includes timeout logic to ensure access security.

## System Architecture
The system follows a client-server architecture:

1. **The user requests access** to the IoT device through an interface (web/app or directly from the device).
2. **The server generates an OTP** and sends it via email to the user.
3. **The user enters the OTP** on the IoT device.
4. **The device sends the OTP to the server for verification**.
5. **If valid, the device is activated**; otherwise, access is denied.

## Installation and Configuration
### Requirements
- Installed **Node.js** and **npm**
- A Gmail account for sending OTP emails
- An **Arduino** device compatible with Internet connection (e.g., ESP8266, ESP32 or Arduino Uno R4 Wifi)

### Server Setup
1. Clone the repository:
   git clone https://github.com/fil1409/ProgettoIoTSecurity.git
   cd ProgettoIoTSecurity
2. Install dependencies:
   npm install
3. Configure email credentials in `server.js`.
4. Start the server:
   node server.js
