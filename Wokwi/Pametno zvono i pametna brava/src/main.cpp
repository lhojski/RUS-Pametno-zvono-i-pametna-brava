/**
 * @file main.cpp
 * @brief Glavni program za pametno zvono i bravu s ESP32
 *
 * Ovaj program implementira sustav pametnog zvona i brave koji uključuje:
 * - Detekciju pokreta pomoću HC-SR04 senzora
 * - Upravljanje bravom pomoću servo motora
 * - Interakciju preko tipkovnice za unos lozinke
 * - Blynk integraciju za daljinsko upravljanje
 * - Notifikacijski sustav
 * - Štednju energije kroz duboki san
 */

/* ========== KONFIGURACIJA BLYNKA ========== */
#define BLYNK_TEMPLATE_ID "TMPL4yAFYgzJz"                       ///< ID Blynk predloška
#define BLYNK_TEMPLATE_NAME "RUS Pametno zvono i pametna brava" ///< Naziv Blynk projekta
#define BLYNK_AUTH_TOKEN "tsLSOhZc685__mTKTP0wBSQgw78Ln261"     ///< Autentifikacijski token za Blynk

/* ========== INCLUDE DIREKTIVE ========== */
#include <Keypad.h>           ///< Biblioteka za upravljanje tipkovnicom
#include <ESP32Servo.h>       ///< Biblioteka za servo motor
#include <esp_sleep.h>        ///< Funkcije za upravljanje snom ESP32
#include <WiFi.h>             ///< WiFi funkcionalnosti
#include <WiFiClient.h>       ///< WiFi klijent
#include <BlynkSimpleEsp32.h> ///< Blynk integracija

/* ========== BLYNK KONFIGURACIJA ========== */
char auth[] = BLYNK_AUTH_TOKEN; ///< Autentifikacijski token za Blynk
char ssid[] = "Wokwi-GUEST";    ///< SSID WiFi mreže
char pass[] = "";               ///< Lozinka WiFi mreže (prazna za Wokwi)

/* ========== DEFINICIJE PINOVA ========== */
#define BLYNK_UNLOCK_PIN V1       ///< Virtualni pin za otključavanje
#define BLYNK_NOTIFICATION_PIN V2 ///< Virtualni pin za notifikacije

/* ========== KONFIGURACIJA TIPKOVNICE ========== */
const byte ROWS = 4; ///< Broj redaka tipkovnice
const byte COLS = 3; ///< Broj stupaca tipkovnice

/* ========== KONFIGURACIJA NOTIFIKACIJA ========== */
bool lastButtonState = HIGH;
bool lastDetectionState = false;
unsigned long lastNotificationTime = 0;
const unsigned long NOTIFICATION_COOLDOWN = 1000; // 5 seconds between notifications

/// Raspored tipki na tipkovnici
char keys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};

/// GPIO pinovi povezani na retke tipkovnice
byte rowPins[ROWS] = {16, 17, 18, 19};

/// GPIO pinovi povezani na stupce tipkovnice
byte colPins[COLS] = {21, 22, 23};

/// Objekt tipkovnice
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

/* ========== KONFIGURACIJA LOZINKE ========== */
const String PASSWORD = "1234"; ///< Lozinka za otključavanje
String inputPassword;           ///< Privremena varijabla za unos lozinke

/* ========== DEFINICIJE PINOVA ========== */
const int BUZZER_PIN = 12; ///< Pin za piezo zujalicu
const int LED_PIN = 13;    ///< Pin za status LED
const int SERVO_PIN = 14;  ///< Pin za servo motor
const int RGB_RED = 25;    ///< Pin za crvenu boju RGB LED
const int RGB_GREEN = 26;  ///< Pin za zelenu boju RGB LED
const int RGB_BLUE = 27;   ///< Pin za plavu boju RGB LED
const int TRIG_PIN = 32;   ///< Trig pin HC-SR04 senzora
const int ECHO_PIN = 33;   ///< Echo pin HC-SR04 senzora
const int BUTTON_PIN = 4;  ///< Pin za tipku zvona

/* ========== POSTAVKE HC-SR04 SENZORA ========== */
const float DETECTION_THRESHOLD = 200.0;        ///< Prag detekcije u cm
const unsigned long DETECTION_INTERVAL = 500;   ///< Interval provjere senzora (ms)
const unsigned long INACTIVITY_TIMEOUT = 10000; ///< Vrijeme neaktivnosti prije spavanja (ms)
const unsigned long BUTTON_LED_DURATION = 2000; ///< Trajanje LED nakon pritiska tipke (ms)

/* ========== KONFIGURACIJA SERVO MOTORA ========== */
Servo servo;                      ///< Objekt za upravljanje servo motorom
const int SERVO_OPEN_ANGLE = 90;  ///< Kut otvorene brave
const int SERVO_CLOSED_ANGLE = 0; ///< Kut zatvorene brave

