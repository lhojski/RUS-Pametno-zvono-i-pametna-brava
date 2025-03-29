# 🚦 Arduino Interrupt Priority System

**Sustav za upravljanje višestrukim prekidima s prioritetima na Arduino Mega 2560**

![Arduino Mega Compatible](https://img.shields.io/badge/Platform-Arduino_Mega_2560-blue)
![LED Visualization](<https://img.shields.io/badge/Visualization-3_LEDs_(RGB)-yellowgreen>)

## 📖 Sadržaj

1. [Opis projekta](#-opis-projekta)
2. [Hardverske komponente](#-hardverske-komponente)
3. [Shema spojeva](#-shema-spojeva)
4. [Instalacija i upotreba](#%EF%B8%8F-instalacija-i-upotreba)
5. [Funkcionalnosti](#-funkcionalnosti)
6. [Detalji implementacije](#-detalji-implementacije)
7. [Testiranje](#-testiranje)
8. [Moguća poboljšanja](#-moguća-poboljšanja)

## 🎯 Opis projekta

Sustav demonstrira obradu višestrukih prekida s različitim prioritetima koristeći:

- 3 tipkala za generiranje prekida (INT0, INT1, INT2)
- 3 LED-ice (crvena, žuta, zelena) za vizualizaciju prioriteta
- Arduino Mega 2560 mikrokontroler

**Ključna svojstva**:

- Softversko upravljanje prioritetima
- Debounce za pouzdano detektiranje tipkala
- Serijski izlaz za debug informacije

## 🔌 Hardverske komponente

| Komponenta        | Količina | Pin na Arduino Mega |
| ----------------- | -------- | ------------------- |
| Arduino Mega 2560 | 1        | -                   |
| Tipkalo           | 3        | 2, 3, 21            |
| Crvena LED        | 1        | 13                  |
| Žuta LED          | 1        | 12                  |
| Zelena LED        | 1        | 11                  |
| Otpornik 220Ω     | 3        | -                   |

## 🛠️ Shema spojeva

![Wiring Diagram](Prekidi.png) _(Primjer sheme - zamijenite sa stvarnom slikom)_

```text
Tipkala:
  [BTN1] ---- D2 (INT0) -- GND
  [BTN2] ---- D3 (INT1) -- GND
  [BTN3] ---- D21 (INT2) -- GND

LED-ice:
  [Crvena] ---- D13 --| 220Ω |-- GND
  [Žuta]   ---- D12 --| 220Ω |-- GND
  [Zelena] ---- D11 --| 220Ω |-- GND
```
