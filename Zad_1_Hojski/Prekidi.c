#include <avr/interrupt.h>

#define LED_INT0 13 // Crvena (najviši prioritet)
#define LED_INT1 12 // Žuta
#define LED_INT2 11 // Zelena
#define BUTTON0 2   // INT0
#define BUTTON1 3   // INT1
#define BUTTON2 21  // INT2

volatile bool int0Flag = false;
volatile bool int1Flag = false;
volatile bool int2Flag = false;
unsigned long lastDebounceTime = 0;
bool processingInterrupt = false;

void setup()
{
  pinMode(LED_INT0, OUTPUT);
  pinMode(LED_INT1, OUTPUT);
  pinMode(LED_INT2, OUTPUT);
  pinMode(BUTTON0, INPUT_PULLUP);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BUTTON0), ISR_INT0, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON1), ISR_INT1, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON2), ISR_INT2, FALLING);

  Serial.begin(9600);
  sei();
}

void loop()
{
  if (!processingInterrupt)
  {
    if (int0Flag)
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
  }
}

// ISR funkcije
void ISR_INT0()
{
  if (millis() - lastDebounceTime > 20)
    int0Flag = true;
  lastDebounceTime = millis();
}

void ISR_INT1()
{
  if (millis() - lastDebounceTime > 20)
    int1Flag = true;
  lastDebounceTime = millis();
}

void ISR_INT2()
{
  if (millis() - lastDebounceTime > 20)
    int2Flag = true;
  lastDebounceTime = millis();
}

void handleInterrupt(int interruptNum)
{
  processingInterrupt = true;

  // Ugasi sve LED-ice
  digitalWrite(LED_INT0, LOW);
  digitalWrite(LED_INT1, LOW);
  digitalWrite(LED_INT2, LOW);

  // Upali odgovarajuću LEDicu
  switch (interruptNum)
  {
  case 0:
    digitalWrite(LED_INT0, HIGH);
    Serial.println("INT0 (HIGHEST priority)");
    int0Flag = false;
    break;
  case 1:
    digitalWrite(LED_INT1, HIGH);
    Serial.println("INT1 (Medium priority)");
    int1Flag = false;
    break;
  case 2:
    digitalWrite(LED_INT2, HIGH);
    Serial.println("INT2 (Low priority)");
    int2Flag = false;
    break;
  }

  delay(100); // Pauza da se LEDica vidljivo upali
  processingInterrupt = false;
}