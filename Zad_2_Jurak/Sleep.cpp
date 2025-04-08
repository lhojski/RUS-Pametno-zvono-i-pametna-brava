#include <avr/sleep.h>
#include <avr/power.h>

const int ledPin = 13;    // LED spojena na pin 13
const int buttonPin = 2;  // Tipkalo spojeno na pin 2 (INT0)

volatile bool sleepMode = false;
volatile bool interrupted = false;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);
  
  Serial.println("Sustav pokrenut");
  Serial.println("LED ce treptati 5 sekundi prije ulaska u sleep mode");
}

void loop() {
  if (!sleepMode) {
    activePhase();    // Aktivni period
    prepareForSleep(); // Priprema za sleep
    enterSleep();     // Ulazak u sleep mode
  }
}

void activePhase() {
  Serial.println("Aktivni mod - LED treperi");
  
  for (int i = 0; i < 5; i++) {
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);
  }
}

void prepareForSleep() {
  Serial.println("Priprema za sleep mode...");
  digitalWrite(ledPin, LOW);
  delay(100);
  
  // Resetiraj zastavicu prekida
  interrupted = false;
  
  // Konfiguriraj prekid
  attachInterrupt(digitalPinToInterrupt(buttonPin), wakeUp, FALLING);
  
  Serial.println("Spreman za sleep mode");
  Serial.println("Pritisnite tipkalo za buđenje");
  Serial.println("----------------------------------");
}

void enterSleep() {
  sleepMode = true;
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  
  // Isključi nepotrebne module
  power_adc_disable();
  power_spi_disable();
  power_twi_disable();
  
  // Čekaj dok se ne dogodi prekid
  while(!interrupted) {
    sleep_cpu();  // Ulazak u sleep mode
  }
  
  sleep_disable();
  power_all_enable();
  sleepMode = false;
  
  Serial.println("Sustav se probudio!");
}

void wakeUp() {
  // Postavi zastavicu za prekid
  interrupted = true;
  detachInterrupt(digitalPinToInterrupt(buttonPin));
}
