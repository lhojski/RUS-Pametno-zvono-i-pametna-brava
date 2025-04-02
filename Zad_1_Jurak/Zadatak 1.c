#include <Arduino.h>

// Definiranje pinova
const int buttonPins[] = {2, 3, 18};  // INT0, INT1, INT5
const int ledPin = 13;
const int potPin = A0;

// Globalne varijable
volatile bool buttonPressed[] = {false, false, false};
volatile int sensorValue = 0;
volatile bool timerFlag = false;
volatile bool ledState = false;

// Semafori
volatile bool serialInUse = false;
volatile bool sensorInUse = false;

// Prototipovi funkcija
void handleButtonEvents();
void handleSensorReading();
void normalOperation();

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
  
  // Konfiguracija eksternih prekida
  attachInterrupt(digitalPinToInterrupt(buttonPins[0]), ISR_button0, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPins[1]), ISR_button1, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPins[2]), ISR_button5, FALLING);
  
  // Konfiguracija timer prekida (Timer1)
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  
  OCR1A = 15624; // 1Hz (16MHz/1024/1Hz - 1)
  TCCR1B |= (1 << WGM12); // CTC mod
  TCCR1B |= (1 << CS12) | (1 << CS10); // 1024 prescaler
  TIMSK1 |= (1 << OCIE1A); // Omogući timer compare interrupt
  interrupts();
  
  Serial.println("Sustav spreman");
}

// INT0 prekidna rutina (najviši prioritet)
void ISR_button0() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  
  if (interruptTime - lastInterruptTime > 200) {
    buttonPressed[0] = true;
    ledState = true;
    digitalWrite(ledPin, HIGH);
    
    // Simulacija kratke obrade
    for (volatile int i = 0; i < 500; i++);
    
    Serial.println("INT0 prekid - najviši prioritet");
  }
  lastInterruptTime = interruptTime;
}

// INT1 prekidna rutina (srednji prioritet)
void ISR_button1() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  
  if (interruptTime - lastInterruptTime > 200) {
    buttonPressed[1] = true;
    ledState = true;
    digitalWrite(ledPin, HIGH);
    
    // Kratka obrada
    for (volatile int i = 0; i < 300; i++);
  }
  lastInterruptTime = interruptTime;
}

// INT5 prekidna rutina (najniži prioritet)
void ISR_button5() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  
  if (interruptTime - lastInterruptTime > 200) {
    buttonPressed[2] = true;
    ledState = true;
    digitalWrite(ledPin, HIGH);
  }
  lastInterruptTime = interruptTime;
}

// Timer1 interrupt rutina
ISR(TIMER1_COMPA_vect) {
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
}

void loop() {
  handleButtonEvents();
  handleSensorReading();
  normalOperation();
}

void handleButtonEvents() {
  // Obrada INT0 događaja
  if (buttonPressed[0]) {
    buttonPressed[0] = false;
    while (serialInUse);
    serialInUse = true;
    Serial.println("Obrada INT0 događaja");
    serialInUse = false;
  }
  
  // Obrada INT1 događaja
  if (buttonPressed[1]) {
    buttonPressed[1] = false;
    while (serialInUse);
    serialInUse = true;
    Serial.println("Obrada INT1 događaja");
    serialInUse = false;
  }
  
  // Obrada INT5 događaja
  if (buttonPressed[2]) {
    buttonPressed[2] = false;
    while (serialInUse);
    serialInUse = true;
    Serial.println("Obrada INT5 događaja");
    serialInUse = false;
    
    // Blinkanje LED nakon INT5
    for (int i = 0; i < 3; i++) {
      digitalWrite(ledPin, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);
      delay(200);
    }
    ledState = false;
  }
  
  // Gašenje LED ako nije aktivna obrada
  if (!buttonPressed[0] && !buttonPressed[1] && !buttonPressed[2] && ledState) {
    digitalWrite(ledPin, LOW);
    ledState = false;
  }
}

void handleSensorReading() {
  if (timerFlag) {
    timerFlag = false;
    while (serialInUse);
    serialInUse = true;
    Serial.print("Vrijednost senzora: ");
    Serial.println(sensorValue);
    serialInUse = false;
  }
}

void normalOperation() {
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 2000) {
    lastPrint = millis();
    while (serialInUse);
    serialInUse = true;
    Serial.println("Normalan rad sustava...");
    serialInUse = false;
  }
}
