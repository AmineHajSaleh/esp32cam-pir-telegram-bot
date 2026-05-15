#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "esp_camera.h"
#include <UniversalTelegramBot.h>

// -------- CONFIG --------
const char* ssid     = "AminesiPhone";
const char* password = "al3ablibihmarra";
const char* botToken = "8456735953:AAFOm4qQH3MzU_jGda7-g-1qGTgtTqsKnn0";
const char* chatId   = "6077821125";

// -------- AI THINKER PINS --------
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
#define FLASH_LED_PIN      4
#define PIR_PIN           13

WiFiClientSecure client;
UniversalTelegramBot bot(botToken, client);

// -------- PIR INTERRUPT --------
volatile bool motionDetected = false;
unsigned long lastTrigger = 0;
const unsigned long debounceTime = 10000; // 10 s

void IRAM_ATTR pirISR() {
  motionDetected = true;
}

// -------- TELEGRAM CALLBACK --------
camera_fb_t *telegram_fb = nullptr;
size_t telegram_index = 0;

bool moreDataAvailable() {
  return telegram_index < telegram_fb->len;
}

uint8_t getNextByte() {
  return telegram_fb->buf[telegram_index++];
}

// -------- CAMERA INIT --------
void initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size   = FRAMESIZE_VGA;
    config.jpeg_quality = 10;
    config.fb_count     = 2;
  } else {
    config.frame_size   = FRAMESIZE_CIF;
    config.jpeg_quality = 12;
    config.fb_count     = 1;
  }

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init failed");
    ESP.restart();
  }
}

// -------- SETUP --------
void setup() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(FLASH_LED_PIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(PIR_PIN), pirISR, RISING);

  initCamera();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connecté");

  client.setInsecure();
  Serial.println("Système prêt (sans deep sleep)");
}

// -------- LOOP --------
void loop() {

  if (motionDetected) {
    unsigned long now = millis();
    if (now - lastTrigger > debounceTime) {
      lastTrigger = now;
      Serial.println(" Mouvement détecté");

      digitalWrite(FLASH_LED_PIN, HIGH);
      delay(300);

      camera_fb_t *fb = esp_camera_fb_get();
      esp_camera_fb_return(fb);
      fb = esp_camera_fb_get();

      digitalWrite(FLASH_LED_PIN, LOW);

      if (fb) {
        telegram_fb = fb;
        telegram_index = 0;

        String res = bot.sendPhotoByBinary(
          chatId,
          "image/jpeg",
          telegram_fb->len,
          moreDataAvailable,
          getNextByte,
          nullptr,
          nullptr
        );

        if (res.length() > 0) {
          bot.sendMessage(chatId, " Mouvement détecté", "");
        }

        esp_camera_fb_return(fb);
      }
    }
    motionDetected = false;
  }
}
