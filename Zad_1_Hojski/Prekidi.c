#include <avr/interrupt.h>

// LED pinovi
#define LED_INT0 13   // Crvena (tipka INT0)
#define LED_INT1 12   // Žuta (tipka INT1)
#define LED_INT2 11   // Zelena (tipka INT2)
#define LED_Sensor 10 // Plava (senzor)
#define LED_Timer 9   // Bijela (timer - najviši prioritet)

// Tipkala
#define BUTTON0 2  // INT0
#define BUTTON1 3  // INT1
#define BUTTON2 21 // INT2

// HC-SR04 senzor
#define TRIG_PIN 4
#define ECHO_PIN 5 // INT3

// Timer1 konstante
#define TIMER1_PRESCALER 1024
#define TIMER1_COMPARE 15624

// Globalne varijable
volatile bool int0Flag = false;
volatile bool int1Flag = false;
volatile bool int2Flag = false;
volatile bool distanceAlert = false;
volatile bool timerFlag = false; // Zastavica za timer interrupt
volatile unsigned long lastDebounceTime = 0;
const int ALARM_DISTANCE = 100; // Prag udaljenosti u cm

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

  // Postavljanje prekida za tipkala
  attachInterrupt(digitalPinToInterrupt(BUTTON0), ISR_INT0, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON1), ISR_INT1, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON2), ISR_INT2, FALLING);

  // Konfiguracija Timer1 za prekid svake sekunde
  TCCR1A = 0;                                        // Normal mode
  TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // CTC mode, prescaler 1024
  OCR1A = TIMER1_COMPARE;
  TIMSK1 = (1 << OCIE1A); // Omogući Timer1 compare interrupt

  Serial.begin(9600);
  sei(); // Omogući globalne prekide
}

void loop()
{
  // Mjerenje udaljenosti
  float distance = measureDistance();

  // Provjera alarmnog praga
  if (distance > 0 && distance < ALARM_DISTANCE)
  {
    if (!distanceAlert)
    {
      distanceAlert = true;
    }
  }
  else
  {
    distanceAlert = false;
  }

  // Obrada prekida po prioritetima
  if (timerFlag)
  {
    handleTimerInterrupt();
  }
  else if (int0Flag)
  {
    handleInterrupt(0);
  }
  else if (int1Flag)
  {
    handleInterrupt(1);
  }
  else if (int2Flag)
  {
    handleInterrupt(2);
  }
  else if (distanceAlert)
  {
    triggerDistanceAlert();
  }
}

// Timer1 compare interrupt rutina
ISR(TIMER1_COMPA_vect)
{
  timerFlag = true;
}

void handleTimerInterrupt()
{
  // Ugasi sve LEDice
  digitalWrite(LED_INT0, LOW);
  digitalWrite(LED_INT1, LOW);
  digitalWrite(LED_INT2, LOW);
  digitalWrite(LED_Sensor, LOW);

  // Upali LED_Timer i ispiši poruku
  digitalWrite(LED_Timer, HIGH);
  Serial.println("TIMER INTERRUPT (HIGHEST PRIORITY)");
  delay(1000);
  digitalWrite(LED_Timer, LOW);

  timerFlag = false;
}

float measureDistance()
{
  // Generiraj trigger impuls
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Mjeri Echo puls
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // Timeout 30ms

  // Izračunaj udaljenost
  if (duration > 0)
  {
    return duration / 58.0;
  }
  return -1; // Nevažeće mjerenje
}

void triggerDistanceAlert()
{
  // Bljeskaj plavom ledicom
  digitalWrite(LED_Sensor, HIGH);
  delay(200);
  digitalWrite(LED_Sensor, LOW);
  delay(200);
  Serial.println("ALARM: Predmet preblizu (<100cm, LOWEST PRIORITY)!");
}

// Prekidne rutine za tipkala
void ISR_INT0()
{
  if (millis() - lastDebounceTime > 10)
    int0Flag = true;
  lastDebounceTime = millis();
}

void ISR_INT1()
{
  if (millis() - lastDebounceTime > 10)
    int1Flag = true;
  lastDebounceTime = millis();
}

void ISR_INT2()
{
  if (millis() - lastDebounceTime > 10)
    int2Flag = true;
  lastDebounceTime = millis();
}

void handleInterrupt(int interruptNum)
{
  // Ugasi sve LEDice
  digitalWrite(LED_INT0, LOW);
  digitalWrite(LED_INT1, LOW);
  digitalWrite(LED_INT2, LOW);

  // Upali odgovarajuću LEDicu
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