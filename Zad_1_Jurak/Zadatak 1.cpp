/**
 * @file main.ino
 * @brief Glavni program za upravljanje prekidima i potrošnjom energije
 * @version 1.0
 * @date 2023-12-15
 */

#include <Arduino.h>

/** @defgroup Pinovi Definirani pinovi
 *  @{
 */
const int buttonPins[] = {2, 3, 18};  ///< Pinovi za tipkala (INT0, INT1, INT5)
const int ledPin = 13;                ///< Pin za LED diodu
const int potPin = A0;                ///< Pin za potenciometar
/** @} */

/** @defgroup DebugPinovi Debug pinovi za logički analizator
 *  @{
 */
const int debug_INT0 = 14;  ///< Debug pin za INT0 aktivnost
const int debug_INT1 = 15;  ///< Debug pin za INT1 aktivnost
const int debug_INT5 = 16;  ///< Debug pin za INT5 aktivnost
const int debug_LED = 19;   ///< Debug pin za LED stanje
const int debug_Timer = 20; ///< Debug pin za Timer događaje
const int debug_Serial = 21;///< Debug pin za serijsku komunikaciju
/** @} */

/** @defgroup GlobalneVarijable Globalne varijable
 *  @{
 */
volatile bool buttonPressed[] = {false, false, false}; ///< Stanje tipkala
volatile int sensorValue = 0;         ///< Očitana vrijednost senzora
volatile bool timerFlag = false;      ///< Zastavica za timer prekid
volatile bool ledState = false;       ///< Trenutno stanje LED diode
/** @} */

/** @defgroup Semafori Semafori za zaštitu resursa
 *  @{
 */
volatile bool serialInUse = false;    ///< Zaštita serijske komunikacije
volatile bool sensorInUse = false;    ///< Zaštita čitanja senzora
/** @} */

/**
 * @brief Inicijalizacija sustava
 * 
 * Funkcija koja se poziva jednom pri pokretanju za postavljanje početnih vrijednosti
 */
void setup() {
  // Inicijalizacija serijske komunikacije
  Serial.begin(9600);
  
  // Inicijalizacija LED
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  // Inicijalizacija tipkala s pullup otpornicima
  for (int i = 0; i < 3; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  // Inicijalizacija debug pinova
  pinMode(debug_INT0, OUTPUT);
  pinMode(debug_INT1, OUTPUT);
  pinMode(debug_INT5, OUTPUT);
  pinMode(debug_LED, OUTPUT);
  pinMode(debug_Timer, OUTPUT);
  pinMode(debug_Serial, OUTPUT);
  digitalWrite(debug_INT0, LOW);
  digitalWrite(debug_INT1, LOW);
  digitalWrite(debug_INT5, LOW);
  digitalWrite(debug_LED, LOW);
  digitalWrite(debug_Timer, LOW);
  digitalWrite(debug_Serial, LOW);

  // Konfiguracija prekida
  attachInterrupt(digitalPinToInterrupt(buttonPins[0]), ISR_button0, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPins[1]), ISR_button1, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPins[2]), ISR_button5, FALLING);

  // Konfiguracija Timer1
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 15624; // 1Hz (16MHz/1024/1Hz - 1)
  TCCR1B |= (1 << WGM12); // CTC mod
  TCCR1B |= (1 << CS12) | (1 << CS10); // 1024 preskaler
  TIMSK1 |= (1 << OCIE1A); // Omogući timer compare interrupt
  interrupts();
  
  Serial.println("Sustav spreman");
}

/**
 * @brief Prekidna rutina za INT0 (najviši prioritet)
 * 
 * Funkcija koja se poziva pritiskom tipkala na pinu 2
 * @note Vrijeme debouncea: 200ms
 */
void ISR_button0() {
  digitalWrite(debug_INT0, HIGH); // Debug signal
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  
  if (interruptTime - lastInterruptTime > 200) {
    buttonPressed[0] = true;
    ledState = true;
    digitalWrite(ledPin, HIGH);
    digitalWrite(debug_LED, HIGH);
    
    // Simulacija obrade
    for (volatile int i = 0; i < 500; i++);
    
    Serial.println("INT0 prekid - najviši prioritet");
    digitalWrite(debug_Serial, HIGH);
    delayMicroseconds(100);
    digitalWrite(debug_Serial, LOW);
  }
  lastInterruptTime = interruptTime;
  digitalWrite(debug_INT0, LOW);
}

/**
 * @brief Prekidna rutina za INT1 (srednji prioritet)
 * 
 * Funkcija koja se poziva pritiskom tipkala na pinu 3
 */
