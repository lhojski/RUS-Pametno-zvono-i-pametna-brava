#include <avr/interrupt.h>

/** @file Prekidi.cpp
 *  @brief Glavna datoteka za demonstraciju prekida na Arduino Mega 2560
 *
 *  Ova datoteka sadrži implementaciju sustava s različitim vrstama prekida
 *  (vanjski, timer, senzor) s različitim prioritetima.
 */

// LED pinovi
#define LED_INT0 13   ///< Crvena LED (tipka INT0)
#define LED_INT1 12   ///< Žuta LED (tipka INT1)
#define LED_INT2 11   ///< Zelena LED (tipka INT2)
#define LED_Sensor 10 ///< Plava LED (senzor)
#define LED_Timer 9   ///< Bijela LED (timer - najviši prioritet)

// Tipkala
#define BUTTON0 2  ///< Pin za INT0 tipku
#define BUTTON1 3  ///< Pin za INT1 tipku
#define BUTTON2 21 ///< Pin za INT2 tipku (najveći pin na Mega 2560)

// HC-SR04 senzor
#define TRIG_PIN 4 ///< Trigger pin za HC-SR04 senzor
#define ECHO_PIN 5 ///< Echo pin za HC-SR04 senzor (koristi INT3)

// Timer1 konstante
#define TIMER1_PRESCALER 1024 ///< Vrijednost preskalera za Timer1
#define TIMER1_COMPARE 15624  ///< Vrijednost za OCR1A registar

// Logički analizator (koristimo dostupne pinove do 21)
#define LOGIC_ANALYZER_PIN0 14   ///< Pin za praćenje INT0
#define LOGIC_ANALYZER_PIN1 15   ///< Pin za praćenje INT1
#define LOGIC_ANALYZER_PIN2 16   ///< Pin za praćenje INT2
#define LOGIC_ANALYZER_TIMER 17  ///< Pin za praćenje Timer prekida
#define LOGIC_ANALYZER_SENSOR 18 ///< Pin za praćenje senzorskog prekida

// Globalne varijable
volatile bool int0Flag = false;              ///< Zastavica za INT0 prekid
volatile bool int1Flag = false;              ///< Zastavica za INT1 prekid
volatile bool int2Flag = false;              ///< Zastavica za INT2 prekid
volatile bool distanceAlert = false;         ///< Zastavica za alarm udaljenosti
volatile bool timerFlag = false;             ///< Zastavica za timer prekid
volatile unsigned long lastDebounceTime = 0; ///< Vrijeme za debounce
const int ALARM_DISTANCE = 100;              ///< Prag udaljenosti u cm za alarm

/** @brief Funkcija za inicijalizaciju hardvera
 *
 *  Ova funkcija postavlja sve potrebne pinove i konfigurira prekide.
 */
void setup()
{
  // Inicijalizacija LEDica
  pinMode(LED_INT0, OUTPUT);
  pinMode(LED_INT1, OUTPUT);
  pinMode(LED_INT2, OUTPUT);
  pinMode(LED_Sensor, OUTPUT);
  pinMode(LED_Timer, OUTPUT);

  // Inicijalizacija tipkala
  pinMode(BUTTON0, INPUT_PULLUP);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);

  // Inicijalizacija HC-SR04
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Inicijalizacija logičkog analizatora
  pinMode(LOGIC_ANALYZER_PIN0, OUTPUT);
  pinMode(LOGIC_ANALYZER_PIN1, OUTPUT);
  pinMode(LOGIC_ANALYZER_PIN2, OUTPUT);
  pinMode(LOGIC_ANALYZER_TIMER, OUTPUT);
  pinMode(LOGIC_ANALYZER_SENSOR, OUTPUT);

  // Postavljanje prekida za tipkala
  attachInterrupt(digitalPinToInterrupt(BUTTON0), ISR_INT0, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON1), ISR_INT1, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON2), ISR_INT2, FALLING);

  // Konfiguracija Timer1
  TCCR1A = 0;
  TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // CTC mode, prescaler 1024
  OCR1A = TIMER1_COMPARE;
  TIMSK1 = (1 << OCIE1A);

  Serial.begin(9600);
  sei();
}

/** @brief Glavna petlja programa
 *
 *  Ova funkcija kontinuirano mjeri udaljenost i provjerava zastavice prekida.
 *  Prekidi se obrađuju prema prioritetima.
 */
void loop()
{
  float distance = measureDistance();

  if (distance > 0 && distance < ALARM_DISTANCE)
  {
    if (!distanceAlert)
      distanceAlert = true;
  }
  else
  {
    distanceAlert = false;
  }

  // Obrada prekida po prioritetima
  if (timerFlag)
    handleTimerInterrupt();
  else if (int0Flag)
    handleInterrupt(0);
  else if (int1Flag)
    handleInterrupt(1);
  else if (int2Flag)
    handleInterrupt(2);
  else if (distanceAlert)
    triggerDistanceAlert();
}

