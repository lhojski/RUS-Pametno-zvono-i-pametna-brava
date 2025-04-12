# Pametno zvono i pametna brava

Ovaj projekt kombinira pametno zvono i pametnu bravu s daljinskim upravljanjem. Koristi se Arduino za osnovnu funkcionalnost.

## **Funkcionalnosti**

### **1. Pametno zvono**

| ID       | Funkcionalnost     | Opis                                                           |
| -------- | ------------------ | -------------------------------------------------------------- |
| **FR-1** | Detekcija pokreta  | PIR senzor detektira pokret i aktivira LED osvjetljenje gumba. |
| **FR-2** | Osvjetljenje gumba | LED dioda osvijetli gumb nakon detekcije pokreta.              |
| **FR-3** | Zvuk zvona         | Piezo buzzer reproducira zvuk prilikom pritiska na gumb.       |

### **2. Pametna brava**

| ID       | Funkcionalnost                       | Opis                                                                 |
| -------- | ------------------------------------ | -------------------------------------------------------------------- |
| **FR-4** | Servo/elektromagnetska brava         | Servo motor ili elektromagnetska brava izvodi fizicko otključavanje. |
| **FR-5** | Automatsko zaključavanje             | Brava se automatski zaključa nakon određenog vremena (npr. 10s).     |
| **FR-6** | Status brave (otključano/zaključano) | LED indikator ili notifikacija prikazuje trenutni status brave.      |
| **FR-7** | Otključavanje pomoću lozinke         | Unosom lozinke pomoću keypada moguće je otključati bravu.            |

---

## 📌**Potrebne komponente**

| Komponenta            | Količina | Opis                                                           |
| --------------------- | -------- | -------------------------------------------------------------- |
| Arduino Mega 2560     | 1        | Glavni mikrokontroler (ESP32 je bolji zbog ugrađenog Wi-Fi-a). |
| PIR senzor (HC-SR501) | 1        | Detektira pokret i aktivira LED.                               |
| LED dioda             | 1        | Osvjetljava gumb za zvono.                                     |
| Tipka                 | 1        | Za aktivaciju zvona.                                           |
| Piezo buzzer          | 1        | Reproducira zvuk prilikom pritiska na gumb.                    |
| Servo motor (SG90)    | 1        | Simulira mehanizam brave (ili elektromagnetska brava + relej). |
| Keypad                | 1        | Služi za upisivanje lozinke prilikom otključavanja brave.      |
| Breadboard + žice     | -        | Za povezivanje komponenti.                                     |

---

**Autori:** Damjan Jurak i Leon Hojski
