#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

const int ledPin = 13;        // LED na pinu 13
const int buttonPin = 2;      // Tipkalo na pinu 2 (INT0)
const int longDelay = 100000; // Very long delay until waking up

volatile bool nothing = true;

volatile bool wakeUpByButton = false;
volatile bool wakeUpByTimer = false;

void setup()
{
    pinMode(ledPin, OUTPUT);
    pinMode(buttonPin, INPUT_PULLUP);

    // Konfiguriraj prekid za buđenje tipkalom (FALLING zbog PULLUP)
    attachInterrupt(digitalPinToInterrupt(buttonPin), wakeUpFromButton, FALLING);

    Serial.begin(9600);
    Serial.println("Inicijalizacija zavrsena");
}

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

    // Postavi WDT za ~8 sekundi
    setupWatchdog();

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

void enterSleep()
{
    set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Najniža potrošnja energije
    sleep_enable();
    sleep_cpu();
    sei();
    nothing = true;
    while (nothing)
    {
    };
    sleep_disable();
}

void setupWatchdog()
{
    cli();
    wdt_reset();

    // Postavi WDT za ~8 sekundi (WDP3 + WDP0)
    WDTCSR = (1 << WDCE) | (1 << WDE);
    WDTCSR = (1 << WDIE) | (1 << WDP3) | (1 << WDP0);

    sei();
}

// WDT interrupt rutina
ISR(WDT_vect)
{
    nothing = false;
    wakeUpByTimer = true;
}

// Prekidna rutina za buđenje tipkalom
void wakeUpFromButton()
{
    nothing = false;
    wakeUpByButton = true;
}