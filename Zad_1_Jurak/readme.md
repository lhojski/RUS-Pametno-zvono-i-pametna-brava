# Sustav za upravljanje prekidima - Zadatak 1

## 📖 Sadržaj
1. [Opis zadatka](#1-opis-zadatka)
2. [Korištene komponente](#2-korištene-komponente)
3. [Shema spojeva](#3-shema-spojeva)
4. [Opis rješenja](#4-opis-rješenja)
5. [Zaključak](#5-zaključak)

## 1. Opis zadatka
Sustav demonstrira upravljanje prekidima s različitim prioritetima koristeći:

- 3 tipkala za eksterne prekide (INT0, INT1, INT5)
- LED diodu na pinu 13 za vizualizaciju
- Potenciometar na A0 za simulaciju senzora
- Arduino Mega 2560 mikrokontroler

## 2. Korištene komponente

| Komponenta       | Količina | Pin na Arduino Mega |
|------------------|----------|---------------------|
| Arduino Mega 2560| 1        | -                   |
| Tipkalo          | 3        | 2 (INT0), 3 (INT1), 18 (INT5) |
| LED dioda        | 1        | 13                  |
| Potenciometar    | 1        | A0                  |
| Otpornik 220Ω    | 1        | -                   |

## 3. Tablica funkcionalnosti

| ID     | Description |
|--------|-------------|
| FR-1   | System must detect button presses (BUTTON0, BUTTON1, BUTTON2) and activate corresponding LEDs |
| FR-2   | Button interrupts must have defined priorities: INT0 - High, INT1 - Medium, INT2 - Low |
| FR-3   | System must ignore multiple signals caused by mechanical button bouncing (implement debounce mechanism) |
| FR-4   | TIMER1 must generate interrupt every 1 second and briefly turn on Timer LED |
| FR-5   | System must measure distance using HC-SR04 sensor |
| FR-6   | If measured distance is less than 100 cm, Sensor LED must blink every 200 ms |
| FR-7   | If pulseIn() returns invalid result, system must ignore the reading |
| FR-8   | If multiple interrupts occur simultaneously, following priority must be enforced: TIMER1 > INT0 > INT1 > INT2 > sensor |
| FR-9   | Implementation must use millis() instead of delay() where possible |
| FR-10  | Each LED must have separate blinking function within its corresponding ISR routine |

## 4. Opis rješenja

### 4.1 Arhitektura sustava
* Sustav koristi sljedeću logiku upravljanja prekidima:
* Hierarhija prioriteta: INT0 > INT1 > Timer1 > INT5
* Mehanizam preklapanja: Visoki prioriteti mogu prekinuti niže
* Zaštita resursa: Korištenje volatile varijabli i kritičnih sekcija

### 4.2 Detalji implementacije
* INT0 (pin 2) - Najviši prioritet
 > Aktivira se padajućom ivicom
 > Trenutno pali LED (pin 13)
 > Sadrži kratku simulaciju obrade (500 iteracija petlje)
 > Ispisuje status na Serial Monitor
* INT1 (pin 3) - Srednji prioritet
 > Aktivira se padajućom ivicom
 > Pali LED na 300ms
 > Sadrži zaštitu od preklapanja Serial komunikacije
* Timer prekid
 > Konfiguracija Timer1:
 > CTC mod (Clear Timer on Compare)
 > Preddjelitelj 1024
 > Prekid svakih 500ms
* Obrada senzora
 > Očitavanje potenciometra u Timer ISR-u
 > Zaštita očitanja semaforom
 > Ispis vrijednosti na Serial Monitor

### 4.3 Upravljanje LED diodom
* Globalno stanje: volatile bool ledState
* Kontrola u glavnoj petlji:
* Automatsko gašenje nakon obrade

### Testni scenariji:
1. Pojedinačni pritisci tipkala - provjera odgovora LED i Serial ispisa
2. Istovremeni pritisci - provjera prioriteta (INT0 > INT1 > INT5)
3. Promjena potenciometra - provjera očitanja svakih 500ms

## 5. Zaključak
Projekt uspješno demonstrira:
- Upravljanje prekidima s različitim prioritetima
- Ispravnu obradu višestrukih prekida
- Vizualnu povratnu informaciju kroz LED diodu
- Rad s analognim ulazom (potenciometar)
