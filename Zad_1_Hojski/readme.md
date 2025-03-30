# 🚦 Arduino Interrupt Priority System

**Sustav za upravljanje višestrukim prekidima s prioritetima na Arduino Mega 2560**

![Arduino Mega Compatible](https://img.shields.io/badge/Platform-Arduino_Mega_2560-blue)
![LED Visualization](<https://img.shields.io/badge/Visualization-3_LEDs_(RGB)-yellowgreen>)

## 📖 Sadržaj

1. [Opis projekta](#opis-projekta)
2. [Hardverske komponente](#hardverske-komponente)
3. [Slika spojeva](#slika-spojeva)
4. [Testiranje](#testiranje)
5. [Rezultat](#rezultat)
6. [Moguća poboljšanja](#moguca-poboljsanja)

## <a name="opis-projekta"></a>1. Opis projekta

Sustav demonstrira obradu višestrukih prekida s različitim prioritetima koristeći:

- 3 tipkala za generiranje prekida (INT0, INT1, INT2)
- 3 LED-ice (crvena, žuta, zelena) za vizualizaciju prioriteta
- Arduino Mega 2560 mikrokontroler

**Ključna svojstva**:

- Softversko upravljanje prioritetima
- Debounce za pouzdano detektiranje tipkala
- Serijski izlaz za debug informacije

## <a name="hardverske-komponente"></a>2. Hardverske komponente

| Komponenta        | Količina | Pin na Arduino Mega |
| ----------------- | -------- | ------------------- |
| Arduino Mega 2560 | 1        | -                   |
| Tipkalo           | 3        | 2, 3, 21            |
| Crvena LED        | 1        | 13                  |
| Žuta LED          | 1        | 12                  |
| Zelena LED        | 1        | 11                  |
| Otpornik 220Ω     | 3        | -                   |

## <a name="slika-spojeva"></a> 3. Slika spojeva

![Wiring Diagram](Prekidi.png)

## 4. <a name="testiranje"></a>Testiranje

## 📌 Test Slučaj 1: Prioritetna obrada prekida

**Cilj**:
Demonstrirati kako sustav prvo obrađuje prekid s najvišim prioritetom kada se aktiviraju višestruki prekidi istovremeno.

**Koraci testiranja**:

1. Pritisnite sva tri tipkala (INT0, INT1, INT2) istovremeno
2. Promatrajte redoslijed paljenja LED dioda

**Očekivano ponašanje**:

1. Crvena LED (INT0) upali se prva (najviši prioritet)
2. Žuta LED (INT1) upali se nakon 1 sekunde
3. Zelena LED (INT2) upali se nakon dodatne 1 sekunde

**Tehnički detalji**:

- INT0 (pin 2) ima najviši prioritet u kodu
- Varijabla `processingInterrupt` osigurava redoslijednu obradu
- Pauza od 1 sekunde omogućuje vizualnu potvrdu

## 📌 Test Slučaj 2: Neovisna obrada pojedinačnih prekida

**Cilj**:
Pokazati kako sustav ispravno reagira na pojedinačne prekide bez utjecaja drugih ulaza.

**Koraci testiranja**:

1. Pritisnite samo tipkalo 1 (INT0)
2. Ponovite za tipkalo 2 (INT1) i tipkalo 3 (INT2) posebno

**Očekivano ponašanje**:

- Svaki pojedinačni pritisak:
  - Upali odgovarajuću LED diodu (crvena/žuta/zelena)
  - Dioda ostaje upaljena točno 1 sekundu
  - U Serial Monitoru ispisuje odgovarajuću poruku (npr. "INT0 aktiviran")

**Tehnički detalji**:

- Svaki prekid ima vlastitu ISR funkciju
- Debounce osigurava jedan odaziv po pritisku
- Neovisna obrada bez blokiranja glavnog programa

## 5. <a name="rezultat"></a>Rezultat

## 6. <a name="moguca-poboljsanja"></a>Moguća poboljšanja