void ISR_button1() {
  digitalWrite(debug_INT1, HIGH);
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  
  if (interruptTime - lastInterruptTime > 200) {
    buttonPressed[1] = true;
    ledState = true;
    digitalWrite(ledPin, HIGH);
    digitalWrite(debug_LED, HIGH);
    
    for (volatile int i = 0; i < 300; i++);
  }
  lastInterruptTime = interruptTime;
  digitalWrite(debug_INT1, LOW);
}

/**
 * @brief Prekidna rutina za INT5 (najniži prioritet)
 * 
 * Funkcija koja se poziva pritiskom tipkala na pinu 18
 */
void ISR_button5() {
  digitalWrite(debug_INT5, HIGH);
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  
  if (interruptTime - lastInterruptTime > 200) {
    buttonPressed[2] = true;
    ledState = true;
    digitalWrite(ledPin, HIGH);
    digitalWrite(debug_LED, HIGH);
  }
  lastInterruptTime = interruptTime;
  digitalWrite(debug_INT5, LOW);
}

/**
 * @brief Timer1 interrupt rutina (1Hz)
 * 
 * Očitava vrijednost potenciometra svakih 500ms
 */
ISR(TIMER1_COMPA_vect) {
  digitalWrite(debug_Timer, HIGH);
  static unsigned long lastSensorRead = 0;
  unsigned long currentMillis = millis();
  
  if (currentMillis - lastSensorRead >= 500) {
    lastSensorRead = currentMillis;
    
    if (!sensorInUse) {
      sensorInUse = true;
      sensorValue = analogRead(potPin);
      sensorInUse = false;
      timerFlag = true;
    }
  }
  digitalWrite(debug_Timer, LOW);
}

/**
 * @brief Glavna petlja programa
 */
void loop() {
  handleButtonEvents();
  handleSensorReading();
  normalOperation();
}

/**
 * @brief Obrada događaja tipkala
 * 
 * Upravlja LED diodom ovisno o pritisnutom tipkalu
 */
void handleButtonEvents() {
  // Obrada INT0 događaja
  if (buttonPressed[0]) {
    buttonPressed[0] = false;
    while (serialInUse);
    serialInUse = true;
    Serial.println("Obrada INT0 događaja");
    digitalWrite(debug_Serial, HIGH);
    delayMicroseconds(100);
    digitalWrite(debug_Serial, LOW);
    serialInUse = false;
  }
  
  // Obrada INT1 događaja
  if (buttonPressed[1]) {
    buttonPressed[1] = false;
    while (serialInUse);
    serialInUse = true;
    Serial.println("Obrada INT1 događaja");
    digitalWrite(debug_Serial, HIGH);
    delayMicroseconds(100);
    digitalWrite(debug_Serial, LOW);
    serialInUse = false;
  }
  
  // Obrada INT5 događaja
  if (buttonPressed[2]) {
    buttonPressed[2] = false;
    while (serialInUse);
    serialInUse = true;
    Serial.println("Obrada INT5 događaja");
    digitalWrite(debug_Serial, HIGH);
    delayMicroseconds(100);
    digitalWrite(debug_Serial, LOW);
    serialInUse = false;
    
    // Blinkanje LED nakon INT5
    for (int i = 0; i < 3; i++) {
      digitalWrite(ledPin, HIGH);
      digitalWrite(debug_LED, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);
      digitalWrite(debug_LED, LOW);
      delay(200);
    }
    ledState = false;
  }
  
  // Gašenje LED ako nije aktivna obrada
  if (!buttonPressed[0] && !buttonPressed[1] && !buttonPressed[2] && ledState) {
    digitalWrite(ledPin, LOW);
    digitalWrite(debug_LED, LOW);
    ledState = false;
  }
}

/**
 * @brief Obrada očitanja senzora
 * 
 * Ispisuje vrijednost potenciometra na serijski monitor
 */
void handleSensorReading() {
  if (timerFlag) {
    timerFlag = false;
    while (serialInUse);
    serialInUse = true;
    Serial.print("Vrijednost senzora: ");
    Serial.println(sensorValue);
    digitalWrite(debug_Serial, HIGH);
    delayMicroseconds(100);
    digitalWrite(debug_Serial, LOW);
    serialInUse = false;
  }
}

/**
 * @brief Normalan rad sustava
 * 
 * Ispisuje statusnu poruku svake 2 sekunde
 */
void normalOperation() {
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 2000) {
    lastPrint = millis();
    while (serialInUse);
    serialInUse = true;
    Serial.println("Normalan rad sustava...");
    digitalWrite(debug_Serial, HIGH);
    delayMicroseconds(100);
    digitalWrite(debug_Serial, LOW);
    serialInUse = false;
  }
}