/** @brief Prekidna rutina za Timer1
 *
 *  Ova funkcija se poziva kada Timer1 dostigne vrijednost OCR1A.
 *  Postavlja zastavicu za timer prekid.
 */
ISR(TIMER1_COMPA_vect)
{
  digitalWrite(LOGIC_ANALYZER_TIMER, HIGH);
  timerFlag = true;
  digitalWrite(LOGIC_ANALYZER_TIMER, LOW);
}

/** @brief Obrada timer prekida
 *
 *  Ova funkcija gasi sve LEDice i pali bijelu LEDicu koja označava
 *  timer prekid (najviši prioritet).
 */
void handleTimerInterrupt()
{
  digitalWrite(LED_INT0, LOW);
  digitalWrite(LED_INT1, LOW);
  digitalWrite(LED_INT2, LOW);
  digitalWrite(LED_Sensor, LOW);

  digitalWrite(LED_Timer, HIGH);
  Serial.println("TIMER INTERRUPT (HIGHEST PRIORITY)");
  delay(1000);
  digitalWrite(LED_Timer, LOW);

  timerFlag = false;
}

/** @brief Mjerenje udaljenosti pomoću HC-SR04 senzora
 *
 *  @return Udaljenost u centimetrima ili -1 ako je mjerenje neuspješno
 */
float measureDistance()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  return (duration > 0) ? duration / 58.0 : -1;
}

/** @brief Aktivacija alarma za udaljenost
 *
 *  Ova funkcija treperi plavom LEDicom i ispisuje poruku kada je
 *  detektiran predmet preblizu (najniži prioritet).
 */
void triggerDistanceAlert()
{
  digitalWrite(LOGIC_ANALYZER_SENSOR, HIGH);
  digitalWrite(LED_Sensor, HIGH);
  delay(200);
  digitalWrite(LED_Sensor, LOW);
  delay(200);
  Serial.println("ALARM: Predmet preblizu (<100cm, LOWEST PRIORITY)!");
  digitalWrite(LOGIC_ANALYZER_SENSOR, LOW);
}

/** @brief Prekidna rutina za INT0
 *
 *  Ova funkcija se poziva na padajuću ivicu na INT0 pin.
 *  Implementira debounce mehanizam i postavlja zastavicu.
 */
void ISR_INT0()
{
  digitalWrite(LOGIC_ANALYZER_PIN0, HIGH);
  if (millis() - lastDebounceTime > 10)
    int0Flag = true;
  lastDebounceTime = millis();
  digitalWrite(LOGIC_ANALYZER_PIN0, LOW);
}

/** @brief Prekidna rutina za INT1
 *
 *  Ova funkcija se poziva na padajuću ivicu na INT1 pin.
 *  Implementira debounce mehanizam i postavlja zastavicu.
 */
void ISR_INT1()
{
  digitalWrite(LOGIC_ANALYZER_PIN1, HIGH);
  if (millis() - lastDebounceTime > 10)
    int1Flag = true;
  lastDebounceTime = millis();
  digitalWrite(LOGIC_ANALYZER_PIN1, LOW);
}

/** @brief Prekidna rutina za INT2
 *
 *  Ova funkcija se poziva na padajuću ivicu na INT2 pin.
 *  Implementira debounce mehanizam i postavlja zastavicu.
 */
void ISR_INT2()
{
  digitalWrite(LOGIC_ANALYZER_PIN2, HIGH);
  if (millis() - lastDebounceTime > 10)
    int2Flag = true;
  lastDebounceTime = millis();
  digitalWrite(LOGIC_ANALYZER_PIN2, LOW);
}

/** @brief Obrada prekida
 *
 *  Ova funkcija obrađuje prekide ovisno o proslijeđenom broju prekida.
 *
 *  @param interruptNum Broj prekida (0=INT0, 1=INT1, 2=INT2)
 */
void handleInterrupt(int interruptNum)
{
  digitalWrite(LED_INT0, LOW);
  digitalWrite(LED_INT1, LOW);
  digitalWrite(LED_INT2, LOW);

  switch (interruptNum)
  {
  case 0:
    digitalWrite(LED_INT0, HIGH);
    Serial.println("INT0 (HIGH priority)");
    int0Flag = false;
    delay(500);
    digitalWrite(LED_INT0, LOW);
    break;
  case 1:
    digitalWrite(LED_INT1, HIGH);
    Serial.println("INT1 (Medium priority)");
    int1Flag = false;
    delay(500);
    digitalWrite(LED_INT1, LOW);
    break;
  case 2:
    digitalWrite(LED_INT2, HIGH);
    Serial.println("INT2 (LOW priority)");
    int2Flag = false;
    delay(500);
    digitalWrite(LED_INT2, LOW);
    break;
  }
}