/* ========== VREMENSKE VARIJABLE ========== */
unsigned long unlockTime = 0;                ///< Vrijeme zadnjeg otključavanja
unsigned long lastDetectionTime = 0;         ///< Vrijeme zadnje detekcije pokreta
unsigned long lastActivityTime = 0;          ///< Vrijeme zadnje aktivnosti
unsigned long buttonPressTime = 0;           ///< Vrijeme zadnjeg pritiska tipke
const unsigned long UNLOCK_DURATION = 10000; ///< Trajanje otključanog stanja (ms)
const unsigned long RED_LED_DURATION = 3000; ///< Trajanje crvene LED nakon zaključavanja (ms)

/* ========== VARIJABLE STANJA ========== */
bool deviceUnlocked = false; ///< Stanje brave (otključano/zaključano)
bool inRedState = false;     ///< Indikacija crvene LED
bool objectDetected = false; ///< Detekcija objekta
bool wokeFromSleep = false;  ///< Stanje buđenja iz spavanja
bool wifiConnected = false;  ///< Status WiFi veze
bool buttonPressed = false;  ///< Stanje tipke

/* ========== DEKLARACIJE FUNKCIJA ========== */
float measureDistance();
void checkDistance();
void goToSleep();
void beep(int duration);
void setRGBColor(int red, int green, int blue);
void unlockDevice();
void resetToLockedState();
void wrongPassword();
void IRAM_ATTR buttonISR();
void sendBlynkNotification(const char *message);

/**
 * @brief Rukovalac za Blynk virtualni pin za otključavanje
 * @param param Vrijednost poslana s Blynka
 */
BLYNK_WRITE(BLYNK_UNLOCK_PIN)
{
  int pinValue = param.asInt();
  if (pinValue == 1)
  {
    unlockDevice();
    lastActivityTime = millis(); // Resetiranje timera neaktivnosti
  }
}

/**
 * @brief Funkcija za inicijalizaciju sustava
 */
void setup()
{
  Serial.begin(115200); // Inicijalizacija serijske komunikacije

  // Inicijalizacija izlaznih pinova
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(RGB_RED, OUTPUT);
  pinMode(RGB_GREEN, OUTPUT);
  pinMode(RGB_BLUE, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Inicijalizacija tipke s internim pull-up otpornikom
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);

  // Inicijalizacija servo motora
  servo.setPeriodHertz(50);           // Standardna frekvencija za servo
  servo.attach(SERVO_PIN, 500, 2400); // Povezivanje servo motora na pin
  servo.write(SERVO_CLOSED_ANGLE);    // Postavljanje u početno zaključano stanje

  // Provjera razloga buđenja
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT1)
  {
    // Ako je buđenje zbog HC-SR04 senzora
    wokeFromSleep = true;
    float distance = measureDistance();

    if (distance > 0 && distance <= DETECTION_THRESHOLD)
    {
      // Ako je detekcija valjana
      digitalWrite(LED_PIN, HIGH);
      beep(200);
      delay(100);
      beep(200);
      digitalWrite(LED_PIN, LOW);
    }
    else
    {
      // Ako je lažna detekcija, vraćanje u san
      goToSleep();
    }
  }

  // Povezivanje na WiFi
  WiFi.begin(ssid, pass);
  Serial.print("Povezivanje na WiFi");
  unsigned long wifiStartTime = millis();

  // Čekanje na WiFi vezu
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStartTime < 10000)
  {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    wifiConnected = true;
    Serial.println("\nUspjesno povezano na WiFi");
    Blynk.config(auth);
    if (Blynk.connect())
    {
      Serial.println("Uspjesno povezano na Blynk");
    }
    else
    {
      Serial.println("Neuspjesno povezivanje na Blynk");
    }
  }
  else
  {
    Serial.println("\nNeuspjesno povezivanje na WiFi");
  }

  lastActivityTime = millis(); // Postavljanje početnog vremena aktivnosti
}

/**
 * @brief Glavna petlja programa
 */
