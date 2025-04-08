# 💤 Sustav za upravljanje potrošnjom energije

**Sustav za upravljanje potrošnjom energije na Arduino Mega 2560**

![Arduino Mega Compatible](https://img.shields.io/badge/Platform-Arduino_Mega_2560-blue)
![LED Visualization](<https://img.shields.io/badge/Visualization-1_LEDs_(RGB)-yellowgreen>)

1. [Opis zadatka](#opis-projekta)
2. [Hardverske komponente](#hardverske-komponente)
3. [Slika spojeva](#slika-spojeva)
4. [Opis rješenja](#opis-rjesenja)
5. [Sleep modovi mikrokontrolera: Usporedba i primjena](#sleep_modovi)
6. [Zaključak](#zakljucak)

## <a name="opis-zadatka"></a>1. Opis zadatka

Sustav demonstrira korištenje sleep modova za smanjenje potrošnje energije:

- LED treperi 5 sekundi u aktivnom modu
- Prelazak u sleep mode nakon aktivnog perioda
- Buđenje pomoću:
  - Eksternog prekida (tipkalo na INT0)
  - Watchdog timera (~8s)
- Vizualna indikacija stanja LED diodom

**Ključne karakteristike:**

- Automatsko gašenje LED u sleep modu
- Podrška za više sleep modova (Idle, Power-down, Standby)
- Optimizirana potrošnja energije

## <a name="hardverske-komponente"></a>2. Hardverske komponente

| Komponenta        | Količina | Pin na Arduino Mega |
| ----------------- | -------- | ------------------- |
| Arduino Mega 2560 | 1        | -                   |
| LED dioda         | 1        | 13                  |
| Tipkalo           | 1        | 2 (INT0)            |
| Otpornik 220Ω     | 1        | -                   |

## <a name="slika-spojeva"></a>3. Slika spojeva

![Wiring Diagram](https://github.com/your-repo/blob/main/sleep-mode-wiring.png?)

- LED spojena na pin 13 kroz otpornik 220Ω
- Tipkalo spojeno na pin 2 i GND (s internim PULLUP)

## <a name="opis-rjesenja"></a>4. Opis rješenja

[Ovdje možete vidjeti simulaciju zadatka rješenu preko platforme wokwi](https://wokwi.com/projects/427675671903754241)

- Kôd koristi tri glavna dijela:

- Aktivni period: LED trepće 5 puta uz ispis poruka u Serial monitor.

- Priprema za sleep:

  - LED se gasi

  - Inicijalizira se Watchdog timer na otprilike 8 sekundi

  - Arduino prelazi u SLEEP_MODE_PWR_DOWN

- Buđenje:

  - Ako je uzrok buđenja tipkalo, ispisuje se "Probudio me prekid s tipkala!"

  - Ako je uzrok buđenja WDT, ispisuje se "Probudio me Watchdog Timer!"

- Prekidi

  - Tipkalo koristi attachInterrupt za FALLING rub, jer koristimo INPUT_PULLUP.

  - WDT interrupt postavljen da se javi nakon otprilike 8 sekundi koristeći WDTCSR.

## <a name="sleep_modovi"></a>5. Sleep modovi mikrokontrolera: Usporedba i primjena

## Tablica usporedbe sleep modova

| Sleep Mode     | Potrošnja energije | Što se isključuje?                        | Vrijeme buđenja  | Načini buđenja                      |
| -------------- | ------------------ | ----------------------------------------- | ---------------- | ----------------------------------- |
| **Idle**       | Minimalna ušteda   | CPU, Flash memorija, ALU                  | Vrlo brzo (~6µs) | Bilo koji prekid                    |
| **Power-down** | Maksimalna ušteda  | Gotovo sve (osim WDT i eksternih prekida) | Sporo (~6ms)     | Eksterni prekid, WDT, promjena pina |
| **Standby**    | Velika ušteda      | Gotovo sve (osim WDT i eksternih prekida) | Brzo (~6µs)      | Eksterni prekid, WDT                |

## Detaljna objašnjenja

### 1. **Idle Mode**

- **Karakteristike**:
  - Najmanja ušteda energije od tri moda
  - Brzo buđenje (~6 mikrosekundi)
- **Ostaje aktivan**:
  - Svi periferni moduli
  - Timeri
  - Watchdog timer
  - Eksterni prekidi
- **Primjena**:
  - Kratki periodi neaktivnosti
  - Aplikacije gdje je brzina odgovora kritična

### 2. **Power-down Mode**

- **Karakteristike**:
  - Najveća ušteda energije
  - Sporo buđenje (~6 milisekundi)
- **Ostaje aktivan**:
  - Watchdog timer (ako je omogućen)
  - Eksterni prekidi (INT0, INT1)
- **Primjena**:
  - Dugi periodi neaktivnosti
  - Aplikacije na bateriju gdje je životni vijek bitan

### 3. **Standby Mode**

- **Karakteristike**:
  - Slična ušteda energije kao Power-down
  - Brže buđenje (~6 mikrosekundi)
- **Ostaje aktivan**:
  - Watchdog timer
  - Eksterni prekidi
- **Primjena**:
  - Aplikacije koje zahtijevaju ravnotežu između uštede energije i brzine odgovora

## Preporuke za korištenje

1. **Power-down**:

   - Koristite za maksimalnu uštedu energije
   - Primjer: Senzorske stanice koje mjere svakih sat vremena

2. **Standby**:

   - Koristite kada trebate dobru uštedu uz relativno brzo buđenje
   - Primjer: Daljinski upravljači

3. **Idle**:
   - Koristite kada je brzina odgovora važnija od uštede energije
   - Primjer: Uređaji u kontinuiranoj komunikaciji

## Najefikasniji mod

**Power-down mode** nudi najveću uštedu energije i najbolje je rješenje za aplikacije gdje mikrokontroler većinu vremena provodi u neaktivnom stanju. Međutim, za aplikacije koje zahtijevaju brže buđenje, **Standby mode** predstavlja dobar kompromis.

> **Napomena:** Vrijeme buđenja može varirati ovisno o specifičnom mikrokontroleru i njegovim postavkama.

## <a name="zakljucak"></a>6. Zaključak

Implementacija sleep modova omogućuje:

- Smanjenje potrošnje do 99% u odnosu na aktivni mod

- Fleksibilno buđenje pomoću više izvora

- Jednostavnu integraciju u postojeće projekte

Primjena:

- Baterijski napajani uređaji

- Senzorske stanice

- IoT uređaji s periodičnim radom
