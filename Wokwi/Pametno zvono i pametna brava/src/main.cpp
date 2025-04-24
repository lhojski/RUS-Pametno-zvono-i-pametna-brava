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

byte rowPins[ROWS] = {16, 17, 18, 19}; // Connect to keypad rows
byte colPins[COLS] = {21, 22, 23};     // Connect to keypad columns

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Password setup
const String PASSWORD = "1234"; // Change this to your desired password
String inputPassword;

// Pin definitions
const int BUZZER_PIN = 12;
const int LED_PIN = 13;
const int SERVO_PIN = 14;
const int RGB_RED = 25;
const int RGB_GREEN = 26;
const int RGB_BLUE = 27;

// Servo setup
Servo servo;
const int SERVO_OPEN_ANGLE = 90;
const int SERVO_CLOSED_ANGLE = 0;

// Timing variables
unsigned long unlockTime = 0;
const unsigned long UNLOCK_DURATION = 10000; // 10 seconds
const unsigned long RED_LED_DURATION = 3000; // 3 seconds

// State variables
bool deviceUnlocked = false;
bool inRedState = false;

void wakeupSequence();
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

  // Initialize servo
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  servo.setPeriodHertz(50); // standard 50 hz servo
  servo.attach(SERVO_PIN, 500, 2400);
  servo.write(SERVO_CLOSED_ANGLE);

  // Wakeup reason
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT0)
  {
    // Woken by button press
    wakeupSequence();
  }
  else
  {
    // First boot or other wakeup reason
    setRGBColor(0, 0, 0); // Turn off RGB
    digitalWrite(LED_PIN, LOW);
  }

  // Configure wakeup on GPIO 0 (button)
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, LOW);
}

void loop()
{
  // Check if we're in the unlocked state with timeout
  if (deviceUnlocked && millis() - unlockTime > UNLOCK_DURATION)
  {
    resetToLockedState();
  }

  // Check if we're in the red LED state with timeout
  if (inRedState && millis() - unlockTime > RED_LED_DURATION)
  {
    setRGBColor(0, 0, 0); // Turn off RGB
    inRedState = false;

    // Go back to sleep
    Serial.println("Going to sleep...");
    esp_deep_sleep_start();
  }

  // Keypad input
  char key = keypad.getKey();

  if (key)
  {
    Serial.println(key);

    if (key == '#')
    {
      // Check password
      if (inputPassword == PASSWORD)
      {
        unlockDevice();
      }
      else
      {
        // Wrong password
        wrongPassword();
      }
      inputPassword = ""; // Reset input
    }
    else if (key == '*')
    {
      // Clear input
      inputPassword = "";
      setRGBColor(0, 0, 255); // Blue for input cleared
      delay(300);
      setRGBColor(0, 0, 0);
    }
    else
    {
      // Append to password
      inputPassword += key;
      beep(50); // Short beep for keypress
    }
  }
}

void wakeupSequence()
{
  // Buzzer beep and LED on
  digitalWrite(LED_PIN, HIGH);
  beep(200);
  delay(100);
  beep(200);
  digitalWrite(LED_PIN, LOW);
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

  // Move servo to open position
  servo.write(SERVO_OPEN_ANGLE);

  // Set RGB to green
  setRGBColor(0, 255, 0);

  // Success beep
  beep(100);
  delay(50);
  beep(100);
  delay(50);
  beep(100);
}

void resetToLockedState()
{
  deviceUnlocked = false;

  // Move servo back to closed position
  servo.write(SERVO_CLOSED_ANGLE);

  // Set RGB to red
  setRGBColor(255, 0, 0);
  inRedState = true;

  // Update timer for red state
  unlockTime = millis();
}

void wrongPassword()
{
  Serial.println("Wrong password!");

  // Set RGB to red
  setRGBColor(255, 0, 0);

  // Error beep sequence
  beep(500);
  delay(200);
  beep(500);

  // Turn off RGB
  delay(500);
  setRGBColor(0, 0, 0);
}