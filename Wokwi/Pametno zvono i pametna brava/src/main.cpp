#include <Keypad.h>
#include <ESP32Servo.h>
#include <esp_sleep.h>

// Keypad setup
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};

byte rowPins[ROWS] = {16, 17, 18, 19};
byte colPins[COLS] = {21, 22, 23};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Password setup
const String PASSWORD = "1234";
String inputPassword;

// Pin definitions
const int BUZZER_PIN = 12;
const int LED_PIN = 13;
const int SERVO_PIN = 14;
const int RGB_RED = 25;
const int RGB_GREEN = 26;
const int RGB_BLUE = 27;
const int TRIG_PIN = 32;
const int ECHO_PIN = 33;

// HC-SR04 settings
const float DETECTION_THRESHOLD = 50.0; // 50cm threshold
const unsigned long DETECTION_INTERVAL = 500;
const unsigned long INACTIVITY_TIMEOUT = 10000; // 10 seconds no activity -> sleep

// Servo setup
Servo servo;
const int SERVO_OPEN_ANGLE = 90;
const int SERVO_CLOSED_ANGLE = 0;

// Timing variables
unsigned long unlockTime = 0;
unsigned long lastDetectionTime = 0;
unsigned long lastActivityTime = 0;
const unsigned long UNLOCK_DURATION = 10000;
const unsigned long RED_LED_DURATION = 3000;

// State variables
bool deviceUnlocked = false;
bool inRedState = false;
bool objectDetected = false;
bool wokeFromSleep = false;

float measureDistance();
void checkDistance();
void goToSleep();
void beep(int duration);
void setRGBColor(int red, int green, int blue);
void unlockDevice();
void resetToLockedState();
void wrongPassword();

void setup()
{
  Serial.begin(115200);

  // Initialize pins
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(RGB_RED, OUTPUT);
  pinMode(RGB_GREEN, OUTPUT);
  pinMode(RGB_BLUE, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Initialize servo
  servo.setPeriodHertz(50);
  servo.attach(SERVO_PIN, 500, 2400);
  servo.write(SERVO_CLOSED_ANGLE);

  // Check wakeup reason
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT1)
  {
    // Woken by HC-SR04 - verify distance before proceeding
    wokeFromSleep = true;
    float distance = measureDistance();

    if (distance > 0 && distance <= DETECTION_THRESHOLD)
    {
      // Valid wakeup - object is within range
      digitalWrite(LED_PIN, HIGH);
      beep(200);
      delay(100);
      beep(200);
      digitalWrite(LED_PIN, LOW);
    }
    else
    {
      // False trigger - go back to sleep
      goToSleep();
    }
  }

  lastActivityTime = millis();
}

void loop()
{
  // Check for inactivity timeout
  if (millis() - lastActivityTime > INACTIVITY_TIMEOUT && !deviceUnlocked)
  {
    // Perform final distance check before sleeping
    float distance = measureDistance();
    if (distance > 0 && distance <= DETECTION_THRESHOLD)
    {
      // Object detected - stay awake
      lastActivityTime = millis();
      digitalWrite(LED_PIN, HIGH);
      delay(1000);
      digitalWrite(LED_PIN, LOW);
    }
    else
    {
      // No object - safe to sleep
      goToSleep();
    }
  }

  // Check distance periodically
  if (millis() - lastDetectionTime > DETECTION_INTERVAL)
  {
    checkDistance();
    lastDetectionTime = millis();
  }

  // Check if we're in the unlocked state with timeout
  if (deviceUnlocked && millis() - unlockTime > UNLOCK_DURATION)
  {
    resetToLockedState();
  }

  // Check if we're in the red LED state with timeout
  if (inRedState && millis() - unlockTime > UNLOCK_DURATION + RED_LED_DURATION)
  {
    setRGBColor(0, 0, 0);
    inRedState = false;
  }

  // Keypad input
  char key = keypad.getKey();

  if (key)
  {
    lastActivityTime = millis(); // Reset inactivity timer

    Serial.println(key);

    if (key == '#')
    {
      if (inputPassword == PASSWORD)
      {
        unlockDevice();
      }
      else
      {
        wrongPassword();
      }
      inputPassword = "";
    }
    else if (key == '*')
    {
      inputPassword = "";
      setRGBColor(0, 0, 255);
      delay(300);
      setRGBColor(0, 0, 0);
    }
    else
    {
      inputPassword += key;
      beep(50);
    }
  }
}

float measureDistance()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2;

  Serial.print("Measured distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  return distance;
}

void checkDistance()
{
  float distance = measureDistance();

  if (distance > 0 && distance <= DETECTION_THRESHOLD)
  {
    if (!objectDetected)
    {
      objectDetected = true;
      digitalWrite(LED_PIN, HIGH);
      beep(100);
      lastActivityTime = millis(); // Reset inactivity timer
    }
  }
  else
  {
    if (objectDetected)
    {
      objectDetected = false;
      digitalWrite(LED_PIN, LOW);
    }
  }
}

void goToSleep()
{
  Serial.println("Going to sleep...");

  // Configure wakeup on echo pin (GPIO 33)
  // We'll wake on any HIGH pulse but verify distance after waking
  uint64_t mask = 1ULL << ECHO_PIN;
  esp_sleep_enable_ext1_wakeup(mask, ESP_EXT1_WAKEUP_ANY_HIGH);

  // Set all outputs to low power state
  digitalWrite(LED_PIN, LOW);
  setRGBColor(0, 0, 0);
  servo.detach();

  // Give serial time to flush
  delay(100);

  // Go to deep sleep
  esp_deep_sleep_start();
}

void beep(int duration)
{
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duration);
  digitalWrite(BUZZER_PIN, LOW);
}

void setRGBColor(int red, int green, int blue)
{
  digitalWrite(RGB_RED, red);
  digitalWrite(RGB_GREEN, green);
  digitalWrite(RGB_BLUE, blue);
}

void unlockDevice()
{
  Serial.println("Correct password! Unlocking...");
  deviceUnlocked = true;
  unlockTime = millis();
  servo.write(SERVO_OPEN_ANGLE);
  setRGBColor(0, 255, 0);
  beep(100);
  delay(50);
  beep(100);
  delay(50);
  beep(100);
}

void resetToLockedState()
{
  deviceUnlocked = false;
  servo.write(SERVO_CLOSED_ANGLE);
  setRGBColor(255, 0, 0);
  inRedState = true;
  unlockTime = millis();
}

void wrongPassword()
{
  Serial.println("Wrong password!");
  setRGBColor(255, 0, 0);
  beep(500);
  delay(200);
  beep(500);
  delay(500);
  setRGBColor(0, 0, 0);
}