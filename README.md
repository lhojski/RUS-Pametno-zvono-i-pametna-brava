# Pametno zvono i pametna brava

## Opis projekta

Pametno zvono i pametna brava projekt su usmjereni na modernizaciju sigurnosti doma. Ideja je automatizirati zaključavanje vrata te omogućiti praćenje aktivnosti ispred ulaza putem senzora pokreta i zvona. Sustav korisniku šalje notifikacije na mobitel prilikom detekcije pokreta ili pritiska na zvono, a vrata se mogu otključati i daljinski pomoću mobilnog uređaja.

## Cilj projekta

Cilj projekta je povećati sigurnost i praktičnost ulaska u dom, omogućiti brzo reagiranje na događaje ispred vrata te smanjiti potrebu za fizičkim ključevima.

## Motivacija

Motivacija za razvoj projekta proizašla je iz želje za većom kontrolom nad pristupom vlastitom domu, bržim reagiranjem na posjetitelje te unaprjeđenjem svakodnevne sigurnosti korištenjem modernih rješenja.

---

## 📋 Funkcionalni zahtjevi

### Pametno zvono

| ID       | Funkcionalnost      | Opis                                                     |
| -------- | ------------------- | -------------------------------------------------------- |
| **FR-1** | Detekcija pokreta   | PIR senzor detektira pokret ispred vrata                 |
| **FR-2** | Osvjetljenje gumba  | LED osvijetli gumb nakon detekcije pokreta               |
| **FR-3** | Aktivacija zvona    | Piezo buzzer reproducira zvuk prilikom pritiska na gumb  |
| **FR-4** | Slanje notifikacija | Šalje obavijest vlasniku putem Blynk/Telegram aplikacije |

### Pametna brava

| ID       | Funkcionalnost          | Opis                                                   |
| -------- | ----------------------- | ------------------------------------------------------ |
| **FR-6** | Daljinsko otključavanje | Otključavanje putem mobilne aplikacije                 |
| **FR-7** | Upravljanje bravom      | Servo motor izvodi fizičko otključavanje/zaključavanje |
| **FR-8** | Auto-zaključavanje      | Automatsko zaključavanje nakon 10 sekundi              |
| **FR-9** | Status brave            | RGB indikator prikazuje trenutno stanje brave          |

---

## Instalacija i testiranje

### Online testiranje

Projekt je moguće testirati online preko sljedećeg linka:  
[Pokreni simulaciju](https://wokwi.com/projects/429053956552990721)

### Lokalan razvoj i simulacija

Za lokalno pokretanje projekta potrebno je:

1. Klonirati repozitorij:
   ```bash
   git clone https://github.com/lhojski/RUS-Pametno-zvono-i-pametna-brava
   ```
2. Otvoriti projekt u Visual Studio Code.
3. Instalirati ekstenzije
   - Wokwi for VS Code
   - PlatformIO IDE
4. Buildati projekt
5. Pokrenuti simulaciju unutar Visual Studio Code-a.

### Realna implementacija

Za stvarnu instalaciju potrebno je nabaviti sljedeće komponente:

- ESP32 razvojna pločica
- Piezo buzzer
- Gumb
- HC-SR04 ultrazvučni senzor
- RGB LED dioda
- Obična LED dioda
- Keypad tipkovnica
- Servo motor (za zaključavanje)

Komponente treba povezati prema priloženoj shemi (vidi sliku u projektu).

**Autori:** Damjan Jurak i Leon Hojski
