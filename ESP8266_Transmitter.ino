/********************************************************************
   MPU6050 + BUTTON WIRELESS CONTROLLER NODE (ESP8266)

   MPU6050  -> movement (joystickTilt)
   PushBtn  -> attack trigger (joystickWobble)
********************************************************************/

#include <ESP8266WiFi.h>
extern "C" {
  #include <espnow.h>
}

#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include "RunningMedian.h"

// ============================================================
// ================= GAME SETTINGS (MATCH LED ESP) ============
#define JOYSTICK_ORIENTATION 1
#define JOYSTICK_DIRECTION   1
#define JOYSTICK_DEADZONE    5

#define ATTACK_THRESHOLD     30000   // MUST match game

// ============================================================
// ================= BUTTON ==========================
#define ATTACK_PIN 12   // GPIO14==D5

// ============================================================
// ================= MPU6050 ==========================
MPU6050 accelgyro;

int16_t ax, ay, az;
int16_t gx, gy, gz;

RunningMedian MPUAngleSamples = RunningMedian(5);

// ============================================================
// ================= ESP-NOW STRUCT ===========================
typedef struct {
  int tilt;
  int wobble;
  unsigned long t;
} MPUData;

MPUData packet;
//8C:AA:B5:51:82:17

// 🔴 CHANGE TO YOUR LED ESP MAC
uint8_t receiverMAC[] = {0x8C,0xAA,0xB5,0x51,0x82,0x17};

// ============================================================

void onSent(uint8_t *mac_addr, uint8_t sendStatus) {
  // Debug optional
}

// ============================================================

void setup() {

  Serial.begin(115200);
  delay(100);

  Serial.println("\nMPU6050 + Button Sender Starting...");

  pinMode(ATTACK_PIN, INPUT_PULLUP);

  // -------- WiFi / ESP-NOW ----------
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  Serial.print("My MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW Init Failed!");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(onSent);
  esp_now_add_peer(receiverMAC, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  // -------- MPU6050 ----------
  Wire.begin(); // SDA=D2, SCL=D1

  accelgyro.initialize();

  if (!accelgyro.testConnection()) {
    Serial.println("MPU6050 NOT FOUND!");
    while (1);
  }

  Serial.println("MPU6050 Ready.");
}

// ============================================================

void loop() {

  // -------- READ MPU ----------
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  int a = (JOYSTICK_ORIENTATION == 0 ? ax :
          (JOYSTICK_ORIENTATION == 1 ? ay : az)) / 166;

  // Deadzone (same as game)
  if (abs(a) < JOYSTICK_DEADZONE) a = 0;
  if (a > 0) a -= JOYSTICK_DEADZONE;
  if (a < 0) a += JOYSTICK_DEADZONE;

  MPUAngleSamples.add(a);

  int joystickTilt = MPUAngleSamples.getMedian();

  if (JOYSTICK_DIRECTION == 1) {
    joystickTilt = -joystickTilt;
  }

  // ==========================================================
  // ================= BUTTON → ATTACK ========================
  // Active LOW button
  bool attackPressed = (digitalRead(ATTACK_PIN) == LOW);

  int joystickWobble = 0;

  if (attackPressed) {
    // Force attack trigger
    joystickWobble = ATTACK_THRESHOLD + 1000;
  }

  // ==========================================================

  packet.tilt   = joystickTilt;
  packet.wobble = joystickWobble;
  packet.t      = millis();
  Serial.print("Packet Tilt:");
  Serial.print(packet.tilt);
  Serial.print("  Attack:");
  Serial.println(packet.wobble);

  esp_now_send(receiverMAC, (uint8_t*)&packet, sizeof(packet));

  // Debug (optional)
  /*
  Serial.print("Tilt: ");
  Serial.print(packet.tilt);
  Serial.print("  Attack: ");
  Serial.println(attackPressed);
  */

  delay(15); // ~60Hz update
}