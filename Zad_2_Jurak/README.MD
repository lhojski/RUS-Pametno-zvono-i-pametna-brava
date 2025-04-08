# 💤 Sustav za upravljanje potrošnjom energije na Arduino Mega 2560

## 📖 Sadržaj
1. [Opis zadatka](#1-opis-zadatka)  
2. [Hardverske komponente](#2-hardverske-komponente)  
3. [Shema spojeva](#3-shema-spojeva)  
4. [Princip rada](#4-princip-rada)  
5. [Sleep modovi mikrokontrolera](#5-sleep-modovi-mikrokontrolera)  
6. [Zaključak](#6-zaključak)  

---

## 1. Opis zadatka
Sustav demonstrira napredne tehnike upravljanja potrošnjom energije kroz ciklički radni režim s periodičnim ulaskom u sleep mode. Glavne karakteristike uključuju:

- **Dvofazni radni ciklus**:
  - Aktivna faza (5s) s LED indikacijom
  - Sleep faza s minimalnom potrošnjom
- **Višestruki mehanizmi buđenja**:
  - Eksterni prekidi (tipkalo)
  - Watchdog timer (~8s)
  - Promjena stanja pina
- **Optimizirana potrošnja**:
  - Automatsko isključivanje nepotrebnih periferija
  - Dinamička konfiguracija sleep modova

---

## 2. Hardverske komponente

| Komponenta          | Količina | Pin na Arduino Mega | Funkcija                  |
|---------------------|----------|---------------------|---------------------------|
| Arduino Mega 2560   | 1        | -                   | Glavni mikrokontroler     |
| LED dioda           | 1        | 13                  | Vizualna indikacija stanja|
| Tipkalo             | 1        | 2 (INT0)            | Eksterni prekid za buđenje|
| Otpornik 220Ω       | 1        | -                   | Zaštita LED diode         |

---

## 3. Spojni detalji

1. **LED krug**:
   - Anoda → Digitalni pin 13
   - Katoda → GND preko 220Ω otpornika
2. **Prekidni krug**:
   - Tipkalo → INT0 (pin 2) i GND
   - Aktivacija padajućom ivicom
3. **Dodatni senzori** (opcionalno):
   - Analogni senzori → A0-A15
   - Digitalni ulazi → Pinovi 0-53

---

## 4. Princip rada

### Radni ciklus:
1. **Aktivna faza** (5 sekundi):
   - LED periodički treperi
   - Mikrokontroler obavlja osnovne zadatke
   - Serijska komunikacija aktivna

2. **Prijelaz u sleep mode**:
   - Postupno gašenje periferija
   - Konfiguracija izvora buđenja
   - Postavljanje SLEEP_MODE_PWR_DOWN

3. **Sleep faza**:
   - Potrošnja < 0.5mA
   - Čuvanje stanja registara
   - Reagiranje na definirane prekide

4. **Buđenje sustava**:
   - Detekcija izvora buđenja
   - Ponovna inicijalizacija periferija
   - Nastavak radnog ciklusa

### Mehanizmi buđenja:
1. **Eksterni prekid** (tipkalo):
   - Niska latencija (~100μs)
   - Direktna reakcija na korisnički input

2. **Watchdog timer**:
   - Periodično buđenje (~8s)
   - Idealno za autonomne uređaje

3. **Promjena stanja pina**:
   - Detekcija senzorskih promjena
   - Podrška za multipleksirane ulaze

---

## 5. Sleep modovi mikrokontrolera

### Usporedba karakteristika:

| Karakteristika       | Idle Mode       | Power-down      | Standby         |
|----------------------|----------------|----------------|----------------|
| **Potrošnja**        | ~15mA          | ~0.5mA         | ~1.2mA         |
| **Vrijeme buđenja**  | ~6μs           | ~6ms           | ~6μs           |
| **Aktivni moduli**   | Periferije     | WDT+INT        | WDT+INT        |
| **Primjena**         | Komunikacijski | Senzorski      | Bežični        |

### Preporučeni scenariji primjene:
1. **Power-down mode**:
   - Senzorske stanice s dugim intervalima mjerenja
   - Baterijski napajani uređaji s dugim vijekom trajanja

2. **Standby mode**:
   - Uređaji s periodičkom komunikacijom
   - Sustavi gdje je potrebna ravnoteža između uštede i brzine

3. **Idle mode**:
   - Aplikacije s kontinuiranim procesiranjem
   - Uređaji gdje je brzina odgovora kritična

---

## 6. Zaključak

### Ključne prednosti implementacije:
- **Energetska efikasnost**:
  - Smanjenje potrošnje >95% u odnosu na kontinuirani rad
  - Dinamička adaptacija prema zahtjevima aplikacije

- **Operativna fleksibilnost**:
  - Podrška za više izvora buđenja
  - Kompatibilnost s različitim vrstama senzora

- **Praktična primjenjivost**:
  - Jednostavna integracija u postojeće projekte
  - Minimalni hardverski zahtjevi

### Potencijalne primjene:
1. **Pametna poljoprivreda**:
   - Senzorske stanice za praćenje uvjeta okoline
   - Sustavi za navodnjavanje na bazi senzora

2. **IoT uređaji**:
   - Bežični senzorski čvorovi
   - Pametni uređaji na baterije

3. **Industrijska automatizacija**:
   - Udaljeni monitoring opreme
   - Sustavi za uštedu energije u postrojenjima