void loop()
{
  // Pokretanje Blynka ako je WiFi povezan
  if (wifiConnected)
  {
    Blynk.run();
  }

  // Obrada pritiska tipke
  if (buttonPressed)
  {
    buttonPressed = false;
    digitalWrite(LED_PIN, HIGH);
    beep(500);
    buttonPressTime = millis();
    sendBlynkNotification("Pritisnuto je zvono!");
  }

  // Gašenje LED nakon isteka vremena
  if (digitalRead(LED_PIN) && millis() - buttonPressTime > BUTTON_LED_DURATION)
  {
    digitalWrite(LED_PIN, LOW);
  }

  // Provjera neaktivnosti za prijelaz u san
  if (millis() - lastActivityTime > INACTIVITY_TIMEOUT && !deviceUnlocked)
  {
    // Zadnja provjera udaljenosti prije spavanja
    float distance = measureDistance();
    if (distance > 0 && distance <= DETECTION_THRESHOLD)
    {
      // Ako je detektiran objekt, ostati budan
      lastActivityTime = millis();
      digitalWrite(LED_PIN, HIGH);
      delay(1000);
      digitalWrite(LED_PIN, LOW);
    }
    else
    {
      // Ako nema objekta, ići u san
      goToSleep();
    }
  }

  // Periodička provjera udaljenosti
  if (millis() - lastDetectionTime > DETECTION_INTERVAL)
  {
    checkDistance();
    lastDetectionTime = millis();
  }

  // Automatsko zaključavanje nakon isteka vremena
  if (deviceUnlocked && millis() - unlockTime > UNLOCK_DURATION)
  {
    resetToLockedState();
  }

  // Gašenje crvene LED nakon isteka vremena
  if (inRedState && millis() - unlockTime > UNLOCK_DURATION + RED_LED_DURATION)
  {
    setRGBColor(0, 0, 0);
    inRedState = false;
  }

  // Obrada unosa s tipkovnice
  char key = keypad.getKey();

  if (key)
  {
    lastActivityTime = millis(); // Resetiranje timera neaktivnosti

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

/**
 * @brief Interrupt servisna rutina za tipku
 */
void IRAM_ATTR buttonISR()
{
  buttonPressed = true;
}

/**
 * @brief Mjerenje udaljenosti pomoću HC-SR04 senzora
 * @return Udaljenost u centimetrima
 */
float measureDistance()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2;

  Serial.print("Izmjerena udaljenost: ");
  Serial.print(distance);
  Serial.println(" cm");

  return distance;
}

/**
 * @brief Provjera udaljenosti i detekcija pokreta
 */
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
      lastActivityTime = millis();
      sendBlynkNotification("Netko je pred vratima!");
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

/**
 * @brief Prijelaz uređaja u stanje dubokog sna
 */
void goToSleep()
{
  Serial.println("Prelazak u stanje spavanja...");

  // Konfiguracija buđenja na echo pinu (GPIO 33)
  uint64_t mask = 1ULL << ECHO_PIN;
  esp_sleep_enable_ext1_wakeup(mask, ESP_EXT1_WAKEUP_ANY_HIGH);

  // Postavljanje svih izlaza u niskopotrošni režim
  digitalWrite(LED_PIN, LOW);
  setRGBColor(0, 0, 0);
  servo.detach();

  delay(100); // Pauza za završetak serijske komunikacije

  esp_deep_sleep_start(); // Pokretanje dubokog sna
}

/**
 * @brief Generiranje tona na zujalici
 * @param duration Trajanje tona u milisekundama
 */
void beep(int duration)
{
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duration);
  digitalWrite(BUZZER_PIN, LOW);
}

/**
 * @brief Postavljanje boje RGB LED
 * @param red Intenzitet crvene boje (0-255)
 * @param green Intenzitet zelene boje (0-255)
 * @param blue Intenzitet plave boje (0-255)
 */
void setRGBColor(int red, int green, int blue)
{
  digitalWrite(RGB_RED, red);
  digitalWrite(RGB_GREEN, green);
  digitalWrite(RGB_BLUE, blue);
}

/**
 * @brief Otključavanje brave
 */
void unlockDevice()
{
  Serial.println("Tocna lozinka! Otkljucavanje...");
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

/**
 * @brief Zaključavanje brave
 */
void resetToLockedState()
{
  deviceUnlocked = false;
  servo.write(SERVO_CLOSED_ANGLE);
  setRGBColor(255, 0, 0);
  inRedState = true;
  unlockTime = millis();
}

/**
 * @brief Obrada pogrešne lozinke
 */
void wrongPassword()
{
  Serial.println("Pogrešna lozinka!");
  setRGBColor(255, 0, 0);
  beep(500);
  delay(200);
  beep(500);
  delay(500);
  setRGBColor(0, 0, 0);
}

/**
 * @brief Slanje notifikacije preko Blynka
 * @param message Tekst poruke za slanje
 */
void sendBlynkNotification(const char *message)
{
  if (wifiConnected && Blynk.connected())
  {
    if (millis() - lastNotificationTime > NOTIFICATION_COOLDOWN)
    {
      Blynk.virtualWrite(BLYNK_NOTIFICATION_PIN, message);
      Blynk.logEvent("security_alert", message);
      lastNotificationTime = millis();
      Serial.print("Poslana notifikacija: ");
      Serial.println(message);
    }
  }
}