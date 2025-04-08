#include <avr/sleep.h>
#include <avr/interrupt.h>

#define LED_PIN 13
#define BUTTON_PIN 2

volatile bool wakeUp = false;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // External interrupt setup on pin 2 (INT0)
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), wakeUpNow, FALLING);
}

void loop() {
  // Aktivna faza
  digitalWrite(LED_PIN, HIGH);
  delay(5000); // Svijetli 5 sekundi
  digitalWrite(LED_PIN, LOW);

  // Ulazak u sleep mod
  enterSleep();
}

void enterSleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Najniži sleep mode
  sleep_enable();

  // Onemogući nepotrebne module (opcionalno)

  cli(); // Isključi prekide na trenutak
  if (!wakeUp) {
    sei(); // Ponovno omogući prekide
    sleep_cpu(); // Uđi u sleep
  }
  sleep_disable(); // Nakon buđenja
  wakeUp = false;
}

void wakeUpNow() {
  wakeUp = true; // Ova funkcija se poziva na interrupt
}
