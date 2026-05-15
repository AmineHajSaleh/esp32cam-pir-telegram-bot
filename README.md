# 📷 Caméra de Sécurité IoT & Détection de Mouvement (ESP32-CAM)

## 📝 Description
Ce projet implémente un système de vidéosurveillance embarqué et réactif. Basé sur un module ESP32-CAM (AI-Thinker), il utilise un capteur infrarouge passif (PIR) pour détecter les intrusions. 

La détection est gérée par une interruption matérielle (ISR) qui sort le système de son état de veille, active le flash LED intégré pour garantir la qualité de l'image, capture une photographie (JPEG) et l'envoie de manière sécurisée à l'utilisateur via un bot Telegram. Un système d'anti-rebond (debounce) logiciel de 10 secondes prévient le spam de notifications.

## 🛠️ Matériel Utilisé
* **Microcontrôleur :** Module ESP32-CAM (AI-Thinker) avec caméra OV2640
* **Capteur :** Détecteur de mouvement PIR (HC-SR501)
* **Interface de programmation :** Une carte ESP32 standard (utilisée comme convertisseur USB-Série)

## 🔌 Architecture et Câblage

**1. Connexion du Capteur PIR (Fonctionnement normal) :**
* `VCC` ➔ `5V` (ESP32-CAM)
* `GND` ➔ `GND` (ESP32-CAM)
* `OUT` ➔ `GPIO 13` (ESP32-CAM)

**2. Configuration de Programmation (Bypass via un autre ESP32) :**
Plutôt que d'utiliser un module FTDI externe, le téléversement a été réalisé en utilisant une carte ESP32 standard comme pont USB-UART. Pour ce faire, le microcontrôleur de la carte ESP32 principale a été désactivé en reliant sa broche `EN` à la masse. 

*Attention : Dans cette configuration matérielle spécifique, les signaux UART ne se croisent pas.*

* `EN` (ESP32 Pont) ➔ `GND` *(Désactive le MCU principal)*
* `TX` (ESP32 Pont) ➔ `U0T` *(TX de l'ESP32-CAM)*
* `RX` (ESP32 Pont) ➔ `U0R` *(RX de l'ESP32-CAM)*
* `5V` (ESP32 Pont) ➔ `5V` *(ESP32-CAM)*
* `GND` (ESP32 Pont) ➔ `GND` *(ESP32-CAM)*
* `IO0` (ESP32-CAM) ➔ `GND` *(Uniquement lors du téléversement, à déconnecter pour l'exécution)*
## 🔌 Schéma Électrique
![Schéma de câblage Fritzing](https://github.com/AmineHajSaleh/esp32cam-pir-telegram-bot/blob/main/Screenshot%202026-05-15%20170005.png)
## 🚀 Installation et Déploiement

1. Cloner le dépôt localement :
   ```bash
   git clone [https://github.com/aminehajsaleh/esp32cam-pir-telegram-bot.git](https://github.com/aminehajsaleh/esp32cam-pir-telegram-bot.git)
