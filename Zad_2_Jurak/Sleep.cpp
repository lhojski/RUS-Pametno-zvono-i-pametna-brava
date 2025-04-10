/**
 * @file sleep_mode_watchdog.ino
 * @brief Program za upravljanje sleep modom i watchdog timerom
 * @version 1.1
 * @date 2023-12-16
 * 
 * @details Program demonstrira rad sa sleep modovima i watchdog timerom.
 * Nakon 5 sekundi aktivnog perioda, sustav ulazi u sleep mode i može se probuditi
 * pritiskom na tipkalo. Watchdog timer osigurava reset sustava ako se dogodi zastoja.
 */

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h> // Dodano za watchdog timer

/** @defgroup Pinovi Definirani pinovi
 *  @{
 */
const int ledPin = 13;    ///< LED spojena na pin 13
const int buttonPin = 2;  ///< Tipkalo spojeno na pin 2 (INT0)
/** @} */

/** @defgroup GlobalneVarijable Globalne varijable
 *  @{
 */
volatile bool sleepMode = false;  ///< Zastavica koja označava da li je sustav u sleep modu
volatile bool interrupted = false;///< Zastavica koja označava da je dogoden prekid
/** @} */

/**
 * @brief Inicijalizacija sustava
 * 
 * Funkcija koja se poziva jednom pri pokretanju za postavljanje početnih vrijednosti.
 * Konfigurira pinove, serijsku komunikaciju i watchdog timer.
 */
void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);
  
  // Konfiguracija watchdog timera na 4 sekunde
  wdt_disable(); // Prvo onemogući watchdog radi sigurne konfiguracije
  wdt_enable(WDTO_4S); // Postavi watchdog na 4 sekunde
  
  Serial.println("Sustav pokrenut");
  Serial.println("LED ce treptati 5 sekundi prije ulaska u sleep mode");
}

/**
 * @brief Glavna petlja programa
 * 
 * Upravlja radom sustava kroz aktivnu fazu i sleep mode.
 * Resetira watchdog timer u regularnim intervalima.
 */
void loop() {
  wdt_reset(); // Resetiraj watchdog timer
  
  if (!sleepMode) {
    activePhase();    // Aktivni period
    prepareForSleep(); // Priprema za sleep
    enterSleep();     // Ulazak u sleep mode
  }
}

/**
 * @brief Aktivna faza rada sustava
 * 
 * U ovoj fazi LED treperi 5 sekundi prije nego što sustav prijeđe u sleep mode.
 */
void activePhase() {
  Serial.println("Aktivni mod - LED treperi");
  
  for (int i = 0; i < 5; i++) {
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);
    wdt_reset(); // Resetiraj watchdog timer nakon svake iteracije
  }
}

/**
 * @brief Priprema sustava za sleep mode
 * 
 * Resetira zastavice prekida, konfigurira prekid na tipkalu i ispisuje poruke.
 */
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

/**
 * @brief Ulazak u sleep mode
 * 
 * Konfigurira najdublji sleep mode (PWR_DOWN), isključuje nepotrebne module
 * i čeka na prekid za buđenje.
 */
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

/**
 * @brief Prekidna rutina za buđenje
 * 
 * Postavlja zastavicu za prekid i detašira prekid na tipkalu.
 */
void wakeUp() {
  // Postavi zastavicu za prekid
  interrupted = true;
  detachInterrupt(digitalPinToInterrupt(buttonPin));
}
