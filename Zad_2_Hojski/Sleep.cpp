#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

const int ledPin = 13;   ///< Pin na kojem je spojena LED dioda
const int buttonPin = 2; ///< Pin na kojem je spojeno tipkalo (INT0)

volatile bool nothing = true; ///< Zastavica za kontrolu spavanja

volatile bool wakeUpByButton = false; ///< Zastavica koja označava buđenje tipkalom
volatile bool wakeUpByTimer = false;  ///< Zastavica koja označava buđenje timerom

/**
 * @brief Funkcija za inicijalizaciju uređaja
 *
 * Postavlja pinove za LED i tipkalo, konfigurira prekid za buđenje
 * te postavlja sleep mode na najnižu potrošnju energije.
 */
void setup()
{
    pinMode(ledPin, OUTPUT);
    pinMode(buttonPin, INPUT_PULLUP);

    // Konfiguriraj prekid za buđenje tipkalom (FALLING zbog PULLUP)
    attachInterrupt(digitalPinToInterrupt(buttonPin), wakeUpFromButton, FALLING);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Najniža potrošnja energije

    Serial.begin(9600);
    Serial.println("Inicijalizacija zavrsena");
}

/**
 * @brief Glavna petlja programa
 *
 * Petlja koja periodično treperi LED diodom 5 sekundi,
 * zatim ulazi u sleep mode i čeka buđenje.
 */
void loop()
{
    // Aktivni period: LED trepti 5 sekundi
    for (int i = 0; i < 5; i++)
    {
        digitalWrite(ledPin, HIGH);
        delay(500);
        digitalWrite(ledPin, LOW);
        delay(500);
        Serial.print("Aktivni period: ");
        Serial.println(i + 1);
    }

    // Ugasi LED prije ulaska u sleep mode
    digitalWrite(ledPin, LOW);
    Serial.println("Ulazim u sleep mode...");

    wakeUpByButton = false;
    wakeUpByTimer = false;

    // Uđi u sleep mode
    enterSleep();

    if (wakeUpByButton)
    {
        Serial.println("Probudio me prekid s tipkala!");
    }
    else if (wakeUpByTimer)
    {
        Serial.println("Probudio me Watchdog Timer!");
    }
}

/**
 * @brief Funkcija za ulazak u sleep mode
 *
 * Postavlja sleep mode, konfigurira watchdog timer
 * i stavlja procesor u stanje spavanja.
 */
void enterSleep()
{
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    // Interrupts are not counted as ADXL require

    noInterrupts(); // timed sequence coming up
    nothing = true;
    // Postavi WDT za ~8 sekundi
    setupWatchdog();
    sleep_enable(); // ready to sleep
    interrupts();   // interrupts are required now

    while (nothing)
    {
        sleep_cpu(); // sleep
    }

    sleep_disable(); // precaution
}

/**
 * @brief Funkcija za konfiguraciju watchdog timera
 *
 * Postavlja watchdog timer na ~8 sekundi (WDP3 + WDP0)
 * i omogućuje prekid od strane WDT-a.
 */
void setupWatchdog()
{
    cli();
    wdt_reset();

    // Postavi WDT za ~8 sekundi (WDP3 + WDP0)
    WDTCSR = (1 << WDCE) | (1 << WDE);
    WDTCSR = (1 << WDIE) | (1 << WDP3) | (1 << WDP0);

    sei();
}

/**
 * @brief Interrupt rutina za watchdog timer
 *
 * Postavlja zastavice za buđenje kada WDT istekne.
 */
ISR(WDT_vect)
{
    nothing = false;
    wakeUpByTimer = true;
}

/**
 * @brief Prekidna rutina za buđenje tipkalom
 *
 * Postavlja zastavice za buđenje kada se pritisne tipkalo.
 */
void wakeUpFromButton()
{
    nothing = false;
    wakeUpByButton = true;
}